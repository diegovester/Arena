/** LinkedListShifted.c
*
* This code implements a sorted linked list using an array as the underlying data structure.
* The underlying implementation is to be hidden from the end user.  They should interact
* with the linked list using the insertNode(int size) and removeNode(int size) functions.
* The elements in the array are not sorted from element 0 to the end of the array and should
* not be used in that manner.  The internal previous and next elements of the nodes are used
* to traverse the linked list in order.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include "mavalloc.h"

/* The maximum entries in our linked list / array */
#define MAX_LINKED_LIST_SIZE 10000

/* *** INTERNAL USE ONLY *** In an in-line implementation the root node
 * is always 0
 */
#define ROOTNODE 0

 /* *** INTERNAL USE ONLY *** Has the list been initialized */
static int initialized = 0;

/* *** INTERNAL USE ONLY *** Keep track of the last item placed
* so we can optimize the shifting slightly and only shift what is needed.
*/
static int lastUsed = -1;

static enum ALGORITHM gAlgorithm;
static void * gArena;


/**
*
* \struct Node
*
* \brief The node in the linked list
*
* This structure represents the node in our linked list implementation.
* Since this linked list is implemented in an array the previous and next
* members, which would be pointers in a dynamically allocated linked list,
* are integers in this implementation.  Their sizes correspond to the array
* index where the previous and next nodes reside.
*
* The size in_use is to let us track which array entries are currently used.
* If you are re-using this code in a heap or arena assignment this size does
* NOT represent whether your heap block is free or in-use.  You will need to add
* a new member to this struct to track that.
*
* The size element is an integer to demonstrate the functionality of a sorted list.
* If you are reusing this code you will need to change that datatype to be the
* actual data type in your implementation, e.g you're creating a linked list of strings
* then you would change the dataype of size from an int to a char*.
*
*/

enum TYPE {
	P = 0,
	H
};

struct Node
{
	/** If this array entry is being used as a node. 1 for in-use. 0 for empty */
	int  in_use;
	int  size;
	void * arena;
	enum TYPE type;
};

/**
* This array is the linked list we are implementing.  The linked list represented by this array
* is a sorted linked list using the "size" member to determine the sorting.  This array is
* not sorted in the same order as the list and, after multiple insertions and removals, may not
* even be close to the same order.  This is by design.  Rather than sort each element in the array
* every time a new node is added instead the next and previous links are adjusted to link
* the linked list element in the correct spot.
*
* This array is *** INTERNAL USE ONLY *** and should not be exposed to or used by the end user.
* The user will interface with the linked list through insertNode() and removeNode().  From the
* end-user perspective it's a linked list.  We hide that we have implemented the list in an array.
*/
static struct Node LinkedList[MAX_LINKED_LIST_SIZE];

/**
 *
 * \fn findFreeNodeInternal()
 *
 * \brief Find a free array entry  *** INTERNAL USE ONLY ***
 *
 * Search the array to a free entry that we can use
 * to store a new node in the linked list.  Returns
 * the index of the free node or -1 is a node is not found.
 * This function is for  *** INTERNAL USE ONLY ***.  We do not expose
 * the user to the fact that the linked list is in the array.
 * We're also not searching the list to find where this new node will
 * fit in the sorted list.  That is independant of this array.  The insertion
 * into the sorted spot is done in the insertNodeInternal() function after
 * the call to findFreeNodeInternal().
 *
 * \return Array index that is free on success
 * \return -1 on failure
 */

int findFreeNodeInternal()
{
	int i = 0;

	/**
	 *  Start searching the array beginning at the 0th element
	 *  and once we've found an element not in use we'll drop out
	 *  and return the index that is free.
	*/
	for (i = 0; i < MAX_LINKED_LIST_SIZE; i++)
	{
		if (LinkedList[i].in_use == 0)
		{
			return i;
		}
	}
	return -1;
}

