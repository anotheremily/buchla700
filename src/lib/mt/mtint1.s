* ------------------------------------------------------------------------------
* mtint1.s -- Multi-Tasker -- Video Second Level Interrupt Handler
* Version 2 -- 1988-04-13 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
*
		.text
*
		.xdef	__MTInt1	* Video Second Level Interrupt Handler
*
		.xref	SM_Wait		* Sempahore wait function
*
		.xref	MT_ISEM1	* Video interrupt semaphore
		.xref	resvec4		* Video interrupt vector
*
* __MTInt1 -- Video Second Level Interrupt Handler
* --------    ------------------------------------
__MTInt1:	move.l	#MT_ISEM1,-(a7)		* Wait on interrupt semaphore
		jsr	SM_Wait			* ...
		tst.l	(a7)+			*
		movea.l	resvec4,a0		* (*resvec4)()
		jsr	(a0)			* ...
		bra	__MTInt1		* do it again
*
*
		.end
