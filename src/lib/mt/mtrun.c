/*
   =============================================================================
	mtrun.c -- Multi-Tasker -- task initialization, etc.
	Version 8 -- 1988-04-12 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"
#include "mtdefs.h"
#include "debug.h"

extern short setipl ();		/* set processor IPL function */
extern short MTExit ();		/* task termination function */

extern struct _mt_def *_MT_;	/* Multi-Tasker structure pointer */

/* 
*/

/*
   =============================================================================
	MTRun() -- initialize and start a task

		SUCCESS		TCB created and placed on ready queue
		FAILURE		no TCB available for task
   =============================================================================
*/

short
MTRun (tid, pri, stat, slice, pstack, func, par)
     unsigned tid;
     register unsigned pri;
     unsigned stat;
     long slice, pstack;
     short (*func) ();
     long par;
{
  register short oldipl;
  register TCB *rcur, *rprv, *tcp;

  DB_ENTR ("MTRun");

  if ((struct _mt_def *) NIL == _MT_)
    _MT_ = (struct _mt_def *) XBIOS (X_MTDEFS);

  tcp = _MT_->mtp->TCBs;	/* point at the first TCB */
  oldipl = setipl (7);		/* DISABLE INTERRUPTS */

  while (tcp)
    {				/* search for a free TCB */

      if (! (tcp->flags & MTF_OCC))
	{

	  /* setup the TCB */

	  tcp->tos = pstack;	/* put top of stack in TCB */

	  pstack -= 4L;		/* adjust stack pointer */
	  *(long *) pstack = par;	/* store task parameter */

	  pstack -= 4L;		/* adjust stack pointer */
	  *(long *) pstack = MTExit;	/* store return address */

	  tcp->flags = MTF_OCC;	/* flags = TCB occupied */
	  tcp->tid = tid;	/* ID */
	  tcp->pri = pri;	/* priority */
	  tcp->slice = slice;	/* time slice */
	  tcp->sp = pstack;	/* stack pointer */
	  tcp->reg[15] = pstack;	/* a7 */
	  tcp->pc = func;	/* program counter */
	  tcp->sr = stat;	/* status register */
/* 
*/
	  rcur = (TCB *) & _MT_->mtp->RdyQ;	/* point at the head of the queue */

	  while (TRUE)
	    {

	      rprv = rcur;	/* previous TCB = current TCB */
	      rcur = rprv->next;	/* current TCB = next TCB */

	      if (rcur == (TCB *) NIL)	/* enqueue here if next was NIL */
		break;

	      if (pri > rcur->pri)	/* enqueue here if priority is greater */
		break;
	    }

	  rprv->next = tcp;	/* set next of previous TCB to new TCB */
	  tcp->next = rcur;	/* set next of new TCB to old next */
	  tcp->flags |= MTF_RDY;	/* set the ready flag in the new TCB */

	  setipl (oldipl);	/* RESTORE INTERUPTS */
	  DB_EXIT ("MTRun - SUCCESS");
	  return (SUCCESS);	/* return -- SUCCESS */
	}

      tcp = tcp->fwd;		/* look at next TCB */
    }

  setipl (oldipl);		/* RESTORE INTERRUPTS */
  DB_EXIT ("MTRun - FAILURE");
  return (FAILURE);		/* return -- FAILURE */
}
