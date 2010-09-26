* ------------------------------------------------------------------------------
* sreset.s -- reset score highlighting
* Version 14 -- 1988-07-28 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_sreset		* sreset()
*
		.xref	_vputa		* vputa(sbase, row, col, attrib)
*
		.xref	_ndisp		* WORD - display number
		.xref	_obj8		* LONG - object base address
		.xref	_vrbw08		* WORD - detail word for bit 8
		.xref	_vrbw09		* WORD - detail word for bit 9
		.xref	_vrbw10		* WORD - detail word for bit 10
		.xref	_vrbw11		* WORD - detail word for bit 11
		.xref	_vrbw12		* WORD - detail word for bit 12
		.xref	_vrbw13		* WORD - detail word for bit 13
		.xref	_vrbw14		* WORD - detail word for bit 14
		.xref	_vrbw15		* WORD - detail word for bit 15
		.xref	_vrcw		* WORD - video reset control word
*
ROW		.equ	4
COL		.equ	6
ATR		.equ	8
*
AT01		.equ	$0014
AT04		.equ	$0013
AT05		.equ	$0014
AT06		.equ	$0013
AT07		.equ	$0014
AT08		.equ	$0013
AT09		.equ	$0014
AT10		.equ	$0013
AT11		.equ	$0012
AT12		.equ	$0012
*
		.page
*
* sreset() -- reset highlighting
* --------    ------------------
_sreset:	link	a6,#0		* link stack frame
		cmp.w	#2,_ndisp	* see if we should update display
		bne	srsexit		* jump if not
*
		move.w	_vrcw,d0	* get and check vrcw
		bne	srs0		* jump if something to do
*
srsexit:	unlk	a6		* unlink stack frame
		rts			* return to caller
*
srs0:		move.w	sr,d1		* <<<<< disable interrupts >>>>>
		ori.w	#$0700,sr	* ...
*
		move.w	_vrcw,vrcw	* make local copies of control variables
		clr.w	_vrcw		* ... and clear them for the next pass
		move.w	_vrbw08,vrbw08	* ...
		clr.w	_vrbw08		* ...
		move.w	_vrbw09,vrbw09	* ...
		clr.w	_vrbw09		* ...
		move.w	_vrbw10,vrbw10	* ...
		clr.w	_vrbw10		* ...
		move.w	_vrbw11,vrbw11	* ...
		clr.w	_vrbw11		* ...
		move.w	_vrbw12,vrbw12	* ...
		clr.w	_vrbw12		* ...
		move.w	_vrbw13,vrbw13	* ...
		clr.w	_vrbw13		* ...
		move.w	_vrbw14,vrbw14	* ...
		clr.w	_vrbw14		* ...
		move.w	_vrbw15,vrbw15	* ...
		clr.w	_vrbw15		* ...
*
		move.w	d1,sr		* <<<<< restore interrupts >>>>>
*
* Setup STACK for subsequent calls to vputa(sbase, row, col, atr):
*
*	0(a7)	sbase
*
*	4(a7)	row	ROW
*	6(a7)	col	COL
*	8(a7)	atr	ATR
*
		clr.w	-(a7)		* put dummy attribute on stack
		clr.w	-(a7)		* put dummy column on stack
		clr.w	-(a7)		* put dummy row on stack
		move.l	_obj8,-(a7)	* put sbase on stack
*
		.page
*
* assignment
* ----------
		btst	#0,d0		* assignment ?
		beq	srs1		* jump if not
*
		move.w	#AT04,ATR(a7)	* put attribute on stack
		move.w	#1,ROW(a7)	* put row on stack
		move.w	#11,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#12,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	vrcw,d0		* restore vrcw to d0
*
* tuning
* ------
srs1:		btst	#1,d0		* tuning ?
		beq	srs2		* jump if not
*
		move.w	#AT05,ATR(a7)	* put attribute on stack
		move.w	#1,ROW(a7)	* put row on stack
		move.w	#19,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
		move.w	vrcw,d0		* restore vrcw to d0
*
* tempo
* -----
srs2:		btst	#2,d0		* tempo ?
		beq	srs3		* jump if not
*
		move.w	#AT06,ATR(a7)	* put attribute on stack
		move.w	#1,ROW(a7)	* put row on stack
		move.w	#27,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#28,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#29,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	vrcw,d0		* restore vrcw to d0
*
		.page
