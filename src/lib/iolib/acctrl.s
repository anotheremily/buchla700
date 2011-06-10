* ------------------------------------------------------------------------------
* acctrl.s -- VSDD access table control functions
* Version 6 -- 1987-04-13 -- D.N. Lynx Crowe
* (c) Copyright 1987 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
*
*	objclr(obj)
*	unsigned int obj;
*
*		Clear bits in access table for object 'obj'.
*		Disables object 'obj'.
*
*	objoff(obj, line, num)
*	unsigned int obj, line, num;
*
*		Disable object obj at line thru line+num.
*
*	objon(obj, line, num)
*	unsigned int obj, line, num;
*
*		Enable object obj at line thru line+num.
*
*
*	Assumes VSDD is looking at bank 0.
*	Assumes a screen height of 350 lines.
*	No error checks are done, so beware.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_objclr,_objoff,_objon
*
		.xref	_v_actab
*
SCSIZE		.equ	350		* Screen height
*
OBJ		.equ	8		* Object number argument offset
LINE		.equ	10		* Beginning line argument offset
NUM		.equ	12		* Object height argument offset
* ------------------------------------------------------------------------------
		.page
* ------------------------------------------------------------------------------
*
*	objclr(obj)
*	unsigned int obj;
*
*		Disables object obj in access table by turning on
*		its bit in all words of the access table.
* ------------------------------------------------------------------------------
_objclr:	link	a6,#0		* Link stack frames
		move.w	OBJ(a6),d1	* Get object bit number in d1
		lea	_v_actab,a0	* Get base of object table in a0
		move.w	#SCSIZE-1,d2	* Put line count in d2
*
objclr1:	move.w	(a0),d0		* Get access table word
		bset.l	d1,d0		* Set the bit
		move.w	d0,(a0)+	* Update word in access table
		dbf	d2,objclr1	* Loop until done
*
		unlk	a6		* Unlink stack frame
		rts			* Return to caller
*
		.page
* ------------------------------------------------------------------------------
*	objoff(obj, line, num)
*	unsigned int obj, line, num;
*
*		Turn on access table bits for object 'obj' at
*		lines 'line' through 'line'+'num'.  Disables the object.
*		Assumes object bits were set at those locations.
* ------------------------------------------------------------------------------
_objoff:	link	a6,#0		* Link stack frames
		move.w	OBJ(a6),d1	* Get object bit number into d1
		move.w	LINE(a6),d2	* Get top line number
		add.w	d2,d2		* Convert to word offset
		lea	_v_actab,a0	* Get base address of access table
		move.w	0(a0,d2),d0	* Get top line access word
		bset.l	d1,d0		* Set object bit
		move.w	d0,0(a0,d2)	* Update word in access table
		tst.w	NUM(a6)		* Number of lines = 0 ?
		beq	objoff1		* Done if so
*
		move.w	NUM(a6),d2	* Get object depth
		add.w	LINE(a6),d2	* Add to top line number
		cmpi.w	#SCSIZE,d2	* Bottom line >= screen height ?
		bge	objoff1		* Done if so
*
		add.w	d2,d2		* Convert to word offset
		move.w	0(a0,d2),d0	* Get bottom line access word
		bset.l	d1,d0		* Set object bit
		move.w	d0,0(a0,d2)	* Update word in access table
*
objoff1:	unlk	a6		* Unlink stack frame
		rts			* Return to caller
*
		.page
* ------------------------------------------------------------------------------
*	objon(obj, line, num)
*	unsigned int obj, line, num;
*
*		Turn off access table bits for object 'obj'
*		at 'line' thru 'line'+'num'.  Enables the object.
* ------------------------------------------------------------------------------
_objon:		link	a6,#0		* Link stack frames
		move.w	OBJ(a6),d1	* Get object bit number into d1
		move.w	LINE(a6),d2	* Get top line number
		add.w	d2,d2		* Convert to word offset
		lea	_v_actab,a0	* Get base address of access table
		move.w	0(a0,d2),d0	* Get top line access word
		bclr.l	d1,d0		* Clear object bit
		move.w	d0,0(a0,d2)	* Update word in access table
		tst.w	NUM(a6)		* Number of lines = 0 ?
		beq	objon1		* Done if so
*
		move.w	NUM(a6),d2	* Get object depth
		add.w	LINE(a6),d2	* Add top line number
		cmpi.w	#SCSIZE,d2	* Bottom line >= screen height ?
		bge	objon1		* Done if so
*
		add.w	d2,d2		* Convert to word offset
		move.w	0(a0,d2),d0	* Get bottom line access word
		bclr.l	d1,d0		* Clear object bit
		move.w	d0,0(a0,d2)	* Update word in access table
*
objon1:		unlk	a6		* Unlink stack frame
		rts			* Return to caller
*
		.end
