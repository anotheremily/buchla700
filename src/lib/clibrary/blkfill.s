* ------------------------------------------------------------------------------
* blkfill.s -- block fill function
* Version 1 -- 1987-08-28 -- D.N. Lynx Crowe
*
*	void
*	blkfill(where, what, count)
*	char *where;
*	char what;
*	short count;
*
*		Fills 'count' bytes at 'where' with 'what'.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_blkfill
*
_blkfill:	link	a6,#0
		movem.l	d5-d7/a5-a5,-(a7)
		movea.l	8(a6),a5
		move.b	13(a6),d7
		move.w	14(a6),d6
		bra	blkf2
*
blkf1:		move.b	d7,(a5)+
*
blkf2:		move.w	d6,d0
		subq.w	#1,d6
		tst.w	d0
		bgt	blkf1
*
		tst	(a7)+
		movem.l	(a7)+,d6-d7/a5-a5
		unlk	a6
		rts
*
		.end