*
* interpolate
* -----------
srs3:		btst	#3,d0		* interpolate ?
		beq	srs4		* jump if not
*
		move.w	#AT07,ATR(a7)	* put attribute on stack
		move.w	#1,ROW(a7)	* put row on stack
		move.w	#35,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#36,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#37,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#38,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
		move.w	vrcw,d0		* restore vrcw to d0
*
* section begin
* -------------
srs4:		btst	#4,d0		* section begin ?
		beq	srs5		* jump if not
*
		move.w	#AT01,ATR(a7)	* put attribute on stack
		move.w	#0,ROW(a7)	* put row on stack
		move.w	#6,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#7,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
*
		.page
*
* punch in
* --------
srs5:		btst	#5,d0		* punch in ?
		beq	srs6		* jump if not
*
		move.w	#AT09,ATR(a7)	* put attribute on stack
		move.w	#1,ROW(a7)	* put row on stack
		move.w	#50,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#51,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	vrcw,d0		* restore vrcw to d0
*
* punch out
* ---------
srs6:		btst	#6,d0		* punch out ?
		beq	srs7		* jump if not
*
		move.w	#AT09,ATR(a7)	* put attribute on stack
		move.w	#1,ROW(a7)	* put row on stack
		move.w	#53,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#54,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#55,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	vrcw,d0		* restore vrcw to d0
*
* Output
* ------
srs7:		btst	#7,d0		* output ?
		beq	srs8		* jump if not
*
		move.w	#AT10,ATR(a7)	* put attribute on stack
		move.w	#1,ROW(a7)	* put row on stack
		move.w	#57,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#58,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#59,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#60,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
		move.w	#61,COL(a7)	* put 5th column on stack
		jsr	_vputa		* reset fifth column
		move.w	#62,COL(a7)	* put 6th column on stack
		jsr	_vputa		* reset sixth column
		move.w	vrcw,d0		* restore vrcw to d0
*
		.page
*
* instrument
* ----------
srs8:		btst	#8,d0		* instrument ?
		beq	srs9		* jump if not
*
		move.w	#2,ROW(a7)	* put row on stack
		move.w	#AT11,ATR(a7)	* put attribute on stack
		btst	#0,vrbw08+1	* group 1 ?
		beq	srs8a		* jump if not
*
		move.w	#7,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#8,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
*
srs8a:		btst	#1,vrbw08+1	* group 2 ?
		beq	srs8b		* jump if not
*
		move.w	#12,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first character
		move.w	#13,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second character
*
srs8b:		btst	#2,vrbw08+1	* group 3 ?
		beq	srs8c		* jump if not
*
		move.w	#17,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first character
		move.w	#18,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second character
*
srs8c:		btst	#3,vrbw08+1	* group 4 ?
		beq	srs8d		* jump if not
*
		move.w	#22,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first character
		move.w	#23,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second character
*
		.page
*
srs8d:		btst	#4,vrbw08+1	* group 5 ?
		beq	srs8e		* jump if not
*
		move.w	#27,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first character
		move.w	#28,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second character
*
srs8e:		btst	#5,vrbw08+1	* group 6 ?
		beq	srs8f		* jump if not
*
		move.w	#32,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first character
		move.w	#33,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second character
*
srs8f:		btst	#6,vrbw08+1	* group 7 ?
		beq	srs8g		* jump if not
*
		move.w	#37,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first character
		move.w	#38,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second character
*
srs8g:		btst	#7,vrbw08+1	* group 8 ?
		beq	srs8h		* jump if not
*
		move.w	#42,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first character
		move.w	#43,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second character
*
		.page
*
srs8h:		btst	#0,vrbw08	* group 9
		beq	srs8j		* jump if not
*
		move.w	#47,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first character
		move.w	#48,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second character
*
srs8j:		btst	#1,vrbw08	* group 10
		beq	srs8k		* jump if not
*
		move.w	#52,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first character
		move.w	#53,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second character
*
srs8k:		btst	#2,vrbw08	* group 11
		beq	srs8m		* jump if not
*
		move.w	#57,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first character
		move.w	#58,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second character
*
srs8m:		btst	#3,vrbw08	* group 12
		beq	srs8x		* jump if not
*
		move.w	#62,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first character
		move.w	#63,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second character
*
srs8x:		move.w	vrcw,d0		* restore vrcw to d0
*
		.page
*
* transpose
* ---------
srs9:		btst	#9,d0		* transpose ?
		beq	srs10		* jump if not