/**
 *
 * \fn insertNodeInternal(int previous, int current)
 *
 * \brief Insert a new node in the list *** INTERNAL USE ONLY ***
 *
 * Insert a new node into the linked list.  This function
 * is for *** INTERNAL USE ONLY ***.  The end user of this
 * code should not be aware that their linked list is stored
 * in an array rather than a dynamically allocated linked list
 * with pointers.
 *
 * End Users should call insertNode( int node ) instead
 *
 * This function will insert the node located at "current" right
 * after the node indexed by "previous".
 *
 * We will shuffle every node after this spot one index along the array
 * and make a hole.
 *
 *
 * \param previous  The index of the node that will be
 *                  previous to this node in the list
 *
 *
 * \return 0
 */
int insertNodeInternal(int previous, int size)
{
	int i = 0;

	if (initialized && previous < 0 || previous >= MAX_LINKED_LIST_SIZE)
	{
		printf("ERROR: Tried to insert a node beyond our bounds %d\n", previous);
		return -1;
	}

	/**
	 * On the first node being inserted make sure that
	 * the entire list has been initialized and then
	 * set the initialized flag to we don't do this initialization
	 * again.
	 */
	if (initialized == 0)
	{
		int index = 0;
		for (index = ROOTNODE; index < MAX_LINKED_LIST_SIZE; index++)
		{
			LinkedList[index].in_use = 0;
		}
		initialized = 1;
	}

	/**
	 * Shift everything down and make room for the new node. But we only
	 * need to shift from lastUsed to previous + 1
	*/
	if (lastUsed == -1)
	{
		LinkedList[i].size = size;
		LinkedList[i].in_use = 1;
		lastUsed = i;
	}
	else
	{
		for (i = 10; i >= previous; i--)
		{
			LinkedList[i + 1].size = LinkedList[i].size;
			LinkedList[i + 1].in_use = LinkedList[i].in_use;
		}
	
		LinkedList[previous].in_use = 1;
		LinkedList[previous].size = size;

		lastUsed++;
	}

	return 0;
}

/**
 *
 * \fn removeNodeInternal(int node)
 *
 * \brief Remove a node in the list *** INTERNAL USE ONLY ***
 *
 * Remove a new node from the linked list.  This function
 * is for *** INTERNAL USE ONLY ***.  The end user of this
 * code should not be aware that their linked list is stored
 * in an array rather than a dynamically allocated linked list
 * with pointers.
 *
 * End Users should call remove( int node ) instead.
 *
 * This function will remove the node that is specified by the parameter
 * from the linked list.  This does not remove any data.  Only the next,
 * previous, and in_use flags are updated.
 *
 * \param node The index of the node that will be
 *             removed from the linked list
 *
 * \return 0 on success
 * \return -1 on failure
 */
int removeNodeInternal(int node)
{
	int i;

	/**
	 * Check to make sure we haven't tried to insert a node beyond the bounds of
	 * the array.  This shouldn't ever happen.
	 */
	if (node < 0 || node >= MAX_LINKED_LIST_SIZE)
	{
		printf("ERROR: Can not remove node %d because it is out of our array bounds"
			" of 0 ... %d\n", node, MAX_LINKED_LIST_SIZE);
		return -1;
	}

	/**
	 * And make sure that the node we've been asked to remove is actually one in use
	 */
	if (LinkedList[node].in_use == 0)
	{
		printf("ERROR: Can not remove node %d.  It is not in use\n", node);
		return -1;
	}

	/**
	 * Mark this node as not in-use so we can reuse it if we need to allocate
	 * another node.
	 */
	LinkedList[node].in_use = 0;


	/**
	 * If we have a previous node then hook up the previous nodes next size
	 * to point to our next size. That will cause our node to be snipped out
	 * of the linked list.
	 */
	
	for (i = node; i < lastUsed; i++)
	{
		LinkedList[i].size = LinkedList[i+1].size;

		/* This line could be removed as it's not needed */
		LinkedList[i].in_use = LinkedList[i + 1].in_use;
	}

	

	/**
	 * Return our next and previous to default sizes so this node is ready
	 * be reused
	 */
	LinkedList[lastUsed].size = -1;
	LinkedList[lastUsed].in_use = 0;

	lastUsed = lastUsed - 1;


	return 0;
}


