/*
   =============================================================================
	memory.c - memory allocation functions for GEMDOS
	Version 4 -- 1988-05-03 -- Beckemeyer (original) / Lynxcairne (mods)

	These routines implement the Unix standard C runtime
	library routines malloc(), free(), and realloc(),
	but are NOT compatible with intermixed brk() and sbrk() calls.

	The routines manage "heaps" allocated from the system.
	Each heap is carved up into some number of user memory
	blocks, as requested by malloc() and realloc() calls.

	As blocks are returned with free() calls, they are merged
	with any neighboring blocks that are free. Un-mergable
	blocks are stored on a doubly linked list.

	As heaps become full, new ones are created. The list of
	heaps is a singly linked list.  Heaps are returned to the
	system during garbage collection, which occurs whenever
	the current set of heaps cannot fill a memory request.

	This scheme avoids GEMDOS memory management problems
	and minimizes fragmentation.

	MINSEG below defines the minimum segment size allocated.
	Whenever the remaining portion of a block is smaller than
	this value, the entire block is returned to the caller.

	HEAPSIZE is the smallest system allocation we will ever
	make.  This value can be adjusted to your application.
	If it is small, more GEMDOS Malloc calls will have to
	be performed.  If it is large compared to the amount of
	memory actually aquired at runtime, there will be wasted
	memory.  Since too many GEMDOS Malloc calls may produce
	a crash, it is wise to make HEAPSIZE at least 8K bytes.

   =============================================================================
*/

#define	DEBUGIT		1	/* define non-zero to compile debug code */

#include "stdio.h"
#include "osbind.h"
#include "stddefs.h"

/* memory manager definitions */

#define	HEAPSIZE	16384L	/* minimum size of each heap */
#define	MINSEG		256L	/* minimum size of allocated memory chunk */

#define	MAGIC		0x55AA	/* magic number used for validation */

/* the structure controlling the object known as a "heap" */

#define HEAP struct _heap

/* Memory Control Block */

#define MCB struct _mcb

struct _mcb
{

  MCB *fore;			/* forward link */
  MCB *aft;			/* backward link */
  MCB *buddy;			/* nearest lower address neighbor */
  long size;			/* size of this chunk including MCB */
  HEAP *heap;			/* 0L if free, else owner of this chunk */
  int magic;			/* magic number for validation */
};

/* and here is the heap control block */

struct _heap
{

  HEAP *link;			/* pointer to next heap (0 if end) */
  MCB *avail;			/* pointer to first free block or 0 */
  MCB *limit;			/* address of the end of this heap */
};

#if	DEBUGIT
short mal_dbg;			/* set non-zero for malloc debug trace */
#endif

/*
	List of allocated heaps aquired from GEMDOS.
	Start off with no heaps allocated (NULL terminated linked list).
 */

static HEAP heaps = { (HEAP *) 0 };

/* 
*/

/*
   =============================================================================
	a_heap() --get another heap from GEMDOS
   =============================================================================
*/

static HEAP *
a_heap (x)
     long x;
{
  MCB *m;
  HEAP *heap;

  /* locate end of the heap list */
  /* (a tail pointer would help here) */

  for (heap = &heaps; heap->link; heap = heap->link)
    ;

  /* adjust the request for the minmum required overhead */

  x = (x + sizeof (HEAP) + sizeof (MCB) + 1) & ~1L;

  /* grab a chunk from GEMDOS */

  if ((heap->link = (HEAP *) Malloc (x)) EQ 0)
    {

#if	DEBUGIT
      if (mal_dbg)
	{

	  printf ("a_heap(%ld):  GEMDOS Malloc() returned 0\n", x);
	  printf ("      largest block available = %ld\n", Malloc (-1L));
	}
#endif
      return ((HEAP *) 0);
    }

  /* add the heap to the heap list */

  heap = heap->link;
  heap->link = (HEAP *) 0;

  /* first chunk is just after header */

  m = (MCB *) (heap + 1);

  /* set up size and mark it as a free chunk */

  m->size = x - sizeof (HEAP);
  m->heap = 0L;

  /* this is the last (only) chunk on the linked list */

  m->fore = (MCB *) 0;
  m->aft = (MCB *) (&heap->avail);

  /* there is no lower addressed neighbor to this chunk */

  m->buddy = (MCB *) 0;

  /* mark the heap limit and place chunk on the free list */

  heap->limit = (MCB *) ((char *) heap + x);
  heap->avail = m;

  return (heap);
}

/* 
*/

/*
   =============================================================================
	s_split() -- split a segment into two chunks
   =============================================================================
*/

