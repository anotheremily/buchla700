* ------------------------------------------------------------------------------
* vwputp.s -- put a pixel into a 2-bit per pixel bitmap object
* Version 2 -- 1987-04-14 -- D.N. Lynx Crowe
* (c) Copyright 1987 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
*	int
*	vwputp(octad, xloc, yloc, val)
*	struct octent *octad;
*	int xloc, yloc;
*
*		Writes the pixel value 'val' into the 2-bit per pixel
*		bitmap object described by 'octad' at ('xloc','yloc').
*
*	-----
*
*	struct octent {
*
*		uint	ysize,
*			xsize;
*
*		int	objx,
*			objy;
*
*		uint	*obase;
*
*		char	opri,
*			oflags;
*
*		uint	odtw0,
*			odtw1;
*	};
*
*	Upper left corner of screen is (0,0) origin.
*
		.text
*
		.xdef	_vputp
*
OCTAD		.equ	8
XLOC		.equ	12
YLOC		.equ	14
VAL		.equ	16
*
YSIZE		.equ	0
XSIZE		.equ	2
OBJX		.equ	4
OBJY		.equ	6
OBASE		.equ	8
OPRI		.equ	12
OFLAGS		.equ	13
ODTW0		.equ	14
ODTW1		.equ	16
*
		.page
*
_vwputp:	link	a6,#0			* Link stack frames
		movea.l	OCTAD(a6),a1		* Get OCTAD base into a1
		move.w	XLOC(a6),d0		* Get XLOC into d0
		cmp.w	XSIZE(a1),d0		* Check XLOC range
		bge	vputerr			* ERROR if too large
*
		tst.w	d0			* Check XLOC sign
		bmi	vputerr			* ERROR if negative
*
		move.w	YLOC(a6),d1		* Get YLOC into d1 to test
		cmp.w	YSIZE(a1),d1		* Check YLOC range
		bge	vputerr			* ERROR if too large
*
		tst.w	d1			* Check YLOC sign
		bmi	vputerr			* ERROR if negative
*
		lsr.w	#3,d0			* Divide XLOC by 8
		move.w	XSIZE(a1),d1		* Get width into d1
		lsr.w	#3,d1			* Divide width by 8
		mulu	YLOC(a6),d1		* Multiply width/8 by YLOC
		ext.l	d0			* Extend XLOC/8 to a long
		add.l	d0,d1			* ... and add it to d1
		add.l	OBASE(a1),d1		* Add OBASE to d1
		movea.l	d1,a0			* Make a0 point at bitmap data
		move.w	XLOC(a6),d0		* Get XLOC
		andi.l	#$07,d0			* Mask to low 3 bits
		add.l	d0,d0			* Multiply by 2 for word index
		move.l	d0,d1			* Save index in d1
		add.l	#MTAB,d0		* Add mask table base
		movea.l	d0,a2			* a2 points at mask
		add.l	#STAB,d1		* Add shift table base to index
		move.l	d1,a1			* a1 points at shift count
		move.w	(a1),d0			* Get shift count in d0
		move.w	VAL(a6),d1		* Get new pixel in d1
		andi.w	#$03,d1			* Mask down to 2 bits
		lsl.w	d0,d1			* Shift into position for OR
		move.w	(a0),d0			* Get old bitmap word in d0
		and.w	(a2),d0			* Mask out old pixel
		or.w	d1,d0			* OR in new pixel
		move.w	d0,(a0)			* Store updated word in bitmap
		clr.l	d0			* Set return value = 0 = OK
*
vputexit:	unlk	a6			* Unlink stack frame
		rts				* Return to caller
*
vputerr:	moveq.l	#-1,d0			* Set return value = -1 = ERROR
		bra	vputexit		* Go unlink stack and return
*
MTAB:		dc.w	$FFFC,$FFF3,$FFCF,$FF3F	* Mask table
		dc.w	$FCFF,$F3FF,$CFFF,$3FFF
STAB:		dc.w	0,2,4,6,8,10,12,14	* Shift table
*
		.end
