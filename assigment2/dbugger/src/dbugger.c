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

//Declaring methods which will be used during the program execution
void run_target(const char* programname);
void run_debugger(pid_t child_pid, int _step_mode, int _check_var, int _glob_var);
void procmsg(const char* format, ...);

//Main method
//This is the method in charge to execute the program
//It will call all the methods required to execute the application

int main(int argc, char** argv)
{

//Variable declaration
	char* program = NULL;
	int bin_file=0;
	int step_mode=0;
	int check_var=0;
	char* s_var_addr = NULL;
	unsigned int var_addr;
	int c;

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
			//	print_autor();
				break;
			case 'b':
			//'b' It will have the path for the binary file to be executed
				program = optarg;
				bin_file = 1;
				break;			
			case 'h':
			//'h' is used to print the help menu
			//	print_help();
				break;
			case 'w':
			//'w' has the address which will be monitored during the test execution
				s_var_addr = optarg;
				sscanf(s_var_addr,"%08x",&var_addr);
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
	The following if will check if this argument was added in the command line, and if the 
	argument was not added, the program will fail.	
	*/

	if(!bin_file){
		fprintf(stderr, "-Error- Argument -b <binary file> is required\n");
		exit(0);
	}else{
		fprintf(stderr, "-Info- Program %s will be executed with the debugger\n", program);
	}

	/*
	-w argument is optional, it will have the address for the global var which will be 
	monitored during the binary execution.
	The following if is used to print if a global var will be checked in this execution
	*/

	if(check_var){
		printf("-Info- Address to monitor during execution is: 0x%08x\n", var_addr);
	}else{
		printf("-Info- Warning... No global variable will be monitored\n");
	}	

	/*
	The following fork is used to execute the binary file through the debugger
	*/
	pid_t child_pid;

	child_pid = fork();
	if (child_pid == 0)
		/*
		run_target method is used to execute the binary file
		but using PTRACE_TRACEME, it will tell the kernel 
		the process is being traced		
		*/
		run_target(program);
	else if (child_pid > 0)
		/*
		When the process finish the execution, the method run_debugger is executed
		to get all the information required for our program
		This is the method in charge to get the RIPs executed, the values for the global vars
		and if the program will be executed by steps.
		*/
		run_debugger(child_pid, step_mode,check_var, var_addr);
	else {
		perror("fork");
		return -1;
	}

	return 0;
}

/*
 As you can see in the example, a process forks a child and the child executes the process we want to trace. Before running exec, the child calls ptrace with the first argument, equal to PTRACE_TRACEME. This tells the kernel that the process is being traced, and when the child executes the execve system call, it hands over control to its parent. The parent waits for notification from the kernel with a wait() call. Then the parent can check the arguments of the system call or do other things, such as looking into the registers.

When the system call occurs, the kernel saves the original contents of the eax register, which contains the system call number. We can read this value from child's USER segment by calling ptrace with the first argument PTRACE_PEEKUSER, shown as above.

*/

void procmsg(const char* format, ...)
{
    va_list ap;
    fprintf(stdout, "[%d] ", getpid());
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
}



void run_target(const char* programname)
{
    procmsg("target started. will run '%s'\n", programname);

    // Allow tracing of this process /
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
        perror("ptrace");
        return;
    }

    // Replace this process's image with the given program 
    execl(programname, programname, 0);
}




void run_debugger(pid_t child_pid, int _step_mode, int _check_var, int _glob_var)
{

	int wait_status;
	long new_val=0;
	long old_val=0;

	unsigned icounter = 0;
	procmsg("debugger started\n");

	// Wait for child to stop on its first instruction 
	wait(&wait_status);

	while (WIFSTOPPED(wait_status)) {
		icounter++;
		struct user_regs_struct regs;
		ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
		unsigned instr = ptrace(PTRACE_PEEKTEXT, child_pid, regs.rip, 0);
	       // procmsg("icounter = %u.  EIP = 0x%08x.  instr = 0x%08x\n", icounter, regs.rip, instr);
		procmsg("PC = 0x%08x.  instr = 0x%08x\n", regs.rip, instr);
		if(_step_mode){
			procmsg("Press ENTER key to continue ...");
			while(getchar()!='\n');
			
		}
	
		if(_check_var){
			new_val = ptrace(PTRACE_PEEKDATA, child_pid, _glob_var, NULL);
			if(old_val != new_val){
				printf("Variable 0x%08x changed its value...\n",_glob_var);
				procmsg("Old value was %08x\n", old_val);
				procmsg("New value is %08x\n", new_val);
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

//	procmsg("the child executed %u instructions\n", icounter);

}

