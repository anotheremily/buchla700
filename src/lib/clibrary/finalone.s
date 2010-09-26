* ------------------------------------------------------------------------------
* finalone.s -- the last stuff to get loaded
* Version 4 -- 1987-06-30 -- D.N. Lynx Crowe
*
* This is so we can figure out where things got loaded.
* ------------------------------------------------------------------------------
*
		.text
*
		.xdef	FinalOne
		.xdef	The_Fini
		.xdef	Text_End
*
*
* This is the last piece of code in the 'text' segment.
*
FinalOne:	rts			* we just need the address here
*
Text_End:	rts			* we just need the address here
*
* ------------------------------------------------------------------------------
*
		.data
		.even
*
The_Fini:	dc.w	$FFFF		* last thing in the 'data' segment
*
* ------------------------------------------------------------------------------
*
		.end
