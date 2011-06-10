* ------------------------------------------------------------------------------
* seexec.s -- score event execution driver
* Version 40 -- 1988-10-06 -- D.N. Lynx Crowe
*
*	struct s_entry *
*	se_exec(ep, sd)
*	struct s_entry *ep;
*	short sd;
*
*		Executes the event at 'ep', scrolling in direction 'sd'.
*
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_se_exec
*
		.xdef	BadEvnt
*
		.xdef	_xevent
*
		.xref	_asgvce
		.xref	_clkset
		.xref	_getasg
		.xref	_gettun
		.xref	_procpfl
		.xref	_selscor
		.xref	_setdyn
		.xref	_setloc
		.xref	_setsv
		.xref	_settmpo
		.xref	_settune
		.xref	_setv2gi
*
		.xref	_anrs
		.xref	_var2src
		.xref	_curasg
		.xref	_curintp
		.xref	_cursect
		.xref	_grpmode
		.xref	_grpstat
		.xref	_ins2grp
		.xref	_nxtflag
		.xref	_recsw
		.xref	_s_trns
		.xref	_trgtab
		.xref	_varmode
		.xref	_vce2grp
		.xref	_vce2trg
		.xref	_veltab
*
		.page
*
* parameter offsets
* -----------------
P_EP		.equ	8		* WORD - 'ep' parameter offset
P_SD		.equ	12		* WORD - 'sd' parameter offset
*
* event structure offsets -- MUST match score.h definitions
* -----------------------    ------------------------------
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
* Miscellaneous constants
* -----------------------
N_ETYPES	.equ	25		* number of event types
*
M_KSTATE	.equ	$01		* keys status bit
N_KSTATE	.equ	$FE		* keys status bit complement
*
D_BAK		.equ	1		* code for backward scrolling
*
LCL_PRT		.equ	2		* 0-origin local port number
LCL_PCH		.equ	$1080		* port and channel for trigger
*
		.page
*
* A few words about se_exec:
*
* se_exec has to be very fast, so it's written in assembly language,
* rather than C, which is usually pretty good, but not quite good enough
* for this application.  The faster this routine runs, the higher the
* tempo we can keep up with.  If this code is fast enough, we end up
* hardware limited by the timer.
*
_se_exec:	link	a6,#0			* link stack frames
		movea.l	P_EP(a6),a0		* get event pointer 'ep' into a0
		move.l	a0,_xevent		* save in xevent
		move.b	E_TYPE(a0),d1		* get event type into d1.W
		andi.w	#$007F,d1		* ... and mask off new-data flag
		cmp.b	#N_ETYPES,d1		* see if it's valid
		blt	sexc1			* jump if it is
*
BadEvnt:	move.l	a0,d0			* setup to return pointer we got
		bra	done			* exit
*
exexit:		movea.l	_xevent,a0		* point at next event
		move.l	E_FWD(a0),d0		* ...
*
done:		unlk	a6			* done -- unlink stack frames
		rts				* return to caller
*
sexc1:		lea	sextab,a1		* get base of dispatch table
		lsl.w	#2,d1			* multiplty event by 4 for index
		movea.l	0(a1,d1.W),a2		* get address of event routine
		jmp	(a2)			* jump to event execution routine
*
* On entry, the individual execution routines only depend on a0 pointing at the
* event they were dispatched for.
*
* The usual C function register usage conventions apply:
*
* d0..d2 and a0..a2 are used for scratch, and are not preserved.
*
* d3..d6 and a3..a5 are register variables, and are preserved.
* a6 = frame pointer, a7 = stack pointer, and are preserved.
*
		.page
*
* exnbeg -- execute note begin
* ------    ------------------
* If things need to be sped up, we could incorporate the functions of
* asgvce() here, rather than calling it.  asgvce() could also be re-written in
* assembly language to make it a shade faster.
*
exnbeg:		cmpi.w	#D_BAK,P_SD(a6)		* check direction
		beq	nendex			* if backward, treat as note end
*
nbegex:		clr.w	d1			* clear d1
		move.b	E_GROUP(a0),d1		* get group number
		add.w	d1,d1			* ... * 2
		lea	_grpstat,a1		* point at grpstat
		tst.w	0(a1,d1.W)		* see if group is enabled
		beq	exexit			* done if not
