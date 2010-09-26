/*
   =============================================================================
	sminit.c -- Multi_Tasker -- Semphore functions
	Version 12 -- 1988-04-17 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"
#include "mtdefs.h"
#include "debug.h"

extern	short	setipl();		/* set processor IPL function */

extern	struct _mt_def	*_MT_;

/*
   =============================================================================
	SMInit() -- Initialize a sempaphore
   =============================================================================
*/

SMInit(psem, n)
SEM *psem;
long n;
{
	register short oldipl;

	oldipl = setipl(7);		/* DISABLE INTERRUPTS */

	*psem = (SEM)((n << 1) | 1L);	/* set the semaphore counter */

	setipl(oldipl);			/* RESTORE INTERRUPTS */
}

/* 
*/

/*
   =============================================================================
	SMStat() -- Check the status of a semaphore

		-1	Awaited		at least one process is waiting
		 0	Not signalled	no signals, no waiting processes
		 1	Signalled	at least one unreceived signal exists
   =============================================================================
*/

short
SMStat(psem)
SEM *psem;
{
	register short oldipl, rv;
	register long semval;

	oldipl = setipl(7);		/* DISABLE INTERRUPTS */

	semval = (long)*psem;		/* get semaphore value */

	setipl(oldipl);			/* RESTORE INTERRUPTS */

	if (semval & 1L) {		/* in use as a counter ? */

		if (semval & ~1L)
			rv = 1;		/* signalled */
		else
			rv = 0;		/* not signalled */

	} else {			/* ... used as a pointer */

		if (semval & ~1L)
			rv = -1;	/* awaited */
		else
			rv = 0;		/* not signalled */
	}

	return(rv);			/* return semaphore status */
}

/* 
*/

/*
   =============================================================================
	SMSig() -- Signal a semaphore
   =============================================================================
*/

SMSig(psem)
SEM *psem;
{
	register TCB *rcur, *rprv, *tcp;
	register short oldipl;
	register long semval;
	register unsigned rpri;

	DB_ENTR("SMSig");

	if ((struct _mt_def *)NIL EQ _MT_)
		_MT_ = (struct _mt_def *)XBIOS(X_MTDEFS);

	oldipl = setipl(7);		/* DISABLE INTERRUPTS */

	semval = (long)*psem;		/* get semaphore value */

	if (semval & 1L) {		/* is it a count ?  (LSB EQ 1) */

		if (~1L NE (semval & ~1L))	/* check for overflow */
			semval += 2L;		/* update the counter */

	} else {			/* ... it may be a queue  (LSB EQ 0) */

/* 
*/
chksem:
		if (semval) {		/* is there something in the queue ? */

			tcp = (TCB *)semval;	/* extract TCB address */
			semval = tcp->next;	/* point to next TCB in queue */
			tcp->next = (TCB *)0L;	/* clear NEXT of TCB */
			tcp->flags &= ~MTF_SWT;	/* turn off 'wait' bit */

			if (tcp->flags & MTF_STP) {	/* 'stop' bit set ? */

				tcp->flags &= ~MTF_STP;	/* clear 'stop' bit */
				goto chksem;	/* try for another task */
			}

			rpri = tcp->pri;	/* get priority of task to be enqueued */
			rcur = (TCB *)&_MT_->mtp->RdyQ;	/* point at ready queue */

			while (TRUE) {

				rprv = rcur;		/* previous TCB = current TCB */
				rcur = rprv->next;	/* current TCB = next TCB */

				if (rcur EQ (TCB *)NIL)	/* enqueue here if next was NIL */
					break;

				if (rpri > rcur->pri)	/* enqueue here if priority is greater */
					break;
			}

			rprv->next = tcp;		/* set next of previous TCB to new TCB */
			tcp->next = rcur;		/* set next of new TCB to old next */
			tcp->flags |= MTF_RDY;		/* set the ready flag in the new TCB */

		} else {		/* ... queue empty, treat as a counter */

			semval = 3L;	/* set the counter to 1 */
		}
	}

	*psem = (SEM)semval;		/* update the semaphore */

	setipl(oldipl);			/* RESTORE INTERRUPTS */
	DB_EXIT("SMSig");
}

/* 
*/

/*
   =============================================================================
	SMWait() -- Wait on a semaphore
   =============================================================================
*/

