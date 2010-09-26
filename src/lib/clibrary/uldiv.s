* ------------------------------------------------------------------------------
* uldiv.s -- unsigned long division, with remainder
* Version 2 -- 1987-06-08 -- D.N. Lynx Crowe
* Lifted from the Alcyon C library by disassembly so I could fix a bug -
*	_uldivr must be in the bss segment so the code will work in PROM.
*
*	long
*	uldiv(dividnd, divisor)
*	long dividnd, divisor;
*
*	extern long uldivr;
*
*		Divides 'dividnd' by 'divisor', treating both as unsigned
*		long integers.  Returns the quotient and leaves the
*		remainder in 'uldivr'.  Produces a divide check on division
*		by zero, with $80000000 returned for both quotient and
*		remainder.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_uldiv,_uldivr
*
DIVIDEND	.equ	8
DIVISOR		.equ	12
*
_uldiv:		link	a6,#0			* Link stack frames
		movem.l	d3-d7,-(a7)		* Save registers
		move.l	DIVIDEND(a6),d7		* d7 = DIVIDEND
		move.l	DIVISOR(a6),d6		* d6 = DIVISOR
		tst.l	d6			* Divide by zero ?
		bne	notdzero		* Jump if not
*
		move.l	#$80000000,_uldivr	* Force error result
		move.l	#$80000000,d0		* ... by dividing
		divu	#0,d0			* ... by zero
		bra	ulexit			* ... then exit
*
notdzero:	cmp.l	d7,d6			* Divide underflow ?
		bls	notunflo		* Jump if not
*
		move.l	d7,_uldivr		* Remainder = dividend
		clr.l	d0			* Quotient = 0
		bra	ulexit			* Exit
*
notunflo:	cmp.l	d6,d7			* Is dividend = divisor ?
		bne	startdiv		* Go start dividing if not
*
		moveq.l	#1,d5			* Quotient = 1
		clr.l	d7			* Remainder = 0
		bra	setreslt		* Go set result
*
		.page
*
startdiv:	moveq.l	#1,d4			* Set result bit in d4
*
divloop1:	cmp.l	d6,d7			* Divisor aligned OK ?
		bcs	divloop2		* Jump if so
*
		move.l	d6,d0			* Can we align things better ?
		asl.l	#1,d0			* ...
		cmp.l	d0,d6			* ...
		bhi	divloop2		* Jump if not
*
		asl.l	#1,d6			* Shift the divisor
		asl.l	#1,d4			* Shift the result bit
		bra	divloop1		* Loop for next bit
*
divloop2:	clr.l	d5			* Clear quotient
*
divloop3:	tst.l	d4			* More bits to do ?
		beq	setreslt		* Go set result if not
*
		cmp.l	d6,d7			* Can we subtract ?
		bcs	divloop4		* Jump if not
*
		or.l	d4,d5			* Set a bit in the quotient
		sub.l	d6,d7			* Subtract divisor from dividend
*
divloop4:	lsr.l	#1,d4			* Shift the result bit
		lsr.l	#1,d6			* Shift the divisor
		bra	divloop3		* Loop for next bit
*
setreslt:	move.l	d7,_uldivr		* Store remainder
		move.l	d5,d0			* Put quotient in d0
*
ulexit:		tst.l	(a7)+			* Discard top of stack
		movem.l	(a7)+,d4-d7		* Restore registers
		unlk	a6			* Unlink stack frames
		rts				* Return to caller
*
		.bss
		.even
*
_uldivr:	ds.l	1
*
		.end
