/*
   =============================================================================
	mtstat.c -- Multi-Tasker -- task status check
	Version 4 -- 1988-04-14 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"
#include "mtdefs.h"

extern short setipl ();		/* set processor IPL function */

extern struct _mt_def *_MT_;

/* 
*/

/*
   =============================================================================
	MTStat() -- check task status

		-3	stopped, waiting on a sempahore
		-2	stopped, not waiting
		-1	not found
		 0	running  (current task)
		 1	ready to run
		 2	waiting on a sempahore
   =============================================================================
*/

short
MTStat (tid)
     unsigned tid;
{
  register TCB *tcp;
  register short oldipl, rv;

/* 
*/
  if ((struct _mt_def *) NIL EQ _MT_)
    _MT_ = (struct _nt_def *) XBIOS (X_MTDEFS);

  tcp = _MT_->mtp->TCBs;	/* point at start of  TCB list */
  rv = -1;			/* preset return value */

  oldipl = setipl (7);		/* DISABLE INTERRUPTS */

  while (tcp)
    {				/* check each TCB */

      if (tcp->flags & MTF_OCC)
	{			/* occupied ? */

	  if (tcp->tid NE tid)	/* continue if not tid */
	    goto nexttcb;

	  if (tcp->flags & MTF_RUN)
	    {			/* running ? */

	      rv = 0;
	      break;
	    }

	  if (tcp->flags & MTF_RDY)
	    {			/* ready to run ? */

	      rv = 1;
	      break;
	    }

	  if (tcp->flags & MTF_STP)
	    {			/* to be stopped ? */

	      rv = -3;
	      break;
	    }

	  if (tcp->flags & MTF_SWT)
	    {			/* waiting ? */

	      rv = 2;
	      break;
	    }

	  rv = -2;		/* stopped */
	  break;
	}

    nexttcb:
      tcp = tcp->fwd;		/* look at next TCB */
    }

  setipl (oldipl);		/* RESTORE INTERRUPTS */

  return (rv);			/* return status of task */
}
