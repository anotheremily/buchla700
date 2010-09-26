*
* traps.s -- define trap linkages for C routines
* -------    -----------------------------------
* Version 8 -- 1987-06-08 -- D.N. Lynx Crowe
*
* Caution:  these are serially re-useable, but NOT reentrant, so
* don't use them in interrupt processing code.
*
* An exception is made for ROMP in _trap15 for breakpoints so that the
* debug code can be debugged.
*
		.text
*
		.xdef	_trap1
		.xdef	_trap13,_trap14
		.xdef	_xtrap15
*
		.xdef	tr1sav,tr13sav,tr14sav
		.xdef	tr1rmp,tr13rmp,tr14rmp
*
* _trap1 -- provide access to BDOS functions
* ------    --------------------------------
_trap1:		move.l	(a7)+,tr1sav		* Save return address
		trap	#1			* Do the trap
		move.l	tr1sav,-(a7)		* Restore return address
		rts				* Return to caller
*
* _trap13 -- provide access to BIOS functions
* -------    --------------------------------
_trap13:	move.l	(a7)+,tr13sav		* Save return address
		trap	#13			* Do the trap
		move.l	tr13sav,-(a7)		* Restore return address
		rts				* Return to caller
*
* _trap14 -- provide access to extended BIOS functions
* -------    -----------------------------------------
_trap14:	move.l	(a7)+,tr14sav		* Save return address
		trap	#14			* Do the trap
		move.l	tr14sav,-(a7)		* Restore return address
		rts				* Return to caller
*
* _xtrap15 -- Setup initial register trap for ROMP
* --------    ------------------------------------
_xtrap15:	trap	#15			* TRAP into ROMP
		rts				* Return  (usually won't happen)
*
		.page
*
* RAM storage areas
* -----------------
		.bss
		.even
*
tr1sav:		ds.l	1		* Return address for trap1
tr13sav:	ds.l	1		* Return address for trap13
tr14sav:	ds.l	1		* Return address for trap14
*
tr1rmp:		ds.l	1		* Save area for tr1sav for ROMP
tr13rmp:	ds.l	1		* Save area for tr13sav for ROMP
tr14rmp:	ds.l	1		* Save area for tr14sav for ROMP
*
		.end
