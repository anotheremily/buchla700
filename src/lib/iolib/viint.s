* ------------------------------------------------------------------------------
* viint.s -- VSDD Vertical Interval interrupt handler for the Buchla 700
* Version 17 -- 1989-12-19 -- D.N. Lynx Crowe
*
*	VIint
*
*		VSDD Vertical Interval interrupt handler.  Enables display of
*		any object whose bit is set in vi_ctl.  Bit 0 = object 0, etc.
*
*		SetPri() uses BIOS(B_SETV, 25, VIint) to set the interrupt
*		vector and lets VIint() enable the object.  If vi_dis
*		is set, SetPri() won't enable the interrupt or set the vector
*		so that several objects can be started up at once.
*
*		This routine also sets the base address and scroll offset
*		for the score display object if vi_sadr is non-zero,
*		after a delay for VSDD FRAMESTOP synchronization.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_VIint			* Vertical Interval int. handler
*
		.xdef	_vi_sadr		* score object base address
		.xdef	_vi_scrl		* score object scroll offset
		.xdef	lclsadr			* local scroll address
		.xdef	lclscrl			* local scroll offset
		.xdef	vdelay			* VSDD scroll delay
*
		.xdef	VIct1			* VSDD interrupt R11 
		.xdef	VIct2			* VSDD interrupt R11 
		.xdef	VIct3			* VSDD interrupt R11 
		.xdef	VIct4			* VSDD interrupt R11 
*
		.xref	_v_regs			* VSDD registers
		.xref	_v_odtab		* VSDD object descriptor table
*
		.xref	_vi_clk			* scroll delay timer
		.xref	_vi_ctl			* unblank control word
		.xref	_vi_tag			* VSDD 'needs service' tag
*
		.page
*
* Miscellaneous equates:
* ----------------------
*
DELAY		.equ	17			* FRAMESTOP sync delay in Ms
STACKSR		.equ	32			* offset to sr on stack
V_BLA		.equ	4			* V_BLA (blank) bit number
VSDD_R5		.equ	10			* VSDD R5 byte offset in _v_regs
VSDD_R11	.equ	22			* VSDD R11 byte offset in _v_regs
*
VT_BASE		.equ	128			* word offset of VSDD Access Table
*
VT_1		.equ	VT_BASE+300		* high time
VT_2		.equ	VT_BASE+2		* low time
* ------------------------------------------------------------------------------
*
* Stack picture after movem.l at entry:
* -------------------------------------
*
*	LONG	PC	+34
*	WORD	SR	+32	STACKSR
*	LONG	A6	+28
*	LONG	A2	+24
*	LONG	A1	+20
*	LONG	A0	+16
*	LONG	D3	+12
*	LONG	D2	+8
*	LONG	D1	+4
*	LONG	D0	+0
*
* ------------------------------------------------------------------------------
*
		.page
*
* _VIint -- Vertical interval interrupt handler
* ------    -----------------------------------
_VIint:		movem.l	d0-d3/a0-a2/a6,-(a7)	* save registers
		addi.w	#$0100,STACKSR(a7)	* raise IPL in sr on the stack
*
		move.w	_v_regs+VSDD_R11,VIct1	* save the VSDD R11 value
*
		tst.w	_vi_sadr		* see if we should scroll
		beq	viunbl			* jump if not
*
* ------------------------------------------------------------------------------
* setup delayed scroll parameters
* ------------------------------------------------------------------------------
		move.w	_v_regs+VSDD_R5,d0	* get VSDD R5
		move.w	d0,d1			* save it for later
		andi.w	#$0180,d0		* see if we're already in bank 0
		beq	dlyscrl			* jump if so
*
		clr.w	_v_regs+VSDD_R5		* set bank 0
*
		move.w	_v_regs+VSDD_R11,VIct2	* save the VSDD R11 value
*
vw1a:		cmp.w	#VT_1,_v_regs+VSDD_R11	* wait for FRAMESTOP
		bcc	vw1a			* ...
*
vw2a:		cmp.w	#VT_1,_v_regs+VSDD_R11	* ...
		bcs	vw2a			* ...
*
vw3a:		cmp.w	#VT_1,_v_regs+VSDD_R11	* ...
		bcc	vw3a			* ...
*
vw4a:		cmp.w	#VT_2,_v_regs+VSDD_R11	* ...
		bcs	vw4a			* ...
*
dlyscrl:	tst.w	_vi_tag			* wait for previous scroll
		bne	dlyscrl			* ...
*
		move.w	_vi_sadr,lclsadr	* save address for timeint
		move.w	_vi_scrl,lclscrl	* save offset for timeint
		clr.w	_vi_sadr		* reset for next time
		clr.w	_vi_scrl		* ...
		move.w	vdelay,_vi_clk		* set the scroll delay timer
		st	_vi_tag			* set the 'need service' tag
