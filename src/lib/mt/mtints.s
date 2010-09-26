* ------------------------------------------------------------------------------
* mtints.s -- Multi-Tasker -- first level interrupt handlers
* Version 11 -- 1988-04-15 -- D.N. Lynx Crowe
* (c) Copyright 1988 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
*
		.text
*
DEBUGGER	.equ	0			* define non-zero for any debug
*
DEBUG_I1	.equ	0			* define non-zero for int1 debug
DEBUG_I2	.equ	0			* define non-zero for int2 debug
DEBUG_I3	.equ	0			* define non-zero for int3 debug
DEBUG_I4	.equ	0			* define non-zero for int4 debug
DEBUG_I5	.equ	0			* define non-zero for int5 debug
*
		.xdef	MTInt1
		.xdef	MTInt2
		.xdef	MTInt3
		.xdef	MTInt4
		.xdef	MTInt5
*
		.ifne	DEBUGGER
*
		.xref	_DB_Cmnt		* debug comment function
*
		.endc
*
		.xref	MT_ITCB1
		.xref	MT_ITCB2
		.xref	MT_ITCB3
		.xref	MT_ITCB4
		.xref	MT_ITCB5
*
		.xref	MT_ISEM1
		.xref	MT_ISEM2
		.xref	MT_ISEM3
		.xref	MT_ISEM4
		.xref	MT_ISEM5
*
		.xref	MT_Enq
*
		.xref	_MT_CurP
		.xref	_MT_RdyQ
*
		.page
*
* TCB offsets		('NEXT' must be the first thing in the TCB)
* -----------
NEXT		.equ	0			* LONG - next TCB in queue
FWD		.equ	NEXT+4			* LONG - next TCB in chain
TID		.equ	FWD+4			* WORD - task ID
PRI		.equ	TID+2			* WORD - task priority
SLICE		.equ	PRI+2			* LONG - slice time limit
REGS		.equ	SLICE+4			* LONG[16] - registers
TCB_SP		.equ	REGS+64			* LONG - stack pointer
TCB_PC		.equ	TCB_SP+4		* LONG - program counter
TCB_SR		.equ	TCB_PC+4		* WORD - status register
FLAGS		.equ	TCB_SR+2		* WORD - task flags
TOS		.equ	FLAGS+2			* LONG - top of stack
*
TCB_A6		.equ	REGS+56			* LONG - task a6 image
TCB_A7		.equ	REGS+60			* LONG - task a7 image
*
TCBLEN		.equ	TOS+4			* length of TCB
*
* TCB flags
* ---------
MTF_RDY		.equ	$0001			* 'ready' bit
NOT_RDY		.equ	$FFFE			* 'ready' bit complement
MTF_SWT		.equ	$0002			* 'wait' bit
NOT_SWT		.equ	$FFFD			* 'wait' bit complement
MTF_RUN		.equ	$0004			* 'run' bit
NOT_RUN		.equ	$FFFB			* 'run' bit complement
MTF_STP		.equ	$0008			* 'stop' bit
NOT_STP		.equ	$FFF7			* 'stop' bit complement
*
* Miscellaneous definitions
* -------------------------
IPL7		.equ	$0700			* processor IPL 7 mask
*
RPRV		.equ	a1			* previous TCB pointer
RCUR		.equ	a2			* current TCB pointer
RNXT		.equ	a3			* next TCB pointer
RTCP		.equ	a6			* general TCB pointer
*
RPRI		.equ	d0			* task priority
*
		.page
*
* MTInt1 -- Level 1 -- VSDD frame interrupt
* ------    -------------------------------
MTInt1:		ori.w	#IPL7,sr		* disable interrupts
*
		.ifne	DEBUG_I1
		movem.l	d0-d7/a0-a6,-(a7)	* DB_CMNT("MTInt1");
		move.l	#DB_msg1,-(a7)		* ...
		jsr	_DB_Cmnt		* ...
		tst.l	(a7)+			* ...
		movem.l	(a7)+,d0-d7/a0-a6	* ...
		.endc
