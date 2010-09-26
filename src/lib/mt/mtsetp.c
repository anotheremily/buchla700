/*
   =============================================================================
	mtsetp.c -- Multi-Tasker -- change task priority
	Version 4 -- 1988-04-11 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"
#include "mtdefs.h"

extern	short		setipl();	/* set processor IPL function */

extern	struct	_mt_def	*_MT_;

/*
   =============================================================================
	MTSetP() -- reset task priority and dispatch to highest priority task

		Returns old task priority.
   =============================================================================
*/

unsigned
MTSetP(pri)
unsigned pri;
{
	register short oldipl;
	register unsigned oldpri;

	if ((struct _mt_def *)NIL EQ _MT_)
		_MT_ = (struct _mt_def *)XBIOS(X_MTDEFS);

	oldipl = setipl(7);	/* DISABLE INTERRUPTS */

	oldpri = _MT_->mtp->CurP->pri;	/* get old task priority */
	_MT_->mtp->CurP->pri = pri;	/* set new priority for task */

	setipl(oldipl);		/* RESTORE INTERRUPTS */

	MTSwap();		/* swap to highest priority ready task */
	return(oldpri);		/* return old task priority */
}

