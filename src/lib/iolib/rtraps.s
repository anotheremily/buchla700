* ------------------------------------------------------------------------------
* rtraps.s -- define ROMP debug trap
* Version 7 -- 1988-01-12 -- Copyright 1987, 1988 by D.N. Lynx Crowe
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_trap15
*
		.xref	_rompbp
*
		.xref	tr1sav,tr13sav,tr14sav
		.xref	tr1rmp,tr13rmp,tr14rmp
*
BUCHLA		.equ	1		* IPL value:  0 = NASA , 1 = Buchla
*
		.ifeq	BUCHLA
IPLEVEL		.equ	$0400		* NASA IPL = 4    (enable 5 and above)
		.endc
*
		.ifne	BUCHLA
IPLEVEL		.equ	$0200		* Buchla IPL = 2  (enable 3 and above)
		.endc
*
IPL7		.equ	$0700		* IPL 7
*
* _trap15 -- ROMP debug trap  (used to implement breakpoints)
* -------    ------------------------------------------------
_trap15:	ori.w	#IPL7,sr		* Disable interrupts
		move.w	#0,-(a7)		* Keep stack long aligned
		movem.l	d0-d7/a0-a7,-(a7)	* Save regs on stack
		move.l	tr1sav,tr1rmp		* Save trap save areas
		move.l	tr13sav,tr13rmp		* ...
		move.l	tr14sav,tr14rmp		* ...
		move.w	sr,d0			* Get status register
		andi.w	#$F8FF,d0		* Enable serial I/O interrupts
		ori.w	#IPLEVEL,d0		* ...
		move.w	d0,sr			* ...
		jsr	_rompbp			* Pass control to ROMP
*
		ori.w	#IPL7,sr		* Disable interrupts
		move.l	tr14rmp,tr14sav		* Restore trap save areas
		move.l	tr13rmp,tr13sav		* ...
		move.l	tr1rmp,tr1sav		* ...
		movem.l	(a7)+,d0-d7/a0-a7	* Restore regs
		addq.l	#2,a7			* Discard alignment filler word
		rte				* Back to what we interrupted
*
		.end
