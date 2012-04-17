/*
   =============================================================================
	testcq.c -- test the character queue functions
	Version 1 -- 1988-11-02 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"
#include "charq.h"

#define	QSIZE	128		/* queue length (characters) for testing */

#define	QLO	(QSIZE >> 2)	/* lo water mark = 1/4 queue length */
#define	QHI	(QSIZE - QLO)	/* hi water mark = 3/4 queue length */

/*
   =============================================================================
	Test the character queue functions
   =============================================================================
*/

struct charq queue1;

char chars[QSIZE];

char rs;

main (argc, argv)
     int argc;
     char *argv[];
{
  register struct charq *qp;
  register unsigned short i, qsiz;
  register short rc;

  qp = &queue1;

  printf ("Initializing queue:  size = %u, lo water = %u, hi water = %u\n",
	  QSIZE, QLO, QHI);

  if (QSIZE != (qsiz = setcq (qp, chars, QSIZE, QHI, QLO)))
    {

      printf ("ERROR -- setcq() returned %u\n", qsiz);
      exit (1);
    }

  printf ("Filling queue of length %u with %u entries\n", QSIZE, QSIZE + 2);

  for (i = 0; i < QSIZE + 2; i++)
    {				/* fill queue to overflow */

      if (rc = putcq (qp, i))
	printf ("putcq() returned %d on entry %u\n", rc, i);
    }

  printf ("Queue $%08.8lx:  size = %u, len = %u, in = %u, out = %u\n",
	  qp, qp->qsize, qp->qlen, qp->qin, qp->qout);

  printf ("Emptying queue of length %u for %u entries\n", QSIZE, QSIZE + 2);

  for (i = 0; i < QSIZE + 2; i++)
    {				/* empty queue to underflow */

      if (rc = getcq (qp, &rs))
	printf ("getcq() returned %d on entry %u\n", rc, i);

      if ((rs != i) && (i < QSIZE))
	printf ("ERROR -- value %u should have been %u\n", rs, i);
    }

  exit (0);
}
