* ------------------------------------------------------------------------------
* smsig.s -- Multi-Tasker -- SM_Sig and SM_Wait for assembly language routines
* Version 22 -- 1988-04-17 -- D.N. Lynx Crowe
* (c) Copyright 1988 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
*
		.text
*
		.xdef	SM_Sig
		.xdef	SM_Wait
*
		.xref	_MT_RdyQ
		.xref	_MT_CurP
*
		.xref	_MT_LSpc
		.xref	_MT_LWpc
*
* Register equates
* ----------------
RSEM		.equ	D7			* semaphore contents
RIPL		.equ	D6			* saved IPL
RPRI		.equ	D5			* task priority
RTMP		.equ	D4			* temporary
*
RTCP		.equ	A5			* TCB pointer
RPRV		.equ	A4			* previous TCB pointer
RCUR		.equ	A3			* current TCB pointer
RPTR		.equ	A2			* temporary TCB pointer
*
* Miscellaneous equates
* ---------------------
IPL7		.equ	$0700			* processor IPL 7 mask
TRAP_NXT	.equ	9			* swapper TRAP number 2
STP_BIT		.equ	3			* stop bit number  (LS byte of flags)
*
* Parameter offsets
* -----------------
PSEM		.equ 	8			* LONG -- pointer to semaphore
*
		.page
*
* TCB definitions
* ---------------
NEXT		.equ	0			* LONG - next TCB in queue
FWD		.equ	NEXT+4			* LONG - next TCB in chain
TID		.equ	FWD+4			* WORD - task ID
PRI		.equ	TID+2			* WORD - task priority
SLICE		.equ	PRI+2			* LONG - slice time limit
TCB_REG		.equ	SLICE+4			* LONG[16] - registers
TCB_SP		.equ	TCB_REG+64		* LONG - stack pointer
TCB_PC		.equ	TCB_SP+4		* LONG - program counter
TCB_SR		.equ	TCB_PC+4		* WORD - status register
FLAGS		.equ	TCB_SR+2		* WORD - task flags
TOS		.equ	FLAGS+2			* LONG - top of stack pointer
*
MTF_RDY		.equ	$0001			* 'ready' bit
*
MTF_SWT		.equ	$0002			* 'wait' bit
NOT_SWT		.equ	$FFFD			* 'wait' bit complement
*
		.page
*
* SM_Sig -- signal a semaphore
* ------    ------------------
SM_Sig:		link	a6,#0			* link stack frames
		move.l	4(a6),_MT_LSpc		* log last PC
		movem.l	RTMP-RSEM/RPTR-RTCP,-(sp)	* save registers
		move.w	sr,RIPL			* save interrupt level
		ori.w	#IPL7,sr		* DISABLE INTERRUPTS
		movea.l	PSEM(a6),RPTR		* get pointer to semaphore
*
L00:		move.l	(RPTR),RSEM		* get semaphore contents
		beq	L13			* jump if empty
*
		btst	#0,RSEM			* check for waiting tasks
		beq	L10			* jump if something waiting
*
		move.l	RSEM,RTMP		* check for maximum count
		andi.l	#$FFFFFFFE,RTMP		* ...
		cmpi.l	#$FFFFFFFE,RTMP		* ...
		beq	L9			* jump if limit hit
*
		addq.l	#2,RSEM			* add 1 to semaphore count
		bra	L12			* go update semaphore
*
L10:		movea.l	RSEM,RTCP		* point at waiting task TCB
		move.l	(RTCP),RSEM		* update semaphore
		clr.l	(RTCP)			* clear NEXT of TCB
		andi.w	#NOT_SWT,FLAGS(RTCP)	* turn off TCB wait bit
		btst.b	#STP_BIT,FLAGS+1(RTCP)	* check stop bit
		beq	L01			* jump if not set
*
		bclr.b	#STP_BIT,FLAGS+1(RTCP)	* clear stop bit
		bra	L00			* go check for another task
*
L01:		move.w	PRI(RTCP),RPRI		* setup to put TCB on ready queue
		lea	_MT_RdyQ,RCUR		* ...
*
L15:		movea.l	RCUR,RPRV		* follow TCB chain
		movea.l	(RPRV),RCUR		* ...
		move.l	RCUR,RTMP		* ...
		beq	L14			* jump if end of chain
*
		cmp.w	PRI(RCUR),RPRI		* is this the place ?
		bls	L15			* loop to next one if not
*
L14:		move.l	RTCP,(RPRV)		* put TCB on ready queue
		move.l	RCUR,(RTCP)		* ...
		ori.w	#MTF_RDY,FLAGS(RTCP)	* set TCB ready bit
		bra	L12
*
L13:		moveq.l	#3,RSEM			* set semaphore count = 1
*
L12:		move.l	RSEM,(RPTR)		* update semaphore
*
L9:		move.w	RIPL,sr			* RESTORE INTERRUPTS
		movem.l	(sp)+,RTMP-RSEM/RPTR-RTCP	* restore registers
		unlk	a6			* unlink stack frames
		rts
*
		.page
*
* SM_Wait -- wait on a semaphore
* -------    -------------------
SM_Wait:	link	a6,#0			* link stack frames
		move.l	4(a6),_MT_LWpc		* log last PC
		movem.l	RTMP-RSEM/RPTR-RCUR,-(sp)	* save registers
		move.w	sr,RIPL			* save interrupt level
		ori.w	#IPL7,sr		* DISABLE INTERRUPTS
		movea.l	PSEM(a6),RPTR		* get semaphore pointer
		move.l	(RPTR),RSEM		* get semaphore contents
		beq	L19			* jump if not signalled yet
*
		btst.l	#0,RSEM			* see if anything is waiting
		beq	L19			* jump if so
*
		move.l	RSEM,RTMP		* check semaphore count
		andi.l	#$FFFFFFFE,RTMP		* ...
		beq	L20			* jump if not signalled yet
*
		subq.l	#2,RSEM			* decrement semaphore count
		move.l	RSEM,(RPTR)		* update semaphore
		bra	L18			* exit
*
L20:		clr.l	(RPTR)			* make SEM a NIL pointer
*
L19:		movea.l	_MT_CurP,RCUR		* point at current TCB
		clr.l	(RCUR)			* clear NEXT of current TCB
		ori.w	#MTF_SWT,FLAGS(RCUR)	* indicate we're waiting
*
L30:		tst.l	(RPTR)			* pointing at SEM queue end ?
		beq	L31			* jump if so
*
		movea.l	(RPTR),RPTR		* point at next TCB in SEM queue
		bra	L30			* keep searching for end of queue
*
L31:		move.l	RCUR,(RPTR)		* set NEXT of end of queue
		trap	#TRAP_NXT		* swap to next ready task
*
L18:		move.w	RIPL,sr			* RESTORE INTERRUPTS
		movem.l	(sp)+,RTMP-RSEM/RPTR-RCUR	* restore registers
		unlk	a6			* unlink stack frames
		rts				* return to caller
*
		.end
