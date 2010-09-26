/*
   =============================================================================
	mtstop.c -- Multi-Tasker -- stop task
	Version 3 -- 1988-04-11 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"
#include "mtdefs.h"

extern	short		setipl();	/* set processor IPL function */

extern	struct _mt_def	*_MT_;

/* 
*/

/*
   =============================================================================
	MTStop() -- stop a task

		-3	- task not stopped	(was the current task)
		-2	- task not stopped	(can't find it on ready queue)
		-1	- task not stopped	(can't find it at all)

		 0	- task stopped	 	(was ready to run)

		 1	- task stopped  	(was waiting)
		 2	- task already stopped	(wasn't waiting or on ready queue)
		 3	- task will be stopped	(has MTF_STP set)
   =============================================================================
*/

short
MTStop(tid)
unsigned tid;
{
	register short oldipl;
	register TCB *tcbcur, *tcbprv;

/* 
*/
	if ((struct _mt_def *)NIL EQ _MT_)
		_MT_ = (struct _mt_def *)XBIOS(X_MTDEFS);

	tcbcur = _MT_->mtp->TCBs;		/* point at TCB table */
	
	oldipl = setipl(7);			/* DISABLE INTERRUPTS */

	while (tcbcur) {				/* check each TCB */

		if (tcbcur->flags & MTF_OCC) {		/* TCB in use ? */

			if (tcbcur->tid EQ tid) {	/* task we want ? */

				if (tcbcur->flags & MTF_STP) {	/* stopped ? */

					setipl(oldipl);	/* RESTORE INTERRUPTS */
					return(3);	/* task in stop state */
				}

				if (tcbcur->flags & MTF_RUN) {

					setipl(oldipl);	/* RESTORE INTERRUPTS */
					return(-3);	/* can't stop current task */
				}

				if (tcbcur->flags & MTF_SWT) {	/* waiting ? */

					tcbcur->flags |= MTF_STP;	/* set to stop */

					setipl(oldipl);	/* RESTORE INTERRUPTS */
					return(1);	/* task is waiting */
				}

				if (NOT (tcbcur->flags & MTF_RDY)) {	/* stopped ? */

					setipl(oldipl);	/* RESTORE INTERRUPTS */
					return(2);	/* task was stopped */
				}
/* 
*/
				tcbprv = (TCB *)&_MT_->mtp->RdyQ;
				tcbcur = tcbprv->next;

				while(tcbcur) {		/* locate task on ready queue */

					tcbprv = tcbcur;
					tcbcur = tcbprv->next;

					if (tcbcur NE (TCB *)NIL) {

						if (tcbcur->tid EQ tid) {

							tcbprv->next = tcbcur->next;
							tcbcur->flags &= ~MTF_RDY;

							setipl(oldipl);	/* RESTORE INTERRUPTS */
							return(0);	/* stopped a ready task */
						}
					}
				}

				setipl(oldipl);	/* RESTORE INTERRUPTS */
				return(-2);	/* can't find a ready task */
			}
		}

		tcbcur = tcbcur->fwd;		/* point at next TCB */
	}

	setipl(oldipl);		/* RESTORE INTERRUPTS */
	return(-1);		/* can't find the task at all */
}

