#include <stdint.h>
 
//Versatile struct created to manipulate the UART port
typedef volatile struct {
  uint32_t DR;              //Data register used to transmit and receive from UART
  uint32_t RSR_ECR;
  uint8_t reserved1[0x10];
  const uint32_t FR;        //FIFO control register
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
 
pl011_T * const UART0 = (pl011_T *)0x101f1000;  //Address for UART0 DR register
 
unsigned int op_done = 0;               //Flag to indicate when an operation was completed
unsigned int mul_found = 0;             //Flag to indicate that operand is a multiplication
unsigned int add_found = 0;             //Flag to indicate that operand is an addition
unsigned int first_num_size = 0;        //Size of first number of the operation in terms of amount of characters
unsigned int second_num_size = 0;       //Size of second number of the operation in terms of amount of characters
unsigned int first_num_complete = 0;    //Flag to indicate that the first operand was received correctly
unsigned int second_num_complete = 0;   //Flag to indicate that the second operand was received correctly
unsigned int result = 0;                //Integer to temporarily store the result 
unsigned int number_size = 0;           //Result number size
 
char first_num_buff[2];                 //Buffer to temporally store the first number as characters
char second_num_buff[2];                //Buffer to temporally store the second number as characters
 
//Function to convert from positive integer of max size 9999 to a char*
char *int_to_string(int number) {
  int i;
  char *number_string;
   
  //Calculate the size of the number in terms of characters
  //Ideally it should be calculated by using log10
  if(number >= 1000) {number_size = 3;}
  else if(number >= 100) {number_size = 2;}
  else if(number >= 10) {number_size = 1;}
  else {number_size = 0;}
   
  //Conversion of the number to char*
  for (i = 3; i >= 0; --i, number /= 10) {
    if(3 - i <= number_size){
      number_string[i] = number % 10 + '0'; //An offset of 48 = '0' needs to be added to convert each number to ascii
    } else {
      number_string[i] = '@';
    }
  }
  return (char *)number_string;
}
 
//Function to print a string in UART0 (console)
void print_uart0(const char *s) {
  while(*s != '\0') { /* Loop until end of string */
    if(*s != '@') {
      UART0->DR = (unsigned int)(*s); /* Transmit char */
    }
    s++; /* Next char */
  }
}
 
void print_uart0_binary(const char *s) {
  while(*s != '\0') { /* Loop until end of string */
    int tmp = (unsigned int)(*s);
    int i;
    for (i = 0; i < 16; i++) {
      UART0->DR = !!((tmp << i) & 0x8000) + 48;
    }
    s++; /* Next char */
    UART0->DR = '\n';
  }
}
 
//Function to perform multiplication or addition of two integers in form of characters 
static void operation() {
 
  //Addition
  //Depending of the size of the two operands, it will be needed to adjust the addition
  if((add_found == 1) && (first_num_size == 2) && (second_num_size == 2)) {
    result = (first_num_buff[0] + second_num_buff[0])*10 + first_num_buff[1] + second_num_buff[1]-1056;
  } else if((add_found == 1) && (first_num_size == 2) && (second_num_size == 1)) {
    result = (first_num_buff[0])*10 + first_num_buff[1] + second_num_buff[0]-576;
  } else if((add_found == 1) && (first_num_size == 1) && (second_num_size == 2)) {
    result = first_num_buff[0] + (second_num_buff[0])*10 + second_num_buff[1]-576;
  } else if((add_found == 1) && (first_num_size == 1) && (second_num_size == 1)) {
    result = first_num_buff[0] + second_num_buff[0]-96;
  }
   
  //Multiplication
  //If and else statements to adjust the operation depending of number of characters in umber 1 or 2. 
  else if((mul_found == 1) && (first_num_size == 2) && (second_num_size == 2)) {
    result = ((first_num_buff[0])*10 + first_num_buff[1]-528) * ((second_num_buff[0])*10 + second_num_buff[1]-528);
  } else if((mul_found == 1) && (first_num_size == 2) && (second_num_size == 1)) {
    result = ((first_num_buff[0])*10 + first_num_buff[1]-528) * (second_num_buff[0]-'0');
  } else if((mul_found == 1) && (first_num_size == 1) && (second_num_size == 2)) {
    result = (first_num_buff[0]-'0') * ((second_num_buff[0])*10 + second_num_buff[1]-528);
  } else if((mul_found == 1) && (first_num_size == 1) && (second_num_size == 1)) {
    result = (first_num_buff[0]-'0') * (second_num_buff[0]-'0');
  }
   
  //Operation was completed
  op_done = 1;
}
 
//Function to validate the upcoming character depending of operations supported (multiplication or addition of two character numbers)
static inline char fill_operation(char c) {
  if(c == 13) { //If the user press "Enter"
    //Executes the operation
    operation();
    return '\n';
  }else {
    return c;
  }
}
 
//Function for correctly receive and transmit from/to UART
static void uart_echo() {
  if ((UART0->FR & RXFE) == 0) {
    while(UART0->FR & TXFF);
    //Every received character that matches get_valid_operation() requirements will be directly echoed to the console
    UART0->DR = get_valid_operation(UART0->DR);
  }
}
 
//App's main function 
void c_entry() {
  //Infitine loop to perform as many operations as the user want
  while(1){
   
    print_uart0("\nWhat operation do you want to perform?\n");
 
    //This section collects and validate the operation to perform
    while(op_done == 0){
      uart_echo(UART0);
    }
     
    //Print final result in console
    print_uart0("Result = ");
    print_uart0(int_to_string(result));
    print_uart0("\n");
     
    //Restart parameters to perform a new operation
    op_done = 0;
    mul_found = 0;
    add_found = 0;
    first_num_complete = 0;
    second_num_complete = 0;
    first_num_size = 0;
    second_num_size = 0;
    number_size = 0;
    first_num_buff[0] = '0';
    first_num_buff[1] = '0';
    second_num_buff[0] = '0';
    second_num_buff[1] = '0';
    result = 0;
  }
}
