
/*
Exercises to do:
1. Explain the program code
2. Run ./overflow_example hola
	- See how variables are stored contiguously in memory.
	- Check that each variable correctly stores its value.

3. Run overflow on buf_two:
	./overflow_example 1234567890
	- Check that '90' is stored in buf_one

4. Overflow value:
	./overflow_example 12345678123456781
	- Check that the ascii code for '1' is stored: 49

5. Overflow value with a value using perl:
	./overflow_example `perl -e 'print "1234567812345678"x2 . "\x06"'`

*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void usage (char *progName) {

	printf ("Usage: %s <string>\n", progName); 

}


int main (int argc, char *argv[]) {

	int value = 5; 
	char buf_one[8], buf_two[8];

	if (argc<2) {
		usage(argv[0]);
		exit(0);
	}

	strcpy (buf_one, "one"); 
	strcpy (buf_two, "two"); 

	printf ("\n\t[BEFORE] buf_two is at %p and contains '%s'\n", buf_two, buf_two); 
	printf ("\t[BEFORE] buf_one is at %p and contains '%s'\n", buf_one, buf_one); 
	printf ("\t[BEFORE] value is at %p and is %d\n", &value, value); 

	printf ("\n\t[STRCPY] Copying %d bytes into buf_two\n\n", strlen (argv[1])); 
	strcpy (buf_two, argv[1]); 

	printf ("\t[AFTER] buf_two is at %p and contains '%s'\n", buf_two, buf_two);
	printf ("\t[AFTER] buf_one is at %p and contains '%s'\n", buf_one, buf_one); 
	printf ("\t[AFTER] value is at %p and is %d\n\n", &value, value); 
}