*
		movem.l	a6-a7,-(a7)		* save a6-a7 on stack
		movea.l	_MT_CurP,RTCP		* get the current TCB pointer
		movem.l	d0-d7/a0-a5,REGS(RTCP)	* save d0-d7/a0-a5 in the TCB
		move.l	(a7)+,TCB_A6(RTCP)	* save a6 in the TCB
		move.l	(a7)+,TCB_A7(RTCP)	* save a7 in the TCB
		move.w	(a7)+,TCB_SR(RTCP)	* save sr in the TCB
		move.l	(a7)+,TCB_PC(RTCP)	* save the pc in the TCB
		move.l	a7,TCB_SP(RTCP)		* save the sp in the TCB
		andi.w	#NOT_RUN,FLAGS(RTCP)	* turn off its run flag
		move.w	PRI(RTCP),RPRI		* get the task priority in RPRI
		lea	_MT_RdyQ,RCUR		* get the ready queue pointer
		lea	MT_ITCB1,RNXT		* get the interrupt TCB pointer
		andi.w	#NOT_SWT,FLAGS(RNXT)	* turn off its wait flag
		clr.l	MT_ISEM1		* clear the interrupt semaphore
		jmp	MT_Enq			* go do the task dispatch
*
		.page
*
* MTInt2 -- Level 2 -- FPU interrupt
* ------    ------------------------
MTInt2:		ori.w	#IPL7,sr		* disable interrupts
*
		.ifne	DEBUG_I2
		movem.l	d0-d7/a0-a6,-(a7)	* DB_CMNT("MTInt2");
		move.l	#DB_msg2,-(a7)		* ...
		jsr	_DB_Cmnt		* ...
		tst.l	(a7)+			* ...
		movem.l	(a7)+,d0-d7/a0-a6	* ...
		.endc
*
		movem.l	a6-a7,-(a7)		* save a6-a7 on stack
		movea.l	_MT_CurP,RTCP		* get the current TCB pointer
		movem.l	d0-d7/a0-a5,REGS(RTCP)	* save d0-d7/a0-a5 in the TCB
		move.l	(a7)+,TCB_A6(RTCP)	* save a6 in the TCB
		move.l	(a7)+,TCB_A7(RTCP)	* save a7 in the TCB
		move.w	(a7)+,TCB_SR(RTCP)	* save sr in the TCB
		move.l	(a7)+,TCB_PC(RTCP)	* save the pc in the TCB
		move.l	a7,TCB_SP(RTCP)		* save the sp in the TCB
		andi.w	#NOT_RUN,FLAGS(RTCP)	* turn off its run flag
		move.w	PRI(RTCP),RPRI		* get the task priority in RPRI
		lea	_MT_RdyQ,RCUR		* get the ready queue pointer
		lea	MT_ITCB2,RNXT		* get the interrupt TCB pointer
		andi.w	#NOT_SWT,FLAGS(RNXT)	* turn off its wait flag
		clr.l	MT_ISEM2		* clear the interrupt semaphore
		jmp	MT_Enq			* go do the task dispatch
*
		.page
*
* MTInt3 -- Level 3 -- Analog processor interrupt
* ------    -------------------------------------
MTInt3:		ori.w	#IPL7,sr		* disable interrupts
*
		.ifne	DEBUG_I3
		movem.l	d0-d7/a0-a6,-(a7)	* DB_CMNT("MTInt3");
		move.l	#DB_msg3,-(a7)		* ...
		jsr	_DB_Cmnt		* ...
		tst.l	(a7)+			* ...
		movem.l	(a7)+,d0-d7/a0-a6	* ...
		.endc
*
		movem.l	a6-a7,-(a7)		* save a6-a7 on stack
		movea.l	_MT_CurP,RTCP		* get the current TCB pointer
		movem.l	d0-d7/a0-a5,REGS(RTCP)	* save d0-d7/a0-a5 in the TCB
		move.l	(a7)+,TCB_A6(RTCP)	* save a6 in the TCB
		move.l	(a7)+,TCB_A7(RTCP)	* save a7 in the TCB
		move.w	(a7)+,TCB_SR(RTCP)	* save sr in the TCB
		move.l	(a7)+,TCB_PC(RTCP)	* save the pc in the TCB
		move.l	a7,TCB_SP(RTCP)		* save the sp in the TCB
		andi.w	#NOT_RUN,FLAGS(RTCP)	* turn off its run flag
		move.w	PRI(RTCP),RPRI		* get the task priority in RPRI
		lea	_MT_RdyQ,RCUR		* get the ready queue pointer
		lea	MT_ITCB3,RNXT		* get the interrupt TCB pointer
		andi.w	#NOT_SWT,FLAGS(RNXT)	* turn off its wait flag
		clr.l	MT_ISEM3		* clear the interrupt semaphore
		jmp	MT_Enq			* go do the task dispatch
