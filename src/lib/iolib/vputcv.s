* ------------------------------------------------------------------------------
* vputcv.s -- store a character and attributes in video RAM
* Version 1 -- 1988-10-05 -- D.N. Lynx Crowe
* (c) Copyright 1988 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
*
*	vputcv(adr, row, col, chr, atr, cols)
*	unsigned int *adr, row, col, chr, atr, cols;
*
*		Stores character 'chr' at ('row', 'col') in the
*		full attribute text object at 'adr' with
*		attribute value 'atr' using a line length of 'len'.
* ------------------------------------------------------------------------------
*
		.text
*
		.xdef	_vputcv
*
P_ADR		.equ	8
P_ROW		.equ	12
P_COL		.equ	14
P_CHR		.equ	16
P_ATR		.equ	18
P_LEN		.equ	20
*
		.page
*
_vputcv:	link	a6,#0		* Link stack frame pointer
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
		bne	vputcv1		* Jump if so
*
		move.w	P_CHR(a6),d0	* Get chr
		andi.w	#$00FF,d0	* Mask off garbage bits
		move.w	(a0),d1		* Get word from video RAM
		andi.w	#$FF00,d1	* Mask off old even character
		or.w	d0,d1		* OR in the new character
		move.w	d1,(a0)+	* Store the updated word in video RAM
		bra	vputcvx		* Done -- go return to caller
*
vputcv1:	move.w	P_CHR(a6),d0	* Get chr
		lsl.w	#8,d0		* Shift to high (odd) byte
		move.w	(a0),d1		* Get word from video RAM
		andi.w	#$00FF,d1	* Mask off old odd character
		or.w	d0,d1		* OR in the new character
		move.w	d1,(a0)+	* Store the updated word in video RAM
		addq.l	#2,a0		* Point at the attribute word
*
vputcvx:	move.w	P_ATR(a6),(a0)	* Store new attributes in video RAM
		unlk	a6		* Unlink the stack frame
		rts			* Return to caller
*
		.end
