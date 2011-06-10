* ------------------------------------------------------------------------------
* jumpto.s -- miscellaneous ROMP support functions
* Version 4 -- 1987-10-14 -- D.N. Lynx Crowe
*
*	WARNING:
*	--------
*		These functions, in general, assume supervisor mode and
*		'sane' arguments, so no error checking is done.
*
*	halt()
*
*		Brings the processor to a grinding halt.  Requires external
*		reset to restart things.  Use only for catastrophic hard halts.
*
*	jumpto(addr)
*	long addr;
*
*		Jumps to 'addr'.  No error check is done on 'addr'.
*
*	rjumpto(addr)
*	long addr;
*
*		Performs the 68000 'RESET' command, then jumps to 'addr'.
*		No error check is made on 'addr'.
*
*	sjumpto(addr, stack)
*	long addr, stack;
*
*		Sets a7 to 'stack', then jumps to 'addr'.
*		No error check is done on 'addr'.
*
*	xreset()
*
*		Performs the 68000 'RESET' command.  This is very dangerous,
*		and should be used with extreme care regarding such
*		things as interrupts, device initialization, vectors,
*		and sundry other reset-related things.
*
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_halt,_jumpto,_rjumpto,_sjumpto,_xreset
*
		.page
*
_halt:		stop	#$2700			* stop dead, interrupts disabled
		jmp	_halt			* stay stopped if stepped thru
*
_jumpto:	movea.l	4(a7),a0		* get jump address
		jmp	(a0)			* go to the jump address
*
_rjumpto:	reset				* reset external devices
		movea.l	4(a7),a0		* get jump address
		jmp	(a0)			* go to the jump address
*
_sjumpto:	movea.l	4(a7),a0		* get jump address
		movea.l	8(a7),a7		* set stack pointer
		jmp	(a0)			* go to the jump address
*
_xreset:	reset				* reset external devices
		rts				* return to caller
*
		.end