static
s_split (mcb, x)
     MCB *mcb;
     long x;
{
  MCB *m;
  HEAP *heap;

  /* check for ownership here */

  if (mcb EQ 0 OR (heap = mcb->heap) EQ 0 OR mcb->magic NE MAGIC)
    {

#if	DEBUGIT
      if (mal_dbg)
	printf ("s_split($%lx, %ld):  MCB invalid\n", mcb, x);
#endif
      return (FAILURE);
    }

  /* make a new chunk inside this one */

  m = (MCB *) ((char *) mcb + x);
  m->size = mcb->size - x;
  m->buddy = mcb;
  m->heap = mcb->heap;
  m->magic = MAGIC;

  /* shrink the old chunk */

  mcb->size = x;

  /* establish the forward neighbor's relationship to us */

  mcb = m;

  if ((m = (MCB *) ((char *) mcb + mcb->size)) < heap->limit)
    m->buddy = mcb;

  free (++mcb);
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	aloc_s() -- allocate a chunk out of a heap
   =============================================================================
*/

static MCB *
aloc_s (x, heap)
     long x;
     HEAP *heap;
{
  MCB *mcb;

  /* use first fit algorithm to find chunk to use */

  for (mcb = heap->avail; mcb; mcb = mcb->fore)
    if (mcb->size GE x + sizeof (MCB))
      break;

  if (mcb)
    {

      /* remove it from the free list */

      unfree (mcb);

      /* set up owner */

      mcb->heap = heap;
      mcb->magic = MAGIC;

      /* if it's bigger than we need and splitable, split it */

      if (mcb->size - x > MINSEG)
	if (s_split (mcb, x + sizeof (MCB)))
	  return ((MCB *) 0);

      /* return start of data area to caller */

      mcb++;
    }

  return (mcb);
}

/* 
*/

/*
   =============================================================================
	unfree() -- remove (unlink) a chunk from the free list
   =============================================================================
*/

static
unfree (mcb)
     MCB *mcb;
{
  if ((mcb->aft->fore = mcb->fore) NE 0)
    mcb->fore->aft = mcb->aft;
}



/*
   =============================================================================
	collect() -- GEMDOS garbage collection, return TRUE if anything changes
   =============================================================================
*/

static
collect ()
{
  HEAP *heap, *h;
  MCB *mcb;
  int flag;

#if	DEBUGIT
  if (mal_dbg)
    printf ("collect():  collecting garbage ...\n");
#endif

  for (flag = 0, heap = &heaps; (h = heap->link) NE 0;)
    {

      if ((mcb = h->avail) NE 0 AND
	  NOT mcb->buddy AND ((char *) mcb + mcb->size) EQ h->limit)
	{

	  heap->link = h->link;
	  Mfree (h);
	  flag++;

	}
      else
	heap = h;
    }

  return (flag);
}

/* 
*/

/*
   =============================================================================

	Unix standard C runtime library routines
	malloc(), free(), and realloc() follow.

	The three calls work as described in K & R,
	except that they don't use brk() or sbrk(),
	so BEWARE.

	This implementation uses a first fit algorithm
	and does occasional garbage collection to
	minimize system memory fragmentation.

   =============================================================================
*/


/*
   =============================================================================
	malloc() -- allocate 'n' bytes of memory
   =============================================================================
*/


char *
malloc (n)
     unsigned n;
{
  register HEAP *heap;
  register long x;
  char *p;

  x = (long) (n + 1) & ~1L;

  /* first check all current heaps */

  for (heap = heaps.link; heap; heap = heap->link)
    if ((p = aloc_s (x, heap)) NE 0)
      return (p);

  /* not enough room on heap list, try garbage collection */

  collect ();

  /* now allocate a new heap */

  if ((heap = a_heap (max (x, HEAPSIZE))) NE 0)
    if ((p = aloc_s (x, heap)) NE 0)
      return (p);

  /* couldn't get a chunk big enough */

#if	DEBUGIT
  if (mal_dbg)
    printf ("malloc(%u):  unable to find a large enough chunk\n", n);
#endif

  return ((char *) 0);
}

/* 
*/


/*
   =============================================================================
	free() -- return a block 'mcb' of memory to the storage pool
   =============================================================================
*/


free (mcb)
     MCB *mcb;
{
  MCB *m;
  HEAP *heap;

  /* address header */

  mcb--;

  /* check for ownership here */

  if (mcb EQ 0 OR (heap = mcb->heap) EQ 0 OR mcb->magic NE MAGIC)
    {

#if	DEBUGIT
      printf ("free($%lx):  MCB invalid\n", mcb);
#endif

      return (-40);
    }

  /* connect to chunks behind this one */

  while (mcb->buddy)
    {

      if (mcb->buddy->heap)
	break;

      mcb->buddy->size += mcb->size;
      mcb = mcb->buddy;
      unfree (mcb);
    }

  /* now connect to chunks after this one */

  while ((m = (MCB *) ((char *) mcb + mcb->size)) < heap->limit)
    {

      m->buddy = mcb;

      if (m->heap)
	break;

      mcb->size += m->size;
      unfree (m);
    }

  /* place the resultant chunk on the free list */

  for (m = (MCB *) (&heap->avail); m->fore; m = m->fore)
    ;

  m->fore = mcb;
  mcb->fore = (MCB *) 0;
  mcb->aft = m;
  mcb->heap = 0L;
  return (0);
}

/* 
*/


/*
   =============================================================================
	realloc() -- reallocate a block of memory
   =============================================================================
*/


char *
realloc (mcb, n)
     MCB *mcb;
     unsigned n;
{
  long x;
  char *t, *s, *p;

  /* address header */

  --mcb;

  /* check for ownership here */

  if (mcb EQ 0 OR mcb->magic NE MAGIC)
    {

#if	DEBUGIT
      printf ("realloc($%lx, %u):  MCB invalid\n", mcb, n);
#endif

      return ((char *) 0);
    }

  /* round up the request and add overhead */

  x = (long) (n + 1 + sizeof (MCB)) & ~1L;

  /* if less than current size, just shrink it */

  if (mcb->size > x)
    {

      if (s_split (mcb, x))
	return ((char *) 0);
      else
	return ((char *) (++mcb));
    }

  /* it's bigger - allocate new block, copy data, and free old one */

  if ((p = malloc (n)) NE 0)
    {

      x = mcb->size - sizeof (MCB);
      s = ++mcb;
      t = p;

      while (x--)
	*t++ = *s++;

      free (mcb);
      return (p);
    }

#if	DEBUGIT
  printf ("realloc($%lx, %u):  unable to reallocate\n", mcb, n);
#endif
  return ((char *) 0);
}
