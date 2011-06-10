* ------------------------------------------------------------------------------
* sedisp.s -- score event display driver
* Version 43 -- 1988-09-26 -- D.N. Lynx Crowe
*
*	se_disp(ep, sd, gdstb, cf)
*	struct s_entry *ep;
*	short sd;
*	struct gdsel *gdstb[];
*	short cf;
*
*		Displays the event at 'ep', scrolling in direction 'sd', by
*		updating 'gdstb'.  Uses the accidental code in 'ac_code', and
*		the note type table 'nsvtab'.  Checks 'cf' to determine if
*		we're displaying in the center of the screen.
*		Allocates gdsel events as needed for new events.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_se_disp
*
		.xdef	_ac_code
		.xdef	numstr
*
		.xref	_dclkmd
		.xref	_dsgmodz
		.xref	_fromfpu
		.xref	_mpcupd
		.xref	_vputa
		.xref	_vputc
		.xref	_vputs
*
		.xref	_angroup
		.xref	_ctrsw
		.xref	_grpstat
		.xref	_ins2grp
		.xref	_ndisp
		.xref	_obj8
		.xref	_gdfsep
		.xref	_lastvel
		.xref	_nsvtab
		.xref	_velflag
		.xref	_vrbw08
		.xref	_vrbw09
		.xref	_vrbw10
		.xref	_vrbw11
		.xref	_vrbw12
		.xref	_vrbw13
		.xref	_vrbw14
		.xref	_vrbw15
		.xref	_vrcw
*
		.page
*
* parameter offsets
* -----------------
* for se_disp:
* ------------
P_EP		.equ	8		* LONG - event pointer
P_SD		.equ	12		* WORD - scroll direction
P_SL		.equ	14		* LONG - slice control table pointer
P_CF		.equ	18		* WORD - center slice flag
*
* for vputa:
* ----------
ROW		.equ	4		* WORD - 'row' parameter offset
COL		.equ	6		* WORD - 'col' parameter offset
ATR		.equ	8		* WORD - 'atr' parameter offset
*
* Character field attributes for highlighting
* -------------------------------------------
AT01		.equ	$0054
AT04		.equ	$0053
AT05		.equ	$0054
AT06		.equ	$0053
AT07		.equ	$0054
AT08		.equ	$0053
AT09		.equ	$0054
AT10		.equ	$0053
AT11		.equ	$0052
AT12		.equ	$0052
*
* Special character equates
* -------------------------
SP_M1		.equ	$A1			* -1
SP_P1		.equ	$A0			* +1
*
		.page
*
* event structure offsets
* -----------------------
*		      offset		 length
*		      ------             ------
E_TIME		.equ	0		* LONG
E_SIZE		.equ	4		* BYTE
E_TYPE		.equ	5		* BYTE
E_DATA1		.equ	6		* BYTE
E_NOTE		.equ	6		* BYTE
E_DATA2		.equ	7		* BYTE
E_GROUP		.equ	7		* BYTE
E_BAK		.equ	8		* LONG
E_FWD		.equ	12		* LONG
E_DN		.equ	16		* LONG
E_VEL		.equ	16		* WORD
E_DATA4		.equ	18		* WORD
E_UP		.equ	20		* LONG
E_LFT		.equ	24		* LONG
E_RGT		.equ	28		* LONG
*
N_ETYPES	.equ	25		* number of event types
*
* gdsel structure definitions
* ---------------------------
G_NEXT		.equ	0		* long - 'next' field	(struct gdsel *)
G_NOTE		.equ	4		* word - 'note' field	(short)
G_CODE		.equ	6		* word - 'code' field	(short)
*
NATCH_B		.equ	3		* uslice note code:  'begin natural'
NOTE_E		.equ	6		* uslice note code:  'end note'
*
NGDSEL		.equ	17		* number of event slots in gdstb
*
BARFLAG		.equ	4*(NGDSEL-1)	* offset to the bar marker flag
*
		.page
*
* A few words about se_disp:
* --------------------------
* se_disp has to be very fast, so it's written in assembly language,
* rather than C, which is usually pretty good, but not quite good enough
* for this application.  The faster this routine runs, the higher the
* tempo we can keep up with.  If this code is fast enough, we end up
* hardware limited by the maximum rate of the timer, and the VSDD update rate.
*
_se_disp:	link	a6,#0			* allocate and link stack frame
		movea.l	P_EP(a6),a0		* get event pointer 'ep' into a0
		move.b	E_TYPE(a0),d1		* get event type into d1.W
		andi.w	#$007F,d1		* mask off new-note flag
		cmp.b	#N_ETYPES,d1		* see if it's valid
		blt	seds1			* jump if it is
*
dsexit:		unlk	a6			* done -- unlink stack frames
		rts				* return to caller
