* ------------------------------------------------------------------------------
* verdate.s -- date and version ID message string
* Version 1 -- 1988-10-28 -- D.N. Lynx Crowe
* (c) Copyright 1988 -- D.N. Lynx Crowe -- All rights reserved
* ------------------------------------------------------------------------------
*
* Version message string.  Updated by 'newver.c' in object form to:
*
*	1. Automatically set the correct date and version
*	2. Avoid the need for a compile or assembly
*
*	12 bytes -- 11 ASCII characters and a terminating zero byte
*
*	char	VerDate = "yyyymmdd.vv";
*				      0 byte
*			   0         1
*			   012345678901
*
*	Use &VerDate[2] for yymmdd.vv format.
*
		.data
*
		.xdef	_VerDate
*
_VerDate:	dc.b	'19881027.01'		* Date and version number
		dc.b	0
*
		.end
