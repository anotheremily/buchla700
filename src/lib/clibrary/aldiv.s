*
* aldiv.s -- indirect long divide
* Version 1 -- 1989-01-24 -- D.N. Lynx Crowe
*
	.text
*
	.xdef	aldiv
	.xdef	_aldiv
*
	.xref	_ldiv
*
aldiv:
_aldiv:
*
	link	a6,#-2
	move.l	12(a6),-(a7)
	movea.l	8(a6),a0
	move.l	(a0),-(a7)
	jsr	_ldiv
	cmpm.l	(a7)+,(a7)+
	movea.l	8(a6),a1
	move.l	d0,(a1)
	unlk	a6
	rts
*
	.end