*
seds1:		lea	sddtab,a1		* get base of dispatch table
		lsl.w	#2,d1			* multiplty event by 4 for index
		movea.l	0(a1,d1.W),a2		* get address of event routine
		jmp	(a2)			* jump to event display routine
*
* On entry, the individual display routines only depend on a0 pointing at the
* event they were dispatched for.  Registers a6 and a7 have their usual meaning,
* a6 = frame pointer, a7 = stack pointer.
*
* d0..d2 are used for scratch, as are a0..a2, and are not saved by this code.
*
		.page
*
* dsnbx -- dispatch to begin / end based on sd
* -----    -----------------------------------
dsnbx:		tst.w	P_SD(a7)		* check direction
		bne	dsne			* treat as end if going backward
*
* dsnb -- display note begin
* ----    ------------------
dsnb:		move.b	E_TYPE(a0),d1		* get event type
		move.w	d1,d2			* save in d2
		andi.w	#$007F,d1		* clear new-note flag
		move.b	d1,E_TYPE(a0)		* store type back in event
		clr.w	d0			* get group number
		move.b	E_GROUP(a0),d0		* ... in d0
		move.w	d0,d1			* save group in d1
		add.w	d0,d0			* make d0 a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	dsexit			* done if not enabled
*
		tst.w	P_CF(a6)		* check center slice flag
		beq	dsnb0			* jump if not center slice
*
		tst.w	_velflag		* see if we display velocity
		beq	dsnvx			* jump if not
*
		move.w	d2,-(a7)		* save new-note flag on stack
		move.w	d1,-(a7)		* save group number on stack
		move.w	E_VEL(a0),d0		* get velocity
		move.w	d1,d2			* point into lastvel[]
		add.w	d2,d2			* ...
		lea	_lastvel,a1		* ...
		move.w	d0,0(a1,d2.W)		* update lastvel[group]
		ext.l	d0			* scale
		divu	#252,d0			* ...
		cmpi.w	#100,d0			* convert MS digit
		bcs	dsnv0			* ...
*
		move.b	#'1',numstr		* ...
		subi.w	#100,d0			* ...
		bra	dsnv1			* ...
*
dsnv0:		move.b	#'0',numstr		* ...
*
dsnv1:		ext.l	d0			* convert middle & LS digits
		divu	#10,d0			* ...
		addi.l	#$00300030,d0		* ...
		move.b	d0,numstr+1		* ...
		swap	d0			* ...
		move.b	d0,numstr+2		* ...
		clr.b	numstr+3		* terminate string
		move.w	d1,d0			* col = group
		asl.w	#2,d0			* ... * 5
		add.w	d1,d0			* ...
		add.w	#6,d0			* ... + 6
		move.l	a0,-(a7)		* save event pointer on stack
		move.w	#AT11,-(a7)		* put attribute on stack
		move.l	#numstr,-(a7)		* put string address on stack
		move.w	d0,-(a7)		* put column on stack
		move.w	#5,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put VSDD address on stack
		jsr	_vputs			* update the screen
		add.l	#14,a7			* clean up stack
		movea.l	(a7)+,a0		* restore event pointer
*
		.page
*
		move.w	(a7)+,d0		* get group from stack
		cmpi.w	#12,d0			* see which byte it's in
		bcc	dsnv2			* jump if in MS byte
*
		bset	d0,_vrbw12+1		* set group bit in LS byte
		bra	dsnv3			* ...
*
dsnv2:		bset	d0,_vrbw12		* set group bit in MS byte
*
dsnv3:		bset	#4,_vrcw		* set video reset type bit
		move.w	(a7)+,d2		* get new-note flag from stack
*
dsnvx:		btst.l	#7,d2			* check new-note flag
		beq	dsexit			* done if not set
*
dsnb0:		clr.w	d1			* get note number nn (0..127)
		move.b	E_NOTE(a0),d1		* ... in d1
		sub.w	#21,d1			* subtract base of piano scale
		bmi	dsexit			* done if not displayable
*
		cmp.w	#87,d1			* see if it's too high
		bgt	dsexit			* done if not displayable
*
		move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		clr.w	d2			* d2 = ep->group
		move.b	E_GROUP(a0),d2		* ...
		lsl.w	#2,d2			* ... * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[ep->group]
		move.l	a1,0(a2,d2.W)		* gdstb[ep->group] = gdsp
		move.w	d1,G_NOTE(a1)		* gdsp->note = nn
		lea	_nsvtab,a2		* a2 points at nsvtab
		tst.b	0(a2,d1.W)		* check nsvtab[nn]
		beq	dsnb1			* jump if natural note
*
		move.b	_ac_code,d1		* setup for an accidental note
		bra	dsnb2			* ...
*
dsnb1:		move.b	#NATCH_B,d1		* setup for a natural note
*
dsnb2:		move.w	d1,G_CODE(a1)		* gdsp->code = note type
		bra	dsexit			* done
