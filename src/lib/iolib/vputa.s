* ------------------------------------------------------------------------------
* vputa.s -- store character attributes in video RAM
* Version 1 -- 1988-03-14 -- D.N. Lynx Crowe
* (c) Copyright 1988 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
*
*	vputa(sbase, row, col, attrib)
*	unsigned int *sbase, row, col, attrib;
*
*		Stores attribute value 'attrib' for the character
*		located at ('row','col') in VSDD RAM starting at 'sbase'.
*		Assumes a 64 character line and full character attributes.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_vputa
*
SBASE		.equ	8		* LONG - 'sbase'
ROW		.equ	12		* WORD - 'row'
COL		.equ	14		* WORD - 'col'
ATTR		.equ	16		* WORD - 'attrib'
*
_vputa:		link	a6,#0		* Link stack frame pointer
		clr.l	d0		* Clear out d0
		move.w	ROW(a6),d0	* Get row
		lsl.l	#6,d0		* Multiply by 64  (shift left 6)
		move.w	COL(a6),d1	* Get col
		andi.l	#$0000003F,d1	* Mask down to 6 bits
		or.l	d1,d0		* OR into d0 to get char. #
		move.w	d0,d1		* Develop cw = (cn/2)*6 in d1
		andi.l	#$FFFFFFFE,d1	* ...
		move.l	d1,d2		* ...
		add.l	d1,d1		* ...
		add.l	d2,d1		* ...
		add.l	SBASE(a6),d1	* Add sbase to cw
		movea.l	d1,a0		* a0 points at the word with the char.
		btst.l	#0,d0		* Odd char. location ?
		bne	vputa1		* Jump if so
*
		move.w	ATTR(a6),2(a0)	* Store new attribute word in video RAM
*
vputax:		unlk	a6		* Unlink the stack frame
		rts			* Done -- return to caller
*
vputa1:		move.w	ATTR(a6),4(a0)	* Store new attribute word in video RAM
		bra	vputax		* Done -- go return to caller
*
		.end
