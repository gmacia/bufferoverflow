
/*
Ejercicios a realizar: 
1. Explicar el código del programa
2. Ejecutar ./overflow_example hola
	- Ver cómo se almacenan las variables de forma contigua en memoria. 
	- Ver que cada variable almacena correctamente su valor. 

3. Ejecutar overflow en buf_dos: 
	./overflow_example 1234567890
	- Comprobar que en buf_uno se almacena '90'

4. Hacer overflow de value:
	./overflow_example 12345678123456781
	- Comprobar que se almacena el código ascii del '1': 49
	
5. Hacer overflow de value con un valor con perl: 
	./overflow_example `perl -e 'print "1234567812345678"x2 . "\x06"'`

*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void usage (char *progName) {

	printf ("Usage: %s <cadena>\n", progName); 

}


int main (int argc, char *argv[]) {

	int value = 5; 
	char buf_uno[8], buf_dos[8];

	if (argc<2) {
		usage(argv[0]);
		exit(0);
	}

	strcpy (buf_uno, "uno"); 
	strcpy (buf_dos, "dos"); 

	printf ("\n\t[ANTES] buf_dos esta en %p y contiene \'%s\'\n", buf_dos, buf_dos); 
	printf ("\t[ANTES] buf_uno esta en %p y contiene \'%s\'\n", buf_uno, buf_uno); 
	printf ("\t[ANTES] value esta en %p y vale %d\n", &value, value); 

	printf ("\n\t[STRCPY] Copiando %d bytes en buf_dos\n\n", strlen (argv[1])); 
	strcpy (buf_dos, argv[1]); 

	printf ("\t[DESPUES] buf_dos esta en %p y contiene \'%s\'\n", buf_dos, buf_dos);
 	printf ("\t[DESPUES] buf_uno esta en %p y contiene \'%s\'\n", buf_uno, buf_uno); 
	printf ("\t[DESPUES] value esta en %p y vale %d\n\n", &value, value); 
}