/**
 *
 * \fn removeNode(int node)
 *
 * \brief Remove a node from the linked list that contains the given size
 *
 * This function will remove the node that is specified by the parameter
 * from the linked list.  This does not remove any data.  Only the next,
 * previous, and in_use flags are updated.
 *
 * \param size The size of the node that will be
 *              removed from the linked list
 *
 * \return 0 on success
 * \return -1 on failure
 */
int removeNode(int size)
{
	/**
	  * We start searching for the node to remove at the root of the linked list
	  */
	int index = ROOTNODE;

	/**
	 * Iterate over the linked list and find the node
	 * containing the size we are looking to remove
	 */
	while (index != -1)
	{
		/**
		 * Once we've found the node to delete then
		 * remove it.
		 */
		if (LinkedList[index].size == size)
		{
			return removeNodeInternal(index);
		}
		index++; 
	}
	return -1;
}


/**
 *
 * \fn insertNode(int size)
 *
 * \brief Insert a node into the linked list that contains the given size
 *
 * This function will insert a node with a size that is specified by the parameter
 * into the linked list.
 *
 * \param size The size of the node that will be
 *              inserted into the linked list
 *
 * \return 0 on success
 * \return -1 on failure
 */
int insertNode(int size)
{
	/*  Hold the index of the node we will insert behind */
	int previous = -1;

	/* Return size 0 for success. -1 for failure. */
	int ret = -1;

	/* Loop variable */
	int i;

	/**
	 * Since this list is sorted, iterate over the linked list and find which node we  would
	 * fit behind with our size.  Once we have found a spot then the while loop will exit
	 * and previous will have the index of the node we will insert behind.
	 */
	for (i = ROOTNODE; i < MAX_LINKED_LIST_SIZE; i++)
	{
		if ((LinkedList[i].in_use && LinkedList[i].size > size) ||
			(LinkedList[i].in_use == 0))
		{
			previous = i;
			break;
		}
	}

	/** If we found a free node and we haven't run off the end of the list */
	if (previous >= -1)
	{
		ret = insertNodeInternal(previous, size);
	}
	else if (previous >= MAX_LINKED_LIST_SIZE || previous < 0)
	{
		printf("Error: Tried to insert beyond the bounds of the allocated list.\n");

	}
	return ret;
}

/**
 *
 * \fn printList()
 *
 * \brief Print the linked list
 *
 * This function iterates over the list and prints the sizes in list order.
 * If you need a function to iterate over the linked list in order, this is
 * how you would do that.
 *
 * Since the array is kept in sorted order, iterating in order over the array
 * iterates in order.
 *
 */
void printList()
{
	/** Start at the root of the linked list */
	int i = 0;

	/** Iterate over the linked list in node order and print the nodes. */

	for (i = ROOTNODE; i < MAX_LINKED_LIST_SIZE; i++)
	{
		if (LinkedList[i].in_use == 0)
		{
			/** This array is not sparse so the first entry which has a 0 size for
			 *  in-use means we've reached the end so no need to continue the loop
			 */
			break;
		}
		printf("LinkedList[%d]: %d\n", i, LinkedList[i].size);
	}
}



int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
  printf("\n----mavalloc_init----\n");
  // initialize the linked list
  int i = 0;
  for( i = 0; i < MAX_LINKED_LIST_SIZE; i++)
  {
    LinkedList[i].size = 0;
    LinkedList[i].in_use = 0;
    LinkedList[i].arena = 0;
    LinkedList[i].type = H;
  }
  // allocate the pool
  gArena = malloc( ALIGN4( size ) );
  printf("\ngArena = %p", gArena);
  printf("%p\n", gArena);

  // save the algorithm type
  gAlgorithm = algorithm;

  // set the first entry to point to the area
  LinkedList[0].in_use = 1;
  LinkedList[0].size = ALIGN4( size );
  LinkedList[0].type = H;
  LinkedList[0].arena = gArena;

  

  
  return 0;
}