*
		move.w	#AT11,ATR(a7)	* put attribute on stack
		move.w	#3,ROW(a7)	* put row on stack
		btst	#0,vrbw09+1	* group 1 ?
		beq	srs9a		* jump if not
*
		move.w	#5,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#6,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#7,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#8,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
srs9a:		btst	#1,vrbw09+1	* group 2 ?
		beq	srs9b		* jump if not
*
		move.w	#10,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#11,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#12,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#13,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
srs9b:		btst	#2,vrbw09+1	* group 3 ?
		beq	srs9c		* jump if not
*
		move.w	#15,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#16,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#17,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#18,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
		.page
*
srs9c:		btst	#3,vrbw09+1	* group 4 ?
		beq	srs9d		* jump if not
*
		move.w	#20,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#21,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#22,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#23,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
srs9d:		btst	#4,vrbw09+1	* group 5 ?
		beq	srs9e		* jump if not
*
		move.w	#25,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#26,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#27,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#28,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
srs9e:		btst	#5,vrbw09+1	* group 6 ?
		beq	srs9f		* jump if not
*
		move.w	#30,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#31,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#32,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#33,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
		.page
*
srs9f:		btst	#6,vrbw09+1	* group 7 ?
		beq	srs9g		* jump if not
*
		move.w	#35,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#36,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#37,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#38,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
srs9g:		btst	#7,vrbw09+1	* group 8 ?
		beq	srs9h		* jump if not
*
		move.w	#40,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#41,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#42,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#43,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
srs9h:		btst	#0,vrbw09	* group 9
		beq	srs9j		* jump if not
*
		move.w	#45,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#46,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#47,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#48,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
		.page
*
srs9j:		btst	#1,vrbw09	* group 10
		beq	srs9k		* jump if not
*
		move.w	#50,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#51,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#52,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#53,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
srs9k:		btst	#2,vrbw09	* group 11
		beq	srs9m		* jump if not
*
		move.w	#55,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#56,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#57,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#58,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
srs9m:		btst	#3,vrbw09	* group 12
		beq	srs9x		* jump if not
*
		move.w	#60,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#61,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#62,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#63,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
srs9x:		move.w	vrcw,d0		* restore vrcw to d0
*
		.page
*
* dynamics
* --------
srs10:		btst	#10,d0		* dynamics ?
		beq	srs11		* jump if not
*
		move.w	#AT11,ATR(a7)	* put attribute on stack
		move.w	#4,ROW(a7)	* put row on stack
		btst	#0,vrbw10+1	* group 1 ?
		beq	srs10a		* jump if not
*
		move.w	#6,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs10a:		btst	#1,vrbw10+1	* group 2 ?
		beq	srs10b		* jump if not
*
		move.w	#11,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs10b:		btst	#2,vrbw10+1	* group 3 ?
		beq	srs10c		* jump if not
*
		move.w	#16,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs10c:		btst	#3,vrbw10+1	* group 4 ?
		beq	srs10d		* jump if not
*
		move.w	#21,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
		.page
*
srs10d:		btst	#4,vrbw10+1	* group 5 ?
		beq	srs10e		* jump if not
*
		move.w	#26,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs10e:		btst	#5,vrbw10+1	* group 6 ?
		beq	srs10f		* jump if not
*
		move.w	#31,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs10f:		btst	#6,vrbw10+1	* group 7 ?
		beq	srs10g		* jump if not
*
		move.w	#36,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs10g:		btst	#7,vrbw10+1	* group 8 ?
		beq	srs10h		* jump if not
*
		move.w	#41,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
		.page
*
srs10h:		btst	#0,vrbw10	* group 9
		beq	srs10j		* jump if not
*
		move.w	#46,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs10j:		btst	#1,vrbw10	* group 10
		beq	srs10k		* jump if not
*
		move.w	#51,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs10k:		btst	#2,vrbw10	* group 11
		beq	srs10m		* jump if not
*
		move.w	#56,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs10m:		btst	#3,vrbw10	* group 12
		beq	srs10x		* jump if not
*
		move.w	#61,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs10x:		move.w	vrcw,d0		* restore vrcw to d0
*
		.page
*
* location
* --------
srs11:		btst	#11,d0		* location ?
		beq	srs12		* jump if not
*
		move.w	#AT11,ATR(a7)	* put attribute on stack
		move.w	#4,ROW(a7)	* put row on stack
		btst	#0,vrbw11+1	* group 1 ?
		beq	srs11a		* jump if not
