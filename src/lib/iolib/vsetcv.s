* ------------------------------------------------------------------------------
* vsetcv.s -- set a character's colors in video RAM
* Version 1 -- 1988-10-11 -- D.N. Lynx Crowe
* (c) Copyright 1988 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
*
*	vsetcv(adr, row, col, cfb, len)
*	unsigned int *adr, row, col, cfb, len;
*
*		Sets colors 'cfb' at ('row', 'col') in the
*		full attribute text object at 'adr'
*		using a line length of 'len'.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_vsetcv
*
P_ADR		.equ	8
P_ROW		.equ	12
P_COL		.equ	14
P_CFB		.equ	16
P_LEN		.equ	18
*
_vsetcv:	link	a6,#0		* Link stack frame pointer
		move.w	P_ROW(a6),d0	* Get row
		mulu	P_LEN(a6),d0	* Multiply by len
		clr.l	d1		* Clear out d1
		move.w	P_COL(a6),d1	* Get col
		add.l	d1,d0		* Add col into d0 to get char. #
		move.l	d0,d1		* Develop cw = (cn/2)*6 in d1
		andi.l	#$FFFFFFFE,d1	* ...
		move.l	d1,d2		* ...
		add.l	d1,d1		* ...
		add.l	d2,d1		* ...
		add.l	P_ADR(a6),d1	* Add sbase to cw
		movea.l	d1,a0		* a0 points at the word with the char.
		btst.l	#0,d0		* Odd char. location ?
		bne	vsetcv1		* Jump if so
*
		addq.l	#2,a0		* Point at the attribute word
		bra	vsetcvx		* Go set attribute
*
vsetcv1:	addq.l	#4,a0		* Point at the attribute word
*
vsetcvx:	move.w	(a0),d0		* Get old attributes
		andi.w	#$FF00,d0	* Remove old colors
		or.w	P_CFB(a6),d0	* Get new colors
		move.w	d0,(a0)		* Set new attributes in video RAM
		unlk	a6		* Unlink the stack frame
		rts			* Return to caller
*
		.end
