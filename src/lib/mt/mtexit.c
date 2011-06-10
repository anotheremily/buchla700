/*
   =============================================================================
	mtexit.c -- Multi-Tasker -- task termination
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
	MTExit() -- terminate current task  (never returns)
   =============================================================================
*/

MTExit ()
{
  register short oldipl;

  DB_ENTR ("MTExit");

  if ((struct _mt_def *) NIL EQ _MT_)
    _MT_ = (struct _mt_def *) XBIOS (X_MTDEFS);

  oldipl = setipl (7);		/* DISABLE INTERRUPTS */
  _MT_->mtp->CurP->flags = 0;	/* clear our flags  (deallocate TCB) */
  setipl (oldipl);		/* RESTORE INTERRUPTS */

iquit:
  MTNext ();			/* swap task out of existence */

  printf ("\nMTExit():  ERROR -- MTNext() returned\n\n");

  xtrap15 ();			/* trap to ROMP */
  goto iquit;
}
