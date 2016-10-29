#include <stdarg.h>
#include <signal.h>
#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <errno.h>
#include <sys/user.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

//Global Variables

struct  file_read {
	int index;
	char addr[400];
	struct file_read *next;
};  

struct file_read *_fp;

//Declaring methods which will be used during the program execution
void run_target(const char* programname);
void run_debugger(pid_t child_pid, int _step_mode, int _check_var, int _glob_var);
void run_nm(const char* programname);
void read_var (const char* global_var_read);
unsigned int check_var_addr(char* s_var_addr);
void print_autor();
void print_help();

//Main method
//This is the method in charge to execute the program
//It will call all the methods required to execute the application

int main(int argc, char** argv)
{

//Variable declaration required for the program execution
	char* program = NULL;
	int bin_file=0;
	int step_mode=0;
	int check_var=0;
	char* s_var_addr = NULL;
	unsigned int var_addr;
	int c;
	pid_t child_pid, nm_pid;
	int status;
	opterr = 0;

//This while is used to read the arguments added to through the command line to the application
	while ((c = getopt (argc, argv, "hasw:b:")) != -1){
/*
The following switch will identify the arguments read, and it will call the proper method
and set the proper variables in order to execute the required methods
*/

		switch (c) {
			case 'a':
			//'a' means the autor information will be printed
				print_autor();
				break;
			case 'b':
			//'b' It will have the path for the binary file to be executed
				program = optarg;
				bin_file = 1;
				break;			
			case 'h':
			//'h' is used to print the help menu
				print_help();
				break;
			case 'w':
			//'w' has the global variable name which will be monitored during the test execution
				s_var_addr = optarg;
				check_var=1;
				break;
			case 's':
			//'s' will execute the binary file step by step
				step_mode=1;
				break;

			case '?':
				if (optopt == 'c')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				        return 1;
			default:
			//In case the argument read is not supported
				printf("Option '-%i' not recognized.\n",c);
			}
	}
	/*
	-b argument is required for the program to execute the binary file
	If the argument is not added, the program will fail due to there is not a file to run
	The following "if" will check if this argument was added in the command line,  
	in case the argument was not added, the program will fail.	
	*/

	if(!bin_file){
		fprintf(stderr, "-Error- Argument -b <binary file> is required\n");
		exit(0);
	}else{
		fprintf(stderr, "-Info- Program %s will be executed with the debugger\n", program);
	}

	/*
	-w argument is optional, it will have the global var name which will be 
	monitored during the binary execution.
	The following "if" is used to check if a variable will be monitored during the program
	execution.
	In case the user added a variable to be monitored, the program will get the address related 
	with this variable. 
	*/

	if(check_var){
		/*
		There is a variable to be monitored, a fork is used due to the nm program execution 
		is required in order get the address belonging to the variable name added
		*/
			
		nm_pid = fork();
		if(nm_pid == 0){
			//run_nm method is used to execute the "nm" program
			run_nm(program);
		}else{
			//With this "if" we are pretty sure the process related with nm is already finished
			if (waitpid (nm_pid, &status, 0) == nm_pid){	
				/*
				The nm execution was stored in the file temp_var, this file will be read with the method read_var
				It will store in a linked list the file to get the required from it. 
				*/								
				read_var("temp_var");
				//var_addr is used to get the address value belonging to the variable added through the switch -w
				var_addr = check_var_addr(s_var_addr);
				printf("-Info- Address to monitor during execution is: 0x%08x\n", var_addr);
			}

		}
	}else{
		printf("-Info- Warning... No global variable will be monitored\n");
	}

	child_pid = fork();
	if (child_pid == 0){
		/*
		run_target method is used to execute the binary file
		but using PTRACE_TRACEME, it will tell the kernel 
		the process is being traced		
		*/
		run_target(program);
	}else if (child_pid > 0){
		/*
		When the process finish the execution, the method run_debugger is executed
		to get all the information required for our program
		This is the method in charge to get the RIPs executed, the values for the global vars
		and if the program will be executed by steps.
		*/
		run_debugger(child_pid, step_mode,check_var, var_addr);
	}else {
		perror("fork");
		return -1;
	}
	return 0;
}

