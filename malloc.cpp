#include <unistd.h> // sbrk( )
#include <limits.h> // LONG_MAX

using namespace std;

static bool initialized = false;
static void *heap_top; // the beginning of the heap space
static void *heap_end; // the current boundary of the heap space, obtained from sbrk( 0 )

class MCB { // memory control block
public:
  int available; // true(1): this memory partition is available, false(0) unavailalbe.
  int size;      // MCB size + the user data size
};

void free_( void *dealloc_space ) {
  MCB *mcb;

  // locate this partition's mcb address from dealloc_space
  //  Task 4: implement by yourself (just in one line)
  mcb = (MCB*)((unsigned long long)dealloc_space - sizeof(MCB));
  mcb->available = true;
  return;
}

void *malloc_f( long size ) {
  struct MCB *cur_mcb;          // current MCB
  void *new_space = NULL; // this is a pointer to a new memory space allocated for a user

  if( !initialized )   {
    // find the end of heap memory, upon an initialization
    heap_end = sbrk( 0 );
    heap_top = heap_end;
    initialized = true;
  }

  // append an MCB in front of a requested memroy space
  size = size + sizeof( MCB );

  // scan each mcb from the top to the bottom of the heap
    for (void *cur = heap_top; cur < heap_end; cur = (void *)((unsigned long long)cur + cur_mcb->size)) {
        //   let cur_mcb point to each mcb you are scanning
        cur_mcb = (MCB*) cur;
        //   if cur_mcb->available and cur_mcb->size fits size
        if (cur_mcb->available && cur_mcb->size >= size) {
            // new_space points to this mcb
            new_space = cur;
            // todo remove if needed
            cur_mcb->available = false; // marks as used
            break;
        }
        // cur = (void*)((unsigned long long) cur + cur_mcb->size);
    }

  // no space found yet
  if ( new_space == NULL ) {
      new_space = heap_end;
      heap_end = sbrk(size);

      // initializes new MCB
      cur_mcb = (MCB*)new_space;
      // todo change to 0 if needed
      cur_mcb->available = false;
      cur_mcb->size = size; // stores size
  }

  // new space is after new MCB
  return (void *)( ( long long int )new_space + sizeof( MCB ) );
}

void *malloc_b( long size ) {
  struct MCB *cur_mcb;          // current MCB
  void *new_space = NULL; // this is a pointer to a new memory space allocated for a user
  // added this struct
  struct  MCB *best_mcb = NULL;

    if( !initialized )   {
    // find the end of heap memory, upon an initialization
    heap_end = sbrk( 0 );
    heap_top = heap_end;
    initialized = true;
  }

  // append an MCB in front of a requested memroy space
  size = size + sizeof( MCB );
  long best_size_sofar = LONG_MAX;

  // scan each mcb from top to bottom of heap
    for (void *cur = heap_top; cur < heap_end; cur = (void *)((unsigned long long)cur + cur_mcb->size)) {
        // let current mcb point to mcb we are scanning
        cur_mcb = (MCB*) cur;
      //   if cur_mcb->available and cur_mcb->size fits size and cur_mcb->size is the best size so far
      if (cur_mcb->available &&
                cur_mcb->size >= size &&
                cur_mcb->size < best_size_sofar) {
          //  temporarily memorize this best size so far and this best mcb so far
          best_size_sofar = cur_mcb->size;
          // After scan, check the best mcb so far. If it is not null
          //  new_space points to this best mcb so rar
          // new_space = cur;
          best_mcb = cur_mcb;
      }
    }

    // todo remove if needed
    // if found best fit, mark as used so no mem reuse
    if (best_mcb != NULL) {
        best_mcb->available = false;
        new_space = (void*)best_mcb;

    }
  // no space found yet? Request OS memory
  if ( new_space == NULL ) {
      new_space = heap_end;
      heap_end = sbrk(size);

      // initializes new MCB
      cur_mcb = (MCB*)new_space;
      // todo change to 0 if needed
      cur_mcb->available = false;
      cur_mcb->size = size; // stores size
  }

  // new space is after new MCB
  return (void *)( ( long long int )new_space + sizeof( MCB ) );
}