* ------------------------------------------------------------------------------
* check for unblank requests
* ------------------------------------------------------------------------------
		move.w	_vi_ctl,d2		* get the unblank control word
		beq	viexit			* exit if nothing to unblank
*
		bra	unblnk			* go unblank some objects
*
viunbl:		move.w	_vi_ctl,d2		* get the unblank control word
		beq	vidone			* exit if nothing to unblank
*
		move.w	_v_regs+VSDD_R5,d0	* get VSDD R5
		move.w	d0,d1			* save it for later
		andi.w	#$0180,d0		* see if we're already in bank 0
		beq	unblnk			* jump if so
*
		clr.w	_v_regs+VSDD_R5		* set bank 0
*
		move.w	_v_regs+VSDD_R11,VIct3	* save the VSDD R11 value
*
vw1b:		cmp.w	#VT_1,_v_regs+VSDD_R11	* wait for FRAMESTOP
		bcc	vw1b			* ...
*
vw2b:		cmp.w	#VT_1,_v_regs+VSDD_R11	* ...
		bcs	vw2b			* ...
*
vw3b:		cmp.w	#VT_1,_v_regs+VSDD_R11	* ...
		bcc	vw3b			* ...
*
vw4b:		cmp.w	#VT_2,_v_regs+VSDD_R11	* ...
		bcs	vw4b			* ...
*
		.page
*
* ------------------------------------------------------------------------------
* unblank objects indicated by contents of d2  (loaded earlier from _vi_ctl)
* ------------------------------------------------------------------------------
*
unblnk:		clr.w	d3			* clear the counter
		clr.w	_vi_ctl			* clear the unblank control word
		lea	_v_odtab,a1		* point at first object
*
vicheck:	btst	d3,d2			* check the object bit
		beq	vinext			* go check next one if not set
*
		move.w	(a1),d0			* get v_odtab[obj][0]
		bclr	#V_BLA,d0		* clear the blanking bit
		move.w	d0,(a1)			* set v_odtab[obj][0]
*
vinext:		cmpi.w	#15,d3			* see if we're done
		beq	viexit			* jump if so
*
		addq.l	#8,a1			* point at next object
		addq.w	#1,d3			* increment object counter
		bra	vicheck			* go check next object
*
* ------------------------------------------------------------------------------
* switch back to the bank the interrupted code was using if we changed it
* ------------------------------------------------------------------------------
*
viexit:		move.w	d1,d0			* see if we were in bank 0
		andi.w	#$0180,d0		* ...
		beq	vidone			* jump if so
*
viwait:		tst.w	_vi_tag			* wait for timer to run out
		bne	viwait			* ... so timeint sees bank 0
*
		move.w	d1,_v_regs+VSDD_R5	* restore v_regs[5] to old bank
*
		move.w	_v_regs+VSDD_R11,VIct4	* save the VSDD R11 value
*
vw1c:		cmp.w	#VT_1,_v_regs+VSDD_R11	* wait for FRAMESTOP
		bcc	vw1c			* ...
*
vw2c:		cmp.w	#VT_1,_v_regs+VSDD_R11	* ...
		bcs	vw2c			* ...
*
vw3c:		cmp.w	#VT_1,_v_regs+VSDD_R11	* ...
		bcc	vw3c			* ...
*
vw4c:		cmp.w	#VT_2,_v_regs+VSDD_R11	* ...
		bcs	vw4c			* ...
*
* ------------------------------------------------------------------------------
* restore registers and return to interrupted code
* ------------------------------------------------------------------------------
*
vidone:		movem.l	(a7)+,d0-d3/a0-a2/a6	* restore registers
		rte				* return from interrupt
*
		.page
*
* ------------------------------------------------------------------------------
		.data
* ------------------------------------------------------------------------------
*
vdelay:		.dc.w	DELAY			* VSDD scroll delay
*
* ------------------------------------------------------------------------------
		.bss
* ------------------------------------------------------------------------------
*
_vi_sadr:	.ds.w	1			* score object base address
_vi_scrl:	.ds.w	1			* score object scroll offset
*
lclsadr:	.ds.w	1			* local copy of vi_sadr
lclscrl:	.ds.w	1			* local copy of vi_scrl
*
VIct1:		.ds.w	1			* VSDD R11 value at interrupt
VIct2:		.ds.w	1			* VSDD R11 value at interrupt
VIct3:		.ds.w	1			* VSDD R11 value at interrupt
VIct4:		.ds.w	1			* VSDD R11 value at interrupt
*
		.end
