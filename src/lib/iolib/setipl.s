*
* setipl.s -- Set internal processor interrupt level
* --------    --------------------------------------
* Version 2 -- 1988-06-29 -- D.N. Lynx Crowe
*
*	short
*	setipl(arg);
*	short arg;
*
*	Sets processor interrupt level to arg.
*	Returns old interrupt level, or -1 if arg < 0 or > 7
*
*	Assumes you are in supervisor mode.
*	You get a Privelege Violation TRAP if you aren't.
*
		.text
*
		.xdef	_setipl
*
_setipl:	link	a6,#0			* Link up stack frames
		move.w	8(a6),d0		* Get argument
		tst.w	d0			* Check lower limit
		bmi	setipler		* Jump if < 0  (error)
*
		cmpi.w	#7,d0			* Check upper limit
		bgt	setipler		* Jump if > 7  (error)
*
		move.w	sr,d1			* Get current level
		move.w	d1,d2			* ... save for later
		lsl.w	#8,d0			* Shift argument into position
		andi.w	#$F8FF,d1		* Mask out old level
		or.w	d0,d1			* OR in new level
		move.w	d2,d0			* Setup return of old level
		lsr.w	#8,d0			* ...
		andi.l	#$7,d0			* ...
		move.w	d1,sr			* Set the new interrupt level
		unlk	a6			* Unlink stack frames
		rts				* Return to caller
*
setipler:	moveq.l	#-1,d0			* Setup to return error code
		unlk	a6			* Unlink stack frames
		rts				* Return to caller
*
		.end
