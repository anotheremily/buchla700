*
* alrem.s -- indirect long modulus
* Version 1 -- 1989-01-24 -- D.N. Lynx Crowe
*
		.text
*
		.xdef	alrem
		.xdef	_alrem
*
		.xref	_ldiv
		.xref	_ldivr
*
alrem:
_alrem:
*
		link	a6,#-2
		move.l	12(a6),-(a7)
		movea.l	8(a6),a0
		move.l	(a0),-(a7)
		jsr	_ldiv
		cmpm.l	(a7)+,(a7)+
		move.l	_ldivr,d0
		movea.l	8(a6),a1
		move.l	d0,(a1)
		unlk	a6
		rts
*
		.end
