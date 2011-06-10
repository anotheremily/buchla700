* ------------------------------------------------------------------------------
* execins.s -- execute an instrument selection
* Version 9 -- 1988-09-09 -- D.N. Lynx Crowe
*
* Hand optimized C object code from:  execins.c -- ver 2 -- 1988-06-29
* ------------------------------------------------------------------------------
*
		.text
*
DOUBLED		.equ	1			* double output to WS table
*
		.xdef	_execins
*
		.xref	_clrvce
		.xref	_execkey
*
		.xref	_io_fpu
		.xref	_instmod
		.xref	_s_inst
		.xref	_idefs
		.xref	_vbufs
*
* register equates
* ----------------
SYNC		.equ	D4
OLDSR		.equ	D5
VCE		.equ	D7
*
IP		.equ	A5
P1		.equ	A4
P2		.equ	A3
*
P3		.equ	A2
P4		.equ	A1
*
* parameter offsets
* -----------------
VOICE		.equ	8
INS		.equ	10
TAG		.equ	12
*
		.page
*
_execins:	link	A6,#0			* link stack frames
		movem.l	D3-VCE/P2-IP,-(sp)	* preserve registers
		move.w	VOICE(A6),VCE		* get voice number
		move.w	VCE,D0			* calculate IP
		add.w	D0,D0			* ...
		add.w	D0,D0			* ...
		lea	vibtabl,A0		* ...
		movea.l	0(A0,D0.W),IP		* ...
		add.l	#_vbufs,IP		* ...
		move.w	VCE,(sp)		* clrvce(vce)
		jsr	_clrvce			* ...
		tst.w	TAG(A6)			* if (tag) {
		beq	L2			* ...
*
		move.l	IP,P1			* p1 = ip;
		move.w	INS(A6),D0		* p2 = &idefs[ins]
		add.w	D0,D0			* ...
		add.w	D0,D0			* ...
		lea	vibtabl,A0		* ...
		movea.l	0(A0,D0.W),P2		* ...
		add.l	#_idefs,P2		* ...
		move.w	#1968,D0		* set move count
*
L6:		move.w	(P2)+,(P1)+		* load voice buffer
		dbra	D0,L6			* ...
*
		.page
*
		move.w	VCE,D0			* calculate FPU offset for voice
		move.w	#9,D1			* ...
		asl.w	D1,D0			* ...
		asl.w	#1,D0			* ...
		ext.l	D0			* ...
		move.l	D0,P1			* ...
		move.l	D0,P3			* ...
		add.l	#_io_fpu,P1		* point P1 at FPU WS A
		add.l	#$202,P1		* ...
		lea	1778(IP),P2		* point P2 at instdef WS A
		add.l	#_io_fpu,P3		* point P3 at FPU WS B
		add.l	#2,P3			* ...
		lea	2858(IP),P4		* point P4 at instdef WS B
		move.w	#253,D0			* setup move count
*
		.ifne	DOUBLED
		move.l	P1,-(a7)		* save P1 on stack
		move.l	P2,-(a7)		* save P2 on stack
		move.l	P3,-(a7)		* save P3 on stack
		move.l	P4,-(a7)		* save P4 on stack
		move.w	D0,-(a7)		* save D0 on stack
		.endc
*
		move.w	SR,OLDSR		* turn off FPU interrupts
		move.w	#$2200,SR		* ...
*
L10:		move.w	(P2)+,(P1)+		* set FPU WS A from instdef
		nop				* delay for FPU timing
		nop				* ...
		nop				* ...
		move.w	(P4)+,(P3)+		* set FPU WS B from instdef
		nop				* delay for FPU timing
		nop				* ...
		dbra	D0,L10			* loop until done
*
		.ifne	DOUBLED
		move.w	(a7)+,D0		* restore D0 from stack
		movea.l	(a7)+,P4		* restore P4 from stack
		movea.l	(a7)+,P3		* restore P3 from stack
		movea.l	(a7)+,P2		* restore P2 from stack
		movea.l	(a7)+,P1		* restore P1 from stack
*
L10A:		move.w	(P2)+,(P1)+		* set FPU WS A from instdef
		nop				* delay for FPU timing
		nop				* ...
		nop				* ...
		move.w	(P4)+,(P3)+		* set FPU WS B from instdef
		nop				* delay for FPU timing
		nop				* ...
		dbra	D0,L10A			* loop until done
*
		.endc
*
		move.w	OLDSR,SR		* enable FPU interrupts
*
		.page
*
		move.w	VCE,A0			* instmod[vce] = FALSE
		add.l	A0,A0			* ...
		add.l	#_instmod,A0		* ...
		clr.w	(A0)			* ...
		move.w	VCE,A0			* s_inst[vce] = ins
		add.l	A0,A0			* ...
		add.l	#_s_inst,A0		* ...
		move.w	INS(A6),(A0)		* ...
		move.b	70(IP),SYNC		* get config bits
		ext.w	SYNC			* ...
		asl.w	#8,SYNC			* ...
		btst	#2,74(IP)		* sync 2nd osc
		beq	L15			* ...
*
		ori.w	#$2000,SYNC		* ...
*
L15:		btst	#2,75(IP)		* sync 3rd osc
		beq	L16			* ...
*
		ori.w	#$4000,SYNC		* ...
*
L16:		btst	#2,76(IP)		* sync 4th osc
		beq	L17			* ...
*
		ori.w	#$8000,SYNC		* ...
*
L17:		move.w	VCE,A0			* send sync to FPU
		add.l	A0,A0			* ...
		add.l	#_io_fpu,A0		* ...
		add.l	#$5FE0,A0		* ...
		move.w	SYNC,(A0)		* ...
*
L2:		move.w	#1,(sp)			* execkey(-1, 0, vce, 1)
		move.w	VCE,-(sp)		* ...
		clr.w	-(sp)			* ...
		move.w	#-1,-(sp)		* ...
		jsr	_execkey		* ...
		addq.l	#6,sp			* ...
		tst.l	(sp)+			* clean up stack
		movem.l	(sp)+,SYNC-VCE/P2-IP	* restore registers
		unlk	A6			* unlink stack frames
		rts				* return
*
		.page
*
		.data
*
* vibtabl -- voice and instrument buffer offset table
* -------    ----------------------------------------
vibtabl:	.dc.l	0		* 0
		.dc.l	3938		* 1
		.dc.l	7876		* 2
		.dc.l	11814		* 3
		.dc.l	15752		* 4
		.dc.l	19690		* 5
		.dc.l	23628		* 6
		.dc.l	27566		* 7
		.dc.l	31504		* 8
		.dc.l	35442		* 9
		.dc.l	39380		* 10
		.dc.l	43318		* 11
		.dc.l	47256		* 12
		.dc.l	51194		* 13
		.dc.l	55132		* 14
		.dc.l	59070		* 15
		.dc.l	63008		* 16
		.dc.l	66946		* 17
		.dc.l	70884		* 18
		.dc.l	74822		* 19
		.dc.l	78760		* 20
		.dc.l	82698		* 21
		.dc.l	86636		* 22
		.dc.l	90574		* 23
		.dc.l	94512		* 24
		.dc.l	98450		* 25
		.dc.l	102388		* 26
		.dc.l	106326		* 27
		.dc.l	110264		* 28
		.dc.l	114202		* 29
		.dc.l	118140		* 30
		.dc.l	122078		* 31
		.dc.l	126016		* 32
		.dc.l	129954		* 33
		.dc.l	133892		* 34
		.dc.l	137830		* 35
		.dc.l	141768		* 36
		.dc.l	145706		* 37
		.dc.l	149644		* 38
		.dc.l	153582		* 39
		.dc.l	157520		* 40
*
		.end
