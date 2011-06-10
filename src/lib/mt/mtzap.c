/*
   =============================================================================
	mtzap.c -- Multi-Tasker -- free a TCB
	Version 3 -- 1988-04-14 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"
#include "mtdefs.h"
#include "debug.h"

extern short setipl ();		/* set processor IPL function */

extern struct _mtdef *_MT_;	/* Multi-Tasker structure pointer */

/* 
*/

/*
   =============================================================================
	MTZap() -- delete a task  (free its TCB)

		-1	not deleted	(was active or waiting)
		 0	deleted
		 1	not found
   =============================================================================
*/

short
MTZap (tid)
     unsigned tid;
{
  register short oldipl, rv;
  register TCB *tcp;

  if ((struct _mt_def *) NIL EQ _MT_)
    _MT_ = (struct _mt_def *) XBIOS (X_MTDEFS);

  tcp = _MT_->mtp->TCBs;	/* point at start of TCB table */
  rv = 1;			/* preset return code */

  oldipl = setipl (7);		/* DISABLE INTERRUPTS */

  while (tcp)
    {				/* check each TCB */

      if (tcp->flags & MTF_OCC)
	{			/* occupied ? */

	  if (tcp->tid NE tid)	/* continue if not TCB we want */
	    goto nexttcb;

	  if (tcp->flags & ~MTF_OCC)
	    {			/* stopped ? */

	      rv = -1;		/* not stopped */
	      break;
	    }

	  tcp->flags = 0;	/* free the TCB */
	  rv = 0;		/* TCB deleted */
	  break;
	}

    nexttcb:
      tcp = tcp->fwd;		/* check next TCB */
    }

  setipl (oldipl);		/* RESTORE INTERRUPTS */

  return (rv);			/* return deletion status */
}
