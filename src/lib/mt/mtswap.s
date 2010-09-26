* ------------------------------------------------------------------------------
* mtswap.s -- Multi-tasker -- swapper
* Version 24 -- 1988-04-16 -- D.N. Lynx Crowe
* (c) Copyright 1988 -- D.N. Lynx Crowe
* Machine cycles are given as the first number in the comments for timing.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	__MT_Swp
		.xdef	__MT_Nxt
*
		.xdef	MT_Enq
*
		.xref	_MT_STpc
		.xref	_MT_NTpc
		.xref	_MT_LTCB
*
		.xref	_MT_TCBs		* to force loading
		.xref	_MT_CurP
		.xref	_MT_RdyQ
*
		.page
*
* TCB offsets
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
TOS		.equ	FLAGS+2			* LONG - top of stack pointer
*
TCB_A6		.equ	REGS+56			* LONG - task a6 image
TCB_A7		.equ	REGS+60			* LONG - task a7 image
*
MTF_RDY		.equ	$0001			* 'ready' bit
NOT_RDY		.equ	$FFFE			* 'ready' bit complement
MTF_SWT		.equ	$0002			* 'wait' bit
NOT_SWT		.equ	$FFFD			* 'wait' bit complement
MTF_RUN		.equ	$0004			* 'run' bit
NOT_RUN		.equ	$FFFB			* 'run' bit complement
MTF_STP		.equ	$0008			* 'stop' bit
NOT_STP		.equ	$FFF7			* 'stop' bit complement
*
IPL7		.equ	$0700			* processor IPL 7 mask
*
RPRV		.equ	a1			* previous TCB pointer
RCUR		.equ	a2			* current TCB pointer
RNXT		.equ	a3			* next TCB pointer
RTCP		.equ	a6			* general TCB pointer
*
RPRI		.equ	d0			* task priority
RTST		.equ	d1			* test register
*
TRAP_SWP	.equ	8			* swapper TRAP number 1
TRAP_NXT	.equ	9			* swapper TRAP number 2
*
		.page
*
* _MT_Swp -- swap tasks
* -------    ----------
__MT_Swp:	ori.w	#IPL7,sr		*    8 disable interrupts
		tst.l	_MT_RdyQ		*   20 see if anything is queued
		beq	MT_runit		* 8/10 re-run current task if not
*
		movem.l	a6-a7,-(a7)		*   28 save a6,a7 on stack
		movea.l	_MT_CurP,RTCP		*   20 get current TCB pointer
		movem.l	d0-d7/a0-a5,REGS(RTCP)	*  152 put task d0-d7/a0-a5 in TCB
		move.l	(a7)+,TCB_A6(RTCP)	*   26 put task a6 in TCB
		move.l	(a7)+,TCB_A7(RTCP)	*   26 put task a7 in TCB
		move.w	(a7)+,TCB_SR(RTCP)	*   17 put task sr in TCB
		move.l	(a7)+,TCB_PC(RTCP)	*   26 put task pc in TCB
		move.l	a7,TCB_SP(RTCP)		*   18 put task sp in TCB
		andi.w	#NOT_RUN,FLAGS(RTCP)	*   21 turn off' run' bit
		move.w	PRI(RTCP),RPRI		*   12 get task priority
		lea	_MT_RdyQ,RCUR		*    8 point at ready queue
		movea.l	(RCUR),RNXT		*   12 get highest priority TCB
		move.l	(RNXT),(RCUR)		*   13 dequeue the TCB
		andi.w	#NOT_RDY,FLAGS(RNXT)	*   21 turn off 'ready' bit
*
MT_Enq:		movea.l	RCUR,RPRV		*    4 point at previous TCB
		movea.l	(RPRV),RCUR		*   10 get pointer to next TCB
		move.l	RCUR,RTST		*    4 test pointer
		beq	enq1			* 8/10 enqueue now if pointer is NIL
*
		cmp.w	PRI(RCUR),RPRI		*   12 compare priorities
		bls	MT_Enq			* 8/10 jump if RPRI LE table TCB
*
enq1:		move.l	RTCP,(RPRV)		*    9 set NEXT of previous TCB
		move.l	RTCP,_MT_LTCB		*   22 log last TCB swapped out
		move.l	RCUR,(RTCP)		*    9 set NEXT of rescheduled TCB
		ori.w	#MTF_RDY,FLAGS(RTCP)	*   21 turn on 'ready' bit
		movea.l	RNXT,RTCP		*    4 get new TCB pointer
		move.l	RTCP,_MT_CurP		*   22 make it the current TCB
		clr.l	(RTCP)			*   22 clear NEXT of current TCB
		ori.w	#MTF_RUN,FLAGS(RTCP)	*   21 turn on 'run' bit
		move.l	TCB_PC(RTCP),_MT_STpc	*   25 log dispatch address
		movea.l	TCB_SP(RTCP),a7		*   16 get task sp
		move.l	TCB_PC(RTCP),-(a7)	*   26 put task pc on the stack
		move.w	TCB_SR(RTCP),-(a7)	*   26 put task sr on the stack
		movem.l	REGS(RTCP),d0-d7/a0-a5	*  128 restore task d0-d7/a0-a5
		movea.l	TCB_A6(RTCP),a6		*   26 restore task a6
*
MT_runit:	rte				*   20 (re)start the task
*
		.page
*
* __MT_Nxt -- start the first task in the ready queue
* --------    ---------------------------------------
__MT_Nxt:	ori.w	#IPL7,sr		*    8 disable interrupts
		movem.l	a6-a7,-(a7)		*   28 save a6,a7 on stack
		movea.l	_MT_CurP,RTCP		*   20 get current TCB pointer
		move.l	RTCP,_MT_LTCB		*   22 log last TCB swapped out
		movem.l	d0-d7/a0-a5,REGS(RTCP)	*  152 put task d0-d7/a0-a5 in TCB
		move.l	(a7)+,TCB_A6(RTCP)	*   26 put task a6 in TCB
		move.l	(a7)+,TCB_A7(RTCP)	*   26 put task a7 in TCB
		move.w	(a7)+,TCB_SR(RTCP)	*   17 put task sr in TCB
		move.l	(a7)+,TCB_PC(RTCP)	*   26 put task pc in TCB
		move.l	a7,TCB_SP(RTCP)		*   18 put task sp in TCB
		andi.w	#NOT_RUN,FLAGS(RTCP)	*   21 turn off' run' bit
		lea	_MT_RdyQ,RCUR		*    8 point at ready queue
		movea.l	(RCUR),RNXT		*   12 get highest priority TCB
		move.l	(RNXT),(RCUR)		*   13 dequeue the TCB
		andi.w	#NOT_RDY,FLAGS(RNXT)	*   21 turn off 'ready' bit
		movea.l	RNXT,RTCP		*    4 get new TCB pointer
		move.l	RTCP,_MT_CurP		*   22 make it the current TCB
		clr.l	(RTCP)			*   22 clear NEXT of current TCB
		ori.w	#MTF_RUN,FLAGS(RTCP)	*   21 turn on 'run' bit
		move.l	TCB_PC(RTCP),_MT_NTpc	*   25 log dispatch address
		movea.l	TCB_SP(RTCP),a7		*   16 get task sp
		move.l	TCB_PC(RTCP),-(a7)	*   26 put task pc on the stack
		move.w	TCB_SR(RTCP),-(a7)	*   26 put task sr on the stack
		movem.l	REGS(RTCP),d0-d7/a0-a5	*  128 restore task d0-d7/a0-a5
		movea.l	TCB_A6(RTCP),a6		*   26 restore task a6
		rte				*   20 start the task
*
		.end
