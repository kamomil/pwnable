## insights ragarding icectf2018 twitter,
I did not solve this challange during the competition, I then read a writeup of Benjamin Levy about it:
He was able to deduce the Chip8 structure from the Chip8::Chip8 function - the constructor.
Here is the interesting part of the constructor code:
```
   0) 0x55555555603e <_ZN5Chip8C2EP6ScreenP8Keyboard+8>:	mov    QWORD PTR [rbp-0x18],rdi
   1) 0x555555556042 <_ZN5Chip8C2EP6ScreenP8Keyboard+12>:	mov    QWORD PTR [rbp-0x20],rsi
   2) 0x555555556046 <_ZN5Chip8C2EP6ScreenP8Keyboard+16>:	mov    QWORD PTR [rbp-0x28],rdx
   3) 0x55555555604a <_ZN5Chip8C2EP6ScreenP8Keyboard+20>:	mov    rax,QWORD PTR [rbp-0x18]
   4) 0x55555555604e <_ZN5Chip8C2EP6ScreenP8Keyboard+24>:	mov    rdx,QWORD PTR [rbp-0x20]
   5) 0x555555556052 <_ZN5Chip8C2EP6ScreenP8Keyboard+28>:	mov    QWORD PTR [rax],rdx
   6) 0x555555556055 <_ZN5Chip8C2EP6ScreenP8Keyboard+31>:	mov    rax,QWORD PTR [rbp-0x18]
   7) 0x555555556059 <_ZN5Chip8C2EP6ScreenP8Keyboard+35>:	mov    rdx,QWORD PTR [rbp-0x28]
   8) 0x55555555605d <_ZN5Chip8C2EP6ScreenP8Keyboard+39>:	mov    QWORD PTR [rax+0x8],rdx
   9) 0x555555556061 <_ZN5Chip8C2EP6ScreenP8Keyboard+43>:	mov    rax,QWORD PTR [rbp-0x18]
   10) 0x555555556065 <_ZN5Chip8C2EP6ScreenP8Keyboard+47>:	mov    WORD PTR [rax+0x1020],0x0
   11) 0x55555555606e <_ZN5Chip8C2EP6ScreenP8Keyboard+56>:	mov    rax,QWORD PTR [rbp-0x18]
   12) 0x555555556072 <_ZN5Chip8C2EP6ScreenP8Keyboard+60>:	mov    WORD PTR [rax+0x1022],0x200
   13) 0x55555555607b <_ZN5Chip8C2EP6ScreenP8Keyboard+69>:	mov    rax,QWORD PTR [rbp-0x18]
   14) 0x55555555607f <_ZN5Chip8C2EP6ScreenP8Keyboard+73>:	mov    BYTE PTR [rax+0x1024],0x0
   15) 0x555555556086 <_ZN5Chip8C2EP6ScreenP8Keyboard+80>:	mov    rax,QWORD PTR [rbp-0x18]
   16) 0x55555555608a <_ZN5Chip8C2EP6ScreenP8Keyboard+84>:	mov    BYTE PTR [rax+0x1025],0x0
   17) 0x555555556091 <_ZN5Chip8C2EP6ScreenP8Keyboard+91>:	mov    rax,QWORD PTR [rbp-0x18]
   18) 0x555555556095 <_ZN5Chip8C2EP6ScreenP8Keyboard+95>:	mov    WORD PTR [rax+0x1046],0x0
  ```
  from the type of the pointer (QWORD/WORD/BYTE) PTR we can deduce the type of the field
  What happen here is that $rdi holds the input to the function - the pointer to the struct that we want to initialize - it resides in the stack. So the constructor does not allocate the memory itself but only fills it.
  It puts $rdi in $rbp-0x18 and fills the adress $rdi points to.
  3)-8) - fills two pointer in Chip8 to point to the function
  `NcursesScreen::SetPixel(int,`,  `NcursesKeyboard::Read()`.
  9)-16) initialize to shorts and two bytes, starting from position 0x1020 from the struct.
  17)-18) initialize a short in position 0x1046 in the struct.
  Then threre are 3 memset calls to zerofy arrays within the struct. I will not put the code here.
  Then we have a loop:
  ```
  0x5555555560ee <_ZN5Chip8C2EP6ScreenP8Keyboard+184>:	mov    DWORD PTR [rbp-0x4],0x0
   0x5555555560f5 <_ZN5Chip8C2EP6ScreenP8Keyboard+191>:	cmp    DWORD PTR [rbp-0x4],0x4f
   0x5555555560f9 <_ZN5Chip8C2EP6ScreenP8Keyboard+195>:	ja     0x55555555611a <_ZN5Chip8C2EP6ScreenP8Keyboard+228>
   0x5555555560fb <_ZN5Chip8C2EP6ScreenP8Keyboard+197>:	mov    edx,DWORD PTR [rbp-0x4]
   0x5555555560fe <_ZN5Chip8C2EP6ScreenP8Keyboard+200>:	lea    rax,[rip+0x20305b]        # 0x555555759160 <_ZL13CHIP8_FONTSET>
   0x555555556105 <_ZN5Chip8C2EP6ScreenP8Keyboard+207>:	movzx  ecx,BYTE PTR [rdx+rax*1]
   0x555555556109 <_ZN5Chip8C2EP6ScreenP8Keyboard+211>:	mov    rdx,QWORD PTR [rbp-0x18]
   0x55555555610d <_ZN5Chip8C2EP6ScreenP8Keyboard+215>:	mov    eax,DWORD PTR [rbp-0x4]
   0x555555556110 <_ZN5Chip8C2EP6ScreenP8Keyboard+218>:	mov    BYTE PTR [rdx+rax*1+0x20],cl
   0x555555556114 <_ZN5Chip8C2EP6ScreenP8Keyboard+222>:	add    DWORD PTR [rbp-0x4],0x1
   0x555555556118 <_ZN5Chip8C2EP6ScreenP8Keyboard+226>:	jmp    0x5555555560f5 <_ZN5Chip8C2EP6ScreenP8Keyboard+191>
   0x55555555611a <_ZN5Chip8C2EP6ScreenP8Keyboard+228>:	nop
  ```
  The loop fills the first 0x4f bytes of one of the arrays in the struct to values that reside in
  the .data section, and is labeld CHIP8_FONTSET
  
  overall this is how the stack looks like when returning from the constructor:
  ```
rpb-40(=0x28) | --> ($rbp+0x1070) --> NcursesKeyboard::Read() ($rdx)
rpb-32(=0x20) | --> ($rbp+0x10c0) --> NcursesScreen::SetPixel ($rsi)($rdx)
rpb-24(=0x18) | --> ($rbp+0x20) --> ($rbp-0x10c0) --> NcursesScreen::SetPixel ($rdi)($rax)
-----------------------------------
rpb-16        | 
-----------------------------------
rpb-8         |
-----------------------------------
$rbp          | 0x00007fffffffc2d0
-----------------------------------
$rbp+8        |
-----------------------------------
$rbp+16       |
-----------------------------------
$rbp+24       |
-----------------------------------
|$rbp+0x20     | --> ($rbp+0x10c0) --> NcursesScreen::SetPixel(int,
-----------------------------------
|$rbp+0x28     | --> ($rbp+0x1070) --> NcursesKeyboard::Read()
-----------------------------------
|$rbp+0x30 - $rbp+0x40     | memset to 0
-----------------------------------
|$rbp+0x40 - $rbp+0x1040   | memset to 0 | then $rbp+0x40-$rbp+0x80 is filled with the values of |CHIP8_FONTSET, which is in the .data section:
-----------------------------------
|$rbp+0x1040   | 0x0 as short
|$rbp+0x1042   | 0x200 as short
|$rbp+0x1044   | 0x0 as byte
|$rbp+0x1045   | 0x0 as byte
----------------------------
|$rbp+0x1046 - $rbp+0x1066 | memset to 0
----------------------------
|$rbp+0x1066 | 0x0 as short
  ```
 so the struct is:
 ```
 struct chip8 {
	void *screen; // 0 //points to a place on the stack that points to NcursesScreen::SetPixel(int,
	void *keyboard; // 8 //points to a place on the stack that points to NcursesKeyboard::Read()
	uint8_t V[0x10]; // 0x10
	uint8_t mem[0x1000]; // 0x20 //first 0x4f bytes are memcopied from CHIP8_FONTSET in the .data sect
	uint16_t I; // 0x1020
	uint16_t pc; // 0x1022
	uint8_t unknown1; // 0x1024 //related to the Chip8::Tick60Hz
	uint8_t unknown2; // 0x1025 //related to the Chip8::Tick60Hz
	uint8_t unknown3[0x20]; // 0x1026
	uint16_t unknown3;
} __attribute__((packed));

 ```
 When returning from the constructor, $rbp become `0x7fffffffdbe0`, which is  0x1910 larger so now
 (when returning to `Run` function)
 the struct is inside the current frame (it's addr is smaller than $rbp).
Lets look at the `Run` function:

======
Run
======
`Run(char*)` - gets it's input from the $rdi register, which points to the name of the ROM file

```
-----------------------------------
$rbp-0x18f0     | --> ($rbp+0x10c0) --> NcursesScreen::SetPixel(int,
-----------------------------------
$rbp-0x18e8     | --> ($rbp+0x1070) --> NcursesKeyboard::Read()
-----------------------------------
$rbp-0x18e0 - $rbp-0x18d0     | memset to 0
-----------------------------------
$rbp-0x18d0 - $rbp-0x8d0   | memset to 0 | then $rbp-0x18d0 to $rbp-0x1890 is filled with the values of CHIP8_FONTSET, which is in the .data section:
-----------------------------------
$rbp-0x8d0    | 0x0 as short
$rbp-0x8ce    | 0x200 as short
$rbp-0x8cc   | 0x0 as byte
$rbp-0x8cb   | 0x0 as byte
----------------------------
$rbp+0x1046 - $rbp+0x1066 | memset to 0
----------------------------
$rbp-0x8aa | 0x0 as short
---------------------------
$rbp-0x8a8
---------------------------
$rbp-0x8a0 this is input to _ZN15NcursesKeyboardC2Ev
.
.
---------------------------
$rbp-0x850 ($rax, $rdi) this is the input to the function _ZN13NcursesScreenC2Ev
---------------------------
.
.
----------------------------------
$rbp-0x40     | input to std::__cxx11::basic_string<char, std::char_traits<char>,std::allocator<char> - this will point to the name of the ROM file
-----------------------------------
.
.
.
----------------------------------
$rbp-0x11     | input to _ZNSaIcEC1Ev@plt which is std::allocator<char>::allocator()@plt()
-----------------------------------
$rbp-0x8      | $rbx -> main
----------------------------------
$rbp        | 0x7fffffffdbe0
-------------------------------------
```

from these instructions in the Run function:
```
0x555555555891 <_Z3RunPKc+49>:	lea    rdx,[rbp-0x8a0]
0x555555555898 <_Z3RunPKc+56>:	lea    rcx,[rbp-0x850]
0x55555555589f <_Z3RunPKc+63>:	lea    rax,[rbp-0x18f0]
0x5555555558a6 <_Z3RunPKc+70>:	mov    rsi,rcx
0x5555555558a9 <_Z3RunPKc+73>:	mov    rdi,rax
0x5555555558ac <_Z3RunPKc+76>:	call   0x555555556036 <_ZN5Chip8C2EP6ScreenP8Keyboard> chip8::chip8 constructor

```
we can see that the Chip8::Chip8 constructor get three variables, 
one in $rdi is the struct to initialize
second in $rsi is an adress in the stack segment that holds poiters to 
NcursesScreen::SetPixel
third in is an adress in the stack segment that holds poiters to $rdx NcursesKeyboard::Read()


```
//initialize $rbp-x011 with an allocator
   0x5555555558b1 <_Z3RunPKc+81>:	lea    rax,[rbp-0x11]
   0x5555555558b5 <_Z3RunPKc+85>:	mov    rdi,rax
   0x5555555558b8 <_Z3RunPKc+88>:	call   0x5555555556f0 <_ZNSaIcEC1Ev@plt> (std::allocator<char>::allocator()@plt())

```

```
//set $bp-0x40 to point to the ROM file name string
 0x5555555558bd <_Z3RunPKc+93>:	lea    rdx,[rbp-0x11]
 0x5555555558c1 <_Z3RunPKc+97>:	mov    rcx,QWORD PTR [rbp-0x18f8]
 0x5555555558c8 <_Z3RunPKc+104>:	lea    rax,[rbp-0x40]
 0x5555555558cc <_Z3RunPKc+108>:	mov    rsi,rcx
 0x5555555558cf <_Z3RunPKc+111>:	mov    rdi,rax
 0x5555555558d2 <_Z3RunPKc+114>:	call   0x555555555640 <_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EPKcRKS3_@plt> //std::basic_string

```
```
//didn't look to what Chip8::LoadROM does, but it gets our Chip8::Chip8 struct and the file name
//so it probably reads the file and load the struct with it
=> 0x5555555558d7 <_Z3RunPKc+119>:	lea    rdx,[rbp-0x40]
   0x5555555558db <_Z3RunPKc+123>:	lea    rax,[rbp-0x18f0]
   0x5555555558e2 <_Z3RunPKc+130>:	mov    rsi,rdx
   0x5555555558e5 <_Z3RunPKc+133>:	mov    rdi,rax
   0x5555555558e8 <_Z3RunPKc+136>:	call   0x55555555612a <_ZN5Chip87LoadROMERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE>

```
Instead of digging the code of Chip8::LoadROM, I dumped the content of the Chip8 struct before and after the function
What I see is that at $rbp-0x16d0 the content changes from 0 to:
```
0x7fffffffc510:	0xa2	0x1e	0xc2	0x01	0x32	0x01	0xa2	0x1a
0x7fffffffc518:	0xd0	0x14	0x70	0x04	0x30	0x40	0x12	0x00
0x7fffffffc520:	0x60	0x00	0x71	0x04	0x31	0x20	0x12	0x00
0x7fffffffc528:	0x12	0x18	0x80	0x40	0x20	0x10	0x20	0x40
0x7fffffffc530:	0x80	0x10	0x00	0x00	0x00	0x00	0x00	0x00
``` 
The whole struct is 0x1048 bytes with the following content before calling loadROM, 

looking at the stack layout:
```
-----------------------------------
$rbp-0x18d0 - $rbp-0x8d0   | memset to 0 | then $rbp-0x18d0 to $rbp-0x1890 is filled with the values of CHIP8_FONTSET, which is in the .data section:
-----------------------------------
```
so `$rbp-0x16d0` is in the memory array at offset `0x200` and not supprisingly this is the content of the ROM file

Then we have another call to std::basic_string and another call to std::allocator, not sure why.

Then it seems that $eax holds the return value of Chip8::LoadROM. Then there is a branching according the weather it returned 1 or 0.
In my case it returned 1, and we branch to `=> 0x555555555915 <_Z3RunPKc+181>:	lea    rax,[rbp-0x18f0]`.

Then from the function Run there is a call to function `Chip8::Run` with the Chip8 struct as parameter.
In the case of the MAZE.ROM input, the execution never returns to the Run function

