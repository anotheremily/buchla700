/*
   =============================================================================
	mtsett.c -- Multi-Tasker -- task initialization with a new TCB
	Version 1 -- 1988-04-11 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"
#include "mtdefs.h"
#include "debug.h"

extern	short		setipl();	/* set processor IPL function */
extern	short		MTExit();	/* task terminate function */

extern	struct _mtdef	*_MT_;		/* Multi-Tasker structure pointer */

/* 
*/

/*
   =============================================================================
	MTSetT() -- initialize a TCB
   =============================================================================
*/

MTSetT(tcp, tid, pri, stat, slice, pstack, func, par)
register TCB *tcp;
unsigned tid;
register unsigned pri;
unsigned stat;
long slice, pstack;
short (*func)();
long par;
{
	register short oldipl;
	register TCB *rcur, *rprv;

	DB_ENTR("MTSetT");

	if ((struct _mt_def *)NIL EQ _MT_)
		_MT_ = (struct _mt_def *)XBIOS(X_MTDEFS);

	oldipl = setipl(7);			/* DISABLE INTERRUPTS */

	/* setup the TCB */

	tcp->tos = pstack;		/* put top of stack in TCB */

	pstack -= 4L;			/* adjust stack pointer */
	*(long *)pstack = par;		/* store task parameter */

	pstack -= 4L;			/* adjust stack pointer */
	*(long *)pstack = MTExit;	/* store return address */

	tcp->flags    = MTF_OCC;	/* flags = TCB occupied */
	tcp->tid      = tid;		/* ID */
	tcp->pri      = pri;		/* priority */
	tcp->slice    = slice;		/* time slice */
	tcp->sp       = pstack;		/* stack pointer */
	tcp->reg[15]  = pstack;		/* a7 */
	tcp->pc       = func;		/* program counter */
	tcp->sr       = stat;		/* status register */

/* 
*/
	/* queue the TCB */

	rcur = (TCB *)&_MT_->mtp->RdyQ;	/* point at the head of the queue */

	while (TRUE) {

		rprv = rcur;		/* previous TCB = current TCB */
		rcur = rprv->next;	/* current TCB = next TCB */

		if (rcur EQ (TCB *)NIL)	/* enqueue here if next was NIL */
			break;

		if (pri > rcur->pri)	/* enqueue here if priority is greater */
			break;
	}

	rprv->next  = tcp;		/* set next of previous TCB to new TCB */
	tcp->next   = rcur;		/* set next of new TCB to old next */
	tcp->flags |= MTF_RDY;		/* set the ready flag in the new TCB */

	tcp->fwd = _MT_->mtp->TCBs;	/* add TCB to chain */
	_MT_->mtp->TCBs = tcp;		/* ... */

	setipl(oldipl);		/* RESTORE INTERUPTS */
	DB_EXIT("MTSetT");
}
