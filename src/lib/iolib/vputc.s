* ------------------------------------------------------------------------------
* vputc.s -- store a character and attributes in video RAM
* Version 3 -- 1987-03-30 -- D.N. Lynx Crowe
* (c) Copyright 1987 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
*
*	vputc(sbase, row, col, c, attrib)
*	unsigned int *sbase, row, col, c, attrib;
*
*		Stores character c at (row,col) in sbase with
*		attribute value attrib.
* ------------------------------------------------------------------------------
*
		.text
*
		.xdef	_vputc
*
_vputc:		link	a6,#0		* Link stack frame pointer
		clr.l	d0		* Clear out d0
		move.w	12(a6),d0	* Get row
		lsl.l	#6,d0		* Multiply by 64  (shift left 6)
		move.w	14(a6),d1	* Get col
		andi.l	#$0000003F,d1	* Mask down to 6 bits
		or.l	d1,d0		* OR into d0 to get char. #
		move.w	d0,d1		* Develop cw = (cn/2)*6 in d1
		andi.l	#$FFFFFFFE,d1	* ...
		move.l	d1,d2		* ...
		lsl.l	#1,d1		* ...
		add.l	d2,d1		* ...
		add.l	8(a6),d1	* Add sbase to cw
		movea.l	d1,a0		* a0 points at the word with the char.
		btst.l	#0,d0		* Odd char. location ?
		bne	vputc1		* Jump if so
*
		move.w	16(a6),d0	* Get ch
		andi.w	#$00FF,d0	* Mask off garbage bits
		move.w	(a0),d1		* Get word from video RAM
		andi.w	#$FF00,d1	* Mask off old even character
		or.w	d0,d1		* OR in the new character
		move.w	d1,(a0)+	* Store the updated word in video RAM
		move.w	18(a6),(a0)	* Store new attribute word in video RAM
*
vputcx:		unlk	a6		* Unlink the stack frame
		rts			* Return to caller
*
vputc1:		move.w	16(a6),d0	* Get ch
		lsl.w	#8,d0		* Shift to high (odd) byte
		move.w	(a0),d1		* Get word from video RAM
		andi.w	#$00FF,d1	* Mask off old odd character
		or.w	d0,d1		* OR in the new character
		move.w	d1,(a0)+	* Store the updated word in video RAM
		addq.l	#2,a0		* Point at the attribute word
		move.w	18(a6),(a0)	* Store new attributes in video RAM
		bra	vputcx		* Done -- go return to caller
*
		.end