*
		move.b	E_NOTE(a0),d1		* d1 = note number nn (0..127)
		move.w	#LCL_PCH,d2		* put port and channel in d2
		add.w	d1,d2			* d2 = trg 
		lea	_trgtab,a1		* point at trgtab[trg]
		move.b	0(a1,d2.W),d0		* ...
		or.b	#M_KSTATE,d0		* set trigger table entry on
		move.b	d0,0(a1,d2.W)		* ...
		lea	_veltab,a1		* point at veltab
		add.w	d2,d2			* ...
		move.w	E_VEL(a0),0(a1,d2.W)	* put velocity in veltab
		move.w	E_VEL(a0),-(a7)		* put velocity on the stack
		move.w	d1,-(a7)		* put note number on the stack
		move.w	#1,-(a7)		* put channel on the stack
		move.w	#LCL_PRT,-(a7)		* put port on the stack
		move.b	E_GROUP(a0),d1		* d1 = group number  (0..11)
		move.w	d1,-(a7)		* put group number on the stack
		jsr	_asgvce			* start the note
		add.l	#10,a7			* clean up the stack		
		bra	exexit			* done
*
		.page
*
* exnend -- execute note end
* ------    ----------------
* If things need to be sped up, we could incorporate the functions of
* procpfl() here, rather than calling it.  procpfl() could also be re-written in
* assembly language to make it a shade faster.
*
exnend:		cmpi.w	#D_BAK,P_SD(a6)		* check direction
		beq	nbegex			* if backward, treat as beginning
*
nendex:		clr.w	d1			* clear d1
		move.b	E_GROUP(a0),d1		* get group number
		add.w	d1,d1			* ... * 2
		lea	_grpstat,a1		* point at grpstat
		tst.w	0(a1,d1.W)		* check group status
		beq	exexit			* done if disabled
*
		move.b	E_NOTE(a0),d1		* d1 = note number nn (0..127)
		move.w	#LCL_PCH,d2		* put port and channel in d2
		add.w	d1,d2			* d2 = trg * 2
		add.w	d2,d2			* ...
		lea	_trgtab,a1		* set trigger table entry off
		move.b	0(a1,d2.W),d0		* ...
		and.b	#N_KSTATE,d0		* ...
		move.b	d0,0(a1,d2.W)		* ...
		bne	exexit			* done if note still active
*
		.page
*
		lsr.w	#1,d2			* adjust d2
		clr.w	d1			* set loop index
		lea	_vce2trg,a2		* point at vce2trg table
*
exnend1:	cmp.w	(a2),d2			* see if this voice uses trg
		bne	exnend2			* jump if not
*
		move.w	#-1,(a2)		* set entry to -1
		move.l	a2,-(a7)		* save a2 on stack
		move.w	d1,-(a7)		* save d1 on stack
		move.w	d2,-(a7)		* save d2 on stack
		lea	_vce2grp,a1		* put group on stack
		move.w	d1,d0			* ...
		add.w	d0,d0			* ...
		move.w	0(a1,d0.W),-(a7)	* ...
		move.w	d2,-(a7)		* put trg on stack
		jsr	_procpfl		* process sustained voices
		addq.l	#4,a7			* clean up stack
		move.w	(a7)+,d2		* restore d2
		move.w	(a7)+,d1		* restore d1
		movea.l	(a7)+,a2		* restore a2
*
exnend2:	addq.l	#2,a2			* point at next vce2trg entry
		addq.w	#1,d1			* loop until all are checked
		cmp.w	#12,d1			* ...
		bne	exnend1			* ...
*
		bra	exexit			* done
*
		.page
*
* exsbgn -- execute section begin
* ------    ---------------------
exsbgn:		clr.w	d0			* get section number
		move.b	E_DATA1(a0),d0		* ...
		move.w	d0,_cursect		* set section number
		bra	exexit			* done
*
* exasgn -- execute assignment event
* ------    ------------------------
exasgn:		clr.w	d0			* get assignment
		move.b	E_DATA1(a0),d0		* ...
		move.w	d0,-(a7)		* getasg(curasg = asgn)
		move.w	d0,_curasg		* ...
		jsr	_getasg			* ...
		tst.w	(a7)+			* ...
		bra	exexit			* done
*
* extune -- execute tuning event
* ------    --------------------
extune:		clr.w	d0			* get tuning
		move.b	E_DATA1(a0),d0		* ...
		move.w	d0,-(a7)		* gettun(tuning)
		jsr	_gettun			* ...
		tst.w	(a7)+			* ...
		bra	exexit			* done
*
* extrns -- execute transposition event
* ------    ---------------------------
extrns:		clr.w	d0			* get group number
		move.b	E_DATA1(a0),d0		* ...
		add.w	d0,d0			* ... as an index in d0
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	exexit			* done if disabled
*
		lea	_s_trns,a1		* set group transposition
		move.w	E_LFT(a0),0(a1,d0.W)	* ...
		jsr	_settune		* ...
		bra 	exexit			* done