*
		.page
*
* dsnex -- dispatch to end/begin based on sd
* -----    ---------------------------------
dsnex:		tst.w	P_SD(a7)		* check direction
		bne	dsnb			* treat as begin if going backward
*
* dsne -- display note end
* ----    ----------------
dsne:		move.b	E_TYPE(a0),d1		* get event type
		move.w	d1,d2			* save in d2
		andi.w	#$007F,d1		* clear new-note flag
		move.b	d1,E_TYPE(a0)		* store type back in event
		clr.w	d0			* get group number
		move.b	E_GROUP(a0),d0		* ... in d0
		add.w	d0,d0			* ... as a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	dsexit			* done if not enabled
*
		tst.w	P_CF(a6)		* check center slice flag
		beq	dsne3			* jump if not center slice
*
		btst.l	#7,d2			* check new-note flag
		beq	dsexit			* done if not set
*
dsne3:		move.b	E_NOTE(a0),d1		* d1 = note number nn (0..127)
		sub.w	#21,d1			* subtract base of piano scale
		bmi	dsexit			* done if not displayable
*
		cmp.w	#87,d1			* see if it's too high
		bgt	dsexit			* done if not displayable
*
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		clr.w	d2			* get group in d2
		move.b	E_GROUP(a0),d2		* ...
		lsl.w	#2,d2			* ... * 4
		move.l	0(a2,d2.W),d0		* check gdstb[ep->group]
		beq	dsexit
*
dsne0:		movea.l	d0,a1			* a1 = gdsp
*
dsne1:		cmp.w	G_NOTE(a1),d1		* compare nn to gdsp->note
		bne	dsne2			* jump if not the one we want
*
		move.w	#NOTE_E,G_CODE(a1)	* gdsp->code = NOTE_E  (end note)
*
dsne2:		move.l	G_NEXT(a1),d0		* get gdsp->next
		beq	dsexit			* done if next = NULL
*
		bra	dsne0			* loop for next one
*
		.page
*
* dssbgn -- display section begin
* ------    ---------------------
dssbgn:		tst.w	P_CF(a6)		* center update ?
		beq	dsbgn0			* jump if not
*
		clr.w	d1			* get section number
		move.b	E_DATA1(a0),d1		* ... from the event
		addq.w	#1,d1			* ... adjusted for display
		ext.l	d1			* ... as a long in d1
		divu	#10,d1			* divide by 10 for conversion
		add.l	#$00300030,d1		* add '0' for ASCII conversion
		move.b	d1,numstr		* put MS byte in work area
		swap	d1			* swap register halves
		move.b	d1,numstr+1		* put LS byte in work area
		clr.b	numstr+2		* terminate string
		move.w	#AT01,-(a7)		* put attribute on stack
		move.l	#numstr,-(a7)		* put buffer address on stack
		move.w	#6,-(a7)		* put column on stack
		move.w	#0,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put sbase on stack
		jsr	_vputs			* update the screen
		add.l	#14,a7			* clean up stack
		bset	#4,_vrcw+1		* set video reset type bit
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dsbgn0:		move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#48,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$1111,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#0,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dssend -- display section end
* ------    -------------------
dssend:		tst.w	P_CF(a6)		* center update ?
		beq	dssend0			* jump if not
*
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dssend0:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#48,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$1111,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#2,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
* dsbeat -- display beat
* ------    ------------
dsbeat:		tst.w	P_CF(a6)		* center update ?
		bne	dsexit			* done if so
*
		move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#48,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$1111,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#1,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dstune -- display tuning
* ------    --------------
dstune:		tst.w	P_CF(a6)		* center update ?
		beq	dstune0			* jump if not
*
		clr.w	d1			* get current tuning
		move.b	E_DATA1(a0),d1		* ...
		add.w	#$0030,d1		* add '0' for ASCII conversion
		move.w	#AT05,-(a7)		* put attribute on stack
		move.w	d1,-(a7)		* put character on stack
		move.w	#19,-(a7)		* put column on stack
		move.w	#1,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put sbase on stack
		jsr	_vputc			* display character
		add.l	#12,a7			* clean up stack
		bset	#1,_vrcw+1		* set video reset type bit
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dstune0:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#52,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$CCCC,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#3,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[priority] = gdsp
		bra	dsexit			* done
*
		.page
*
* dstrns -- display transposition
* ------    ---------------------
dstrns:		clr.w	d0			* get group number
		move.b	E_DATA1(a0),d0		* ... in d0
		add.w	d0,d0			* ... as a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	dsexit			* done if not enabled
*
		tst.w	P_CF(a6)		* center update
		beq	dstrns0			* jump if not
*
		move.w	E_LFT(a0),d1		* get transposition value
		bpl	dstrns1			* jump if positive
