#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

void memcheck_execution();
void print_autor();
void print_help();

int main (int argc, char** argv){
// structure for the long options.


	int optionIdx,c;
 
	static struct option lopts[] = {
		{"autor"  ,no_argument,0,'a'},
		{"help"     ,no_argument,0,'h'},
		{"program"     ,no_argument,0,'p'},
		{0,0,0,0}
	};
  
	while ((c = getopt_long(argc, argv, "hpa:o:",lopts,&optionIdx)) != -1) {
		switch (c) {
			case 'a':
				print_autor();
				break;
			case 'p':
				memcheck_execution();				
				break;
			
			case 'h':
				print_help();
				break;

			default:
				printf("Option '-%c' not recognized.\n",c);
			}
	}

	return 0;
}

void memcheck_execution(){
	printf("Ejecutar el programa\n");
	int pid;

	pid = fork();
	printf("Pid %i\n", pid);
	if (pid == 0)
	{   // in child 
		// set up arguments //
		// launch here
		printf("Running ls command \n");
		int result = execl("/bin/sh", "/bin/sh", "-c","LD_PRELOAD=./libmemcheck.so ./mem.so",0);
		printf("After execute execl\n");
		printf("Execk return %i\n",result);
		exit(0);
	}
	else if (pid < 0)
	{   // handle error
	}


	exit(0);
}

void print_autor(){
	printf("###########################################\n");
	printf("#               Memcheck tool             #\n");
	printf("###########################################\n");
	printf("# Autor: Marco Espinoza Murillo           #\n");
	printf("# Carne: 200418646                        #\n");
	printf("# Maestria Sistemas Embebidos             #\n");
	printf("# Instituo Tecnologico de Costa Rica      #\n");
	printf("###########################################\n");
	exit(0);
}

void print_help(){
	printf("###########################################\n");
	printf("#               Memcheck tool             #\n");
	printf("###########################################\n");
	printf("# Argumentos validos:                     #\n");
	printf("#   -a: Muestra la informacion del autor  #\n");
	printf("#   -h: Muestra este menu de ayuda        #\n");
	printf("#   -p: Recibe el programa a evaluar      #\n");
	printf("###########################################\n");
	exit(0);
	
}

/*
void main(){
}


*/
