/*
   =============================================================================
	mtid.c -- Multi-Tasker -- get next task ID
	Version 1 -- 1988-04-11 -- D.N. Lynx Crowe
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
	MTID() -- return next available task identifier
   =============================================================================
*/

unsigned
MTID ()
{
  register short inuse, oldipl;
  register unsigned newtid;
  register TCB *tcp;

  inuse = TRUE;			/* preset inuse to get things started */

  if ((struct _mt_def *) NIL == _MT_)
    _MT_ = (struct _mt_def *) XBIOS (X_MTDEFS);

  oldipl = setipl (7);		/* DISABLE INTERRUPTS */

  while (inuse)
    {

      newtid = _MT_->mtp->IDct++;	/* pick the next ID to try */
      inuse = FALSE;		/* say it's not in use */
      tcp = _MT_->mtp->TCBs;	/* start search at beginning */

      while (tcp)
	{			/* search the TCB table */

	  if (tcp->flags & MTF_OCC)	/* TCB occupied ? */
	    if (tcp->tid == newtid)
	      {			/* tid in use ? */

		inuse = TRUE;	/* set to search again */
		break;
	      }

	  tcp = tcp->fwd;
	}
    }

  setipl (oldipl);		/* RESTORE INTERRUPTS */

  return (newtid);		/* return the new task ID */
}
