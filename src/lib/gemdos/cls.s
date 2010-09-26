*
* CLS.s -- Clear screen
* -----    ------------
* Version 8 -- 1987-02-05 -- D.N. Lynx Crowe
*
ESC		.equ	$1B			* Escape character
*
bconout		.equ	3			* bconout function #
*
		.globl	cls
*
cls:		lea	thestack,a7		* Initialize the stack
*
		move.w	#ESC,-(a7)		* Put ESC on stack
		move.w	#2,-(a7)		* Put 2 on stack
		move.w	#bconout,-(a7)		* Put bconout # on stack
		trap	#13			* Trap to the bios
		addq	#6,a7			* Cleanup stack
*
		move.w	#' E',-(a7)		* Put 'E' on stack
		move.w	#2,-(a7)		* Put 2 on stack
		move.w	#bconout,-(a7)		* Put bconout # on stack
		trap	#13			* Trap to the bios
		addq	#6,a7			* Cleanup stack
*
		clr.w	-(a7)			* Setup for pterm()
		trap	#1			* Exit to GEMDOS
*
		.bss
*
		ds.l	256
thestack:	ds.l	1
*
		.end
