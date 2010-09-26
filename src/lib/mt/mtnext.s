* ------------------------------------------------------------------------------
* mtnext.s -- Multi-tasker -- C function linkages
* Version 2 -- 1988-04-14 -- D.N. Lynx Crowe
* (c) Copyright 1988 -- D.N. Lynx Crowe
* Machine cycles are given as the first number in the comments for timing.
* ------------------------------------------------------------------------------
		.text
*
DEBUGGER	.equ	0			* define non-zero for debug
*
		.xdef	_MTNext
		.xdef	_MTSwap
*
		.ifne	DEBUGGER
*
		.xref	_DB_Cmnt
*
		.endc
*
TRAP_SWP	.equ	8			* swapper TRAP number 1
TRAP_NXT	.equ	9			* swapper TRAP number 2
*
* ------------------------------------------------------------------------------
*
* MTSwap() -- C linkage to the task swapper TRAP -- swap
* --------    ------------------------------------------
*
* ------------------------------------------------------------------------------
		.ifne	DEBUGGER
*
_MTSwap:	movem.l	d0-d7/a0-a6,-(a7)	* DB_CMNT("MTSwap");
		move.l	#DB_msg1,-(a7)		* ...
		jsr	_DB_Cmnt		* ...
		tst.l	(a7)+			* ...
		movem.l	(a7)+,d0-d7/a0-a6	* ...
		trap	#TRAP_SWP		*   37 trap to the swapper
		.endc
*
		.ifeq	DEBUGGER
*
_MTSwap:	trap	#TRAP_SWP		*   37 trap to the swapper
*
		.endc
*
* ------------------------------------------------------------------------------
		rts				*   16 return to the caller
*
		.page
*
* ------------------------------------------------------------------------------
*
* MTNext() -- C linkage to the task swapper TRAP -- next
* --------    ------------------------------------------
*
* ------------------------------------------------------------------------------
		.ifne	DEBUGGER
*
_MTNext:	movem.l	d0-d7/a0-a6,-(a7)	* DB_CMNT("MTNext");
		move.l	#DB_msg2,-(a7)		* ...
		jsr	_DB_Cmnt		* ...
		tst.l	(a7)+			* ...
		movem.l	(a7)+,d0-d7/a0-a6	* ...
		trap	#TRAP_NXT		*   37 trap to the swapper
*
		.endc
*
		.ifeq	DEBUGGER
*
_MTNext:	trap	#TRAP_NXT		*   37 trap to the swapper
*
		.endc
*
* ------------------------------------------------------------------------------
		rts				*   16 return to the caller
*
		.ifne	DEBUGGER
*
		.data
*
DB_msg1:	dc.b	'MTSwap',0
DB_msg2:	dc.b	'MTNext',0
*
		.endc
*
		.end
