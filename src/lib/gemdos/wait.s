*
* wait.s -- Wait for a CR on CON:
* Version 1 -- 1987-02-05 -- D.N. Lynx Crowe
*
* This is hand optimized C compiler output, which is why it looks so strange
*
		.text
*
_main:		link	a6,#0
		movem.l	d6-d7,-(sp)
*
		move.l	#L6,(sp)
		jsr	_Bconws
*
L8:		move	#2,(sp)
		move	#2,-(sp)
		trap	#13
		addq.l	#2,sp
*
		cmp.b	#$d,d0
		bne	L8
*
L7:		clr	(sp)
		trap	#1
*
_Bconws:	link	a6,#0
		movem.l	d6-d7,-(sp)
*
		bra	L4
*
L3:		move.w	d0,(sp)
		move.w	#2,-(sp)
		move.w	#3,-(sp)
		trap	#13
		addq.l	#4,sp
*
L4:		move.l	8(a6),a0
		move.b	(a0),d0
		add.l	#1,8(a6)
*
		cmpi.b	#0,d0
		bne	L3
*
		tst.l	(sp)+
		movem.l	(sp)+,d7-d7
		unlk	a6
		rts
*
		.data
*
L6:		.dc.b	$07
		.dc.b	'   <<<<< Waiting for CR from CON: >>>>>'
		.dc.b	$00
*
		.end
