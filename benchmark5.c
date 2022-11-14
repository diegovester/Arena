#include "malloc.h"

int main( int argc, char * argv[] )
{
  unsigned char* array [ 10000];
  //malloc
  // not enough
  // we have to do more to see a difference

  //instead of calling our function, we call system call
  // compare with time ./benchmark1
      //etc

  int i = 0;
  for( i = 0; i < 1000; i ++)
  {
    array[i] = malloc( 100 );
  }

  for( i = 0; i < 1000; i ++)
  {
    free( array[i] );
  }

  char * ptr = malloc( 10000 );
  char * ptr1 = malloc( 10000 );
  char * ptr2 = malloc( 10000 );
  char * ptr3 = malloc( 10000 );

  free( ptr1 );
  free( ptr3 );

  char * ptr4 = malloc( 500 );
  char * ptr5 = malloc( 500 );
  return 0;
}