*
		move.b	#'-',numstr		* note negative sign
		neg.w	d1			* make number positive
		bra	dstrns2			* ...
*
dstrns1:	move.b	#'+',numstr		* note positive sign
*
dstrns2:	cmpi.w	#1000,d1		* is number GE 1000 ?
		bcs	dstrns3			* jump if not
*
		subi.w	#1000,d1		* adjust number
		cmpi.b	#'-',numstr		* was number negative
		bne	dstrns4			* jump if not
*
		move.b	#SP_M1,numstr		* set -1 in numstr
		bra	dstrns3			* ...
*
dstrns4:	move.b	#SP_P1,numstr		* set +1 in numstr
*
dstrns3:	ext.l	d1			* make d1 a long
		divu	#100,d1			* convert 1st digit
		addi.w	#$0030,d1		* ... to ASCII
		move.b	d1,numstr+1		* ... in numstr
		swap	d1			* convert 2nd digit
		ext.l	d1			* ...
		divu	#10,d1			* ...
		addi.w	#$0030,d1		* ... to ASCII
		move.b	d1,numstr+2		* ... in numstr
		swap	d1			* convert 3rd digit
		addi.w	#$0030,d1		* ... to ASCII
		move.b	d1,numstr+3		* ... in numstr
		clr.b	numstr+4		* terminate numstr
*
		.page
*
		move.w	d0,d1			* get group number
		asr.w	#1,d1			* ... in d1
		move.w	d1,-(a7)		* save group number on stack
		add.w	d0,d0			* calculate column
		add.w	d0,d1			* ... = 5 * group
		addi.w	#5,d1			* ... + 5
		move.w	#AT11,-(a7)		* vputs(obj8, 3, col, numstr, atr11)
		move.l	#numstr,-(a7)		* ...
		move.w	d1,-(a7)		* ...
		move.w	#3,-(a7)		* ...
		move.l	_obj8,-(a7)		* ...
		jsr	_vputs			* ...
		add.l	#14,a7			* ...
		move.w	(a7)+,d0		* get group number
		cmpi.w	#8,d0			* see which byte it's in
		bcc	dstrns5			* jump if in MS byte
*
		bset	d0,_vrbw09+1		* set group bit in LS byte
		bra	dstrns6			* ...
*
dstrns5:	sub.w	#8,d0			* adjust for byte
		bset	d0,_vrbw09		* set group bit in MS byte
*
dstrns6:	bset	#1,_vrcw		* set video reset type bit
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dstrns0:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#52,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$9999,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#4,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dsdyn -- display dynamics
* -----    ----------------
dsdyn:		clr.w	d0			* get group number
		move.b	E_DATA1(a0),d0		* ... in d0
		add.w	d0,d0			* ... as a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	dsexit			* done if not enabled
*
		tst.w	P_CF(a6)		* center update ?
		beq	dsdyn00			* jump if not
*
		clr.w	d0			* get dynamics
		move.b	E_DATA2(a0),d0		* ... in d0
		move.w	d0,-(a7)		* save dyanmics
		clr.w	d1			* get group number
		move.b	E_DATA1(a0),d1		* ... in d1
		move.w	d1,-(a7)		* save group number
		move.w	(a7),d0			* col = group number
		add.w	d0,d0			* ... * 5
		add.w	d0,d0			* ...
		move.w	(a7)+,d2		* ...  (d2 = group number)
		add.w	d2,d0			* ...
		add.w	#6,d0			* ... + 6
		move.w	(a7)+,d1		* get dynamics
		add.w	#$0030,d1		* add '0' for ASCII conversion
		move.w	d2,-(a7)		* save group number
		move.w	#AT11,-(a7)		* put attribute on stack
		move.w	d1,-(a7)		* put digit on stack
		move.w	d0,-(a7)		* put column on stack
		move.w	#4,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put object address on stack
		jsr	_vputc			* update the screen
		add.l	#12,a7			* clean up stack
		move.w	(a7)+,d0		* get group number
		cmp.w	#8,d0			* see which word it's in
		bcc	dsdyn1			* jump if in MS word
*
		bset	d0,_vrbw10+1		* set group bit in LS byte
		bra	dsdyn2			* ...
*
dsdyn1:		sub.w	#8,d0			* adjust for for byte
		bset	d0,_vrbw10		* set group bit in MS byte
*
dsdyn2:		bset	#2,_vrcw		* set video reset type bit
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
		.page
dsdyn00:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#52,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$9999,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#5,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dslocn -- display location
* ------    ----------------
dslocn:		clr.w	d0			* get group number
		move.b	E_DATA1(a0),d0		* ... in d0
		add.w	d0,d0			* ... as a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	dsexit			* done if not enabled
*
		tst.w	P_CF(a6)		* center update ?
		beq	dsloc00			* jump if not
