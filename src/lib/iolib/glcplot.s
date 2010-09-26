* ------------------------------------------------------------------------------
* glcplot.s -- plot a pixel on the LCD display
* Version 2 -- 1987-04-23 -- D.N. Lynx Crowe
* (c) Copyright 1987 -- D.N. Lynx Crowe
*
*	GLCplot(x, y, val)
*	unsigned x, y, val);
*
*		Plot a pixel at ('x', 'y') using lcdbase as the plane address
*		in GLC RAM.  If 'val' is zero, the pixel is cleared,
*		otherwise the pixel is cleared.  No error checking is done.
*		Limits:  0 LE x LE 511,  0 LE y LE 63.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_GLCplot
		.xref	_lcd_a0,_lcd_a1,_lcdbase
*
XLOC		.equ	8			* 'x' parameter offset
YLOC		.equ	10			* 'y' parameter offset
VAL		.equ	12			* 'val' parameter offset
*
G_CRSWR		.equ	$46			* GLC set cursor command
G_MWRITE	.equ	$42			* GLC write command
G_MREAD		.equ	$43			* GLC read command
*
		.page
*
_GLCplot:	link	a6,#0			* Link stack frames
		moveq	#63,d0			* d0 = (63-y) * 85
		sub.w	YLOC(a6),d0		* ...
		mulu	#85,d0			* ...
		clr.l	d1			* d1 = x/6
		move.w	XLOC(a6),d1		* ...
		divu	#6,d1			* ...
		add.w	d1,d0			* d0 = (63-y)*85 + (x/6)
		swap	d1			* d2 = 7 - (x%6) % 8
		moveq	#7,d2			* ...
		sub.w	d1,d2			* ...
		andi.w	#7,d2			* ...
		lsr.w	#3,d1			* d1 = (x%6) / 8
		add.w	d1,d0			* d0 = cursor address
		add.w	_lcdbase,d0		* ...
		move.w	d0,d1			* d1 = cursor address, too
		move.b	#G_CRSWR,_lcd_a1	* Send cursor address to GLC
		move.b	d0,_lcd_a0		* ...
		lsr.w	#8,d0			* ...
		move.b	d0,_lcd_a0		* ...
		move.b	#G_MREAD,_lcd_a1	* Read old pixel byte
		move.b	_lcd_a1,d0		* ... into d0
		tst.w	VAL(a6)			* Check val for zero
		beq	glcplt1			* Jump if val EQ 0
*
		bset	d2,d0			* Set the pixel to 1
		bra	glcplt2			* Go write pixel to GLC
*
glcplt1:	bclr	d2,d0			* Clear the pixel to 0
*
glcplt2:	move.b	#G_CRSWR,_lcd_a1	* Send cursor address to GLC
		move.b	d1,_lcd_a0		* ...
		lsr.w	#8,d1			* ...
		move.b	d1,_lcd_a0		* ...
		move.b	#G_MWRITE,_lcd_a1	* Setup GLC to write pixel
		move.b	d0,_lcd_a0		* Write pixel
		unlk	a6			* Unlink stack frames
		rts				* Return to caller
*
		.end
