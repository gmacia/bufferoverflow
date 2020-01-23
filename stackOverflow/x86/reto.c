/*
 *	APLICACIÓN reto.c 
 * 	RETO Buffer Overflow - 2019
 * 	By: Gabriel Maciá
 * 
 *  Ejecutar en servidor con: 
 *   while true; do socat TCP-LISTEN:12345,reuseaddr,fork SYSTEM:./reto ; done
 * 
 *  Acceder con: nc <ip_maquina_servidor> 12345
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

char password_global[30] = "";

void log_access (char *pass, int tipoAcceso) {
	time_t now;
	FILE *f;
	int codigoAcceso; 
	
	f = fopen("./reto.log", "a");
	if (!f) {
		printf ("Error al abrir fichero de log\n");
		exit(0);
	}
	time (&now);
	codigoAcceso = rand();
	printf ("\n\t[*] Anota esta info para mandar por email al profesor: \n");
	printf ("\tCodigo de Aceso: %d\n\tFecha y hora: %s\n", codigoAcceso, ctime(&now));
	fflush(stdout); 
	// Log a fichero reto.log
	if (tipoAcceso == BOF1)	fprintf (f, "[+] Acceso basico: ");
	else fprintf (f, "[*] Acceso completo: ");
	fprintf (f, "%s\tCode: %d\t%s", pass,codigoAcceso,ctime(&now));
	fclose(f);
}	

int show_password () {

	printf ("\n\t[*] The password is: %s\n\n", password_global); 
	log_access(password_global, BOF2); 
	sleep(0.1);
	return 0;
}


int check_password (char *pass) {

	char password[32] = "hacking";
	char buf[32];
	
	memset (buf, '\0', 32);
	if (strcmp (password_global, "") != 0) {	
		strcpy (password, password_global);
	}
	strcpy (buf, pass);

	if (strncmp (buf+2, password+2,4)==0 && strncmp(buf, "ha",2)==0) {
		log_access(buf, BOF1);
		return 1;
	} else {
		return 0;
	}

}

void imprime_banner() {
	
	char mensaje[500];

	FILE *f = fopen ("./banner.txt", "rt");
	if (f != NULL) {
		while (fgets(mensaje, 500, f) != NULL) { 		
			printf ("%s", mensaje);
		}
	}
	strcpy (mensaje,"--------------------------------------------------------------\n    BIENVENIDO AL RETO HACKING ETICO DE BUFFER OVERFLOW\n--------------------------------------------------------------\n	Introduzca la contraseña: ");
	printf ("%s", mensaje);
}



int main(int argc, char *argv[])
{

    char userpass[1025];
    
    srand(time(NULL));  // Para generar codigos de acceso
	setbuf(stdout, 0); //Evita retrasos en el envío de printados. 
	
	// Lectura de password del fichero
	FILE *fp = fopen ("./password.txt", "r");
    if (fp != NULL) {
		fscanf (fp, "%s", password_global);
		fclose(fp);
    }

	// Bucle principal
    while(1)
    {
		
		// Envío de mensaje de pregunta de contraseña y lectura de contraseña
		imprime_banner ();
		int n = read (0, userpass, sizeof (userpass)-1);	
		userpass[n-1] = '\0';

		// Comprobacion de la contraseña recibida
		int accesopermitido = check_password (userpass);
		if (accesopermitido) {
			printf ("\n\t[+] ACCESO PERMITIDO. ERES UN CRACK\n\n");
			exit(0);
		} else {
			printf ("\n\t[!] password incorrecta\n");
		}
		
	 }
}