*
		clr.w	d0			* get location
		move.b	E_DATA2(a0),d0		* ... in d0
		move.w	d0,-(a7)		* save location
		clr.w	d1			* get group number
		move.b	E_DATA1(a0),d1		* ... in d1
		move.w	d1,-(a7)		* save group number
		move.w	(a7),d0			* col = group number
		add.w	d0,d0			* ... * 5
		add.w	d0,d0			* ...
		move.w	(a7)+,d2		* ...  (d2 = group number)
		add.w	d2,d0			* ...
		add.w	#8,d0			* ... + 8
		move.w	(a7)+,d1		* get location
		add.w	#$0031,d1		* add '0' for ASCII conversion
		move.w	d2,-(a7)		* save group number
		move.w	#AT11,-(a7)		* put attribute on stack
		move.w	d1,-(a7)		* put character on stack
		move.w	d0,-(a7)		* put column on stack
		move.w	#4,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put object address on stack
		jsr	_vputc			* update the screen
		add.l	#12,a7			* clean up stack
		move.w	(a7)+,d0		* get group number
		cmp.w	#8,d0			* see which word it's in
		bcc	dslocn1			* jump if in MS word
*
		bset	d0,_vrbw11+1		* set group bit in LS byte
		bra	dslocn2			* ...
*
dslocn1:	sub.w	#8,d0			* adjust for for byte
		bset	d0,_vrbw11		* set group bit in MS byte
*
dslocn2:	bset	#3,_vrcw		* set video reset type bit
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
		.page
dsloc00:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#52,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$9999,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#5,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dsanrs -- display analog resolution
* ------    -------------------------
dsanrs:		move.b	E_DATA1(a0),d1		* get var / group
		move.w	d1,d0			* extract group number
		andi.w	#$000F,d0		* ... in d0
		add.w	d0,d0			* ... as a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	dsexit			* done if not enabled
*
		tst.w	P_CF(a6)		* center update ?
		beq	dsanrs0			* jump if not
*
		move.w	_angroup,d2		* see if we display
		bmi	dsexit			* jump if not
*
		subq.w	#1,d2			* adust selected group number
		move.w	d1,d0			* extract group from event
		andi.w	#$000F,d0		* ...
		cmp.w	d0,d2			* see if we display
		bne	dsexit			* jump if not
*
		lsr.w	#4,d1			* extract variable number
		andi.w	#$000F,d1		* ...
		move.w	d1,-(a7)		* save variable number
		move.w	d1,d0			* calculate display offset
		lsl.w	#3,d0			* ... (var * 9) + 6
		add.w	d0,d1			* ... in d1
		addq.w	#6,d1			* ...
		move.b	E_DATA2(a0),d0		* get resolution
		addi.w	#$0030,d0		* convert for display
		move.w	#AT12,-(a7)		* put attribute on stack
		move.w	d0,-(a7)		* put character on stack
		move.w	d1,-(a7)		* put column on stack
		move.w	#7,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put sbase on stack
		jsr	_vputc			* update the screen
		add.l	#12,a7			* clean up stack
		move.w	(a7)+,d0		* get variable number
		bset	d0,_vrbw13+1		* set variable bit
		bset	#5,_vrcw		* set video reset type bit
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
		.page
dsanrs0:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#52,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$9999,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#6,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dsanvl -- display analog value
* ------    --------------------
dsanvl:		move.w	_angroup,d2		* see if we display
		bmi	dsexit			* jump if not
*
		move.b	E_DATA1(a0),d1		* get var / group
		move.w	d1,d0			* extract group number
		andi.w	#$000F,d0		* ... in d0
		add.w	d0,d0			* ... as a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	dsexit			* done if not enabled
*
		tst.w	P_CF(a6)		* center update ?
		beq	dsanvl0			* jump if not
*
		subq.w	#1,d2			* adust group number
		move.w	d1,d0			* extract group
		andi.w	#$000F,d0		* ...
		cmp.w	d0,d2			* see if we display
		bne	dsexit			* jump if not
*
		lsr.w	#4,d1			* extract variable number
		andi.w	#$000F,d1		* ...
		move.w	d1,-(a7)		* save variable number
		move.w	d1,d0			* calculate display offset
		lsl.w	#3,d0			* ... (var * 9) + 8
		add.w	d0,d1			* ... in d1
		addi.w	#8,d1			* ...
		move.w	E_DN(a0),d0		* get value
		asr.w	#5,d0			* adjust to low 11 bits
		bpl	dsanvl1			* jump if positive
*
		move.b	#'-',numstr		* set sign = '-'
		neg.w	d0			* make value positive
		bra	dsanvl2
*
dsanvl1:	move.b	#'+',numstr		* set sign = '+'
*
		.page
