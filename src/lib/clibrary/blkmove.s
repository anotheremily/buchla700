* ------------------------------------------------------------------------------
* blkmove.s -- block move function
* Version 1 -- 1987-08-28 -- D.N. Lynx Crowe
*
*	void
*	blkmove(to, from, n)
*	char *to, *from;
*	short n;
*
*		Copies 'n' bytes from address 'from' to address 'to'.
*		Treats overlaps of from and to areas intelligently.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_blkmove
*
_blkmove:	link	a6,#$FFFC
		move.l	8(a6),d0
		cmp.l	12(a6),d0
		bcc	blkm3
*
		bra	blkm2
*
blkm1:		movea.l	8(a6),a0
		movea.l	12(a6),a1
		move.b	(a1),(a0)
		addq.l	#1,8(a6)
		addq.l	#1,12(a6)
*
blkm2:		move.w	16(a6),d0
		subq.w	#1,16(a6)
		tst.w	d0
		bne	blkm1
*
		bra	blkm6
*
blkm3:		move.w	16(a6),d0
		ext.l	d0
		add.l	d0,8(a6)
		add.l	d0,12(a6)
		bra	blkm5
*
blkm4:		subq.l	#1,8(a6)
		subq.l	#1,12(a6)
		movea.l	8(a6),a0
		movea.l	12(a6),a1
		move.b	(a1),(a0)
*
blkm5:		move.w	16(a6),d0
		subq.w	#1,16(a6)
		tst.w	d0
		bne	blkm4
*
blkm6:		unlk	a6
		rts
*
		.end
