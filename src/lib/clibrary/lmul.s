* ------------------------------------------------------------------------------
* lmul.s -- long multiply
* Version 2 -- 1989-07-18
* ------------------------------------------------------------------------------
		.text
*
		.xdef	lmul
*
ARG1		.equ	8
ARG2		.equ	12
*
TEMP		.equ	-4
*
lmul:		link	a6,#-4
		clr.w	d2
		tst.l	ARG1(a6)
		bge	lmul1
*
		neg.l	ARG1(a6)
		addq.w	#1,d2
*
lmul1:		tst.l	ARG2(a6)
		bge	lmul2
*
		neg.l	ARG2(a6)
		addq.w	#1,d2
*
lmul2:		move.w	ARG1+2(a6),d0
		mulu	ARG2+2(a6),d0
		move.l	d0,TEMP(a6)
		move.w	ARG1(a6),d0
		mulu	ARG2+2(a6),d0
		move.w	ARG2(a6),d1
		mulu	ARG1+2(a6),d1
		add.w	d1,d0
		add.w	TEMP(a6),d0
		move.w	d0,TEMP(A6)
		move.l	TEMP(a6),d0
		btst	#0,d2
		beq	lmul3
*
		neg.l	d0
*
lmul3:		unlk	a6
		rts
*
		.end
