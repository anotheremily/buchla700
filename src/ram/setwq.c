/*
   ============================================================================
	setwq.c -- word queue functions
	Version 1 -- 1988-11-02 -- D.N. Lynx Crowe
   ============================================================================
*/

#define	WORDQHDR		/* so wordq.h gets it right */

#include "stddefs.h"
#include "wordq.h"

/*
   ============================================================================
	setwq(qp, qadr, qsiz, hi, lo) -- setup a word queue

		Where:

			struct wordq   *qp	queue structure pointer
			unsigned short *qadr	queue buffer pointer
			unsigned short qsiz	maximum length of queue
			unsigned short hi	high water mark count
			unsigned short lo	low water mark count

		Return value:

			unsigned short qsiz	size of queue
   ============================================================================
*/

unsigned short
setwq (qp, qadr, qsiz, hi, lo)
     register struct wordq *qp;
     unsigned short *qadr;
     unsigned short qsiz, hi, lo;
{
  if ((unsigned short *) 0L EQ qadr)
    qsiz = 0;

  qp->qbuf = qadr;
  qp->qsize = qsiz;
  qp->qlen = 0;
  qp->qin = 0;
  qp->qout = 0;
  qp->qhi = hi;
  qp->qlo = lo;

  return (qsiz);
}

/* 
*/

/*
   ============================================================================
	putwq(qp, c) -- put a word in a word queue

		Where:

			qp	queue structure pointer
			c	word to put in the queue

		Return values:

			-2	error,  queue size was 0
			-1	queue was full,  word not added to queue
			 0	word added to queue
			 1	at high water mark,  word added to queue
   ============================================================================
*/

short
putwq (qp, c)
     register struct wordq *qp;
     register unsigned short c;
{
  if (0 NE qp->qsize)
    {				/* verify queue is ok */

      if (qp->qlen EQ qp->qsize)	/* check queue length */
	return (-1);		/* -1 = full */

      qp->qbuf[qp->qin++] = c;	/* put word in queue */

      if (qp->qin GE qp->qsize)	/* update input index */
	qp->qin = 0;		/* wrap around */

      if (++qp->qlen EQ qp->qhi)	/* check length again */
	return (1);		/* 1 = at hi water */
      else
	return (0);		/* 0 = OK */

    }
  else
    {

      return (-2);		/* -2 = error */
    }
}

/* 
*/

/*
   ============================================================================
	getwq(qp, p) -- get a word from a word queue

		Where:

			qp	queue structure pointer
			p	word pointer for returned word

		Return values:

			-2	error,  queue size was 0
			-1	queue was empty,  no word returned
			 0	word returned
			 1	at low water,  word returned
   ============================================================================
*/

short
getwq (qp, p)
     register struct wordq *qp;
     register unsigned short *p;
{
  if (0 NE qp->qsize)
    {				/* check queue is ok */

      if (0 NE qp->qlen)
	{			/* check queue length */

	  *p = qp->qbuf[qp->qout++];	/* get word from queue */

	  if (qp->qout GE qp->qsize)	/* check out pointer */
	    qp->qout = 0;	/* wrap around */

	  if (--qp->qlen EQ qp->qlo)	/* check length again */
	    return (1);		/* 1 = at low water */
	  else
	    return (0);		/* 0 = OK */

	}
      else
	{

	  return (-1);		/* -1 = emtpy */
	}

    }
  else
    {

      return (-2);		/* -2 = error */
    }
}