*
		.page
*
* MTInt4 -- Level 4 -- Timer interrupt
* ------    --------------------------
MTInt4:		ori.w	#IPL7,sr		* disable interrupts
*
		.ifne	DEBUG_I4
		movem.l	d0-d7/a0-a6,-(a7)	* DB_CMNT("MTInt4");
		move.l	#DB_msg4,-(a7)		* ...
		jsr	_DB_Cmnt		* ...
		tst.l	(a7)+			* ...
		movem.l	(a7)+,d0-d7/a0-a6	* ...
		.endc
*
		movem.l	a6-a7,-(a7)		* save a6-a7 on stack
		movea.l	_MT_CurP,RTCP		* get the current TCB pointer
		movem.l	d0-d7/a0-a5,REGS(RTCP)	* save d0-d7/a0-a5 in the TCB
		move.l	(a7)+,TCB_A6(RTCP)	* save a6 in the TCB
		move.l	(a7)+,TCB_A7(RTCP)	* save a7 in the TCB
		move.w	(a7)+,TCB_SR(RTCP)	* save sr in the TCB
		move.l	(a7)+,TCB_PC(RTCP)	* save the pc in the TCB
		move.l	a7,TCB_SP(RTCP)		* save the sp in the TCB
		andi.w	#NOT_RUN,FLAGS(RTCP)	* turn off its run flag
		move.w	PRI(RTCP),RPRI		* get the task priority in RPRI
		lea	_MT_RdyQ,RCUR		* get the ready queue pointer
		lea	MT_ITCB4,RNXT		* get the interrupt TCB pointer
		andi.w	#NOT_SWT,FLAGS(RNXT)	* turn off its wait flag
		clr.l	MT_ISEM4		* clear the interrupt semaphore
		jmp	MT_Enq			* go do the task dispatch
*
		.page
*
* MTInt5 -- Level 5 -- Serial I/O interrupt
* ------    -------------------------------
MTInt5:		ori.w	#IPL7,sr		* disable interrupts
*
		.ifne	DEBUG_I5
		movem.l	d0-d7/a0-a6,-(a7)	* DB_CMNT("MTInt5");
		move.l	#DB_msg5,-(a7)		* ...
		jsr	_DB_Cmnt		* ...
		tst.l	(a7)+			* ...
		movem.l	(a7)+,d0-d7/a0-a6	* ...
		.endc
*
		movem.l	a6-a7,-(a7)		* save a6-a7 on stack
		movea.l	_MT_CurP,RTCP		* get the current TCB pointer
		movem.l	d0-d7/a0-a5,REGS(RTCP)	* save d0-d7/a0-a5 in the TCB
		move.l	(a7)+,TCB_A6(RTCP)	* save a6 in the TCB
		move.l	(a7)+,TCB_A7(RTCP)	* save a7 in the TCB
		move.w	(a7)+,TCB_SR(RTCP)	* save sr in the TCB
		move.l	(a7)+,TCB_PC(RTCP)	* save the pc in the TCB
		move.l	a7,TCB_SP(RTCP)		* save the sp in the TCB
		andi.w	#NOT_RUN,FLAGS(RTCP)	* turn off its run flag
		move.w	PRI(RTCP),RPRI		* get the task priority in RPRI
		lea	_MT_RdyQ,RCUR		* get the ready queue pointer
		lea	MT_ITCB5,RNXT		* get the interrupt TCB pointer
		andi.w	#NOT_SWT,FLAGS(RNXT)	* turn off its wait flag
		clr.l	MT_ISEM5		* clear the interrupt semaphore
		jmp	MT_Enq			* go do the task dispatch
*
		.ifne	DEBUGGER
*
		.data
*
DB_msg1:	dc.b	'MTInt1',0
DB_msg2:	dc.b	'MTInt2',0
DB_msg3:	dc.b	'MTInt3',0
DB_msg4:	dc.b	'MTInt4',0
DB_msg5:	dc.b	'MTInt5',0
*
		.endc
*
		.end
