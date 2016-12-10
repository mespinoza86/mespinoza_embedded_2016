#include <stdint.h>

typedef volatile struct {
  uint32_t DR;
  uint32_t RSR_ECR;
  uint8_t reserved1[0x10];
  const uint32_t FR;
  uint8_t reserved2[0x4];
  uint32_t LPR;
  uint32_t IBRD;
  uint32_t FBRD;
  uint32_t LCR_H;
  uint32_t CR;
  uint32_t IFLS;
  uint32_t IMSC;
  const uint32_t RIS;
  const uint32_t MIS;
  uint32_t ICR;
  uint32_t DMACR;
} pl011_T;
 
enum {
  RXFE = 0x10,
  TXFF = 0x20,
};

pl011_T * const UART0 = (pl011_T *)0x101f1000;

/*
This method will convert an integer variable to array
*/
void my_itoa(int n, char *s , int base) {
    int i;
	int number = n;
	char extracted_number = 0;
    char *number_string;
	char tmp;
   //clear_array(tmp_array);
   int iterations = 0;
	while(number != 0){
		s[iterations] = number % 10 + '0';
		//print_uart0( s[iterations] );
		number = number / 10;
		iterations++;
	}

    //The following is to have the characters in the correct order
	
	if(iterations==2){	
	  tmp = s[0];
	  s[0] = s[1];
     	  s[1] = tmp;
	}else if (iterations==0){
	  print_uart0("-Error- You must specify a number\n");
	}else if (iterations==3){
  	  tmp = s[0];
	  s[0] = s[2];
     	  s[2] = tmp;		
	}else if (iterations==4){
	  tmp = s[0];
          s[0] = s[3];
          s[3] = tmp;
	  tmp = s[1];
          s[1] = s[2];
          s[2] = tmp;
	}else if (iterations>=5){
  	print_uart0("-Error- Overflow result\n -Error- A maximium of 4 digits are allowed in the result\n");
  
	}

	s[iterations] = '\0';
}

/*
The following method will convert an array to integer
*/
int my_atoi(const char* str){
	int n = 0;
	int i = 0;
	while( str[i] != '\0'){
		n = n*10 + (str[i]  - '0');
		i++;
	}

	return n;
}

 
/*
This method will print a label through the UART0
*/
void print_uart0(const char *s) {
  while(*s != '\0') { 
    if(*s != '@') {
      UART0->DR = (unsigned int)(*s); //print the current character until we get the \0
    }
    s++; 
  }
}

char buffer[25];
char received_string[25];
int count = 0;

unsigned int done_receiving_parameter = 0;

/*
This method will capture data from the uart and will echo it back to the same UART
It will loop until the 13(enter) is received.
*/

static uart_echo() {
	int i = 0;
	char *my_char;
	do{
	  	if ((UART0->FR & RXFE) == 0) {
			while(UART0->FR & TXFF);
			my_char =  UART0->DR;
			if(my_char != 13){ ;
			
				buffer[i] = my_char;
				i++;
			}else{
				done_receiving_parameter = 1;
			}
			UART0->DR = my_char;
		}
	}while(my_char != 13);
	my_char = 0;
	i = 0;
}
 
/*
To avoid garbage data
*/
void clear_array(char *s){
	int i = 0;
	for(i = 0; i < 25 ; i ++) 
		s[i] = '\0';
}

/*
It will calculate the result of each operation
*/
int process_operand(int first_op, int second_op, char op){

	int result;

	switch (op) {
	
		case ('*'):
			result = first_op * second_op;
			return result;
		
		case ('+'):
			result = first_op + second_op;
			return result;

	        default :
			 print_uart0("\n-E- Incorrect operand\n");
			return 0;
		}		
}

/*
main function of the program

*/
void c_entry() {
  int first_operand = 0;
  char operation = 0;
  int result;
  int second_operand = 0;

  
  while(1){
  

	/*
	Get first operand, convert it to int and store it. Then clear the buffer for next usage
	*/
 	print_uart0("\nType the first operand\n");
  	uart_echo(UART0);
   	print_uart0("\n");
	first_operand = my_atoi(buffer);
	my_itoa(first_operand, buffer, 10);
	clear_array(buffer);
   
	/*
	Get operation character and store it. Then clear the buffer for next usage
	*/

        print_uart0("\nType the operation (* +)\n");
        uart_echo(UART0);
        print_uart0("\n");
	operation = buffer[0];
	print_uart0("\n");
	clear_array(buffer);	
 
	/*
	Get second operand, convert it to int and store it. Then clear the buffer for next usage
	*/
	print_uart0("\nType the second operand\n");
        uart_echo(UART0);
        print_uart0("\n");
	second_operand = my_atoi(buffer);
	clear_array(buffer);


	/*
	Calculate the result and print it 
	*/

	result = process_operand(first_operand, second_operand, operation);	
        done_receiving_parameter = 0;
        my_itoa(result, buffer, 10);   
        print_uart0("Result = ");
        print_uart0(buffer);
	clear_array(buffer);

        print_uart0("\n");
  }
}
