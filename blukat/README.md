### blucat 
googling `bluecat vulnerability` this task seems to be inspired by [CVE-2007-4390](https://www.cvedetails.com/cve/CVE-2007-4390/)
Looking at the code, There is a buffer overflow when reading user input:
```
char buf[100];
printf("guess the password!\n");
fgets(buf, 128, stdin);
```
It seems that we should overwrite the input to `strcmp` so that it will compare `password` with `password`
(Instead of `password` and `buf`).

SCP the files:
```
scp -P 2222 blukat@pwnable.kr:~/blukat.c .
scp -P 2222 blukat@pwnable.kr:~/blukat .
```
Apparently, there is no use of these buffer overflow! all return address are assign to lower address on the stack than the `buf` so it can't override them!.
Also, trying:
```
blukat@ubuntu:~$ mkdir /tmp/dafna
blukat@ubuntu:~$ vim /tmp/dafna/sc
blukat@ubuntu:~$ cat /tmp/dafna/sc
import sys
print('\x00'*int(sys.argv[1]))
blukat@ubuntu:~$ for i in $(seq 100 500); do echo $i; python /tmp/dafna/sc $i| ./blukat; done
```
didn't seg fault !
Then I though it is possible to reveal the password from the key.
Then, the solution gives only 3 points, so it must be easy. Then obviously it is easy , we just can see the password by examining password address in gdb!:
```
blukat@ubuntu:~$ gdb ./blukat
GNU gdb (Ubuntu 7.11.1-0ubuntu1~16.04) 7.11.1
Copyright (C) 2016 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./blukat...(no debugging symbols found)...done.
(gdb) b main
Breakpoint 1 at 0x4007fe
(gdb) r
Starting program: /home/blukat/blukat 

Breakpoint 1, 0x00000000004007fe in main ()
(gdb) disas
Dump of assembler code for function main:
   0x00000000004007fa <+0>:	push   %rbp
   0x00000000004007fb <+1>:	mov    %rsp,%rbp
=> 0x00000000004007fe <+4>:	add    $0xffffffffffffff80,%rsp
   0x0000000000400802 <+8>:	mov    %fs:0x28,%rax
   0x000000000040080b <+17>:	mov    %rax,-0x8(%rbp)
   0x000000000040080f <+21>:	xor    %eax,%eax
   0x0000000000400811 <+23>:	mov    $0x40096a,%esi
   0x0000000000400816 <+28>:	mov    $0x40096c,%edi
   0x000000000040081b <+33>:	callq  0x400660 <fopen@plt>
   0x0000000000400820 <+38>:	mov    %rax,-0x78(%rbp)
   0x0000000000400824 <+42>:	mov    -0x78(%rbp),%rax
   0x0000000000400828 <+46>:	mov    %rax,%rdx
   0x000000000040082b <+49>:	mov    $0x64,%esi
   0x0000000000400830 <+54>:	mov    $0x6010a0,%edi
   0x0000000000400835 <+59>:	callq  0x400640 <fgets@plt>
   0x000000000040083a <+64>:	mov    $0x400982,%edi
   0x000000000040083f <+69>:	callq  0x4005f0 <puts@plt>
   0x0000000000400844 <+74>:	mov    0x200835(%rip),%rdx        # 0x601080 <stdin@@GLIBC_2.2.5>
   0x000000000040084b <+81>:	lea    -0x70(%rbp),%rax
   0x000000000040084f <+85>:	mov    $0x80,%esi
   0x0000000000400854 <+90>:	mov    %rax,%rdi
   0x0000000000400857 <+93>:	callq  0x400640 <fgets@plt>
   0x000000000040085c <+98>:	lea    -0x70(%rbp),%rax
   0x0000000000400860 <+102>:	mov    %rax,%rsi
   0x0000000000400863 <+105>:	mov    $0x6010a0,%edi
   0x0000000000400868 <+110>:	callq  0x400650 <strcmp@plt>
   0x000000000040086d <+115>:	test   %eax,%eax
   0x000000000040086f <+117>:	jne    0x4008a0 <main+166>
   0x0000000000400871 <+119>:	mov    $0x400996,%edi
   0x0000000000400876 <+124>:	mov    $0x0,%eax
   0x000000000040087b <+129>:	callq  0x400620 <printf@plt>
   0x0000000000400880 <+134>:	mov    $0x6010a0,%edi
   0x0000000000400885 <+139>:	callq  0x400786 <calc_flag>
   0x000000000040088a <+144>:	mov    $0x0,%eax
   0x000000000040088f <+149>:	mov    -0x8(%rbp),%rcx
   0x0000000000400893 <+153>:	xor    %fs:0x28,%rcx
   0x000000000040089c <+162>:	je     0x4008b9 <main+191>
   0x000000000040089e <+164>:	jmp    0x4008b4 <main+186>
   0x00000000004008a0 <+166>:	mov    $0x4009b4,%edi
   0x00000000004008a5 <+171>:	callq  0x4005f0 <puts@plt>
   0x00000000004008aa <+176>:	mov    $0x0,%edi
   0x00000000004008af <+181>:	callq  0x400670 <exit@plt>
   0x00000000004008b4 <+186>:	callq  0x400610 <__stack_chk_fail@plt>
---Type <return> to continue, or q <return> to quit---
   0x00000000004008b9 <+191>:	leaveq 
   0x00000000004008ba <+192>:	retq   
End of assembler dump.
(gdb) b *0x000000000040083a
Breakpoint 2 at 0x40083a
(gdb) c
Continuing.

Breakpoint 2, 0x000000000040083a in main ()
(gdb) disas 
Dump of assembler code for function main:
   0x00000000004007fa <+0>:	push   %rbp
   0x00000000004007fb <+1>:	mov    %rsp,%rbp
   0x00000000004007fe <+4>:	add    $0xffffffffffffff80,%rsp
   0x0000000000400802 <+8>:	mov    %fs:0x28,%rax
   0x000000000040080b <+17>:	mov    %rax,-0x8(%rbp)
   0x000000000040080f <+21>:	xor    %eax,%eax
   0x0000000000400811 <+23>:	mov    $0x40096a,%esi
   0x0000000000400816 <+28>:	mov    $0x40096c,%edi
   0x000000000040081b <+33>:	callq  0x400660 <fopen@plt>
   0x0000000000400820 <+38>:	mov    %rax,-0x78(%rbp)
   0x0000000000400824 <+42>:	mov    -0x78(%rbp),%rax
   0x0000000000400828 <+46>:	mov    %rax,%rdx
   0x000000000040082b <+49>:	mov    $0x64,%esi
   0x0000000000400830 <+54>:	mov    $0x6010a0,%edi
   0x0000000000400835 <+59>:	callq  0x400640 <fgets@plt>
=> 0x000000000040083a <+64>:	mov    $0x400982,%edi
   0x000000000040083f <+69>:	callq  0x4005f0 <puts@plt>
   0x0000000000400844 <+74>:	mov    0x200835(%rip),%rdx        # 0x601080 <stdin@@GLIBC_2.2.5>
   0x000000000040084b <+81>:	lea    -0x70(%rbp),%rax
   0x000000000040084f <+85>:	mov    $0x80,%esi
   0x0000000000400854 <+90>:	mov    %rax,%rdi
   0x0000000000400857 <+93>:	callq  0x400640 <fgets@plt>
   0x000000000040085c <+98>:	lea    -0x70(%rbp),%rax
   0x0000000000400860 <+102>:	mov    %rax,%rsi
   0x0000000000400863 <+105>:	mov    $0x6010a0,%edi
   0x0000000000400868 <+110>:	callq  0x400650 <strcmp@plt>
   0x000000000040086d <+115>:	test   %eax,%eax
   0x000000000040086f <+117>:	jne    0x4008a0 <main+166>
   0x0000000000400871 <+119>:	mov    $0x400996,%edi
   0x0000000000400876 <+124>:	mov    $0x0,%eax
   0x000000000040087b <+129>:	callq  0x400620 <printf@plt>
   0x0000000000400880 <+134>:	mov    $0x6010a0,%edi
   0x0000000000400885 <+139>:	callq  0x400786 <calc_flag>
   0x000000000040088a <+144>:	mov    $0x0,%eax
   0x000000000040088f <+149>:	mov    -0x8(%rbp),%rcx
   0x0000000000400893 <+153>:	xor    %fs:0x28,%rcx
   0x000000000040089c <+162>:	je     0x4008b9 <main+191>
   0x000000000040089e <+164>:	jmp    0x4008b4 <main+186>
   0x00000000004008a0 <+166>:	mov    $0x4009b4,%edi
   0x00000000004008a5 <+171>:	callq  0x4005f0 <puts@plt>
   0x00000000004008aa <+176>:	mov    $0x0,%edi
   0x00000000004008af <+181>:	callq  0x400670 <exit@plt>
   0x00000000004008b4 <+186>:	callq  0x400610 <__stack_chk_fail@plt>
---Type <return> to continue, or q <return> to quit---
   0x00000000004008b9 <+191>:	leaveq 
   0x00000000004008ba <+192>:	retq   
End of assembler dump.
(gdb) x/16xb 0x6010a0
0x6010a0 <password>:	0x63	0x61	0x74	0x3a	0x20	0x70	0x61	0x73
0x6010a8 <password+8>:	0x73	0x77	0x6f	0x72	0x64	0x3a	0x20	0x50
(gdb) x/16cb 0x6010a0
0x6010a0 <password>:	99 'c'	97 'a'	116 't'	58 ':'	32 ' '	112 'p'	97 'a'	115 's'
0x6010a8 <password+8>:	115 's'	119 'w'	111 'o'	114 'r'	100 'd'	58 ':'	32 ' '	80 'P'
(gdb) x/100cb 0x6010a0
0x6010a0 <password>:	99 'c'	97 'a'	116 't'	58 ':'	32 ' '	112 'p'	97 'a'	115 's'
0x6010a8 <password+8>:	115 's'	119 'w'	111 'o'	114 'r'	100 'd'	58 ':'	32 ' '	80 'P'
0x6010b0 <password+16>:	101 'e'	114 'r'	109 'm'	105 'i'	115 's'	115 's'	105 'i'	111 'o'
0x6010b8 <password+24>:	110 'n'	32 ' '	100 'd'	101 'e'	110 'n'	105 'i'	101 'e'	100 'd'
0x6010c0 <password+32>:	10 '\n'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'
0x6010c8 <password+40>:	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'
0x6010d0 <password+48>:	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'
0x6010d8 <password+56>:	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'
0x6010e0 <password+64>:	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'
0x6010e8 <password+72>:	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'
0x6010f0 <password+80>:	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'
0x6010f8 <password+88>:	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'	0 '\000'
0x601100 <password+96>:	0 '\000'	0 '\000'	0 '\000'	0 '\000'
(gdb) 
(gdb) c
Continuing.
guess the password!
cat: password: Permission denied
congrats! here is your flag: Pl3as_DonT_Miss_youR_GrouP_Perm!!
[Inferior 1 (process 8331) exited normally]
(gdb) 

```
