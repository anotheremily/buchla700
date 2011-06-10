/*
   =============================================================================
	mtquit.c -- Multi-Tasker -- shutdown
	Version 1 -- 1988-04-11 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"
#include "mtdefs.h"

extern short _MT_Nil ();	/* system NIL task */

extern TCB *MT_CurP;		/* current TCB pointer */
extern TCB *MT_RdyQ;		/* ready queue pointer */

extern unsigned *_MT_Vc1;	/* old swapper TRAP vector 1 */
extern unsigned *_MT_Vc2;	/* old swapper TRAP vector 2 */

long _MT_Stk[48];		/* a stack for _MT_Nil() */

TCB _MT_TCB;			/* dummy TCB for shutdown */

/* 
*/

/*
   =============================================================================
	MTQuit() -- shut down the multi-tasker
   =============================================================================
*/

MTQuit ()
{
  register short oldipl;

  oldipl = setipl (7);		/* DISABLE INTERRUPTS */

  BIOS (B_SETV, 40, _MT_Vc1);	/* restore old trap vector 1 */
  BIOS (B_SETV, 41, _MT_Vc2);	/* restore old trap vector 2 */

  _MT_Vc1 = (short *) NIL;	/* clear trap vector save area 1 */
  _MT_Vc2 = (short *) NIL;	/* clear trap vector save area 2 */

  /* setup dummy current TCB in case of 'accidents' */

  _MT_TCB.pri = (unsigned) 0xFFFF;	/* highest priority */
  _MT_TCB.slice = -1L;		/* no time slicing */

  MT_CurP = &_MT_TCB;		/* make dummy TCB current */
  MT_RdyQ = (TCB *) NIL;	/* clear the ready queue */

  setipl (oldipl);		/* RESTORE INTERRUPTS */
}
