*
* almul.s -- indirect long multiply
* Version 1 -- 1989-01-24 -- D.N. Lynx Crowe
*
		.text
*
		.xdef	almul
*
		.xref	lmul
*
almul:		move.l	a5,-(a7)
		movea.l	8(a7),a5
		move.l	12(a7),-(a7)
		move.l	(a5),-(a7)
		jsr	lmul
		addq.w	#8,a7
		move.l	d0,(a5)
		movea.l	(a7)+,a5
		rts
*
		.end