void mavalloc_destroy( )
{
  int i = 0;
  for( i = 0; i < MAX_LINKED_LIST_SIZE; i++)
  {
    LinkedList[i].size = 0;
    LinkedList[i].in_use = 0;
    LinkedList[i].arena = 0;
    LinkedList[i].type = H;
  }

  free( gArena );
  return;
}

// initialize global variable for NEXT_FIT
// will remember where the last search ended off on
int previously_allocated_hole = 0;

// initialize leftover_size globally
int leftover_size = 0;
void * mavalloc_alloc( size_t size )
{
  // allocate size
  void * ptr = NULL;
  
  if( gAlgorithm == FIRST_FIT )
  {
    printf("\nFIRST_FIT\n");
    // Allocate the first hole that is big enough
    // the first search starts at the beginning of the list
    // Allocate the first hole that is big enough
    // start at the beginning of the list
    int i = 0;
    // if the node type is hole and in_use and size < node size
    for( i = 0; i < MAX_LINKED_LIST_SIZE; i++ )
    {
      if( LinkedList[i].type == H && LinkedList[i].in_use && size < LinkedList[i].size )
      {
        leftover_size = LinkedList[i].size - size;

        // Calculate the new arena pointer
        void * arena = (void*)((long int)LinkedList[i].arena + (long int)size);
      //    if there is leftover size then insert a new node as
      //      a hole that holds that leftover space
      //      insertNode( leftover_size, arena );
        if(leftover_size > 0)
        {
          insertNode( size );
          //    then return that node arena pointer
          LinkedList[i].type = P;
          LinkedList[i].arena = arena;

          LinkedList[i+1].in_use = 1;
          LinkedList[i+1].size = leftover_size;
          LinkedList[i+1].type = H;

          


          return LinkedList[i].arena;
        }
      //
      
      }
    }
  }
  // start search from last allocated hole
    // resume from the point of the list that the last search ended on
  else if( gAlgorithm == NEXT_FIT )
  {
    printf("\nNEXT_FIT\n");
    // Allocate the first hole that is big enough
    // start at the beginning of the list
    //previously_allocated_hole
      // initialized globally above mavalloc_alloc()
    // if the node type is hole and in_use and size < node size
    for( int i = previously_allocated_hole; i < MAX_LINKED_LIST_SIZE; i++ )
    {
      if( LinkedList[i].type == H && LinkedList[i].in_use && size < LinkedList[i].size )
      {
        leftover_size = LinkedList[i].size - size;

        // Calculate the new arena pointer
        //unsigned char * arena = (unsigned char *)LinkedList[i].arena + (unsigned char *)size;
      //    if there is leftover size then insert a new node as
      //      a hole that holds that leftover space
      //      insertNode( leftover_size, arena );
        if(leftover_size > 0)
        {
          previously_allocated_hole = i;
          insertNode( size );
          //    then return that node arena pointer
          LinkedList[i].type = P;
          return LinkedList[i].arena;
        }
      //
      
      }
    }
  }
    
  else if( gAlgorithm == BEST_FIT )
  {
    printf("\nBEST_FIT\n");
    // allocate the smallest hole that is big enough
    // start at the beginning of th list
    int i = 0;
    // marker to find the smallest hole
    int smallest_hole = 0;
    // tracker of previously smallest hole
    // initialize with the first linked list node size as a base line for worst case scenario
    int previously_smallest_leftover_size = LinkedList[0].size;
    // if the node type is hole and in_use and size < node size
    for( i = 0; i < MAX_LINKED_LIST_SIZE; i++ )
    {
      //printf("\nif( LinkedList[i].type == H && LinkedList[i].in_use && size < LinkedList[i].size )");
      if( LinkedList[i].type == H && LinkedList[i].in_use && size < LinkedList[i].size )
      {
        //calculate if the hole is big enough
        leftover_size = LinkedList[i].size - size;
        // compare the size of the hole to the previous hole
        // if the leftover_size is smaller than the previously smallest leftover_size
          // then no longer consider the previous hole
        
        if(leftover_size < previously_smallest_leftover_size)
        {
          smallest_hole = i;
          previously_smallest_leftover_size = leftover_size;
          
        }
        // find the smallest hole in the list
        // then run the list again to enter the hole
      }
    }

    // if the node type is hole and in_use and size < node size
    for( i = 0; i < MAX_LINKED_LIST_SIZE; i++ )
    {
      //printf("\nif( LinkedList[i].type == H && LinkedList[i].in_use && size < LinkedList[i].size )");
      if( LinkedList[i].type == H && LinkedList[i].in_use && size < LinkedList[i].size )
      {
        // Calculate the new arena pointer
        void * arena = (void*)((long int)LinkedList[i].arena + (long int)size);
        // find the smallest hole in the list
        // then run the list again to enter the hole
        if(smallest_hole == i)
        {
          insertNode( size );
          //    then return that node arena pointer
          LinkedList[i].type = P;
          LinkedList[i].arena = arena;

          LinkedList[i+1].in_use = 1;
          LinkedList[i+1].size = leftover_size;
          LinkedList[i+1].type = H;
          return LinkedList[i].arena;
        }
      }
    }
  }

  else if( gAlgorithm == WORST_FIT )
  {
    printf("\nWORST_FIT\n");
    // allocate the smallest hole that is big enough
    // start at the beginning of th list
    int i = 0;

    // marker to find the largest hole
    int largest_hole = 0;

    // tracker of previously largest hole
    int previously_largest_leftover_size = 0;

    // if the node type is hole and in_use and size < node size
    for( i = 0; i < MAX_LINKED_LIST_SIZE; i++ )
    {
      if( LinkedList[i].type == H && LinkedList[i].in_use && size < LinkedList[i].size )
      {
        //calculate if the hole is big enough
        leftover_size = LinkedList[i].size - size;

        // Calculate the new arena pointer
        //unsigned char * arena = (unsigned char *)LinkedList[i].arena + (unsigned char *)size;
        // compare the size of the hole to the previous hole
        // if the leftover_size is larger than the previously largest leftover_size
          // then no longer consider the previous hole
        if(leftover_size > previously_largest_leftover_size)
        {
          largest_hole = i;
          previously_largest_leftover_size = leftover_size;
        }
        // find the largest hole in the list
        // then run the list again to enter the hole
      }
    }

    // if the node type is hole and in_use and size < node size
    for( i = 0; i < MAX_LINKED_LIST_SIZE; i++ )
    {
      if( LinkedList[i].type == H && LinkedList[i].in_use && size < LinkedList[i].size )
      {
        // find the largest hole in the list
        // then run the list again to enter the hole
        if(largest_hole == i)
        {
          insertNode( size );
          //    then return that node arena pointer
          LinkedList[i].type = P;
          return LinkedList[i].arena;
        }
      //

      }
    }
  }
  

  // only return NULL on failure
  return NULL;
}

