#include <stdio.h>   // printf( )
#include <stdlib.h>  // rand( )
#include <iostream>  // cout

#define MAX_MEMSIZE 1024
#define N_CHUNKS 10 //  number of separate memory chunks to manage at once

extern void *malloc_f( long ); // first fit allocation
extern void *malloc_b( long ); // best fir allocation
extern void free_( void* );

using namespace std;

// control behavior
char print = 'n'; // Default: no printing unless user passes p
char libc = 'l';  // Default: use Linux's built-in malloc (libc)

// rand_alloc(): randomly chooses a size (1–1024 bytes),
//    then allocates that memory using:
// - libc malloc()      libc == 'l'
// - custom malloc_f()  libc == 'f'
// - custom malloc_b()  libc == 'b'
void *rand_alloc( ) {
  int memsize = rand( ) % MAX_MEMSIZE;
  memsize = ( memsize == 0 ) ? 1 : memsize;
    // If print mode is on, print the size being allocated
  if ( print == 'p' ) printf( "rand_alloc: size = %d ", memsize );
  // 'l' = Linux malloc, 'f' = first-fit malloc, 'b' = best-fit malloc
  return ( libc == 'l' ) ? malloc( memsize ) : 
    ( ( libc == 'f' ) ? malloc_f( memsize ) : malloc_b( memsize ) );
}

//  - Reads command line args to choose allocator type and print mode
//  - Allocates an array of N_CHUNKS random memory blocks
//  - Randomly frees and reallocates them many times to simulate real use
int main( int argc, char **argv ) {
    // Check the arg user passed ('l', 'f', or 'b')
  if ( argc >= 2 )
    libc = argv[1][0];
  if ( argc >= 3 )
    print = argv[2][0];

  void *chunks[N_CHUNKS];

  // allocate all chuncks
  for ( int i = 0; i < N_CHUNKS; i++ ) {
    chunks[i] = rand_alloc( );
    if ( print == 'p' ) printf( "chuncks[%d] = %p allocated\n", i, chunks[i] );
  }

  // random de/re-allcation
  for ( int i = 0; i < N_CHUNKS * 10; i++ ) {
    int target = rand( ) % N_CHUNKS;
    int alloc_free = rand( ) % 2;
      // Case 1: Allocate (if currently empty)
    if ( alloc_free == 1 && chunks[target] == NULL ) {
      // re-allocation
      chunks[target] = rand_alloc( );
      if ( print == 'p' ) printf( "chuncks[%d] = %p allocated\n", target, chunks[target] );
    }
    // Case 2: Free (if currently allocated)
    else if ( alloc_free == 0 && chunks[target] != NULL )  {
      // de-allocation
      ( libc == 'l' ) ? free( chunks[target] ) : free_( chunks[target] );
      if ( print == 'p' ) printf( "chuncks[%d] = %p freed\n", target, chunks[target] );
      chunks[target] = NULL;
    }
  }

  return 0;
}
