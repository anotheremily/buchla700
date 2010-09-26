* ------------------------------------------------------------------------------
* thistak.s -- default minimum stack size definition for diamelst.o
* Version 1 -- 1988-05-03 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
		.data
*
		.xdef	__STKSIZ
*
__STKSIZ:	dc.l	8192		* keep 8K for stack and heap
*
		.end
