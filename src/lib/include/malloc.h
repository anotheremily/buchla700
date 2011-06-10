/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MALLOC.H -- header file for A "smarter" malloc
 * Version 4 -- 1988-05-03 -- USENET
 *
 * A  "smarter" malloc		Original code by William L. Sebok
 *
 * Modified for Aztec C-II by D.N. Lynx Crowe
 *
 * 1986-12-30	Made long constants explicit as fix for bug in compiler
 * 1987-01-01	Made long constants into variables to work around compiler bug
 * 1988-05-03	Modified to work on the Atari under GEMDOS
 * 1988-05-03	Added define of SMARTMAL so we know what we've got
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Algorithm:
 *	Assign to each area an index "n". This is currently proportional to
 *	the log 2 of size of the area rounded down to the nearest integer.
 *	Then all free areas of storage whose length have the same index n are
 *	organized into a chain with other free areas of index n (the "bucket"
 *	chain). A request for allocation of storage first searches the list of
 *	free memory.  The search starts at the bucket chain of index equal to
 *	that of the storage request, continuing to higher index bucket chains
 *	if the first attempt fails.
 *	If the search fails then new memory is allocated.  Only the amount of
 *	new memory needed is allocated.  Any old free memory left after an
 *	allocation is returned to the free list.
 *
 *	All memory areas (free or busy) handled by malloc are also chained
 *	sequentially by increasing address (the adjacency chain).  When memory
 *	is freed it is merged with adjacent free areas, if any.  If a free area
 *	of memory ends at the end of memory (i.e. at the break), and if the
 *	variable "endfree" is non-zero, then the break is contracted, freeing
 *	the memory back to the system.
 *
 *	Notes:
 *		ov_len field includes sizeof(struct overhead)
 *		adjacency chain includes all memory, allocated plus free.
 */

/* **************************** Machine Dependencies ******************** */

/* the following items may need to be configured for a particular machine */

#define	void	int		/* Aztec C doesn't have voids */

/* alignment requirement for machine (in bytes) */

#define NALIGN	2

/* size of an integer large enough to hold a character pointer */

typedef long Size;

/*
 * CURBRK returns the value of the current system break, i.e., the system's
 * idea of the highest legal address in the data area.  It is defined as
 * a macro for the benefit of systems that have provided an easier way to
 * obtain this number (such as in an external variable)
 */

#ifndef CURBRK

#define CURBRK	sbrk(0)
extern char *sbrk ();

#else	/* CURBRK */

#if	CURBRK == curbrk
extern Size curbrk;
#endif

#endif	/* CURBRK */

/*
 * note that it is assumed that CURBRK remembers the last requested break to
 * the nearest byte (or at least the nearest word) rather than the nearest page
 * boundary.  If this is not true then the following BRK macro should be
 * replaced with one that remembers the break to within word-size accuracy.
 */

#ifndef BRK

#define BRK(x)	brk(x)
extern char *brk ();

#endif	/* BRK */

/*
	define NBUCKETS as 18 for big machines, 10 for small ones
	and adjust the definitions of mlsizes[] and buckets[], below.
*/

#define NBUCKETS	18

/* ***************** END of machine dependent portion ******************* */

struct qelem
{

  struct qelem *q_forw;
  struct qelem *q_back;
};

struct overhead
{

  struct qelem ov_adj;		/* adjacency chain pointers */
  struct qelem ov_buk;		/* bucket chain pointers */
  long ov_magic;		/* MAGIC number */
  Size ov_len;			/* length of the area in bytes */
};

/*
 * The following macros depend on the order of the elements in struct overhead
 */

#define TOADJ(p)	((struct qelem *)(p))
#define FROMADJ(p)	((struct overhead *)(p))
#define FROMBUK(p)	((struct overhead *)( (char *)p - sizeof(struct qelem)))
#define TOBUK(p)	((struct qelem *)( (char *)p + sizeof(struct qelem)))

#define	XM_FREE	0x548A934CL	/* MAGIC for free blocks */
#define	XM_BUSY	0xC139569AL	/* MAGIC for busy blocks */

#define	M_FREE	m_free
#define	M_BUSY	m_busy

#ifdef MALLOC

/*
 * return to the system memory freed adjacent to the break 
 * default is Off
 */

char endfree = 0;		/* WARNING -- this isn't PROMable */

/*
 * M_FREE and M_BUSY used to be MAGIC_FREE and MAGIC_BUSY
 * m_free and m_busy were added to work around a BUG in Aztec CII
 */

long m_free = XM_FREE;
long m_busy = XM_BUSY;

/* head of adjacency chain */

struct qelem adjhead = { &adjhead, &adjhead };

/* *************** More Machine Dependencies *********************** */

/* sizes of buckets currently proportional to log 2() */
/* must match NBUCKETS, above */

Size mlsizes[] = { 0, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384,

  /* trim here if on a small machine */

  32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304
};

/* head of bucket chains */
/* must match NBUCKETS, above */

struct qelem buckets[NBUCKETS] = {

  &buckets[0], &buckets[0], &buckets[1], &buckets[1],
  &buckets[2], &buckets[2], &buckets[3], &buckets[3],
  &buckets[4], &buckets[4], &buckets[5], &buckets[5],
  &buckets[6], &buckets[6], &buckets[7], &buckets[7],
  &buckets[8], &buckets[8], &buckets[9], &buckets[9],

  /* trim here if on a small machine */

  &buckets[10], &buckets[10], &buckets[11], &buckets[11],
  &buckets[12], &buckets[12], &buckets[13], &buckets[13],
  &buckets[14], &buckets[14], &buckets[15], &buckets[15],
  &buckets[16], &buckets[16], &buckets[17], &buckets[17]
};

/* ********************* End of Machine Dependencies ********************* */

void (*mlabort) () =
{
0};

#else

extern char endfree;
extern struct qelem adjhead, buckets[NBUCKETS];
extern Size mlsizes[NBUCKETS];
extern void (*mlabort) ();
extern long m_free, m_busy;

#endif

extern void insque (), remque ();
extern void free (), mllcerr ();
extern char *malloc (), *realloc ();

#ifdef debug

#define ASSERT(p,q)	if (!(p)) mllcerr(q)

#else

#define ASSERT(p,q)

#endif

#define	SMARTMAL