void mavalloc_free( void * ptr )
{
  
  // search for the node containing the value given by ptr
  // set that node to be a type H
  int i;
  for( i = 0; i < MAX_LINKED_LIST_SIZE; i ++)
  {
    if( ptr == LinkedList[i].arena )
    {
      printf("\nLinkedList[%d].type = H", i);
      LinkedList[i].type = H;
      //return;
    }
  }
  // check if adjacent nodes are free
  // if they are, then combine them
  for( i = 0; i < MAX_LINKED_LIST_SIZE-1; i ++)
  {
    if( ptr == LinkedList[i].arena )
    {
      if( LinkedList[i].type == H && LinkedList[i+1].type == H )
      {
        // combine the sizes into LinkedList[i]
        // remove LinkedList[i+1]
        //printf("\nremoveNode(LinkedList[%d+1].size", i);
        
        LinkedList[i].size = LinkedList[i].size + LinkedList[i+1].size;
        removeNode(LinkedList[i+1].size);
      }
    }
  }
  
  
  return;
}

int mavalloc_size( )
{
  
  int number_of_nodes = 0;
  //count number of nodes?
  int i = 0;
  for( i = 0; i < MAX_LINKED_LIST_SIZE; i++ )
  {
    if( LinkedList[i].in_use )
    {
      number_of_nodes ++;
    }
  }
  return number_of_nodes;
}
