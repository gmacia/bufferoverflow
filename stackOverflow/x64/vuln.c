#include<stdio.h> 
#include<string.h> 

void vuln(char *cadena)
{ 
  char buf[100]; 
  strcpy(buf,cadena); 
  printf("Has escrito: %s\n",buf); 
}


void main(int argc, char *argv[]) 
{
  if (argc>1) {
	  vuln(argv[1]);
  } else {
  	printf ("Usage: vuln <cadena>\n");
  }
}