SMWait(psem)
register SEM *psem;
{
	register short oldipl;
	register long semval;
	register TCB *ptcb, *tcp;

	DB_ENTR("SMWait");

	if ((struct _mt_def *)NIL EQ _MT_)
		_MT_ = (struct _mt_def *)XBIOS(X_MTDEFS);

	oldipl = setipl(7);			/* DISABLE INTERRUPTS */

	semval = (long)*psem;			/* get semaphore value */

	if (semval & 1L) {			/* is it a count ?  (LSB EQ 1) */

		if (semval & ~1L) {		/* is count non-zero ? */

			semval -= 2L;		/* decrement count */
			*psem = (SEM)semval;	/* update semaphore */

			setipl(oldipl);		/* RESTORE INTERRUPTS */

			DB_EXIT("SMWait - semaphore non-zero");
			return;			/* return -- we got a signal */

		}

		*psem  = 0L;			/* clear the semaphore */
	}
	
	ptcb = _MT_->mtp->CurP;			/* point at current tcb */
	tcp  = (TCB *)*psem;			/* point at head of SEM queue */

	while (tcp->next)			/* find end of queue */
		tcp = tcp->next;

	tcp->next = ptcb;			/* add TCB to queue */
	ptcb->next = (TCB *)0L;			/* ... */
	ptcb->flags |= MTF_SWT;			/* indicate TCB is waiting */
	MTNext();				/* swap tasks */
	setipl(oldipl);				/* RESTORE INTERRUPTS */
	DB_EXIT("SMWait - signalled - 1");
	return;					/* return  (we got a signal) */
}

/* 
*/

/*
   =============================================================================
	SMCSig() -- Conditionally signal a semphore

		-1	stopped task signalled
		 0	nothing signalled
		 1	waiting task signalled
   =============================================================================
*/

short
SMCSig(psem)
SEM *psem;
{
	register TCB *rcur, *rprv, *tcp;
	register short oldipl;
	register long semval;
	register unsigned rpri;

	oldipl = setipl(7);		/* DISABLE INTERRUPTS */

	if ((struct _mt_def *)NIL EQ _MT_)
		_MT_ = (struct _mt_def *)XBIOS(X_MTDEFS);

	semval = (long)*psem;		/* get semaphore value */

	if (semval & 1L) {		/* is it a count ? */

		setipl(oldipl);		/* RESTORE INTERRUPTS */

		return(0);		/* return -- nothing signalled */

	} else {

/* 
*/
		if (semval & ~1L) {	/* is there a waiting task ? */

			tcp = (TCB *)semval;	/* get TCB pointer */
			tcp->flags &= ~MTF_SWT;	/* clear the wait bit */
			tcp->next = (TCB *)0L;	/* clear NEXT of TCB */
			semval = tcp->next;	/* get next in queue */

			if (tcp->flags & MTF_STP) {	/* 'stop' bit set ? */

				tcp->flags &= ~MTF_STP;	/* clear 'stop' bit */
				setipl(oldipl);		/* RESTORE INTERRUPTS */
				return(-1);		/* return -- stopped task signalled */
			}

			rpri = tcp->pri;	/* get priority of task to be enqueued */
			rcur = (TCB *)&_MT_->mtp->RdyQ;	/* point at the head of the queue */

			while (TRUE) {

				rprv = rcur;		/* previous TCB = current TCB */
				rcur = rprv->next;	/* current TCB = next TCB */

				if (rcur EQ (TCB *)NIL)	/* enqueue here if next was NIL */
					break;

				if (rpri > rcur->pri)	/* enqueue here if priority is greater */
					break;
			}

			rprv->next = tcp;		/* set next of previous TCB to new TCB */
			tcp->next = rcur;		/* set next of new TCB to old next */
			tcp->flags |= MTF_RDY;		/* set the ready flag in the new TCB */

			setipl(oldipl);	/* RESTORE INTERRUPTS */

			return(1);	/* return -- waiting task signalled */
		}
	}
}

/* 
*/

/*
   =============================================================================
	SMCWait() -- Conditionally wait on a semaphore

		TRUE	Semaphore was non-zero, and was decremented
		FALSE	Semaphore was zero, or tasks were waiting for it
   =============================================================================
*/

short
SMCWait(psem)
register SEM *psem;
{
	register short oldipl, rv;
	register long semval;

	DB_ENTR("SMCWait");
	rv = FALSE;			/* preset return value */

	oldipl = setipl(7);		/* DISABLE INTERRUPTS */

	semval = (long)*psem;		/* get semaphore value */

	if (semval & 1L) {		/* is it a count ? */

		if (semval & ~1L) {	/* is count non-zero ? */

			semval -= 2L;		/* decrement counter */
			*psem = (SEM)semval;	/* update semaphore */
			rv = TRUE;		/* set return value */
			DB_CMNT("SMCWait - got signal");
		}
	}

	setipl(oldipl);			/* RESTORE INTERRUPTS */
	DB_EXIT("SMCWait");
	return(rv);			/* return semaphore state */
}
