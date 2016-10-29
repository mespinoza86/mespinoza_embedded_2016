#include <stdlib.h>
#include <stdio.h>
int dummy;
void modify_dummy(int *a, int value)
{
 *a = value;
}
int main (int argc, char** argv )
{
 printf("Starting %s\n", __FILE__);
 dummy = 0;
 printf("Starting example with dummy = %i\n", dummy);
 modify_dummy(&dummy, 5);
 printf("Changing dummy = %i\n", dummy);
 modify_dummy(&dummy, 10);
 printf("Exiting example with dummy = %i\n", dummy);
 return 0;
}