*
		move.w	#8,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs11a:		btst	#1,vrbw11+1	* group 2 ?
		beq	srs11b		* jump if not
*
		move.w	#13,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs11b:		btst	#2,vrbw11+1	* group 3 ?
		beq	srs11c		* jump if not
*
		move.w	#18,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs11c:		btst	#3,vrbw11+1	* group 4 ?
		beq	srs11d		* jump if not
*
		move.w	#23,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
		.page
*
srs11d:		btst	#4,vrbw11+1	* group 5 ?
		beq	srs11e		* jump if not
*
		move.w	#28,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs11e:		btst	#5,vrbw11+1	* group 6 ?
		beq	srs11f		* jump if not
*
		move.w	#33,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs11f:		btst	#6,vrbw11+1	* group 7 ?
		beq	srs11g		* jump if not
*
		move.w	#38,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs11g:		btst	#7,vrbw11+1	* group 8 ?
		beq	srs11h		* jump if not
*
		move.w	#43,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
		.page
*
srs11h:		btst	#0,vrbw11	* group 9
		beq	srs11j		* jump if not
*
		move.w	#48,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs11j:		btst	#1,vrbw11	* group 10
		beq	srs11k		* jump if not
*
		move.w	#53,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs11k:		btst	#2,vrbw11	* group 11
		beq	srs11m		* jump if not
*
		move.w	#58,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs11m:		btst	#3,vrbw11	* group 12
		beq	srs11x		* jump if not
*
		move.w	#63,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs11x:		move.w	vrcw,d0		* restore vrcw to d0
*
		.page
*
* velocity
* --------
srs12:		btst	#12,d0		* velocity ?
		beq	srs13		* jump if not
*
		move.w	#AT11,ATR(a7)	* put attribute on stack
		move.w	#5,ROW(a7)	* put row on stack
		btst	#0,vrbw12+1	* group 1 ?
		beq	srs12a		* jump if not
*
		move.w	#6,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#7,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#8,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
*
srs12a:		btst	#1,vrbw12+1	* group 2 ?
		beq	srs12b		* jump if not
*
		move.w	#11,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#12,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#13,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
*
srs12b:		btst	#2,vrbw12+1	* group 3 ?
		beq	srs12c		* jump if not
*
		move.w	#16,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#17,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#18,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
*
		.page
*
srs12c:		btst	#3,vrbw12+1	* group 4 ?
		beq	srs12d		* jump if not
*
		move.w	#21,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#22,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#23,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
*
srs12d:		btst	#4,vrbw12+1	* group 5 ?
		beq	srs12e		* jump if not
*
		move.w	#26,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#27,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#28,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
*
srs12e:		btst	#5,vrbw12+1	* group 6 ?
		beq	srs12f		* jump if not
*
		move.w	#31,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#32,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#33,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
*
		.page
*
srs12f:		btst	#6,vrbw12+1	* group 7 ?
		beq	srs12g		* jump if not
*
		move.w	#36,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#37,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#38,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
*
srs12g:		btst	#7,vrbw12+1	* group 8 ?
		beq	srs12h		* jump if not
*
		move.w	#41,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#42,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#43,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
*
srs12h:		btst	#0,vrbw12	* group 9
		beq	srs12j		* jump if not
*
		move.w	#46,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#47,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#48,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
*
		.page
*
srs12j:		btst	#1,vrbw12	* group 10
		beq	srs12k		* jump if not
*
		move.w	#51,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#52,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#53,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
*
srs12k:		btst	#2,vrbw12	* group 11
		beq	srs12m		* jump if not
*
		move.w	#56,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#57,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#58,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
*
srs12m:		btst	#3,vrbw12	* group 12
		beq	srs12x		* jump if not
*
		move.w	#61,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#62,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#63,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
*
srs12x:		move.w	vrcw,d0		* restore vrcw to d0
*
		.page
*
* resolution
* ----------
srs13:		btst	#13,d0		* resolution ?
		beq	srs14		* jump if not
*
		move.w	#AT12,ATR(a7)	* put attribute on stack
		move.w	#7,ROW(a7)	* put row on stack
		btst	#0,vrbw13+1	* variable 1 ?
		beq	srs13a		* jump if not
*
		move.w	#6,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs13a:		btst	#1,vrbw13+1	* variable 2 ?
		beq	srs13b		* jump if not