*
		.page
*
* extmpo -- execute tempo event
* ------    -------------------
extmpo:		clr.w	d0			* get tempo
		move.b	E_DATA1(a0),d0		* ...
		move.w	d0,-(a7)		* settmpo(tempo)
		jsr	_settmpo		* ...
		tst.w	(a7)+			* ...
		bra	exexit			* done
*
* exstop -- execute stop event
* ------    ------------------
exstop:		clr.w	-(a7)			* stop the clock
		jsr	_clkset			* ...
		tst.w	(a7)+			* ...
		bra	exexit			* that's all, folks
*
* exintp -- execute interpolate event
* ------    -------------------------
exintp:		move.w	E_DATA1(a0),_curintp	* set interpolate value
		bra	exexit			* done
*
		.page
*
* exinst -- execute instrument change event
* ------    -------------------------------
exinst:		clr.w	d0			* get group number
		move.b	E_DATA1(a0),d0		* ... in d0
		add.w	d0,d0			* ... as a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	exexit			* done if not enabled
*
		lea	_ins2grp,a1		* point at ins2grp[]
		clr.w	d0			* get instrument number
		move.b	E_DATA2(a0),d0		* ... in d0
		clr.w	d1			* get group number
		move.b	E_DATA1(a0),d1		* ... in d1
		move.w	d1,-(a7)		* put group number on stack
		add.w	d1,d1			* make d1 a word pointer
		move.w	0(a1,d1.W),d2		* get ins2grp[group]
		and.w	#$FF00,d2		* mask off GTAG1..GTAG8
		or.w	d0,d2			* OR in new instrument number
		move.w	d2,0(a1,d1.W)		* set ins2grp[group]
		jsr	_setv2gi		* setv2gi(group)
		tst.w	(a7)+			* clean up stack
		bra	exexit			* done
*
		.page
*
* exdyn -- execute dynamics event
* -----    ----------------------
exdyn:		clr.w	d0			* get group number
		move.b	E_DATA1(a0),d0		* ... in d0
		add.w	d0,d0			* ... as a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	exexit			* done if not enabled
*
		clr.w	d0			* get dynamics
		move.b	E_DATA2(a0),d0		* ... in d0
		clr.w	d1			* get group number
		move.b	E_DATA1(a0),d1		* ... in d1
		move.w	d0,-(a7)		* setdyn(group, dyn)
		move.w	d1,-(a7)		* ...
		jsr	_setdyn			* ...
		adda.l	#4,a7			* clean up stack
		bra	exexit			* done
*
* exlocn -- execute location event
* ------    ----------------------
exlocn:		clr.w	d0			* get group number
		move.b	E_DATA1(a0),d0		* ... in d0
		add.w	d0,d0			* ... as a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	exexit			* done if not enabled
*
		clr.w	d0			* get location
		move.b	E_DATA2(a0),d0		* ... in d0
		clr.w	d1			* get group number
		move.b	E_DATA1(a0),d1		* ... in d1
		move.w	d0,-(a7)		* setloc(group, loc)
		move.w	d1,-(a7)		* ...
		jsr	_setloc			* ...
		adda.l	#4,a7			* clean up stack
		bra	exexit			* done
*
		.page
*
* exanrs -- execute analog resolution event
* ------    -------------------------------
exanrs:		move.b	E_DATA1(a0),d0		* get group number
		andi.w	#$000F,d0		* ... in d0
		add.w	d0,d0			* ... as a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	exexit			* done if not enabled
*
		clr.w	d1			* get variable / group numbers
		move.b	E_DATA1(a0),d1		* ...
		add.w	d1,d1			* convert to word index
		clr.w	d0			* get resolution
		move.b	E_DATA2(a0),d0		* ... in d0
		lea	_anrs,a1		* point at resolution table base
		move.w	d0,0(a1,d1.W)		* save resolution in table
		bra	exexit			* done
*
* exanvl -- execute analog value event
* ------    --------------------------
exanvl:		move.b	E_DATA1(a0),d0		* get group number
		andi.w	#$000F,d0		* ... in d0
		add.w	d0,d0			* ... as a word offset
		lea	_grpstat,a1		* check grpstat[grp]
		tst.w	0(a1,d0.W)		* ...
		beq	exexit			* done if not enabled
