#include <stdio.h> 
#include <string.h> 

void main(int argc, char *argv[]) 
{
  char buf[100]; 
  printf ("Dime algo> "); 
  gets (buf);
  printf("Has escrito: %s\n",buf); 
}
