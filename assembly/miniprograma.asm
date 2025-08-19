; Compile with: 
; nasm -f elf32 -o miniprograma miniprograma.asm
; Link with: 
; ld -m elf_i386 -o miniprograma miniprograma.o 
; Run with: 
; ./miniprograma_assembler; echo $?


BITS 32

global _start

section text

_start: 
	mov eax, 1  ; Value of the exit syscall (taken from unistd.h)
	mov ebx, 32 ; Returns exit number 32	
	int 0x80


