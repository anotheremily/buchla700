* ------------------------------------------------------------------------------
* fatstak.s -- stack size definition for diamelst.o
* Version 1 -- 1988-05-03 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
		.data
*
		.xdef	__STKSIZ
*
__STKSIZ:	dc.l	-8192		* return 8K to the system
*
		.end
