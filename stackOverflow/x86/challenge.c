/*
 *   reto.c APPLICATION
 *   Buffer Overflow CHALLENGE
 *   By: Gabriel Maciá
 * 
 *  Run on server with: 
 *   while true; do socat TCP-LISTEN:12345,reuseaddr,fork SYSTEM:./reto ; done
 * 
 *  Access with: nc <server_ip> 12345
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#define	BOF1 1
#define BOF2 2

char global_password[30] = "";

void log_access (char *pass, int accessType) {
	time_t now;
	FILE *f;
	int accessCode; 
   
	f = fopen("./challenge.log", "a");
	if (!f) {
		printf ("Error opening log file\n");
		exit(0);
	}
	time (&now);
	accessCode = rand();
	printf ("\n\t[*] Write this info to send by email to the teacher: \n");
	printf ("\tAccess code: %d\n\tDate and time: %s\n", accessCode, ctime(&now));
	fflush(stdout); 
	// Log to reto.log file
	if (accessType == BOF1)	fprintf (f, "[+] Basic access: ");
	else fprintf (f, "[*] Complete access: ");
	fprintf (f, "%s\tCode: %d\t%s", pass,accessCode,ctime(&now));
	fclose(f);
}	

int show_password () {

	printf ("\n\t[*] The password is: %s\n\n", global_password); 
	log_access(global_password, BOF2); 
	sleep(0.1);
	return 0;
}


int check_password (char *pass) {

	char password[32] = "hacking";
	char buf[32];
	
	memset (buf, '\0', 32);
	if (strcmp (global_password, "") != 0) {	
		strcpy (password, global_password);
	}
	strcpy (buf, pass);

	if (strncmp (buf+2, password+2,4)==0 && strncmp(buf, "ha",2)==0) {
		log_access(buf, BOF1);
		return 1;
	} else {
		return 0;
	}

}

void print_banner() {
	
	char message[500];

	FILE *f = fopen ("./banner.txt", "rt");
	if (f != NULL) {
		while (fgets(message, 500, f) != NULL) {
			printf ("%s", message);
		}
	}
		strcpy (message,"--------------------------------------------------------------\n    WELCOME TO THE ETHICAL HACKING BUFFER OVERFLOW CHALLENGE\n--------------------------------------------------------------\n   Enter the password: ");
	printf ("%s", message);
}



int main(int argc, char *argv[])
{

    char userpass[1025];
    
	srand(time(NULL));  // To generate access codes
	setbuf(stdout, 0); // Prevents delays in sending printed output. 
	
	// Reading password from the file
	FILE *fp = fopen ("./password.txt", "r");
    if (fp != NULL) {
		fscanf (fp, "%s", global_password);
		fclose(fp);
    }

	// Main loop
	while(1)
	{
		
		// Sending password prompt message and reading the password
		print_banner ();
		int n = read (0, userpass, sizeof (userpass)-1);	
		userpass[n-1] = '\0';

		// Checking the received password
		int accesspermitted = check_password (userpass);
			if (accesspermitted) {
				printf ("\n\t[+] ACCESS GRANTED. YOU ARE AWESOME\n\n");
				exit(0);
			} else {
				printf ("\n\t[!] Incorrect password\n");
			}
		
	 }
}
