#include <stdio.h>
#include <stdlib.h>

/* First let's create a new struct with just one member.
   For more see: 

   http://www.tutorialspoint.com/cprogramming/c_structures.htm

*/
struct item_s {
  int index;
};

int main (int argc, char** argv)
{
  /* Let's create some pointers, recall that pointers do not 
     point to any valid memory region when created, we need
     to point them to a valid address 
     

     For more about pointers see: 

     http://www.tutorialspoint.com/cprogramming/c_pointers.htm
  */

  int *a = NULL; // Pointer to an integer
  struct item_s *item = NULL; // Pointer to out struct

  /* At start the pointers do not point to any valid address */
  printf("Pointers are pointing to nothing:\n\n\t a->%p\n\titem->%p\n",a,item);

  /* Let's allocate memory for each type of variable and assign it to each pointer.

     For more about dynamic memory allocation:

     http://www.programiz.com/c-programming/c-dynamic-memory-allocation

     http://fresh2refresh.com/c-programming/c-dynamic-memory-allocation/

     malloc returns void* which means the pointer does not have an specific type,
     we need to cast it to the type of our pointer to avoid compiler warnings.

     For more about casting see:

     https://www.tutorialspoint.com/cprogramming/c_type_casting.htm
  */
  a = (int *)malloc(sizeof(int));

  /* Now a points  to a memory address for an integer variable */
  printf("\nPointer a->%p\n", a);

  /* But its value is unkown (sometimes 0 or whatever the memory region had)
     To initialize it to zero use cmalloc.
  */
  printf("Value for a = %i\n", *a);

  /* Let's try the same for our struct */
  item = (struct item_s *)malloc(sizeof(struct item_s));

  printf("\nPointer item->%p\n", item);

  /* And now we can access the struct members */
  item->index = 15;
  printf("Value of item->index = %i\n", item->index);

  /* Now, NEVER forget to free the memory you allocated */
  free(a);
  free(item);
  
  return EXIT_SUCCESS;
}
