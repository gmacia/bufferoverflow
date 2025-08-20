#include <stdio.h> 
#include <string.h> 

void main(int argc, char *argv[]) 
{
  char buf[100]; 
  printf ("Tell me something> "); 
  gets (buf);
  printf("You have written: %s\n", buf);
}
