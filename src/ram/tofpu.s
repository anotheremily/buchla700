* ------------------------------------------------------------------------------
* tofpu.s -- convert between millisecond time interval and FPU increment formats
* Version 9 -- 1987-12-21 -- D.N. Lynx Crowe
*
*	unsigned short
*	tofpu(time)
*	unsigned short time;
*
*		Converts 'time' in milliseconds to FPU format.
*
*	unsigned short
*	fromfpu(fputime)
*	unsigned short fputime;
*
*		Converts 'fputime' from FPU format to milliseconds.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_tofpu
		.xdef	_fromfpu
*
TIME		.equ	8		* WORD - time argument  (either format)
*
TCYCL		.equ	$3B000000	* LONG - scaled FPU cycle time  (.4608)
*
		.page
_tofpu:		link	a6,#0		* link stack frames
		move.w	TIME(a6),d1	* get time interval
		beq	notime		* jump if zero time
*
		clr.w	d2		* clear shift count
*
sloop:		btst	#15,d1		* see if MSB is set yet
		bne	gotexp		* jump if so
*
		lsl.w	d1		* shift time left a bit
		addq.w	#1,d2		* increment the shift count
		cmpi.w	#15,d2		* see if we've hit the maximum
		bne	sloop		* try again if not
*
gotexp:		move.l	#TCYCL,d0	* divide FPU cycle time by shifted value
		divu	d1,d0		* ...
		andi.w	#$FFF0,d0	* mask result
		or.w	d2,d0		* set the exponent
*
finis:		unlk	a6		* unlink stack frames
		rts			* return to caller
*
notime:		clr.w	d0		* zero time is zero ...
		bra	finis		* return value for zero time
*
		.page
_fromfpu:	link	a6,#0		* link stack frames
		move.w	TIME(a6),d1	* get FPU formatted time
		beq	zerotime	* done if it's zero
*
		move.w	d1,d2		* extract shift count
		andi.w	#$000F,d2	* ...
		andi.w	#$FFF0,d1	* extract mantissa
		beq	zerotime	* ... just in case it's zero  (an error)
*
		move.l	#TCYCL,d0	* get FPU cycle time
		divu	d1,d0		* divide by mantissa
		bvc	divok		* jump if divide ok
*
		move.w	#$FFFF,d0	* jam maximum value for overflow
*
divok:		lsr.w	d2,d0		* shift result into position
*
byebye:		unlk	a6		* unlink stack frames
		rts			* return to caller
*
zerotime:	clr.l	d0		* return a zero result
		bra	byebye		* ...
*
		.end
