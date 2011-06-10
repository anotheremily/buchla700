/*
   =============================================================================
	mtinit.c -- Multi-Tasker -- initialization, etc.
	Version 31 -- 1988-04-15 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"
#include "mtdefs.h"
#include "debug.h"

extern short _MT_Swp ();	/* swapper TRAP handler -- swap */
extern short _MT_Nxt ();	/* swapper TRAP handler -- next */
extern short setipl ();		/* set processor IPL function */

extern TCB *MT_TCBs;		/* TCB chain pointer */
extern TCB *MT_CurP;		/* current TCB pointer */
extern TCB *MT_RdyQ;		/* ready queue pointer */
extern TCB _MTptcb[2];		/* private TCBs */

extern unsigned MT_IDct;	/* next available task ID */

extern unsigned *_MT_Vc1;	/* old swapper TRAP vector 1 */
extern unsigned *_MT_Vc2;	/* old swapper TRAP vector 2 */

long _MT_Stk[48];		/* a stack for _MT_Nil() */

TCB _MT_TCB;			/* dummy TCB for shutdown */

/* 
*/

/*
   =============================================================================
	_MT_Nil() -- the NIL task  (must always be present on the ready queue)
   =============================================================================
*/

_MT_Nil ()
{
  register short i, j;

  while (TRUE)
    {

      setipl (2);		/* make sure we're interruptable */

      for (i = 0; i < 10; i++)
	++j;

      DB_CMNT ("_MT_Nil");
      MTSwap ();
    }
}

/*
   =============================================================================
	MTInit() -- initialize the mutitasker and setup the task 0 default TCB
   =============================================================================
*/

MTInit ()
{
  register short i, oldipl;
  register TCB *tcp;

  DB_ENTR ("MTInit");
  oldipl = setipl (7);		/* DISABLE INTERRUPTS */

  MT_TCBs = &_MTptcb[1];	/* setup the TCB chain pointer */
  MT_RdyQ = &_MTptcb[0];	/* task 0 is on the ready queue */

  memsetw (&_MTptcb[0], 0, sizeof (TCB) / 2);	/* clear task 0 TCB */
  memsetw (&_MTptcb[1], 0, sizeof (TCB) / 2);	/* clear task 1 TCB */

  _MTptcb[0].flags = MTF_OCC | MTF_RDY;	/* set task 0 ready ... */
  _MTptcb[0].slice = -1L;	/* ... not time-sliced */
  _MTptcb[0].pc = _MT_Nil;	/* ... NIL task */
  _MTptcb[0].sp = &_MT_Stk[48];	/* ... set stack space */
  _MTptcb[0].reg[15] = &_MT_Stk[48];	/* ... and a7 */
  _MTptcb[0].sr = MT_NilSR;	/* ... set sr */

  MT_CurP = &_MTptcb[1];	/* task 1 is the current task */

  _MTptcb[1].fwd = &_MTptcb[0];	/* point at next TCB */
  _MTptcb[1].flags = MTF_OCC | MTF_RUN;	/* set task 1 running ... */
  _MTptcb[1].pri = MT_DfPri;	/* ... default priority */
  _MTptcb[1].slice = -1L;	/* ... not time-sliced */
  _MTptcb[1].tid = 1;		/* ... set task ID */

  MT_IDct = 2;			/* next task ID = 2 */

  _MT_Vc1 = BIOS (B_SETV, 40, _MT_Swp);	/* set swapper TRAP vector 1 */
  _MT_Vc2 = BIOS (B_SETV, 41, _MT_Nxt);	/* set swapper TRAP vector 2 */

  setipl (oldipl);		/* RESTORE INTERRUPTS */
  DB_EXIT ("MTInit");
}
