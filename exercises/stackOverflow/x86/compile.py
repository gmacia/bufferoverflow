
# Note: to compile 32-bit on UBUNTU18 and Kali, you may have issues unless you run:
# sudo apt-get install gcc-multilib g++-multilib

import sys
import os

if len(sys.argv)<2:
	print('Usage: compile <prog_name.c> [nx]')
	print('nx: compile with NX enabled (disabled by default)')
	exit(0)



print('\n\n[+] Compiling ' + sys.argv[1]  + "...")



outputName = sys.argv[1].split('.')[0]


# COMPILATION WITHOUT NX
if len(sys.argv)<3:
	command = 'gcc -m32 -no-pie -fno-stack-protector -z execstack -o ' + outputName + ' -g ' + sys.argv[1]
# COMPILATION WITH NX
elif sys.argv[2] == "nx":
	command = 'gcc -m32 -no-pie -fno-stack-protector -o ' + outputName + ' -g ' + sys.argv[1]
else:
	command = ''
print(command)
print('----------------------------------------')
os.system(command)
print('\n')

