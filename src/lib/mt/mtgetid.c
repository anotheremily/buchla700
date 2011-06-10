/*
   =============================================================================
	mtgetid.c -- Multi-Tasker -- get current task ID
	Version 2 -- 1988-04-12 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"
#include "mtdefs.h"

extern struct _mtdef *_MT_;	/* Multi-Tasker structure pointer */

/*
   =============================================================================
	MTGetID() -- get current task ID
   =============================================================================
*/

unsigned
MTGetID ()
{
  if ((struct _mt_def *) NIL EQ _MT_)
    _MT_ = (struct _mt_def *) XBIOS (X_MTDEFS);

  return (_MT_->CurP->tid);	/* return current task ID */
}