/*
read_var method gets as argument the name of the file to be open
The method will store the file in a linked list
Reason to use the linked list is to reserve only the the memory required by the file
*/
void read_var (const char* global_var_read){
	//Variable required by the method
	FILE *fp;
	int cont=0; 	
	struct file_read *aux;	

	_fp = malloc( sizeof(struct file_read) );  //storing the memory required for the structure _fp
	aux = _fp;                                 //aux is a pointer used to store each line belonging to the file
 
	fp = fopen(global_var_read,"r");          //Reading the file name stored in the var global_var_read

	/*
	This "while" will read each line from the file, and it will store this line in the linked list
	*/
	
	while (fscanf(fp, "%s",aux->addr) != EOF) {
		aux->index=cont;
		aux->next = malloc( sizeof(struct file_read) ); 
		aux = aux->next;
		memset(aux->addr, 0, sizeof(aux->addr));
		cont++;
	}
}

/*
Due to the file was stored in a linked list, the following method will get as argument the variable added through the command line
And it will check if the variable is stored in the progrm to be executed
In case the var exists, the method will get the address belonging to the var name, and will return this value
It will be used during the program execution to identify the variable changes
*/

unsigned int check_var_addr(char* s_var_addr){
	//Variables required by the program 
	struct file_read *aux;	
	aux = _fp;
	char* str_in_prog;
	int id = -1;
	unsigned int _value;

	/*
	The following while will run the linked list item by item comparing each variable name
	available in the program with the variable name added by the user
	In case the variable is not present in the program, the program execution will finish,
	in the opposite case, the ID will be stored, because in the file stored, we know the address
	for the variable will be stored 2 items before of the actual item
	From this index it is possible gets the address related with the variable added by the user
	*/

	while(aux->next != NULL){
		str_in_prog = aux->addr;
		if(strcmp(str_in_prog, s_var_addr) == 0){
			id = aux->index - 2;
		}
		aux = aux->next;
	}
	
	/*
	The following if is used to check if the variable name is in the program to be debugged
	If the variable exists, the program will run again the linked list to identify the index related with the 
	address value for the variable added by the user
	If the value is identified, this value is returned by the method to check the address during the debugger execution 
	*/
	if(id != -1){	
		aux = _fp;
		while(aux->next != NULL){
			if(id == aux->index){
				str_in_prog = aux->addr;
				sscanf(str_in_prog,"%16x",&_value);
			}
			aux = aux->next;
		}
	}else{
		printf("-Error- The variable added is not in the program\n\n");
		printf("Exiting...");
		exit(0);
	}
	
	printf("-Info- The variable %s is matching the address %16x\n",s_var_addr, _value);
	return _value; 
	
}

/*
The following method just execute the program through NM
NM is used to get all the variables presents in the program 
It will allow us to know if the variable added by the user 
with the option -w is present or not in the program to be debugged 
*/
void run_nm(const char* programname){
	char* nm_lit = "nm ";
	char* nm_exec;

	nm_exec = malloc(strlen(nm_lit)+1+4);	
	strcpy(nm_exec, nm_lit);
	strcat(nm_exec, programname);
	strcat(nm_exec, " > temp_var");

	printf("Executing nm script....\n");
	execl("/bin/sh", "/bin/sh","-c",nm_exec,0);

	
}

/*
run_target method is used to execute PTRACE_TRACEME
This is used to tell the kernel that the process received as argument is being traced.
At the moment the system call occurs, the kernel saves the original contenst of the eax register, because it contains the system call number.
*/

void run_target(const char* programname)
{

	// Allow tracing of this process /
	if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
		perror("ptrace");
		return;
	}

	// Replace this process's image with the given program 
	execl(programname, programname, 0);
}

