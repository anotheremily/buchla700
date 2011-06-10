* ------------------------------------------------------------------------------
* dbdefs.s -- special Multi-Tasker debug RAM definitions
* Version 2 -- 1988-03-31 -- D.N. Lynx Crowe
*
* Use these with the special version of dbentr.c for debugging the Multi-Tasker.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_DB_In
		.xdef	_DB_Out
		.xdef	_DB_Flag
		.xdef	_DB_Full
		.xdef	_DB_Levl
		.xdef	_DB_Last
		.xdef	_DB_Ents
*
DB_BASE		.equ	$40000			* absolute base of debug RAM
*
_DB_In		.equ	DB_BASE			* WORD -- buffer 'in' index
_DB_Out		.equ	_DB_In+2		* WORD -- buffer 'out' index
_DB_Flag	.equ	_DB_Out+2		* WORD -- ROMP trap disable flag
_DB_Full	.equ	_DB_Flag+2		* WORD -- buffer full flag
*
_DB_Levl	.equ	_DB_Full+2		* LONG -- debug call level
_DB_Last	.equ	_DB_Levl+4		* LONG -- last debug string ptr.
*
_DB_Ents	.equ	_DB_Last+4		* STRUCT -- debug buffer
*
		.end
