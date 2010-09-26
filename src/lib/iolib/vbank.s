* ------------------------------------------------------------------------------
* vbank.s -- VSDD bank switching control functions
* Version 3 -- 1989-12-19 -- D.N. Lynx Crowe
*
*	unsigned
*	vbank(b)
*	unsigned b;
*
*		Set VSDD Data Segment bank to b.
*		Return old bank select value.
*
*
*	vfwait()
*
*		Wait for a FRAMESTOP update to transpire.
* ------------------------------------------------------------------------------
		.xdef	_vbank
		.xdef	_vfwait
*
		.xref	_v_regs
*
		.text
*
B		.equ	8
*
OLDB		.equ	d6
NEWB		.equ	d7
*
VSDD_R5		.equ	10
VSDD_R11	.equ	22
*
VT_BASE		.equ	128		* word offset of VSDD Access Table
*
VT_1		.equ	VT_BASE+300	* high time
VT_2		.equ	VT_BASE+2	* low time
*
		.page
* ------------------------------------------------------------------------------
*	vbank(b) -- change VSDD Data Segment bank to b.  Return old bank.
*	Assumes a 128K byte window, sets S15..S11 to zero.
* ------------------------------------------------------------------------------
*

_vbank:		link	a6,#0			* link stack frames
		movem.l	d5-d7,-(sp)		* preserve registers
		move.w	_v_regs+VSDD_R5,OLDB	* get v_regs[5]
		lsr.w	#6,OLDB			* extract BS bits
		move.w	OLDB,d0			* ...
		andi.w	#2,d0			* ...
		move.w	OLDB,d1			* ...
		lsr.w	#2,d1			* ...
		andi.w	#1,d1			* ...
		or.w	d1,d0			* ...
		move.w	d0,OLDB			* ...
		cmp.w	B(a6),OLDB		* see if they're what we want
		bne	L2			* jump if not
*
		move.w	B(a6),d0		* setup to return b
		bra	L1
*
L2:		move.w	B(a6),NEWB		* get bank bits from b
		lsl.w	#6,NEWB			* shift bits from b into BS bits
		move.w	NEWB,d0			* ...
		andi.w	#128,d0			* ...
		lsl.w	#2,NEWB			* ...
		andi.w	#256,NEWB		* ...
		or.w	NEWB,d0			* ...
		move.w	d0,_v_regs+VSDD_R5	* set v_regs[5] with new BS bits
*
vw1b:		cmp.w	#VT_1,_v_regs+VSDD_R11	* wait for FRAMESTOP
		bcc	vw1b			* ...
*
vw2b:		cmp.w	#VT_1,_v_regs+VSDD_R11	* ...
		bcs	vw2b			* ...
*
vw3b:		cmp.w	#VT_1,_v_regs+VSDD_R11	* ...
		bcc	vw3b			* ...
*
vw4b:		cmp.w	#VT_2,_v_regs+VSDD_R11	* ...
		bcs	vw4b			* ...
*
		move.w	OLDB,d0			* setup to return OLDB
*
L1:		tst.l	(sp)+			* fixup stack
		movem.l	(sp)+,OLDB-NEWB		* restore registers
		unlk	a6			* unlink stack frames
		rts				* return to caller
*
		.page
*
* ------------------------------------------------------------------------------
*	vfwait() -- Wait for a FRAMESTOP update to transpire.
* ------------------------------------------------------------------------------
*
_vfwait:	link	a6,#0			* link stack frames
*
vw1a:		cmp.w	#VT_1,_v_regs+VSDD_R11	* wait for FRAMESTOP
		bcc	vw1a			* ...
*
vw2a:		cmp.w	#VT_1,_v_regs+VSDD_R11	* ...
		bcs	vw2a			* ...
*
vw3a:		cmp.w	#VT_1,_v_regs+VSDD_R11	* ...
		bcc	vw3a			* ...
*
vw4a:		cmp.w	#VT_2,_v_regs+VSDD_R11	* ...
		bcs	vw4a			* ...
*
		unlk	a6			* unlink stack frames
		rts				* return to caller
*
		.end
