/*****
 Compilar: gcc -g -o callconvention callconvention.c

 Depurar con gdb: disass main
 Comprobar como se llama a la función con los parametros en: RDI, RSI, RDX

 0x0000000000001135 <+0>:     push   rbp
 0x0000000000001136 <+1>:     mov    rbp,rsp
 0x0000000000001139 <+4>:     mov    edx,0x0
 0x000000000000113e <+9>:     mov    esi,0x0
 0x0000000000001143 <+14>:    lea    rdi,[rip+0xeba]        # 0x2004
 0x000000000000114a <+21>:    call   0x1030 <execve@plt>
 0x000000000000114f <+26>:    nop
 0x0000000000001150 <+27>:    pop    rbp
 0x0000000000001151 <+28>:    ret  
*****/

#include<unistd.h>
void main()
{
	execve("/bin/sh",0,0);
}
