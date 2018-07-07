`gdb`ing the `bof` file in the `bof` challenge in pwnable.kr/play.php . I get:
```
blah@blah:~/blah$ gdb ./bof
(gdb) disas func
Dump of assembler code for function func:
   0x0000062c <+0>:	push   %ebp
   0x0000062d <+1>:	mov    %esp,%ebp
   0x0000062f <+3>:	sub    $0x48,%esp
   0x00000632 <+6>:	mov    %gs:0x14,%eax
   0x00000638 <+12>:	mov    %eax,-0xc(%ebp)
   0x0000063b <+15>:	xor    %eax,%eax
   0x0000063d <+17>:	movl   $0x78c,(%esp)
   0x00000644 <+24>:	call   0x645 <func+25>
   0x00000649 <+29>:	lea    -0x2c(%ebp),%eax
   0x0000064c <+32>:	mov    %eax,(%esp)
   0x0000064f <+35>:	call   0x650 <func+36>
   0x00000654 <+40>:	cmpl   $0xcafebabe,0x8(%ebp)
   0x0000065b <+47>:	jne    0x66b <func+63>
   0x0000065d <+49>:	movl   $0x79b,(%esp)
   0x00000664 <+56>:	call   0x665 <func+57>
   0x00000669 <+61>:	jmp    0x677 <func+75>
   0x0000066b <+63>:	movl   $0x7a3,(%esp)
   0x00000672 <+70>:	call   0x673 <func+71>
   0x00000677 <+75>:	mov    -0xc(%ebp),%eax
   0x0000067a <+78>:	xor    %gs:0x14,%eax
   0x00000681 <+85>:	je     0x688 <func+92>
   0x00000683 <+87>:	call   0x684 <func+88>
   0x00000688 <+92>:	leave  
   0x00000689 <+93>:	ret    
(gdb) b gets
Breakpoint 1 at 0x4c0
(gdb) r
Starting program: /home/dafna/kamomil/bof 
overflow me : 

Breakpoint 1, 0xf7e40ae5 in gets () from /lib32/libc.so.6
(gdb) n
Single stepping until exit from function gets,
which has no line number information.

0x56555654 in func ()
(gdb) disas func
Dump of assembler code for function func:
   0x5655562c <+0>:	push   %ebp
   0x5655562d <+1>:	mov    %esp,%ebp
   0x5655562f <+3>:	sub    $0x48,%esp
   0x56555632 <+6>:	mov    %gs:0x14,%eax
   0x56555638 <+12>:	mov    %eax,-0xc(%ebp)
   0x5655563b <+15>:	xor    %eax,%eax
   0x5655563d <+17>:	movl   $0x5655578c,(%esp)
   0x56555644 <+24>:	call   0xf7e41360 <puts>
   0x56555649 <+29>:	lea    -0x2c(%ebp),%eax
   0x5655564c <+32>:	mov    %eax,(%esp)
   0x5655564f <+35>:	call   0xf7e40ae0 <gets>
=> 0x56555654 <+40>:	cmpl   $0xcafebabe,0x8(%ebp)
   0x5655565b <+47>:	jne    0x5655566b <func+63>
   0x5655565d <+49>:	movl   $0x5655579b,(%esp)
   0x56555664 <+56>:	call   0xf7e16d10 <system>
   0x56555669 <+61>:	jmp    0x56555677 <func+75>
   0x5655566b <+63>:	movl   $0x565557a3,(%esp)
   0x56555672 <+70>:	call   0xf7e41360 <puts>
   0x56555677 <+75>:	mov    -0xc(%ebp),%eax
   0x5655567a <+78>:	xor    %gs:0x14,%eax
   0x56555681 <+85>:	je     0x56555688 <func+92>
   0x56555683 <+87>:	call   0xf7ee1ee0 <__stack_chk_fail>
   0x56555688 <+92>:	leave  
   0x56555689 <+93>:	ret    
```

one can see how `call` instructions in the assembly changed :
```
0x00000644 <+24>: call 0x645 <func+25> changed to 0x56555644 <+24>:	call   0xf7e41360 <puts>
...
0x0000064f <+35>: call 0x650 <func+36> changed to 0x5655564f <+35>:	call   0xf7e40ae0 <gets>
...
0x00000664 <+56>: call 0x665 <func+57> changed to 0x56555664 <+56>:	call   0xf7e16d10 <system>
```

This is due to relocation. Looking at the dynamic relocation:
```
blah@blah:~/blah$ objdump -R bof

...
00000645 R_386_PC32        puts@GLIBC_2.0
00000673 R_386_PC32        puts@GLIBC_2.0
00000650 R_386_PC32        gets@GLIBC_2.0
00000665 R_386_PC32        system@GLIBC_2.0
...
```
we see a relocation of matching offsets as the calls in func.
This is relocation of type  R_386_PC32. It takes the 4 bytes after the `e8` byte that indicate `call` instruction and replace them with the right value. The value is calculated with `S + A – P`
* S is the value of the symbol whose index resides in the relocation entry.
* A is the addend found in the relocation entry.
* P is the place (section offset or address) of the storage unit being relocated
(computed using r_offset ).

Lets look at the second changed call:
`0x0000064f <+35>: call 0x650 <func+36>`
changed to
**`0x5655564f`**` <+35>:	call   `**`0xf7e40ae0`**` <gets>`

If you  `objdump -d bof` you see 
`64f:   e8 fc ff ff ff          call   650 <func+0x24>`
So `fc ff ff ff` should be replaced with a value that indicates the address of `gets`

`S` is our target address, that is `gets` in this case so `S=0xf7e40ae0`.
`A` is `0xfffffffc = -4` 
`P` is `0x56555650`  which is the runtime position where the `fc ff ff ff` start in the `call` instruction.
The instruction at  **`0x5655564f`** and **`0x56555650`** is one byte further.
So now we have `S + A – P = 0xf7e40ae0 -0x4 - 0x56555650 = 0xA18EB48C`
now to make sure we are correct, lets see the actual values of the `call` instruction:
The last `disas func` in gdb showed that the current instruction  pointer  (stored in $eip) is 5 bytes after the `call` instruction:
```
   0x5655564f <+35>: call 0xf7e40ae0 <gets> 
=> 0x56555654 <+40>: cmpl $0xcafebabe,0x8(%ebp)
```
so we have to look 5 bytes back from the current `$eip`:
```
(gdb) x/5b $eip-5
0x5655564f <func+35>:	0xe8	0x8c	0xb4	0x8e	0xa1
```
`e8` is the opcode of `call` and the other 4 bytes are `0x8c	0xb4	0x8e	0xa1` which are exactly the
`0xA18EB48C` we calculated:)


