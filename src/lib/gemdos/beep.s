*
* beep.s -- beep the terminal
* ------    -----------------
* Version 1 -- 1987-02-05 -- D.N. Lynx Crowe
*
BELL		.equ	7			* ASCII BELL
*
BIOS		.equ	13			* BIOS trap
B_PUTC		.equ	3			* B_PUTC function #
CON_DEV		.equ	2			* CON_DEV number
GEMDOS		.equ	1			* GEMDOS trap
*
		.xdef	beep
*
beep:		move.w	#BELL,-(a7)		* Put BELL on stack
		move.w	#CON_DEV,-(a7)		* Put CON_DEV on stack
		move.w	#B_PUTC,-(a7)		* Put B_PUTC # on stack
		trap	#BIOS			* Trap to the bios
		addq	#6,a7			* Cleanup stack
*
		clr.w	-(a7)			* Setup for pterm()
		trap	#GEMDOS			* Exit to GEMDOS
*
		.end