dsanvl2:	ext.l	d0			* convert MS digit
		divu	#1000,d0		* ...
		add.w	#$0030,d0		* ...
		move.b	d0,numstr+1		* ...
		swap	d0			* convert middle digit
		ext.l	d0			* ...
		divu	#100,d0			* ...
		add.w	#$0030,d0		* ...
		move.b	d0,numstr+2		* ...
		move.b	#'.',numstr+3		* insert decimal point
		swap	d0			* convert LS digit
		ext.l	d0			* ...
		divu	#10,d0			* ...
		add.w	#$0030,d0		* ...
		move.b	d0,numstr+4		* ...
		clr.b	numstr+5		* terminate string
		move.w	#AT12,-(a7)		* put attribute on stack
		move.l	#numstr,-(a7)		* put buffer address on stack
		move.w	d1,-(a7)		* put column on stack
		move.w	#7,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put sbase on stack
		jsr	_vputs			* update the screen
		add.l	#14,a7			* clean up stack
		move.w	(a7)+,d0		* get variable number
		bset	d0,_vrbw14+1		* set variable bit
		bset	#6,_vrcw		* set video reset type bit
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dsanvl0:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#52,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$9999,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#6,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dsasgn -- display assignment
* ------    ------------------
dsasgn:		tst.w	P_CF(a6)		* center update ?
		beq	dsasgn0			* jump if not
*
		move.l	a0,-(a7)		* stash a0
		jsr	_mpcupd			* update changed stuff
		movea.l	(a7)+,a0		* restore a0
		clr.w	d1			* get assignment
		move.b	E_DATA1(a0),d1		* ... from the event
		ext.l	d1			* ... as a long in d1
		divu	#10,d1			* divide by 10 for conversion
		add.l	#$00300030,d1		* add '0' for ASCII conversion
		move.b	d1,numstr		* put MS byte in work area
		swap	d1			* swap register halves
		move.b	d1,numstr+1		* put LS byte in work area
		clr.b	numstr+2		* terminate string
		move.w	#AT04,-(a7)		* put attribute on stack
		move.l	#numstr,-(a7)		* put buffer address on stack
		move.w	#11,-(a7)		* put column on stack
		move.w	#1,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put sbase on stack
		jsr	_vputs			* update the screen
		add.l	#14,a7			* clean up stack
		bset	#0,_vrcw+1		* set video reset type bit
		tst.w	_ctrsw			* update center for scupd ?
	 	beq	dsexit			* done if not
*
dsasgn0:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#56,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$3333,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#3,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dstmpo -- display tempo
* ------    -------------
dstmpo:		tst.w	P_CF(a6)		* center update ?
		beq	dstmpo0			* jump if not
*
		clr.w	d1			* get tempo
		move.b	E_DATA1(a0),d1		* ... from event
		ext.l	d1			* ... as a long in d1
		divu	#10,d1			* divide by 10 for conversion
		swap	d1			* swap register halves
		add.w	#$0030,d1		* add '0' for ASCII conversion
		move.b	d1,numstr+2		* put LS byte in work area
		swap	d1			* swap register halves
		ext.l	d1			* divide again
		divu	#10,d1			* ...
		add.l	#$00300030,d1		* add '0' for ASCII conversion
		move.b	d1,numstr		* put MS byte in work area
		swap	d1			* swap register halves
		move.b	d1,numstr+1		* put middle byte in work area
		clr.b	numstr+3		* terminate string
		move.w	#AT06,-(a7)		* put attribute on stack
		move.l	#numstr,-(a7)		* put buffer address on stack
		move.w	#27,-(a7)		* put column on stack
		move.w	#1,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put sbase on stack
		jsr	_vputs			* display tempo
		add.l	#14,a7			* clean up stack
		bset	#2,_vrcw+1		* set video reset type bit
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dstmpo0:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#56,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$3333,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#4,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dsstop -- display stop
* ------    ------------
dsstop:		tst.w	P_CF(a6)		* center update ?
		beq	dsstop0			* jump if not
*
		jsr	_dclkmd			* show that clock is stopped
		move.w	#AT08,-(a7)		* put attribute on stack
		move.w	#40,-(a7)		* put 1st column on stack
		move.w	#1,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put sbase on stack
		jsr	_vputa			* hilite first column
		move.w	#41,COL(a7)		* put 2nd column on stack
		jsr	_vputa			* hilite second column
		move.w	#42,COL(a7)		* put 3rd column on stack
		jsr	_vputa			* hilite third column
		move.w	#43,COL(a7)		* put 4th column on stack
		jsr	_vputa			* hilite fourth column
		add.l	#10,a7			* clean up stack
		bset	#7,_vrcw		* set video reset type bits
		bset	#0,_vrbw15		* ...
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dsstop0:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#56,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$3333,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#5,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dsnext -- display next
* ------    ------------
dsnext:		tst.w	P_CF(a6)		* center update ?
		beq	dsnext0			* jump if not
