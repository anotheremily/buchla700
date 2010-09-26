/*
   ============================================================================
	setcq.c -- character queue functions
	Version 1 -- 1988-11-02 -- D.N. Lynx Crowe
   ============================================================================
*/

#define	CHARQHDR		/* so charq.h gets it right */

#include "stddefs.h"
#include "charq.h"

/*
   ============================================================================
	setcq(qp, qadr, qsiz, hi, lo) -- setup a character queue

		Where:

			struct charq	*qp	queue structure pointer
			char		*qadr	queue buffer pointer
			unsigned short	qsiz	maximum length of queue
			unsigned short	hi	high water mark count
			unsigned short	lo	low water mark count

		Return value:

			unsigned short	qsiz	size of queue
   ============================================================================
*/

unsigned short
setcq(qp, qadr, qsiz, hi, lo)
register struct charq *qp;
char *qadr;
unsigned short qsiz, hi, lo;
{
	if ((char *)0L EQ qadr)
		qsiz = 0;

	qp->qbuf  = qadr;
	qp->qsize = qsiz;
	qp->qlen  = 0;
	qp->qin   = 0;
	qp->qout  = 0;
	qp->qhi   = hi;
	qp->qlo   = lo;

	return(qsiz);
}

/* 
*/

/*
   ============================================================================
	putcq(qp, c) -- put a character in a character queue

		Where:

			qp	queue structure pointer
			c	character to put in the queue

		Return values:

			-2	error,  queue size was 0
			-1	queue was full,  character not added to queue
			 0	character added to queue
			 1	at high water mark,  character added to queue
   ============================================================================
*/

short
putcq(qp, c)
register struct charq *qp;
register unsigned short c;
{
	if (0 NE qp->qsize) {				/* verify queue is ok */

		if (qp->qlen EQ qp->qsize)		/* check queue length */
			return(-1);			/* -1 = full */

		qp->qbuf[qp->qin++] = c;		/* put character in queue */

		if (qp->qin GE qp->qsize)		/* update input index */
			qp->qin = 0;			/* wrap around */

		if (++qp->qlen EQ qp->qhi)		/* check length again */
			return(1);			/* 1 = at hi water */
		else
			return(0);			/* 0 = OK */

	} else {

		return(-2);				/* -2 = error */
	}
}

/* 
*/

/*
   ============================================================================
	getcq(qp, p) -- get a character from a character queue

		Where:

			qp	queue structure pointer
			p	character pointer for returned character

		Return values:

			-2	error,  queue size was 0
			-1	queue was empty,  no character returned
			 0	character returned
			 1	at low water,  character returned
   ============================================================================
*/

short
getcq(qp, p)
register struct charq *qp;
register char *p;
{
	if (0 NE qp->qsize) {				/* check queue is ok */

		if (0 NE qp->qlen) {			/* check queue length */

			*p = qp->qbuf[qp->qout++];	/* get character from queue */

			if (qp->qout GE qp->qsize)	/* check out pointer */
				qp->qout = 0;		/* wrap around */

			if (--qp->qlen EQ qp->qlo)	/* check length again */
				return(1);		/* 1 = at low water */
			else
				return(0);		/* 0 = OK */

		} else {

			return(-1);			/* -1 = emtpy */
		}

	} else {

		return(-2);				/* -2 = error */
	}
}
