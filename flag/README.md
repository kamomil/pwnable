first I see that `readelf` and `objdump` gives nothing, so `strings -n 7` shows a string indicatind that it 
was packed with somehitng called `UPX` so first to unpack: `upx -d flag`
Now `objdump -d flag` gives:
```
0000000000401164 <main>:
  401164:       55                      push   %rbp
  401165:       48 89 e5                mov    %rsp,%rbp
  401168:       48 83 ec 10             sub    $0x10,%rsp
  40116c:       bf 58 66 49 00          mov    $0x496658,%edi
  401171:       e8 0a 0f 00 00          callq  402080 <_IO_puts>
  401176:       bf 64 00 00 00          mov    $0x64,%edi
  40117b:       e8 50 88 00 00          callq  4099d0 <__libc_malloc>
  401180:       48 89 45 f8             mov    %rax,-0x8(%rbp)
  401184:       48 8b 15 e5 0e 2c 00    mov    0x2c0ee5(%rip),%rdx        # 6c2070 <flag>
  40118b:       48 8b 45 f8             mov    -0x8(%rbp),%rax
  40118f:       48 89 d6                mov    %rdx,%rsi
  401192:       48 89 c7                mov    %rax,%rdi
  401195:       e8 86 f1 ff ff          callq  400320 <.plt+0x10>
  40119a:       b8 00 00 00 00          mov    $0x0,%eax
  40119f:       c9                      leaveq 
  4011a0:       c3                      retq   
  4011a1:       90                      nop
  4011a2:       90                      nop
  4011a3:       90                      nop
  4011a4:       90                      nop
  4011a5:       90                      nop

```
We can see it refers to `# 6c2070 <flag>` . `readelf -l flag` shows that `6c2070` belongs to the `.data` section at offset `0x10`:
```
[26] .data             PROGBITS         00000000006c2060  000c2060
       0000000000001690  0000000000000000  WA       0     0     32
```
Let see what's there:
```
blah@blah:~/blah$ objdump -s -j .data flag

flag:     file format elf64-x86-64

Contents of section .data:
 6c2060 00000000 00000000 00000000 00000000  ................
 6c2070 28664900 00000000 00000000 00000000  (fI.............
 6c2080 00080000 00000000 00000000 00000000  ................
```
So it's a pointer that refers to 0x496628, `readelf -l flag` again tell us it's offset 0x08 of `.rodata`:
```
 [10] .rodata           PROGBITS         0000000000496620  00096620
       000000000001d3a4  0000000000000000   A       0     0     32
```

so now all we need is to look at the `.rodata`:
```
$ objdump -s -j .rodata flag|less

Contents of section .rodata:
 496620 01000200 00000000 5550582e 2e2e3f20  ........UPX...? 
 496630 736f756e 6473206c 696b6520 61206465  sounds like a de
 496640 6c697665 72792073 65727669 6365203a  livery service :
 496650 29000000 00000000 49207769 6c6c206d  ).......I will m
 496660 616c6c6f 63282920 616e6420 73747263  alloc() and strc
 496670 70792074 68652066 6c616720 74686572  py the flag ther
 496680 652e2074 616b6520 69742e00 46415441  e. take it..FATA
 496690 4c3a206b 65726e65 6c20746f 6f206f6c  L: kernel too ol
 4966a0 640a002f 6465762f 7572616e 646f6d00  d../dev/urandom.
...
```
And we got:
**UPX...? sounds like a delivery service :)**