*
		move.w	#AT08,-(a7)		* put attribute on stack
		move.w	#45,-(a7)		* put 1st column on stack
		move.w	#1,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put sbase on stack
		jsr	_vputa			* hilite first column
		move.w	#46,COL(a7)		* put 2nd column on stack
		jsr	_vputa			* hilite second column
		move.w	#47,COL(a7)		* put 3rd column on stack
		jsr	_vputa			* hilite third column
		move.w	#48,COL(a7)		* put 4th column on stack
		jsr	_vputa			* hilite fourth column
		add.l	#10,a7			* clean up stack
		bset	#7,_vrcw		* set video reset type bits
		bset	#1,_vrbw15		* ...
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dsnext0:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#56,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$3333,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#5,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dsgrp -- display group status
* -----    --------------------
dsgrp:		tst.w	P_CF(a6)		* center update ?
		beq	dsgrp0			* jump if not
*
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dsgrp0:		move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#60,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$9999,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#3,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dsinst -- display instrument
* ------    ------------------
dsinst:		clr.w	d0			* get group number
		move.b	E_DATA1(a0),d0		* ... in d0
		add.w	d0,d0			* ... as a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	dsexit			* done if not enabled
*
		tst.w	P_CF(a6)		* center update ?
		beq	dsins00			* jump if not
*
		lea	_ins2grp,a1		* point at ins2grp[]
		clr.w	d0			* get instrument number
		move.b	E_DATA2(a0),d0		* ... in d0
		move.w	d0,-(a7)		* save instrument number
		clr.w	d1			* get group number
		move.b	E_DATA1(a0),d1		* ... in d1
		move.w	d1,-(a7)		* save group number
		move.w	(a7),d0			* col = group number
		add.w	d0,d0			* ... * 5
		add.w	d0,d0			* ...
		move.w	(a7)+,d2		* ...  (d2 = group number)
		add.w	d2,d0			* ...
		add.w	#7,d0			* ... + 7
		clr.l	d1			* get instrument number
		move.w	(a7)+,d1		* ... as a long in d1
		divu	#10,d1			* divide by 10 for conversion
		add.l	#$00300030,d1		* add '0' for ASCII conversion
		move.b	d1,numstr		* put MS byte in work area
		swap	d1			* swap register halves
		move.b	d1,numstr+1		* put LS byte in work area
		clr.b	numstr+2		* terminate string
		move.w	d2,-(a7)		* save group number
		move.w	#AT11,-(a7)		* put attribute on stack
		move.l	#numstr,-(a7)		* put buffer address on stack
		move.w	d0,-(a7)		* put column on stack
		move.w	#2,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put object address on stack
		jsr	_vputs			* update the screen
		add.l	#14,a7			* clean up stack
*
		.page
		move.w	(a7)+,d0		* get group number
		cmp.w	#8,d0			* see which word it's in
		bcc	dsinst1			* jump if in MS word
*
		bset	d0,_vrbw08+1		* set group bit in LS byte
		bra	dsinst2			* ...
*
dsinst1:	sub.w	#8,d0			* adjust for for byte
		bset	d0,_vrbw08		* set group bit in MS byte
*
dsinst2:	bset	#0,_vrcw		* set video reset type bit
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dsins00:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#60,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$9999,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#3,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dsintp -- display interpolation
* ------    ---------------------
dsintp:		tst.w	P_CF(a6)		* center update ?
		beq	dsintp0			* jump if not
*
		move.w	E_DATA1(a0),-(a7)	* get interpolate value
		jsr	_fromfpu		* convert to milliseconds
		tst.w	(a7)+			* ...
		andi.l	#$0000FFFF,d0		* clear high bits
		divu	#10000,d0		* convert 1st digit
		addi.w	#$0030,d0		* ... to ASCII
		move.b	d0,numstr		* ... in numstr
		swap	d0			* convert 2nd digit
		ext.l	d0			* ...
		divu	#1000,d0		* ...
		addi.w	#$0030,d0		* ... to ASCII
		move.b	d0,numstr+1		* ... in numstr
		move.b	#'.',numstr+2		* insert decimal point
		swap	d0			* convert 3rd digit
		ext.l	d0			* ...
		divu	#100,d0			* ...
		addi.w	#$0030,d0		* ... to ASCII
		move.b	d0,numstr+3		* ... in numstr
		clr.b	numstr+4		* terminate numstr
		move.w	#AT07,-(a7)		* vputs(obj8, 1, 35, numstr, AT07)
		move.l	#numstr,-(a7)		* ...
		move.w	#35,-(a7)		* ...
		move.w	#1,-(a7)		* ...
		move.l	_obj8,-(a7)		* ...
		jsr	_vputs			* ...
		add.l	#14,a7			* ...
		bset	#3,_vrcw+1		* set video reset bit
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dsintp0:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#60,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$CCCC,G_NOTE(a1)	* gdsp->note = COLOR
		move.w	#4,G_CODE(a1)		* gdsp->code = PATTERN
		move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dspnch -- display punch in/out
