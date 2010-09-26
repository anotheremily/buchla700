* ------------------------------------------------------------------------------
* vcputsv.s -- output characters to a 4-bit / pixel graphics window
* with variable vertical pitch
* Version 2 -- 1987-08-03 -- D.N. Lynx Crowe
* (c) Copyright 1987 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
*
*	vcputsv(obase, nw, fg, bg, row, col, str. pitch)
*	int *obase, nw, fg, bg, row, col, pitch;
*	char *str;
*
*		Outputs characters from the string at 'str' to an 'nw'
*		character wide 4-bit per pixel graphics window at 'obase'
*		at ('row','col'), using 'fg' as the foreground color, and
*		'bg' as the background color.  Uses cgtable[][256] as the
*		VSDD formatted character generator table.  Assumes 12 bit
*		high characters in the cgtable.  Uses 'pitch' as the vertical
*		spacing between character rows.  No error checks are done.
*		The string must fit the output area (no overlaps, single line).
*
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_vcputsv
*
		.xref	_cgtable
*
* Argument offsets from a6:
*
OBASE		.equ	8		* LONG - Output area base address
NW		.equ	12		* WORD - Character width of output area
FG		.equ	14		* WORD - Foreground color
BG		.equ	16		* WORD - Background color
ROW		.equ	18		* WORD - Row
COL		.equ	20		* WORD - Column
STR		.equ	22		* LONG - String base address
PITCH		.equ	26		* WORD - Vertical spacing between rows
*
* Program constant definitions:
*
HPIX		.equ	8		* Character width in pixels
VPIX		.equ	12		* Character height in pixels
HCW		.equ	4		* Horizontal character width (bytes)
PSHIFT		.equ	12		* Pixel shift into MS bits
HSHIFT		.equ	4		* Pixel right shift
*
		.page
*
* Register usage:
*
*	d0	output word and scratch
*	d1	CG word and scratch
*	d2	pixel counter
*
*	d3	foreground color (in the 4 ms bits)
*	d4	background color (in the 4 ms bits)
*	d5	width of the area in bytes
*	d6	scan line counter
*
*	a0	CG table pointer
*	a1	output area scan line pointer
*	a2	input string character pointer
*
*	a3	output area character base pointer
*
*	a6	frame pointer
*	a7	stack pointer
*
		.page
*
_vcputsv:	link	a6,#0		* Link stack frames
		movem.l	d3-d6/a3,-(a7)	* Save registers we use
		move.w	#PSHIFT,d1	* Set shift constant
		move.w	FG(a6),d3	* Setup foreground color
		lsl.w	d1,d3		* ... in ms 4 bits of d3.W
		move.w	BG(a6),d4	* Setup background color
		lsl.w	d1,d4		* ... in ms 4 bits of d4.W
		move.w	NW(a6),d5	* Get line width in d5.W
		lsl.w	#2,d5		* Multiply width by 4 for offset
		move.w	ROW(a6),d0	* Calculate output address
		move.w	PITCH(a6),d1	* ... PITCH
		mulu	d1,d0		* ... * ROW
		add.w	#VPIX-1,d0	* ... + VPIX-1
		mulu	d5,d0		* ... * NW
		clr.l	d1		* ...
		move.w	COL(a6),d1	* ... + 
		lsl.w	#2,d1		* ... COL * 4
		add.l	d1,d0		* ...
		add.l	OBASE(a6),d0	* ... + OBASE
		movea.l	d0,a3		* Leave output address in a3
		movea.l	STR(a6),a2	* Put string address in a2
*
cgl0:		clr.l	d0		* Clear out upper bits of d0
		move.b	(a2)+,d0	* Get next character
		beq	cgl5		* Done if character EQ 0
*
		movea.l	a3,a1		* Establish output pointer in a1
		adda.l	#HCW,a3		* Update output pointer for next char.
		lea	_cgtable,a0	* Establish CG pointer in a0
		lsl.w	#1,d0		* ... 2 * character
		adda.w	d0,a0		* ... + _cgtable address
		move.w	#VPIX-1,d6	* Set scan line counter in d6
*
		.page
cgl1:		move.w	(a0),d1		* Get character generator word in d1
		move.w	#(HPIX/2)-1,d2	* Set pixel counter in d2
*
cgl2:		lsr.w	#HSHIFT,d0	* Shift output word right 1 pixel
		btst.l	#0,d1		* Check CG word ls bit
		beq	cgl3		* Set background color if bit EQ 0
*
		or.w	d3,d0		* OR foreground color into output word
		bra	cgl4		* Go update CG word
*
cgl3:		or.w	d4,d0		* OR background color into output word
*
cgl4:		lsr.w	#1,d1		* Shift CG word right 1 pixel
		dbf	d2,cgl2		* Loop for first 4 pixels
*
		move.w	d0,(a1)+	* Store first output word in scan line
		move.w	#(HPIX/2)-1,d2	* Set pixel counter in d2
*
cgl2a:		lsr.w	#HSHIFT,d0	* Shift output word right 1 pixel
		btst.l	#0,d1		* Check CG word ls bit
		beq	cgl3a		* Set background color if bit EQ 0
*
		or.w	d3,d0		* OR foreground color into output word
		bra	cgl4a		* Go update CG word
*
cgl3a:		or.w	d4,d0		* OR background color into output word
*
cgl4a:		lsr.w	#1,d1		* Shift CG word right 1 pixel
		dbf	d2,cgl2a	* Loop for last 4 pixels
*
		move.w	d0,(a1)		* Store second output word in scan line
		suba.w	d5,a1		* Update output pointer
		suba.w	#2,a1		* ...
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
