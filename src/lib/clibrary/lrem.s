* ------------------------------------------------------------------------------
* lrem.s -- long modulo
* Version 1 -- 1988-01-22
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_lrem
		.xdef	lrem
*
		.xref	ldiv
		.xref	_ldivr
*
ARG1		.equ	8
ARG2		.equ	12
*
_lrem:
lrem:		link	a6,#-2
		move.l	ARG2(a6),-(a7)
		move.l	ARG1(a6),-(a7)
		jsr	ldiv
		cmpm.l	(a7)+,(a7)+
		move.l	_ldivr,d0
		unlk	a6
		rts
*
		.end