/*
run_debugger is the method in charge to get all the required to know the IP executed by the program
Also, in case a variable is being monitored, this method will check those changes and it will notify 
in console about the old value and new value for this variable
*/

void run_debugger(pid_t child_pid, int _step_mode, int _check_var, int _glob_var)
{
	//variables required in the method
	int wait_status;
	unsigned int new_val=0;
	unsigned int old_val=0;
	unsigned int icounter = 0;
	printf("-Info- Debugger started\n");


	// Wait for child to stop on its first instruction 
	wait(&wait_status);

	/*
	WIFSTOPPED will return true if the child process was stopped
	In this case, it is possible because the child is being traced through ptrace
	It allow us to get all the required from the child process while it is stopped
	*/ 		

	while (WIFSTOPPED(wait_status)) {
		icounter++; //Will have the number of instructions executed
		struct user_regs_struct regs; //Used to get the registers values

		ptrace(PTRACE_GETREGS, child_pid, 0, &regs); //PTRACE_GETREGS is used to get the registers values from the core
		unsigned instr = ptrace(PTRACE_PEEKTEXT, child_pid, regs.rip, 0);  //PTRACE_PEEKTEXT is used to get the value stored in the actual PC.

		printf("[%u.]  PC = 0x%08x.  instr = 0x%08x\n", icounter, regs.rip, instr);  

		//If -s switch was added in the command line, the program will wait the ENTER key to continue the program execution
		if(_step_mode){
			printf("Press ENTER key to continue...\n");
			while(getchar()!='\n');
			
		}
	
		//In case a variable is being monitored, PTRACE_PEEKDATA allowed to check if a data value is modified
		//In this case the program will notify the change
		
		if(_check_var){
			new_val = ptrace(PTRACE_PEEKDATA, child_pid, _glob_var, NULL);
			if(old_val != new_val){
				printf("Variable 0x%16x changed its value...\n",_glob_var);
				printf("Old value was %16x\n", old_val);
				printf("New value is %16x\n", new_val);

				old_val = new_val;
			}
		}

		// Make the child execute another instruction 
		if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
			perror("ptrace");
			return;
		}

		// Wait for child to stop on its next instruction 
		wait(&wait_status);
	}

	printf("The program executed %u instructions\n", icounter);
	printf("Program execution finished...\n");


}

//This method will print the autors information
void print_autor(){
	printf("#############################################\n");
	printf("#               Dbugger tool                #\n");
	printf("#############################################\n");
	printf("# Autores: Marco Espinoza Murillo           #\n");
	printf("#          Jose Campos Murillo              #\n");
	printf("#	   Freddy Zeledon Jarquin           #\n");
	printf("# Maestria Sistemas Embebidos               #\n");
	printf("# Instituo Tecnologico de Costa Rica        #\n");
	printf("#############################################\n");
	exit(0);
}


//This method will print the help menu

void print_help(){
	printf("##############################################################\n");
	printf("#                    Dbugger tool                            #\n");
	printf("##############################################################\n");
	printf("# Argumentos validos:                                        #\n");
	printf("#   -a: Muestra la informacion de los autores del programa   #\n");
	printf("#   -h: Muestra este menu de ayuda                           #\n");
	printf("#   -b: Recibe el binario del programa a ejecutar            #\n");
	printf("#   -s: Ejecuta el programa paso a paso                      #\n");
	printf("#   -w: Recibe el nombre de una variable global del programa #\n");
	printf("#       para monitorear los cambios de la misma durante la   #\n");
	printf("#       ejecucion del debugger.                              #\n");
	printf("# Ejemplo de lineas de comando:                              #\n");
	printf("   ./dbugger -b main                                         #\n");
	printf("   ./dbugger -b main -w dummy                                #\n");
	printf("   ./dbugger -b main -w dummy -s                             #\n");
	printf("   ./dbugger -b test1 -w value                               #\n");
	printf("##############################################################\n");
	exit(0);
	
}

