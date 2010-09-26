* ------------------------------------------------------------------------------
* vwputs.s -- output a character string to a 2-bit per pixel graphics window
* Version 9 -- 1987-07-28 -- D.N. Lynx Crowe
* (c) Copyright 1987 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
*
*	vwputs(obase, nw, fg, bg, row, col, str)
*	int *obase, nw, fg, bg, row, col;
*	char *str;
*
*		Outputs characters from the string at 'str' to an 'nw'
*		character wide 2-bit per pixel graphics window at 'obase'
*		at ('row','col'), using 'fg' as the foreground color, and
*		'bg' as the background color.  Uses cgtable[][256] as the
*		VSDD formatted character generator table.
*		No error checks are done.
* ------------------------------------------------------------------------------
*
		.text
		.xdef	_vwputs
*
		.xref	_cgtable
*
* Argument offsets from a6:
*
OBASE		.equ	8		* L:  Output area base address
NW		.equ	12		* W:  Character width of output area
FG		.equ	14		* W:  Foreground color
BG		.equ	16		* W:  Background color
ROW		.equ	18		* W:  Row
COL		.equ	20		* W:  Column
STR		.equ	22		* L:  String base address
*
* Miscellaneous constants:
*
HPIX		.equ	8		* Horizontal pixels in the character
*
NVPIX		.equ	12		* Vertical pixels in the character
VPITCH		.equ	12		* Vertical pitch between characters
*
* Register usage:
*
*	d0	output word and scratch
*	d1	CG word and scratch
*	d2	pixel counter
*	d3	foreground color (in the 2 ms bits)
*	d4	background color (in the 2 ms bits)
*	d5	width of the area in bytes
*	d6	scan line counter
*
*	a0	CG table pointer
*	a1	output area scan line pointer
*	a2	input character pointer
*	a3	output area character pointer
*
		.page
*
_vwputs:	link	a6,#0		* Link stack frames
		movem.l	d3-d6/a3,-(a7)	* Save registers we use
		move.w	#14,d1		* Set shift constant
		move.w	FG(a6),d3	* Setup foreground color
		lsl.w	d1,d3		* ... in ms 2 bits of d3
		move.w	BG(a6),d4	* Setup background color
		lsl.w	d1,d4		* ... in ms 2 bits of d4
		move.w	NW(a6),d5	* Get width in characters in d5
		lsl.w	#1,d5		* ... make it words
		move.w	ROW(a6),d0	* Calculate output address
		move.w	#VPITCH,d1	* ...
		mulu	d1,d0		* ... ROW * VPITCH
		add.w	#NVPIX-1,d0	* ... + NVPIX-1
		mulu	d5,d0		* ... * NW
		clr.l	d1		* ...
		move.w	COL(a6),d1	* ... + (COL * 2)
		lsl.l	#1,d1		* ...
		add.l	d1,d0		* ...
		add.l	OBASE(a6),d0	* ... + OBASE
		movea.l	d0,a3		* Leave output address in a3
		movea.l	STR(a6),a2	* Put string address in a2
*
		.page
cgl0:		clr.l	d0		* Clear out upper bits of d0
		move.b	(a2)+,d0	* Get next character
		beq	cgl5		* Done if character EQ 0
*
		movea.l	a3,a1		* Establish output pointer in a1
		adda.l	#2,a3		* Update output pointer for next char.
		lea	_cgtable,a0	* Establish CG pointer in a0
		lsl.w	#1,d0		* ... 2 * character
		adda.w	d0,a0		* ... + _cgtable address
		move.w	#NVPIX-1,d6	* Set scan line counter in d6
*
cgl1:		move.w	#HPIX-1,d2	* Set pixel counter in d2
		move.w	(a0),d1		* Get character generator word in d1
*
cgl2:		lsr.w	#2,d0		* Shift output word right 1 pixel
		btst.l	#0,d1		* Check CG word ls bit
		beq	cgl3		* Set background color if bit EQ 0
*
		or.w	d3,d0		* OR foreground color into output word
		bra	cgl4		* Go update CG word
*
cgl3:		or.w	d4,d0		* OR background color into  output word
*
cgl4:		lsr.w	#1,d1		* Shift CG word right 1 pixel
		dbf	d2,cgl2		* Loop for all 8 pixels
*
		move.w	d0,(a1)		* Store output word in output bitmap
		suba.w	d5,a1		* Update output pointer
		adda.l	#512,a0		* Update CG pointer for next scan line
		dbf	d6,cgl1		* Loop for all scan lines
*
		bra	cgl0		* Loop for next character
*
cgl5:		movem.l	(a7)+,d3-d6/a3	* Restore registers
		unlk	a6		* Unlink stack frames
		rts			* Return to caller
*
		.end
