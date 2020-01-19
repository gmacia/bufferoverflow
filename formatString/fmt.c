#include <stdio.h>
#include <string.h>

void main (int argc, char *argv[]) {

	char buffer[1024];
	char cadena[8] = "prueba";

	// Lo correcto seria escribir: 
	// printf ("Has escrito: %s\n", argv[1]);
	// Pero en cambio escribimos: 

	strncpy (buffer, argv[1],1023);
	printf ("Direccion de cadena: %p\n",cadena);
	printf ("Has escrito: "); 
	printf (buffer);
	printf ("\n");

}