*
		move.w	#15,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs13b:		btst	#2,vrbw13+1	* variable 3 ?
		beq	srs13c		* jump if not
*
		move.w	#24,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs13c:		btst	#3,vrbw13+1	* variable 4 ?
		beq	srs13d		* jump if not
*
		move.w	#33,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs13d:		btst	#4,vrbw13+1	* variable 5 ?
		beq	srs13e		* jump if not
*
		move.w	#42,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs13e:		btst	#5,vrbw13+1	* variable 6 ?
		beq	srs13x		* jump if not
*
		move.w	#51,COL(a7)	* put column on stack
		jsr	_vputa		* reset column
*
srs13x:		move.w	vrcw,d0		* restore vrcw to d0
*
		.page
*
* analog value
* ------------
srs14:		btst	#14,d0		* analog value ?
		beq	srs15		* jump if not
*
		move.w	#AT12,ATR(a7)	* put attribute on stack
		move.w	#7,ROW(a7)	* put row on stack
		btst	#0,vrbw14+1	* variable 1 ?
		beq	srs14a		* jump if not
*
		move.w	#8,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#9,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#10,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#11,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
		move.w	#12,COL(a7)	* put 5th column on stack
		jsr	_vputa		* reset fifth column
*
srs14a:		btst	#1,vrbw14+1	* variable 2 ?
		beq	srs14b		* jump if not
*
		move.w	#17,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#18,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#19,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#20,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
		move.w	#21,COL(a7)	* put 5th column on stack
		jsr	_vputa		* reset fifth column
*
		.page
*
srs14b:		btst	#2,vrbw14+1	* variable 3 ?
		beq	srs14c		* jump if not
*
		move.w	#26,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#27,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#28,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#29,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
		move.w	#30,COL(a7)	* put 5th column on stack
		jsr	_vputa		* reset fifth column
*
srs14c:		btst	#3,vrbw14+1	* variable 4 ?
		beq	srs14d		* jump if not
*
		move.w	#35,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#36,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#37,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#38,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
		move.w	#39,COL(a7)	* put 5th column on stack
		jsr	_vputa		* reset fifth column
*
		.page
*
srs14d:		btst	#4,vrbw14+1	* variable 5 ?
		beq	srs14e		* jump if not
*
		move.w	#44,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#45,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#46,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#47,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
		move.w	#48,COL(a7)	* put 5th column on stack
		jsr	_vputa		* reset fifth column
*
srs14e:		btst	#5,vrbw14+1	* variable 6 ?
		beq	srs14x		* jump if not
*
		move.w	#53,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#54,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#55,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#56,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
		move.w	#57,COL(a7)	* put 5th column on stack
		jsr	_vputa		* reset fifth column
*
srs14x:		move.w	vrcw,d0		* restore vrcw to d0
*
		.page
*
* stop/next
* ---------
srs15:		btst	#15,d0		* stop/next ?
		beq	srs16		* jump if not
*
		btst	#0,vrbw15	* stop ?
		beq	srs15a		* jump if not
*
		move.w	#AT08,ATR(a7)	* put attribute on stack
		move.w	#1,ROW(a7)	* put row on stack
		move.w	#40,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#41,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#42,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#43,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
srs15a:		btst	#1,vrbw15	* next ?
		beq	srs16		* jump if not
*
		move.w	#AT08,ATR(a7)	* put attribute on stack
		move.w	#1,ROW(a7)	* put row on stack
		move.w	#45,COL(a7)	* put 1st column on stack
		jsr	_vputa		* reset first column
		move.w	#46,COL(a7)	* put 2nd column on stack
		jsr	_vputa		* reset second column
		move.w	#47,COL(a7)	* put 3rd column on stack
		jsr	_vputa		* reset third column
		move.w	#48,COL(a7)	* put 4th column on stack
		jsr	_vputa		* reset fourth column
*
srs16:		add.l	#10,a7		* clean up stack
		bra	srsexit		* done
*
		.page
*
		.bss
*
* local copies of _vrcw, _vrbw08.._vrbw15
*
vrcw:		.ds.w	1
vrbw08:		.ds.w	1
vrbw09:		.ds.w	1
vrbw10:		.ds.w	1
vrbw11:		.ds.w	1
vrbw12:		.ds.w	1
vrbw13:		.ds.w	1
vrbw14:		.ds.w	1
vrbw15:		.ds.w	1
*
		.end
