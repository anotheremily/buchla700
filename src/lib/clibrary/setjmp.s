* ------------------------------------------------------------------------------
* setjmp.s -- Unix(tm) compatible setjmp(env) and longjmp(env,ret)
* Version 3 -- 1987-06-16 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
*
		.text
*
		.xdef	_setjmp,_longjmp
*
_setjmp:	movea.l	4(a7),a0		* Get env pointer
		move.l	(a7),(a0)		* Put return address in env
		movem.l	d1-d7/a1-a7,4(a0)	* Save registers in env
		moveq.l	#0,d0			* Set return value to 0
		rts				* Return to caller
*
_longjmp:	move.w	8(a7),d0		* Get ret value
		bne	lj1			* Jump if non-zero
*
		moveq.l	#1,d0			* Force return value to 1
*
lj1:		movea.l	4(a7),a0		* Get env pointer
		movem.l	4(a0),d1-d7/a1-a7	* Restore registers from env
		move.l	(a0),(a7)		* Get return address from env
		rts				* Return to caller
*
		.end
