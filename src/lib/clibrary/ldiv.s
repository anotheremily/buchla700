* ------------------------------------------------------------------------------
* ldiv.s -- long division
* Version 1 -- 1988-01-22
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_ldiv
		.xdef	ldiv
*
		.xdef	_ldivr
*
ARG1		.equ	8
ARG2		.equ	12
*
_ldiv:
ldiv:		link	a6,#-2
		movem.l	d2-d7,-(a7)
		clr.w	d3
		clr.l	d5
		move.l	ARG1(a6),d7
		move.l	ARG2(a6),d6
		bne	ldiv1
*
		move.l	#$80000000,_ldivr
		move.l	#$80000000,d0
		divs	#0,d0
		bra	ldiv11
*
ldiv1:		bge	ldiv2
*
		neg.l	d6
		addq.w	#1,d3
*
ldiv2:		tst.l	d7
		bge	ldiv3
*
		neg.l	d7
		addq.w	#1,d3
*
ldiv3:		cmp.l	d7,d6
		bgt	ldiv9
*
		bne	ldiv4
*
		moveq.l	#1,d5
		clr.l	d7
		bra	ldiv9
*
ldiv4:		cmp.l	#$10000,d7
		bge	ldiv5
*
		divu	d6,d7
		move.w	d7,d5
		swap	d7
		ext.l	d7
		bra	ldiv9
*
ldiv5:		moveq.l	#1,d4
*
ldiv6:		cmp.l	d6,d7
		bcs	ldiv7
*
		asl.l	#1,d6
		asl.l	#1,d7
		bra	ldiv6
*
ldiv7:		tst.l	d4
		beq	ldiv9
*
		cmp.l	d6,d7
		bcs	ldiv8
*
		or.l	d4,d5
		sub.l	d6,d7
*
ldiv8:		lsr.l	#1,d4
		lsr.l	#1,d6
		bra	ldiv7
*
ldiv9:		cmp.w	#1,d3
		bne	ldiv10
*
		neg.l	d7
		move.l	d7,_ldivr
		move.l	d5,d0
		neg.l	d0
		bra	ldiv11
*
ldiv10:		move.l	d7,_ldivr
		move.l	d5,d0
*
ldiv11:		tst.l	(a7)+
		movem.l	(a7)+,d3-d7
		unlk	a6
		rts
*
		.bss
*
_ldivr:		.ds.l	1
*
		.end
