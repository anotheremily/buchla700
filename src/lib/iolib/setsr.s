*
* setsr.s -- Set processor status register
* -------    -----------------------------
* Version 1 -- 1988-06-29 -- D.N. Lynx Crowe
*
*	short
*	setsr(arg);
*	short arg;
*
*	Sets processor status register to 'arg'.
*	Returns old status register value.
*
*	Assumes you are in supervisor mode.
*	You get a Privelege Violation TRAP if you aren't.
*
*	Coded for speed -- this is as fast as you can get.
*	No error checking is done -- assumes you know what you're doing.
*
		.text
*
		.xdef	_setsr
*
_setsr:		move.w	sr,d0			* Get current sr
		move.w	4(sp),sr		* Set new sr
		rts				* Return to caller
*
		.end
