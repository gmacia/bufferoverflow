/* 
	Compilation: gcc -g -m32 -o fmt fmt.c
	Challenge: write "pronta" instead of "prueba"
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void main (int argc, char *argv[]) {

		char buffer[1024];
		char string[8] = "prueba";
		int n; 
	
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);	

		printf ("String address: %p\n",string);
		printf ("Write something> ");
		n = read (0, buffer, 1024);
		buffer [n] = '\0';
		
		// The correct way would be to write: 
		// printf ("You have written: %s\n", argv[1]);
		// But instead we write: 

		printf ("You have written: "); 
		printf (buffer);
		printf ("\n");
		printf ("String: %s\n", string);
}