* ------    --------------------
dspnch:		tst.w	P_CF(a6)		* center update ?
		beq	dspnch0			* jump if not
*
		tst.w	E_DATA1(a0)		* punch in ?
		beq	dspnch1			* jump if not
*
		move.w	#AT09,-(a7)		* put attribute on stack
		move.w	#50,-(a7)		* put 1st col on stack
		move.w	#1,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put object address on stack
		jsr	_vputa			* highlight 1st column
		move.w	#51,COL(a7)		* put 2nd col on stack
		jsr	_vputa			* highlight 2nd col
		add.l	#10,a7			* clean up stack
		bset	#5,_vrcw+1		* set video reset bit
		bra	dspnch2			* go do maker update
*
dspnch1:	move.w	#AT09,-(a7)		* put attribute on stack
		move.w	#53,-(a7)		* put 1st col on stack
		move.w	#1,-(a7)		* put row on stack
		move.l	_obj8,-(a7)		* put object address on stack
		jsr	_vputa			* highlight 1st column
		move.w	#54,COL(a7)		* put 2nd col on stack
		jsr	_vputa			* highlight 2nd column
		move.w	#55,COL(a7)		* put 3rd col on stack
		jsr	_vputa			* highlight 3rd column
		add.l	#10,a7			* clean up stack
		bset	#6,_vrcw+1		* set video reset bit
*
dspnch2:	jsr	_dsgmodz		* display updated modes
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dspnch0:	move.l	_gdfsep,d0		* quit if no elements left
		beq	dsexit			* ...
*
		movea.l	d0,a1			* a1 = gdsp
		move.l	G_NEXT(a1),_gdfsep	* gdfsep = gdsp->next
		move.w	#60,d2			* d2 = event PRIORITY * 4
		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	0(a2,d2.W),G_NEXT(a1)	* gdsp->next = gdstb[pri]
		move.w	#$CCCC,G_NOTE(a1)	* gdsp->note = COLOR
		tst.w	E_DATA1(a0)		* see which kind we have
		bne	dspnchi			* jump if 'punch in'
*
		move.w	#6,G_CODE(a1)		* gdsp->code = 'out' PATTERN
		bra	dspnchx
*
dspnchi:	move.w	#5,G_CODE(a1)		* gdsp->code = 'in' PATTERN
*
dspnchx:	move.l	a1,0(a2,d2.W)		* gdstb[pri] = gdsp
		bra	dsexit			* done
*
		.page
*
* dsbar -- display a bar marker
* -----    --------------------
dsbar:		tst.w	P_CF(a6)		* center update ?
		beq	dsbar0			* jump if not
*
		tst.w	_ctrsw			* update center for scupd ?
		beq	dsexit			* done if not
*
dsbar0:		movea.l	P_SL(a6),a2		* a2 points at gdstb
		move.l	#-1,BARFLAG(a2)		* set the bar marker flag
		bra	dsexit			* done
*
		.page
*
* ==============================================================================
		.data
* ==============================================================================
*
* sddtab -- score display dispatch table -- MUST match score.h definitions
* ------    ----------------------------    ------------------------------
sddtab:		dc.l	dsexit		* 0	null
		dc.l	dsexit		* 1	score begin
		dc.l	dssbgn		* 2	section begin
		dc.l	dssend		* 3	section end
		dc.l	dsinst		* 4	instrument change
		dc.l	dsnbx		* 5	note begin
		dc.l	dsnex		* 6	note end
		dc.l	dsstop		* 7	stop
		dc.l	dsintp		* 8	interpolate
		dc.l	dstmpo		* 9	tempo
		dc.l	dstune		* 10	tuning
		dc.l	dsgrp		* 11	group status
		dc.l	dslocn		* 12	location
		dc.l	dsdyn		* 13	dynamics
		dc.l	dsanvl		* 14	analog value
		dc.l	dsanrs		* 15	analog resolution
		dc.l	dsasgn		* 16	I/O assign
		dc.l	dstrns		* 17	transposition
		dc.l	dsexit		* 18	repeat
		dc.l	dspnch		* 19	punch in/out
		dc.l	dsexit		* 20	polyphonic pressure
		dc.l	dsexit		* 21	score end
		dc.l	dsexit		* 22	channel pressure
		dc.l	dsbar		* 23	bar marker
		dc.l	dsnext		* 24	next score
*
* ==============================================================================
		.bss
* ==============================================================================
*
* globals:
* --------
_ac_code:	ds.b	1		* accidental code
*
* locals:
* -------
numstr:		ds.b	65		* video display update work area
*
* ------------------------------------------------------------------------------
*
		.end
