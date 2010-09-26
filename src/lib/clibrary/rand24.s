* ------------------------------------------------------------------------------
* rand24.s -- generate a 24 bit random number
* Version 3 -- 1988-04-29 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
* Synopsis:
*		long
*		rand24()
*
* Based on:
*		Knuth, Donald E.
*		The Art of Computer Programming,
*		Volume 2: Semi-Numerical Algorithms
*
* Computes:
*		S = [S * C] + K
*
* Where:
*		K = 1
*		C = 3141592621
*		S = the seed  (if zero, it gets set from the 200 Hz clock)
*
* Returns:
*		S >> 8  (a 24 bit pseudo-random number)
*
* Note:  this function has an LSB with an exactly 50% distribution,  so using
* individual bits is probably not a good idea.  Using more bits makes things
* appear more random.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_rand24
*
		.xdef	_rseed
*
* equates for things in the BIOS
*
RCLOCK		.equ	$49E			* LONG - 200 Hz clock
*
* equates for stack offsets
*
ARG1		.equ	8			* LONG / WORD - arg1 / MS bits
ARG1L		.equ	10			* WORD - arg1 LS bits
ARG2		.equ	12			* LONG / WORD - arg2 / MS bits
ARG2L		.equ	14			* WORD - arg2 LS bits
*
PART		.equ	-4			* LONG - partial product
*
PI		.equ	$BB40E62D		* LONG - PI as a hex value
*
		.page
*
* mult32 -- 32 bit signed multiply
* ------    ----------------------
mult32:		link	a6,#-4			* link stack frames
		clr.w	d2			* clear sign flags
		tst.l	ARG1(a6)		* check sign of 1st argument
		bge	mult32a			* ...
*
		neg.l	ARG1(a6)		* make 1st argument positive
		addq.w	#1,d2			* log its sign as negative
*
mult32a:	tst.l	ARG2(a6)		* check sign of 2nd argument
		bge	mult32b			* ...
*
		neg.l	ARG2(a6)		* make 2nd argument positive
		addq.w	#1,d2			* log its sign as negative
*
mult32b:	move.w	ARG1L(a6),d0		* generate 1st partial product
		mulu	ARG2L(a6),d0		* ...
		move.l	d0,PART(a6)		* ...
		move.w	ARG1(a6),d0		* generate 2nd partial product
		mulu	ARG2L(a6),d0		* ...
		move.w	ARG2(a6),d1		* generate 3rd partial product
		mulu	ARG1L(a6),d1		* ...
		add.w	d1,d0			* add partial products
		add.w	PART(a6),d0		* ...
		move.w	d0,PART(a6)		* ...
		move.l	PART(a6),d0		* ...
		btst	#0,d2			* adjust sign of result
		beq	mult32c			* ...
*
		neg.l	d0			* ...
*
mult32c:	unlk	a6			* unlink stack frames
		rts				* return
*
		.page
*
* _rand24 -- Generate a random number
* -------    ------------------------
_rand24:	link	a6,#0			* Link stack frames
		tst.l	_rseed			* See if the seed is zero
		bne	rand01			* Jump if not
*
		move.l	RCLOCK,d0		* Pick up the 200 Hz clock
		moveq.l	#16,d1			* Shift it left
		asl.l	d1,d0			* ...
		or.l	RCLOCK,d0		* OR in current 200 Hz clock
		move.l	d0,_rseed		* Use that as the seed
*
rand01:		move.l	#PI,-(a7)		* Put PI on the stack
		move.l	_rseed,-(a7)		* ... and _rseed, too
		bsr	mult32			* Multiply them
		addq.l	#8,a7			* Cleanup stack
		addq.l	#1,d0			* Add 1 to the result
		move.l	d0,_rseed		* Save as new seed
		asr.l	#8,d0			* Make it a 24 bit number
		and.l	#$00FFFFFF,d0		* ...
		unlk	a6			* Unlink stack frames
		rts				* Return to caller
*
* ------------------------------------------------------------------------------
		.bss
* ------------------------------------------------------------------------------
*
_rseed:		.ds.l	1			* random number seed
*
		.end