*
		move.w	E_DN(a0),-(a7)		* put value on stack
		clr.w	d2			* get variable / group numbers
		move.b	E_DATA1(a0),d2		* ... into d2
		move.w	d2,d1			* extract group number
		andi.w	#$000F,d1		* ... into d1
		lsr.w	#3,d2			* extract variable number
		andi.w	#$001E,d2		* ... as a word index in d2
		lea	_var2src,a1		* point at variable map
		move.w	0(a1,d2.W),-(a7)	* put source number on stack
		move.w	d1,-(a7)		* put group number on stack
		jsr	_setsv			* setsv(group, src, val)
		adda.l	#6,a7			* clean up stack
		bra	exexit			* done
*
* exnext -- next score
* ------    ----------
exnext:		move.w	#1,_nxtflag		* set next score flag
		bra	exexit			* done
*
		.page
*
* expnch -- execute punch in/out
* ------    --------------------
expnch:		tst.w	_recsw			* recording ?
		beq	exexit			* ignore if not
*
		tst.w	E_DATA1(a0)		* punch in ?
		bne	expnch5			* jump if so
*
* punch out
* 
		lea	_grpmode,a1		* setup for group modes
		move.w	#11,d0			* ...
*
expnch0:	cmpi.w	#2,(a1)			* in record mode ?
		bne	expnch1			* jump if not
*
		clr.w	(a1)			* set to play mode
*
expnch1:	addq.l	#2,a1			* point at next entry
		dbra	d0,expnch0		* loop through all groups
*
		lea	_varmode,a1		* setup for variable modes
		move.w	#5,d1			* set variable count
*
expnch4:	clr.w	d0			* clear offset
*
expnch2:	cmpi.w	#2,0(a1,d0.W)		* in record mode ?
		bne	expnch3			* jump if not
*
		clr.w	0(a1,d0.W)		* set to play mode
*
expnch3:	addq.w	#2,d0			* update offset
		cmpi.w	#24,d0			* check for final group
		bne	expnch2			* loop through all groups
*
		add.l	#32,a1			* point at next variable
		dbra	d1,expnch4		* loop through all variables
*
		bra	exexit
*
		.page
*
* punch in
*
expnch5:	lea	_grpmode,a1		* setup for group modes
		move.w	#11,d0			* ...
*
expnch6:	cmpi.w	#1,(a1)			* in standby mode ?
		bne	expnch7			* jump if not
*
		move.w	#2,(a1)			* set to record mode
*
expnch7:	addq.l	#2,a1			* point at next entry
		dbra	d0,expnch6		* loop through all groups
*
		lea	_varmode,a1		* setup for variable modes
		move.w	#5,d1			* set variable count
*
expnch10:	clr.w	d0			* clear offset
*
expnch8:	cmpi.w	#1,0(a1,d0.W)		* in standby mode ?
		bne	expnch9			* jump if not
*
		move.w	#2,0(a1,d0.W)		* set to record mode
*
expnch9:	addq.w	#2,d0			* update offset
		cmpi.w	#24,d0			* check for final group
		bne	expnch8			* loop through all groups
*
		adda.l	#32,a1			* point at next variable
		dbra	d1,expnch10		* loop through all variables
*
		bra	exexit
*
		.page
*
* sextab -- score execution dispatch table -- MUST match score.h definitions
* ------    ----------------------------------------------------------------
sextab:		dc.l	exexit		* 0	null
		dc.l	exexit		* 1	score begin
		dc.l	exsbgn		* 2	section begin
		dc.l	exexit		* 3	section end
		dc.l	exinst		* 4	instrument change
		dc.l	exnbeg		* 5	note begin
		dc.l	exnend		* 6	note end
		dc.l	exstop		* 7	stop
		dc.l	exintp		* 8	interpolate
		dc.l	extmpo		* 9	tempo
		dc.l	extune		* 10	tuning
		dc.l	exexit		* 11	group status
		dc.l	exlocn		* 12	location
		dc.l	exdyn		* 13	dynamics
		dc.l	exanvl		* 14	analog value
		dc.l	exanrs		* 15	analog resolution
		dc.l	exasgn		* 16	I/O assign
		dc.l	extrns		* 17	transposition
		dc.l	exexit		* 18	repeat
		dc.l	expnch		* 19	punch in/out
		dc.l	exexit		* 20	-unused- (polyphonic pressure)
		dc.l	exexit		* 21	score end
		dc.l	exexit		* 22	-unused- (channel pressure)
		dc.l	exexit		* 23	bar marker
		dc.l	exnext		* 24	next score
*
		.bss
*
* Variable storage areas
* ----------------------
* globals:
* --------
_xevent:	ds.l	1		* next event pointer
*
		.end
