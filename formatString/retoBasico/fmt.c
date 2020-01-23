/* 
	Compilación: gcc -g -m32 -o fmt fmt.c
	Reto: que escriba "pronta" en lugar de "prueba"
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void main (int argc, char *argv[]) {

	char buffer[1024];
	char cadena[8] = "prueba";

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);	

	printf ("Direccion de cadena: %p\n",cadena);
	printf ("Escribe algo> ");
	read (0, buffer, 1024);
	
	// Lo correcto seria escribir: 
	// printf ("Has escrito: %s\n", argv[1]);
	// Pero en cambio escribimos: 

	printf ("Has escrito: "); 
	printf (buffer);
	printf ("\n");
	printf ("Cadena: %s\n", cadena);
}
