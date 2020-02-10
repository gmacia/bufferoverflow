; Compilar con: 
; nasm -f elf32 -o miniprograma miniprograma.asm
; Enlazar con: 
; ld -m elf_i386 -o miniprograma miniprograma.o 
; Ejecutar con: 
; ./miniprograma_assembler; echo $?


BITS 32

global _start

section text

_start: 
	mov eax, 1  ; Valor de la syscall exit (tomado de unistd.h)
	mov ebx, 32 ; Retorna un numero 32 de exit	
	int 0x80


