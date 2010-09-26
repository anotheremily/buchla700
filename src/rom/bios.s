********************************************************************************
*									       *
* bios.s -- BIOS for the Buchla 700 and NASA 3D Helmet Display		       *
* ------    --------------------------------------------------		       *
*									       *
* See VM1,VM2 and VDATE for version and date				       *
* Written by D.N. Lynx Crowe						       *
*									       *
* Very loosely based on ideas from:					       *
*									       *
*	"Atari ST Internals", by Abacus Software,			       *
*	"A Hitchhiker's Guide to the BIOS", by Atari,			       *
* 	"DOS Technical Reference", by IBM,				       *
*	"DOS/360", by IBM,
*	"CP/M" , by Digital Research, and, of course,			       *
*	"Unix (tm)", by AT&T, et al.					       *
*									       *
* After all,  Why re-invent the wheel?					       *
*									       *
* Some of the functions are the same as Ataris, but quite a few are different, *
* and our I/O configuration is much different, so BEWARE!  Make no	       *
* assumptions, and read the documentation and comments very carefully.	       *
*									       *
* Especially watch out for the extended BIOS functions.  Most of them	       *
* are nothing at all like the Atari / GEMDOS extended bios functions,	       *
* and even the ones that are similar have different numbers.		       *
********************************************************************************
*
		.text
*
* Version and date
* ----------------
VM1		.equ	18		* First part of version number
VM2		.equ	0		* Second part of version number
VDATE		.equ	$19880620	* Version date
*
BUCHLA		.equ	1		* Non-Zero for Buchla 700, 0 for NASA
NASASIO		.equ	0		* Non-zero for NASA, 0 for Buchla addr.
*
FL_SKR		.equ	$00		* Seek rate  (WD1772, 6Ms/step)
*************************************************************************
*
		.page
*
		.xref	start_		* Where ROMP starts
		.xref	_errno		* Start of ROMP bss space
*
		.xdef	basepage	* Pseudo base page for romp
		.xdef	_rsflag		* Register save area overflow flag
*
		.ifne	BUCHLA
*
		.xdef	_hdvini		* Disk init
*
		.endc
*
		.xdef	_wzcrsh		* ROMP crash flag
		.xdef	_crshvc		* Crash vector
		.xdef	_crshsr		* Crash SR
		.xdef	_crshpc		* Crash PC
		.xdef	_crshsp		* Crash SP
		.xdef	_crshus		* Crash USP
		.xdef	_crshrg		* Crash SP registers
		.xdef	_crshst		* Crash top of stack
*
		.page
*
		.ifne	BUCHLA
*
		.xdef	_hz_1k
		.xdef	_hz_200
		.xdef	frclock
		.xdef	t1count
		.xdef	t2count
		.xdef	t3count
*
		.xdef	nflops
		.xdef	disknum
		.xdef	booted
		.xdef	seekrate
		.xdef	flock
		.xdef	fverify
*
		.xdef	retrycnt
		.xdef	wpstatus
		.xdef	wplatch
		.xdef	bootdev
*
		.xdef	cdev
		.xdef	ctrack
		.xdef	csect
		.xdef	cside
		.xdef	ccount
*
		.xdef	cdma
		.xdef	edma
		.xdef	tmpdma
		.xdef	rseed
*
		.xdef	savptr
*
		.xdef	flpsrsv
		.xdef	flpregs
*
		.xdef	dsb0
		.xdef	dsb1
*
		.xdef	dskmode
		.xdef	dskerrs
		.xdef	drvbpbs
*
		.endc
*
		.xdef	biostop
*
		.page
*
* Some critical equates:
* ----------------------
* Interrupt masks
* ---------------
IPL3		.equ	$0300		* IPL 3 value for sr
IPL4		.equ	$0400		* IPL 4 value for sr
IPL7		.equ	$0700		* IPL 7 value for sr	(4 for debug)
*
		.ifne	BUCHLA
*
INITIPL		.equ	IPL3+$2000	* Initial Internal Priority Level
*
		.endc
*
		.ifeq	BUCHLA
*
INITIPL		.equ	IPL4+$2000	* Initial Internal Priority Level
*
		.endc
*
* Memory allocation
* -----------------
		.ifeq	BUCHLA
*
TPA		.equ	$008000		* Put TPA 32K up from the bottom
*
		.endc
*
		.ifne	BUCHLA
*
TPA		.equ	$010000		* Put TPA 64K up from the bottom
*
		.endc
*
SSTACK		.equ	TPA		* Put system stack just below TPA
*
p_bbase		.equ	$0018		* Basepage offset to bss base
p_blen		.equ	$001C		* Basepage offset to bss length
*
* It's magic ...
* --------------
FMAGIC		.equ	$87654321	* Magic for formatting
RETRYIT		.equ	$10000		* Magic for re-try from criter
RSMAGIC		.equ	$78563412	* Magic for register save area OK
*
* Miscellaneous constants
* -----------------------
FCMAX		.equ	$00FFFFFF	* Maximum frame counter value (24 bits)
*
		.page
*
* CFR0 Baud Rates:
* ----------------
BR_300		.equ	$04
BR_600		.equ	$05
BR_1200		.equ	$06
BR_1800		.equ	$07
BR_2400		.equ	$08
BR_3600		.equ	$09
BR_4800		.equ	$0A
BR_7200		.equ	$0B
BR_9600		.equ	$0C
BR_19K2		.equ	$0D
BR_EXT		.equ	$0F
*
* CFR0 Stop bits (includes TBR select bit):
* -----------------------------------------
NSB_1		.equ	$40
NSB_2		.equ	$60
*
* CFR1 Number of Data Bits (includes MSB):
* ----------------------------------------
NDB_5		.equ	$80
NDB_6		.equ	$A0
NDB_7		.equ	$C0
NDB_8		.equ	$E0
*
* CFR1 Parity Selection:
* ----------------------
P_ODD		.equ	$04
P_EVEN		.equ	$0C
P_MARK		.equ	$14
P_SPACE		.equ	$1C
*
P_NONE		.equ	$00
*
* Line Discipline:
* ----------------
L_NUL		.equ	$00		* No protocol
L_XON		.equ	$01		* XON/XOFF
L_RTS		.equ	$02		* RTS/CTS
*
		.page
*
* I/O Buffer Sizes:
* -----------------
SR1IBS		.equ	256
SR1OBS		.equ	256
SR2IBS		.equ	256
SR2OBS		.equ	256
*
		.ifne	BUCHLA
*
MC1IBS		.equ	512
MC1OBS		.equ	256
MC2IBS		.equ	512
MC2OBS		.equ	256
*
		.endc
*
* Line Disciplines:
* -----------------
SR1DISC		.equ	L_XON
SR2DISC		.equ	L_XON
*
		.ifne	BUCHLA
*
MC1DISC		.equ	L_NUL
MC2DISC		.equ	L_NUL
*
		.endc
*
* CFR Setings:
* ------------
SR1CFR0		.equ	BR_9600+NSB_1
SR1CFR1		.equ	NDB_8+P_NONE
SR2CFR0		.equ	BR_9600+NSB_1
SR2CFR1		.equ	NDB_8+P_NONE
*
		.ifne	BUCHLA
*
MC1CFR0		.equ	BR_EXT+NSB_1
MC1CFR1		.equ	NDB_8+P_NONE
MC2CFR0		.equ	BR_EXT+NSB_1
MC2CFR1		.equ	NDB_8+P_NONE
*
		.endc
*
		.page
*
* I/O Addresses:
* --------------
VSDDINIT	.equ	$200400		* VSDD initial register base address
VSDDDATA	.equ	$200000		* VSDD data segment base address
*
		.ifeq	BUCHLA
*
		.ifeq	NASASIO
*
SR1ACIA		.equ	$3A8001		* Serial-1 ACIA base address	(BUCHLA)
SR2ACIA		.equ	$3A8009		* Serial-2 ACIA base address
*
		.endc
*
		.ifne	NASASIO
*
SR1ACIA		.equ	$398001		* Serial-1 ACIA base address	(NASA)
SR2ACIA		.equ	$398009		* Serial-2 ACIA base address
*
		.endc
*
		.endc
*
		.ifne	BUCHLA
*
FPUBASE		.equ	$180000		* FPU base address
TIMER		.equ	$3A0001		* Timer base address
LCD		.equ	$3A4001		* LCD driver base address
SR1ACIA		.equ	$3A8001		* Serial-1 ACIA base address	(BUCHLA)
SR2ACIA		.equ	$3A8009		* Serial-2 ACIA base address
MC1ACIA		.equ	$3AC001		* MIDI-1 ACIA base address
MC2ACIA		.equ	$3AC009		* MIDI-2 ACIA base address
FLOPPY		.equ	$3B0001		* Floppy controller base address
PSG		.equ	$3B4001		* Sound chip base address
LEDS		.equ	$3B8001		* LED driver base address
ANALOG		.equ	$3BC001		* Analog processor base address
*
* FPU address offsets and misc. values
*
FPU_CTL		.equ	$4000		* FPU control
*
FPU_IN		.equ	$4000		* FPU input address
FPU_CLR		.equ	$6000		* FPU interrupt reset address
*
FPU_RST		.equ	$0015		* FPU reset value
*
* PSG address offsets
*
PSG_RD		.equ	PSG		* Read PSG data
PSG_WL		.equ	PSG		* Write PSG address latch
PSG_WD		.equ	PSG+2		* Write PSG data
*
		.endc
*
* VSDD data structure offsets
*
VSDD_REG	.equ	VSDDDATA	* Registers
VSDD_AT		.equ	VSDDDATA+$0100	* Access Table
*
		.ifne	BUCHLA
*
* Timer registers
*
TIME_CRX	.equ	TIMER		* Control register 1 or 3
TIME_CR2	.equ	TIMER+2		* Control register 2
TIME_T1H	.equ	TIMER+4		* Timer 1 high byte
TIME_T1L	.equ	TIMER+6		* Timer 1 low byte
TIME_T2H	.equ	TIMER+8		* Tiemr 2 high byte
TIME_T2L	.equ	TIMER+10	* Timer 2 low byte
TIME_T3H	.equ	TIMER+12	* Timer 3 high byte
TIME_T3L	.equ	TIMER+14	* Timer 3 low byte
*
		.endc
*
* ACIA Register offsets:
* ----------------------
ACIA_IER	.equ	0		* ACIA IER offset
ACIA_ISR	.equ	0		* ACIA ISR offset
ACIA_CSR	.equ	2		* ACIA CSR offset
ACIA_CFR	.equ	2		* ACIA CFR offset
ACIA_TBR	.equ	4		* ACIA TBR offset
ACIA_TDR	.equ	6		* ACIA TDR offset
ACIA_RDR	.equ	6		* ACIA RDR offset
*
		.page
*
		.ifne	BUCHLA
*
* Floppy register offsets:
* ------------------------
DSKCMD		.equ	0		* Command / status
DSKTRK		.equ	2		* Track
DSKSEC		.equ	4		* Sector
DSKDAT		.equ	6		* Data
*
* Miscellaneous equates:
* ----------------------
FL_NC		.equ	$02		* Non-compensated R/W/F bit
*
APISIZE		.equ	256		* Analog processor buffer size
*
* Floppy commands:
* ----------------
FL_RS		.equ	$80		* Read sector
FL_RM		.equ	$90		* Read multiple
FL_WS		.equ	$A0		* Write sector
FL_WT		.equ	$F0		* Write track  (format)
FL_FR		.equ	$D0		* Force reset
FL_SK		.equ	$10		* Seek
FL_SV		.equ	$14		* Seek w/verify
*
		.endc
*
		.page
*
* iorec structure definitions:
* ----------------------------
IORECLN		.equ	40		* Length of an iorec structure
*
ibuf		.equ	0		* Input buffer base address
ibufsize	.equ	4		* Input buffer size  (bytes)
ibufhd		.equ	6		* Input buffer head index
ibuftl		.equ	8		* Input buffer tail index
ibuflow		.equ	10		* Input buffer low water mark
ibufhi		.equ	12		* Input buffer high water mark
obuf		.equ	14		* Output buffer base address
obufsize	.equ	18		* Output buffer size  (bytes)
obufhd		.equ	20		* Output buffer head index
obuftl		.equ	22		* Output buffer tail index
obuflow		.equ	24		* Output buffer low water mark
obufhi		.equ	26		* Output buffer high water mark
cfr0		.equ	28		* ACIA CFR, MS bit = 0
cfr1		.equ	29		* ACIA CFR, MS bit = 1
flagxon		.equ	30		* XON flag  (non-zero = XOFF sent)
flagxoff	.equ	31		* XOFF flag  (non-zero = active)
linedisc	.equ	32		* Line discipline flags
erbyte		.equ	33		* Last error byte
isr		.equ	34		* ACIA ISR on interrupt
csr		.equ	35		* ACIA CSR on interrupt
errct		.equ	36		* Error count  (FRM/OVR/BRK)
ibfct		.equ	38		* Input buffer full count
*
		.page
*
* Error codes:
* ------------
ERR01		.equ	-1		* All purpose error
ERR02		.equ	-2		* Drive not ready
ERR03		.equ	-3		* Unknown command
ERR04		.equ	-4		* CRC Error
ERR05		.equ	-5		* Invalid request
ERR06		.equ	-6		* Seek error
ERR07		.equ	-7		* Unknown media
ERR08		.equ	-8		* Sector not found
ERR09		.equ	-9		* End of media  (out of paper)
ERR10		.equ	-10		* Write fault
ERR11		.equ	-11		* Read fault
ERR12		.equ	-12		* General mishap
ERR13		.equ	-13		* Write protected
ERR14		.equ	-14		* Media changed
ERR15		.equ	-15		* Unknown device
ERR16		.equ	-16		* Bad sectors
ERR17		.equ	-17		* Insert disk
*
		.page
*
* In the Beginning...
* -------------------
*
* Low PROM -- Contains the initial sp (unused), and the initial pc,
* as well as some version and creation date stuff and a copyright message,
* just to be thorough about it.
*
begin:		bra.s	biosinit		* Jump to bios init
*
vermsg:		dc.b	VM1,VM2			* Version number
*
		dc.l	biosinit		* Reset address (bios init)
*
created:	dc.l	VDATE			* Creation date
*
copyrite:	dc.b	'{Copyright 1988 by '	* Copyright message
		dc.b	'D.N. Lynx Crowe}',0
*
		dc.l	0			* Some padding
*
		.page
		.even
*
* biosinit -- Setup the defaults for the BIOS
* --------    -------------------------------
biosinit:	move.w	#$2700,sr		* Set sup mode, no interrupts
		move.l	#SSTACK,a7		* Setup supervisor stack pointer
*
		lea	badtrap,a1		* Set default trap vector
		adda.l	#$02000000,a1		* ... with trap # in 31..24
		lea	8,a0			* ... for traps 2..255
		move.l	#253,d1
*
binit1:		move.l	a1,(a0)+		* Store trap vector
		adda.l	#$01000000,a1		* Increment trap #
		dbf	d1,binit1		* Loop until done
*
		lea	biosram,a0		* Clear BIOS ram ...
		lea	SSTACK-2,a1		* ... from biosram to SSTACK-1
*
binit2:		move.w	#0,(a0)+		* Zero a word
		cmpa.l	a0,a1			* Last one ?
		bne	binit2			* Loop if not
*
		move.l	#rsarea,savptr		* Setup pointer to register area
		move.l	#RSMAGIC,_rsflag	* Set magic in _rsflag
*
		move.l	#nullrts,timevec	* Set timer interrupt vector
		move.l	#nullrts,critvec	* Set critical error vector
		move.l	#nullrts,termvec	* Set process terminate vector
		move.l	#nullrts,resvec3	* Set software vector 3
		move.l	#nullrts,resvec4	* Set software vector 4
		move.l	#nullrts,resvec5	* Set software vector 5
		move.l	#nullrts,resvec6	* Set software vector 6
		move.l	#nullrts,resvec7	* Set software vector 7
*
		move.l	#hardhlt,$0008		* Set bus error vector
		move.l	#nullrte,$0014		* Set divide error vector
		move.l	#nullrte,$0018		* Set CHK vector
		move.l	#nullrte,$001C		* Set TRAPV vector
		move.l	#nullrte,$0024		* Set trace vector
*
		move.l	#nullrte,$0030		* Set reserved vector 12
		move.l	#nullrte,$0034		* Set reserved vector 13
		move.l	#nullrte,$0038		* Set reserved vector 14
*
		move.l	#nullrte,$003C		* Set uninitialized int. vector
*
		move.l	#nullrte,$0040		* Set reserved vector 16
		move.l	#nullrte,$0044		* Set reserved vector 17
		move.l	#nullrte,$0048		* Set reserved vector 18
		move.l	#nullrte,$004C		* Set reserved vector 19
		move.l	#nullrte,$0050		* Set reserved vector 20
		move.l	#nullrte,$0054		* Set reserved vector 21
		move.l	#nullrte,$0058		* Set reserved vector 22
		move.l	#nullrte,$005C		* Set reserved vector 23
*
		move.l	#nullrte,$0060		* Set spurious int. vector
*
		move.l	#serintr,$0074		* Set ACIA interrupt vector
		move.l	#nullrte,$0078		* Set level 6 vector
		move.l	#nullrte,$007C		* Set level 7 vector
*
		move.l	#trap13,$00B4		* Set trap13 vector
		move.l	#trap14,$00B8		* Set trap14 vector
*
		.page
*
		.ifne	BUCHLA
*
		move.l	#nullfpu,$0068		* Set FPU interrupt vector
		move.l	#api_int,$006C		* Set analog trap vector
		move.l	#timeint,$0070		* Set timer trap vector
*
		move.l	#_hdvini,hdv_init	* Set disk init vector
		move.l	#getbpb,hdv_bpb		* Set get BPB vector
		move.l	#rwabs,hdv_rw		* Set disk I/O vector
		move.l	#bootload,hdv_boot	* Setup boot load vector
		move.l	#mediach,hdv_mchg	* Setup media change vector
*
		move.w	#$FFFF,fverify		* Set read after write flag
		move.w	#FL_SKR,seekrate	* Set default seek rate
		move.w	#$FFFF,booted		* Say we're not booted yet
		move.l	#buffer,dskbufp		* Setup default disk buffer
*
		.page
*
		move.b	#7,PSG_WL		* Select PSG R7
		move.b	#$80,PSG_WD		* Write $80  (port B = output)
		move.b	#15,PSG_WL		* Select PSG R15
		move.b	#$00,PSG_WD		* Write $00  (sync enable)
*
		move.b	#$00,TIME_T1H		* Setup timer 1  (PLL)
		move.b	#$1F,TIME_T1L		* ... for divide by 64
		move.b	#$0C,TIME_T2H		* Setup timer 2  (FC)
		move.b	#$7F,TIME_T2L		* ... for divide by 3200
		move.b	#$03,TIME_T3H		* Setup timer 3  (RTC)
		move.b	#$20,TIME_T3L		* ... for 1Ms interval
		move.b	#$42,TIME_CRX		* Setup CR3
		move.b	#$41,TIME_CR2		* Setup CR2
		move.b	#$81,TIME_CRX		* Setup CR1
		move.b	#$80,TIME_CRX		* Start the timers
*
		.endc
*
		lea	sr1iorec,a0		* Serial-1 iorec address to a0
		lea	SR1ACIA,a1		* Serial-1 ACIA address to a1
		lea	sr1dflt,a2		* Serial-1 dflt table addr to a2
		bsr.w	aciainit		* Go initialize the port
*
		lea	sr2iorec,a0		* Serial-2 iorec address to a0
		lea	SR2ACIA,a1		* Serial-2 ACIA address to a1
		lea	sr2dflt,a2		* Serial-2 dflt table addr to a2
		bsr.w	aciainit		* Go initialize the port
*
		.ifne	BUCHLA
*
		lea	mc1iorec,a0		* MIDI-1 iorec address to a0
		lea	MC1ACIA,a1		* MIDI-1 ACIA address to a1
		lea	mc1dflt,a2		* MIDI-1 dflt table addr to a2
		bsr.w	aciainit		* Go initialize the port
*
		lea	mc2iorec,a0		* MIDI-2 iorec address to a0
		lea	MC2ACIA,a1		* MIDI-2 ACIA address to a1
		lea	mc2dflt,a2		* MIDI-2 dflt table addr to a2
		bsr.w	aciainit		* Go initialize the port
*
		.page
*
		lea	VSDDINIT,a1		* Setup to load VSDD regs
		lea	vsddtab,a0		* ... from vsddtab
		move.w	#15,d0			* ... all 16 registers
*
vsddinit:	move.w	(a0)+,(a1)+		* Load the VSDD registers
		btst.l	#0,d0
		btst.l	#0,d0
		btst.l	#0,d0
		dbf	d0,vsddinit
*
		move.w	vsddit02,VSDDINIT	* Enable the video
*
		move.l	#api_fum,api_fi		* Clear analog processor fifo
		move.l	#api_fum,api_fo		* ...
*
		move.w	#23,d0			* Setup to clear key LEDs
		move.b	#$80,d1			* ...
*
ledclear:	move.b	d1,LEDS			* Clear a LED
		addq.b	#1,d1			* Increment LED number
		dbra	d0,ledclear		* Loop until all are done
*
		move.w	#7,d0			* Setup to clear pot LEDs
		move.b	#$18,d1			* ...
*
ledclr2:	move.b	d1,LEDS			* Clear a LED
		addq.b	#1,d1			* Increment LED number
		dbra	d0,ledclr2		* Loop until all are done
*
		clr.w	fc_sw			* Stop the frame clock
		clr.l	fc_val			* ... and reset it
*
		.page
*
		lea	FPUBASE+FPU_CTL,a0	* Point at FPU master level
		move.w	#$0000,$08(a0)		* Set CV1 to 0
		addq.w	#1,d0			* Delay
		addq.w	#1,d0			* ...
		move.w	#$0000,$0A(a0)		* Set SF1 to 0
		addq.w	#1,d0			* Delay
		addq.w	#1,d0			* ...
		move.w	#$0000,$0C(a0)		* Set CV2 to 0
		addq.w	#1,d0			* Delay
		addq.w	#1,d0			* ...
		move.w	#$0000,$0E(a0)		* Set SF2 to 0
		addq.w	#1,d0			* Delay
		addq.w	#1,d0			* ...
		move.w	#$0000,$10(a0)		* Set CV3 to 0
		addq.w	#1,d0			* Delay
		addq.w	#1,d0			* ...
		move.w	#$0000,$12(a0)		* Set SF3 to 0
		addq.w	#1,d0			* Delay
		addq.w	#1,d0			* ...
		move.w	#$8300,$02(a0)		* Set new value '10' to -10.00
		addq.w	#1,d0			* Delay
		addq.w	#1,d0			* ...
		move.w	#$8300,$1C(a0)		* Set new value '01' to -10.00
		addq.w	#1,d0			* Delay
		addq.w	#1,d0			* ...
		move.w	#$0001,$16(a0)		* Set exponent for shortest time
		addq.w	#1,d0			* Delay
		addq.w	#1,d0			* ...
		move.w	#$FFF0,$14(a0)		* Set mantissa for shortest time
		addq.w	#1,d0			* Delay
		addq.w	#1,d0			* ...
		move.w	#$0005,$00(a0)		* Send control word to FPU
*
		.endc
*
		.page
*
		move.w	#INITIPL,sr		* Enable interrupts
		lea	basepage,a1		* Pass start_ a pseudo base page
		move.l	#_errno,p_bbase(a1)	* ...
		clr.l	p_blen(a1)		* ...
		move.l	a1,-(a7)		* ...
		jsr	start_			* Go start ROMP  (we assume ...)
*
		jmp	biosinit		* Just in case we return ...
*
		.page
*
		.ifne	BUCHLA
*
* nullfpu -- Null FPU trap handler
* -------    ---------------------
nullfpu:	movem.l	d0-d0/a0-a0,-(a7)	* Save registers
		movea.l	#FPUBASE,a0		* Setup FPU base address in a0
		move.w	FPU_IN(a0),d0		* Read FPU interrupt port
		andi.l	#$000000FF,d0		* Mask for voice & parameter
		lsl.l	#5,d0			* Shift for word offset
		addi.l	#FPU_CTL,d0		* Add FPU control offset
		move.w	#FPU_RST,0(a0,d0.L)	* Reset the function
		clr.w	FPU_CLR(a0)		* Clear the interrupt
		movem.l	(a7)+,d0-d0/a0-a0	* Restore registers
		rte				* Return to interrupted code
*
		.endc
*
		.page
*
* hardhlt -- Bus error trap handler
* -------    ----------------------
hardhlt:	stop	#$2700			* stop dead -- system is AFU
		bra	hardhlt			* ...
*
* badtrap -- Bad trap handler
* -------    ----------------
badtrap:	move.w	(a7)+,_crshsr		* Get crash SR
		move.l	(a7)+,_crshpc		* Get crash PC
		move.l	a7,_crshsp		* Get crash SP
		bsr	badtr1			* Get TRAP PC with vector number
		nop
*
badtr1:		move.l	(a7)+,_crshvc		* Save for analysis of vector #
		movem.l	d0-d7/a0-a7,_crshrg	* Save crash registers
		move.l	usp,a0			* Preserve crash USP
		move.l	a0,_crshus		* ...
		move.l	a7,d0			* Get SP
		andi.l	#$FFFFFFFE,d0		* ... make sure it's even
		movea.l	d0,a1			* ...
		move.w	#15,d0			* Save top 16 words
		lea	_crshst,a0		* ... of crash stack in _crshst
*
badtr2:		move.w	(a1)+,(a0)+		* Save a stack value
		dbf	d0,badtr2		* Loop until all are saved
*
		clr.l	d0			* Get TRAP number
		move.b	_crshvc,d0		* ... as LS byte of d0
		move.l	_crshpc,a0		* Save crash PC in a0
		move.w	#$FFFF,_wzcrsh		* Indicate we crashed
		move.l	#rsarea,savptr		* Restore system save pointer
		move.l	#SSTACK,a7		* Reset the stack pointer
		move.l	#RSMAGIC,_rsflag	* ... and the stack sentinel
		trap	#15			* TRAP to ROMP
*
		jmp	biosinit		* Recover with a cold start
*
		.page
*
		.ifne	BUCHLA
*
* timeint -- Timer interrupt handler
* -------    -----------------------
timeint:	movem.l	d0-d7/a0-a6,-(a7)	* Save registers
		move.b	TIME_CR2,d0		* Get timer interrupt status
		btst.l	#2,d0			* Check timer 3 status
		beq	tmi02			* Jump if not active
*
		move.b	TIME_T3H,d1		* Read timer 1 count
		lsl.l	#8,d1			* ...
		move.b	TIME_T3L,d1		* ...
		move.w	d1,t3count		* ... and save it
*
		addq.l	#1,_hz_1k		* Update 1ms clock  (1 KHz)
*
		move.w	tdiv1,d1		* Update divider
		addq.w	#1,d1			* ...
		move.w	d1,tdiv1		* ...
*
		cmpi.w	#5,d1			* Do we need to update _hz_200 ?
		blt	tmi02			* Jump if not
*
		addq.l	#1,_hz_200		* Update 5ms clock   (200 Hz)
*
		move.w	tdiv2,d1		* Update divider
		addq.w	#1,d1			* ...
		move.w	d1,tdiv2		* ...
*
		cmpi.w	#4,d1			* Do we need to update frclock ?
		blt	tmi01			* Jump if not
*
		addq.l	#1,frclock		* Update 20 Ms clock  (50 Hz)
		tst.w	flock			* See if floppy is active
		bne	tmi00			* Don't call flopvbl if so
*
		bsr	flopvbl			* Check on the floppy
*
tmi00:		move.w	#0,tdiv2		* Reset tdiv2
*
tmi01:		move.w	#0,tdiv1		* Reset tdiv1
*
		.page
*
tmi02:		btst.l	#0,d0			* Check timer 1 int
		beq	tmi03			* Jump if not set
*
		move.b	TIME_T1H,d1		* Read timer 1 to clear int.
		lsl.l	#8,d1			* ...
		move.b	TIME_T1L,d1		* ...
		move.w	d1,t1count		* ... and save the count
*
tmi03:		btst.l	#1,d0			* Check for timer 2 int.
		beq	tmi04			* Jump if not set
*
		move.b	TIME_T2H,d1		* Read timer 2 to clear int.
		lsl.l	#8,d1			* ...
		move.b	TIME_T2L,d1		* ...
		move.w	d1,t2count		* ... and save the count
*
		tst.w	fc_sw			* Should we update the frame ?
		beq	tmi04			* Jump if not
*
		bmi	tmi05			* Jump if we count down
*
		move.l	fc_val,d0		* Get the frame count
		cmp.l	#FCMAX,d0		* See it we've topped out
		bge	tmi06			* Jump if limit was hit
*
		addq.l	#1,d0			* Count up 1 frame
		move.l	d0,fc_val		* Store updated frame count
		bra	tmi04			* Done
*
tmi06:		move.l	#FCMAX,fc_val		* Force hard limit, just in case
		bra	tmi04			* Done
*
tmi05:		move.l	fc_val,d0		* Get the frame count
		beq	tmi04			* Done if already zero
*
		subq.l	#1,d0			* Count down 1 frame
		move.l	d0,fc_val		* Store udpated frame count
*
		movea.l	timevec,a0		* Do RTC vector
		move.w	#1,-(a7)		* ... pass 1 msec on stack
		jsr	(a0)			* ...
		addq.l	#4,a7			* ...
*
tmi04:		movem.l	(a7)+,d0-d7/a0-a6		* Restore registers
		rte				* Return to interrupted code
*
		.endc
*
		.page
*
* trap14 -- Extended BIOS entry point
* ------    -------------------------
trap14:		lea	t14tab,a0		* Setup trap 14 table address
		bra	trapent			* Go process trap
*
* trap13 -- Main BIOS entry point
* ------    ---------------------
trap13:		lea	t13tab,a0		* Setup trap 13 table address
*
trapent:	move.l	savptr,a1		* Get save area pointer
		move.w	(a7)+,d0		* Status register to D0
		move.w	d0,-(a1)		* Save in save area
		move.l	(a7)+,-(a1)		* Stash PC in save area
		movem.l	d3-d7/a3-a7,-(a1)	* Save parameter register
		move.l	a1,savptr		* Update save pointer
		btst	#13,d0			* Were we in sup. mode ?
		bne	trwzsup			* Jump if so
*
		move.l	usp,a7			* Move user sp to stack ptr.
*
trwzsup:	move.w	(a7)+,d0		* Get function number from stack
		cmp.w	(a0)+,d0		* Check against limit
		bge	trpexit			* Jump if it's invalid
*
		lsl.w	#2,d0			* Multiply by 4 for use as index
		move.l	0(a0,d0),d0		* Get routine address
		move.l	d0,a0			* ... into a0
		bpl	trpnind			* Jump if it's indirect
*
		move.l	(a0),a0			* Use indirect value
*
trpnind:	sub.l	a5,a5			* Clear a5
		jsr	(a0)			* Execute the routine
*
trpexit:	move.l	savptr,a1		* Get SAVPTR into a1
		movem.l	(a1)+,d3-d7/a3-a7	* Restore registers
		move.l	(a1)+,-(a7)		* Push return onto stack
		move.w	(a1)+,-(a7)		* Push status onto stack
		move.l	a1,savptr		* Update SAVPTR
*
* nullrte -- null rte
* -------    --------
nullrte:	rte				* Return to interrupted code
*
* nullrts -- null return
* -------    -----------
nullrts:	rts				* Just return to the caller
*
		.page
*
* bconstat -- Get character device input status
* --------    ---------------------------------
bconstat:	lea	cdt01,a0		* Point at status table
		bra	condisp			* Jump to dispatcher
*
* bconin -- Get input from character device
* ------    -------------------------------
bconin:		lea	cdt02,a0		* Point at input table
		bra	condisp			* Jump to dispatcher
*
* bconout -- Output to character device
* -------    --------------------------
bconout:	lea	cdt03,a0		* Point at output table
		bra	condisp			* Jump to dispatcher
*
* bcostat -- Get character device output status
* -------    ----------------------------------
bcostat:	lea	cdt04,a0		* Point at status table
*
* condisp -- Character device function dispatcher
* -------    ------------------------------------
condisp:	move.w	4(a7),d0		* Get device number
		lsl.w	#2,d0			* ... times 4 for pointer
		move.l	0(a0,d0),a0		* Get routine address
		jmp	(a0)			* Jump to it
*
		.page
*
* sr1ist -- Check CON (Serial-1) input buffer status
* ------    ----------------------------------------
sr1ist:		lea	sr1iorec,a0		* Address of iorec to a0
		lea	SR1ACIA,a1		* Address of ACIA to a1
		bra	chkist			* Go check buffer status
*
* sr2ist -- Check AUX (Serial-2) input buffer status
* ------    ----------------------------------------
sr2ist:		lea	sr2iorec,a0		* Address of iorec to a0
		lea	SR2ACIA,a1		* Address of ACIA to a1
*
		.ifne	BUCHLA
*
		bra	chkist			* Go check buffer status
*
* mc1ist -- Check MC1 (MIDI-1) input buffer status
* ------    --------------------------------------
mc1ist:		lea	mc1iorec,a0		* Address of iorec to a0
		lea	MC1ACIA,a1		* Address of ACIA to a1
		bra	chkist			* Go check buffer status
*
* mc2ist -- Check MC2 (MIDI-2) input buffer status
* ------    --------------------------------------
mc2ist:		lea	mc2iorec,a0		* Address of iorec to a0
		lea	MC2ACIA,a1		* Address of ACIA to a1
*
		.endc
*
* chkist -- Check input buffer status
* ------    -------------------------
chkist:		moveq.l	#-1,d0			* Default to "Input available"
		lea	ibufhd(a0),a2		* Head index to a2
		lea	ibuftl(a0),a3		* Tail index to a3
		cmpm.w	(a3)+,(a2)+		* Buffer clear ?
		bne	chkist1			* Jump if not
*
		moveq.l	#0,d0			* Set to "Buffer empty"
*
chkist1:	rts				* Return to caller
*
		.page
*
* sr1ost -- Check CON (Serial-1) output buffer status
* ------    -----------------------------------------
sr1ost:		lea	sr1iorec,a0		* Address of iorec to a0
		bra	chkost			* Go check buffer status
*
* sr2ost -- Check AUX (Serial-2) output buffer status
* ------    -----------------------------------------
sr2ost:		lea	sr2iorec,a0		* Address of iorec to a0
*
		.ifne	BUCHLA
*
		bra	chkost			* Go check buffer status
*
* mc1ost -- Check MC1 (MIDI-1) output buffer status
* ------    ---------------------------------------
mc1ost:		lea	mc1iorec,a0		* Address of iorec to a0
		bra	chkost			* Go check buffer status
*
* mc2ost -- Check MC2 (MIDI-2) output buffer status
* ------    ---------------------------------------
mc2ost:		lea	mc2iorec,a0		* Address of iorec to a0
*
		.endc
*
* chkost -- Check output buffer status
* ------    --------------------------
chkost:		moveq.l	#-1,d0			* Default to "Output OK"
		move.w	obuftl(a0),d2		* Tail index to d2
		bsr	wrapout			* Test for pointer wraparound
		cmp.w	obufhd(a0),d2		* Compare with head index
		bne	chkost1			* Jump if not equal
*
		moveq.l	#0,d0			* Set to "Buffer full"
*
chkost1:	rts				* Return to caller
*
		.page
*
* wrapin -- Check input pointer for wraparound
* ------    ----------------------------------
wrapin:		add.w	#1,d1			* Head index +1
		cmp.w	ibufsize(a0),d1		* = buffer size ?
		bcs	wrapin1			* Jump if not
*
		moveq.l	#0,d1			* Wraparound
*
wrapin1:	rts				* Return to caller
*
* wrapout -- Check output pointer for wraparound
* -------    -----------------------------------
wrapout:	addq.w	#1,d2			* Tail index +1
		cmp.w	obufsize(a0),d2		* = buffer size ?
		bcs	wrapout1		* Jump if not
*
		moveq.l	#0,d2			* Wrap around if so
*
wrapout1:	rts				* Return to caller
*
		.page
*
* sr1inp -- Get input from Serial-1  (wait on busy)
* ------    ---------------------------------------
sr1inp:		lea	sr1iorec,a0		* Serial-1 iorec address
		lea	SR1ACIA,a1		* Serial-1 ACIA base
		bra	serinp			* Go get a byte
*
* sr2inp -- Get input from Serial-2  (wait on busy)
* ------    ---------------------------------------
sr2inp:		lea	sr2iorec,a0		* Serial-2 iorec address
		lea	SR2ACIA,a1		* Serial-2 ACIA base
*
* serinp -- Get a byte from a serial port  (with handshaking)
* ------    -------------------------------------------------
serinp:		bsr	chkist			* Check input status
		tst.w	d0			* Character ready ?
		beq	serinp			* Loop until one is ...
*
		bsr	getser			* Get a byte from the buffer
		and.w	#$FFFF,d0		* Isolate LS bits 7..0
		rts				* Return to caller
*
		.page
*
		.ifne	BUCHLA
*
* mc1inp -- Get input from MIDI-1  (wait on busy)
* ------    -------------------------------------
mc1inp:		lea	mc1iorec,a0		* MIDI-1 iorec address
		lea	MC1ACIA,a1		* MIDI-1 ACIA base
		bra	midinp			* Go get a byte
*
* mc2inp -- Get input from MIDI-2  (wait on busy)
* ------    -------------------------------------
mc2inp:		lea	mc2iorec,a0		* MIDI-2 iorec address
		lea	MC2ACIA,a1		* MIDI-2 ACIA base
*
* midinp -- Get input from a MIDI port  (no handshaking)
* ------    --------------------------------------------
midinp:		bsr	chkist			* Check input status
		tst.w	d0			* Character ready ?
		beq	midinp			* Loop until one is ...
*
		move.w	sr,-(a7)		* Save status register
		ori.w	#IPL7,sr		* Set IPL = 7  (disable ints)
		move.w	ibufhd(a0),d1		* Head index to d1
		cmp.w	ibuftl(a0),d1		* Compare to tail index
		beq	midin_1			* Jump if (somehow) empty
*
		addq.w	#1,d1			* Increment head index
		cmp.w	ibufsize(a0),d1		* Did pointer wrap around ?
		bcs	midin_2			* Jump if not
*
		moveq.l	#0,d1			* Wraparound
*
midin_2:	move.l	ibuf(a0),a2		* Get buffer base address in a2
		moveq.l	#0,d0			* Clear MS bits of d0
		move.b	0(a2,d1),d0		* Get character from buffer
		move.w	d1,ibufhd(a0)		* Update buffer head index
*
midin_1:	move.w	(a7)+,sr		* Restore status
		rts				* Return to caller
*
		.endc
*
		.page
*
* getser -- Get a byte from a serial port buffer  (with handshaking)
* ------    --------------------------------------------------------
getser:		move.w	sr,-(a7)		* Save status register
		ori.w	#IPL7,sr		* Set IPL = 7  (disable ints)
		move.w	ibufhd(a0),d1		* Get input buffer head index
		cmp.w	ibuftl(a0),d1		* Compare tail index
		beq	rs_mt			* Jump if buffer empty
*
		bsr	wrapin			* Adjust pointer for wraparound
		move.l	ibuf(a0),a2		* Get buffer address
		moveq.l	#0,d0			* Clear out MS bits of d0
		move.b	0(a2,d1),d0		* Get character from buffer
		move.w	d1,ibufhd(a0)		* Update head index
		move.w	(a7)+,sr		* Restore status
		andi	#$FFFE,sr		* Clear carry = OK
		bra	rs_xnf			* Go do XON/XOFF check
*
rs_mt:		move.w	(a7)+,sr		* Restore status
		ori	#$0001,sr		* Set carry = no character there
*
rs_xnf:		btst	#0,linedisc(a0)		* Check for XON/XOFF mode
		beq	rs_exit			* Jump if not enabled
*
		tst.b	flagxon(a0)		* XON active ?
		beq	rs_exit			* Jump if not
*
		bsr	rsilen			* Get length of buffer used
		cmp.w	ibuflow(a0),d2		* At low water mark ?
		bne	rs_exit			* Jump if not
*
		move.b	#$11,d1			* Send an XON
		bsr	serput			* ...
		clr.b	flagxon(a0)		* Clear XON flag
*
rs_exit:	rts				* Return to caller
*
		.page
*
* sr1out -- Output to serial-1
* ------    ------------------
sr1out:		lea	sr1iorec,a0		* Serial-1 iorec address to a0
		lea	SR1ACIA,a1		* Serial-1 ACIA address to a1
		move.w	6(a7),d1		* Get data byte from stack
		bsr	serput			* Attempt to output
		bcs	sr1out			* Try until it works ...
*
		rts				* Return to caller
*
* sr2out -- Output to Serial-2
* ------    ------------------
sr2out:		lea	sr2iorec,a0		* Serial-2 iorec address to a0
		lea	SR2ACIA,a1		* Serial-2 ACIA address to a1
		move.w	6(a7),d1		* Get data byte from stack
		bsr	serput			* Attempt to output
		bcs	sr2out			* Try until it works ...
*
		rts				* Return to caller
*
		.ifne	BUCHLA
*
* mc1out -- Output to MIDI-1
* ------    ----------------
mc1out:		lea	mc1iorec,a0		* MIDI-1 iorec address to a0
		lea	MC1ACIA,a1		* MIDI-1 ACIA address to a1
		move.w	6(a7),d1		* Get data byte from stack
		bsr	midput			* Attempt to output
		bcs	mc1out			* Try until it works ...
*
		rts				* Return to caller
*
* mc2out -- Output to MIDI-2
* ------    ----------------
mc2out:		lea	mc2iorec,a0		* MIDI-2 iorec address to a0
		lea	MC2ACIA,a1		* MIDI-2 ACIA address to a1
		move.w	6(a7),d1		* Get data byte from stack
		bsr	midput			* Attempt to output
		bcs	mc2out			* Try until it works ...
*
		rts				* Return to caller
*
		.endc
*
		.page
*
* serput -- Output a character to a serial port
* ------    -----------------------------------
serput:		move.w	sr,-(a7)		* Save status register
		ori.w	#IPL7,sr		* Set IPL = 7  (disable ints)
		move.b	ACIA_ISR(a1),isr(a0)	* Get ACIA isr
		move.b	ACIA_CSR(a1),csr(a0)	* Get ACIA csr
		btst	#0,linedisc(a0)		* XON/XOFF mode ?
		beq	serpt_1			* Jump if not
*
		tst.b	flagxoff(a0)		* XON active ?
		bne	serpt_2			* Jump if so
*
serpt_1:	btst.b	#6,isr(a0)		* Is ACIA still sending ?
		beq	serpt_2			* Jump if so
*
		move.w	obufhd(a0),d2		* Head index to d2
		cmp.w	obuftl(a0),d2		* Compare to tail index
		bne	serpt_2			* Jump if buffer not empty
*
		move.b	d1,ACIA_TDR(a1)		* Give byte to ACIA to send
		bra	serpt_3			* Go deal with RTS/CTS if needed
*
serpt_2:	move.w	obuftl(a0),d2		* Tail index to d2
		bsr	wrapout			* Adjust for wraparound
		cmp.w	obufhd(a0),d2		* Compare to head index
		beq	serpt_4			* Jump if buffer full
*
		move.l	obuf(a0),a2		* Get buffer base address in a2
		move.b	d1,0(a2,d2)		* Put character in buffer
		move.w	d2,obuftl(a0)		* Update buffer tail index
*
serpt_3:	bsr	serchk			* Check status on our way out
		bsr	rtschk			* Handle RTS protocol
		move.w	(a7)+,sr		* Restore status register
		andi	#$FFFE,sr		* Clear carry flag = OK
		rts				* Return to caller
*
serpt_4:	bsr	serchk			* Check status on our way out
		bsr	rtschk			* Handle RTS protocol
		move.w	(a7)+,sr		* Restore status register
		ori	#$0001,sr		* Set carry flag = buffer full
		rts				* Return to caller
*
		.page
*
		.ifne	BUCHLA
*
* midput -- Output to MIDI
* ------    --------------
midput:		move.w	sr,-(a7)		* Save status register
		ori.w	#IPL7,sr		* Set IPL = 7  (diable ints)
		move.b	ACIA_ISR(a1),isr(a0)	* Get ACIA isr
		move.b	ACIA_CSR(a1),csr(a0)	* Get ACIA csr
		btst.b	#6,isr(a0)		* Is ACIA still sending ?
		beq	midpt_2			* Jump if so
*
		move.w	obufhd(a0),d2		* Head index to d2
		cmp.w	obuftl(a0),d2		* Compare to tail index
		bne	midpt_2			* Jump if buffer not empty
*
		move.b	d1,ACIA_TDR(a1)		* Give byte to ACIA to send
		bra	midpt_3			* Go set final status and exit
*
midpt_2:	move.w	obuftl(a0),d2		* Tail index to d2
		bsr	wrapout			* Adjust for wraparound
		cmp.w	obufhd(a0),d2		* Compare to head index
		beq	midpt_4			* Jump if buffer full
*
		move.l	obuf(a0),a2		* Get buffer base address in a2
		move.b	d1,0(a2,d2)		* Put character in buffer
		move.w	d2,obuftl(a0)		* Update buffer tail index
*
midpt_3:	bsr	midchk			* Check status on our way out
		move.w	(a7)+,sr		* Restore status register
		andi	#$FFFE,sr		* Clear carry flag = OK
		rts				* Return to caller
*
midpt_4:	bsr	midchk			* Check status on our way out
		move.w	(a7)+,sr		* Restore status register
		ori	#$0001,sr		* Set carry flag = buffer full
		rts				* Return to caller
*
		.endc
*
		.page
*
* rtschk -- Check RTS mode and turn on RTS if it's enabled
* ------    ----------------------------------------------
rtschk:		btst	#1,linedisc(a0)		* RTS/CTS mode set ?
		beq	rts_1			* Jump if not
*
		bsr	rtson			* Turn on RTS
*
rts_1:		rts				* Return to caller
*
* rsilen -- Get length of portion of input buffer that's been used so far
* ------    -------------------------------------------------------------
rsilen:		move.w	ibuftl(a0),d2		* Tail index to d2
		move.w	ibufhd(a0),d3		* Head index to d3
		cmp.w	d3,d2			* Head > Tail ?
		bhi	rsi_1			* Jump if not
*
		add.w	ibufsize(a0),d2		* Add buffer size to tail index
*
rsi_1:		sub.w	d3,d2			* Length = (adjusted)Tail - Head
		rts				* Return to caller
*
* rtson -- Turn on RTS line
* -----    ----------------
rtson:		move.b	cfr1(a0),d0		* Pick up CFR1 image
		bclr	#0,d0			* Turn on RTS line  (active low)
		bset	#7,d0			* Make sure MS bit is set
		move.b	d0,cfr1(a0)		* Update CFR1 image
		move.b	d0,ACIA_CFR(a1)		* Send CFR to hardware
		rts				* Return to caller
*
* rtsoff -- Turn off RTS line
* ------    -----------------
rtsoff:		move.b	cfr1(a0),d0		* Pick up CFR1 image
		bset	#0,d0			* Turn off RTS line  (active low)
		bset	#7,d0			* Make sure MS bit is set
		move.b	d0,cfr1(a0)		* Update CFR1 image
		move.b	d0,ACIA_CFR(a1)		* Send CFR to hardware
		rts
*
		.page
*
* serintr -- Serial (Serial-1, Serial-2, MIDI-1, MIDI-2) interrupt handler
* -------    -------------------------------------------------------------
serintr:	movem.l	d0-d3/a0-a2,-(a7)	* Save registers
		lea	sr1iorec,a0		* Point at Serial-1 iorec
		lea	SR1ACIA,a1		* Point at Serial-1 ACIA
		bsr	serint			* Go process (possible) int.
*
		lea	sr2iorec,a0		* Point at Serial-2 iorec
		lea	SR2ACIA,a1		* Point at Serial-2 ACIA
		bsr	serint			* Go process (possible) int.
*
		.ifne	BUCHLA
*
		lea	mc1iorec,a0		* Point at MIDI-1 iorec
		lea	MC1ACIA,a1		* Point at MIDI-1 ACIA
		bsr	midint			* Go process (possible) int.
*
		lea	mc2iorec,a0		* Point at MIDI-2 iorec
		lea	MC2ACIA,a1		* Point at MIDI-2 ACIA
		bsr	midint			* Go process (possible) int.
*
		.endc
*
		movem.l	(a7)+,d0-d3/a0-a2	* Restore registers
		rte				* Return from exception
*
		.page
*
* serint -- Process an interrupt from Serial-1 or Serial-2
* ------    ----------------------------------------------
serint:		move.b	ACIA_ISR(a1),isr(a0)	* Get and save ISR
		move.b	ACIA_CSR(a1),csr(a0)	* Get and save CSR
*
		btst.b	#7,isr(a0)		* Was int for this device ?
		beq	serintx			* Jump if not
*
serchk:		btst.b	#1,isr(a0)		* FRM/OVR/BRK error ?
		bne	sererr			* Jump if so
*
		btst.b	#0,isr(a0)		* Receiver interrupt ?
		bne	serrx			* Jump if so
*
sertxq:		btst.b	#6,isr(a0)		* Transmitter interrupt ?
		bne	sertx			* Jump if so
*
serctq:		btst.b	#5,isr(a0)		* CTS interrupt ?
		bne	sercts			* Jump if so
*
serintx:	rts				* Return to caller
*
sererr:		addq.w	#1,errct(a0)		* Update error count
		move.b	ACIA_RDR(a1),erbyte(a0)	* Get error byte
		rts
*
* Handle CTS interupt
*
sercts:		btst.b	#1,linedisc(a0)		* RTS/CTS mode ?
		beq	serintx			* Ignore if not
*
		btst.b	#5,csr(a0)		* CTS set ?
		beq	serintx			* Ignore if not
*
sercts1:	btst.b	#6,isr(a0)		* TDRE set ?
		beq	sercts1			* Loop until it is  (!)
*
		move.w	obufhd(a0),d2		* Head index to d2
		cmp.w	obuftl(a0),d2		* Compare to tail index
		beq	serintx			* Done if buffer empty
*
		bsr.w	wrapout			* Adjust pointer for wraparound
		move.l	obuf(a0),a2		* Get buffer base in a2
		move.b	0(a2,d2),ACIA_TDR(a1)	* Send byte on its way
		move.w	d2,obufhd(a0)		* Save updated head index
		rts
*
		.page
*
* Handle receiver interrupt
*
serrx:		btst.b	#1,linedisc(a0)		* RTS/CTS mode set ?
		beq	serrx1			* Jump if not
*
		bsr.w	rtsoff			* Turn off RTS
*
serrx1:		move.b	ACIA_RDR(a1),d0		* Read data from ACIA
		btst.b	#1,linedisc(a0)		* RTS/CTS mode set ?
		bne	serrx3			* Jump if so
*
		btst.b	#0,linedisc(a0)		* XON/XOFF mode set ?
		beq	serrx3			* Jump if not
*
		cmpi.b	#$11,d0			* Is this an XON ?
		bne	serrx2			* Jump if not
*
		move.b	#$00,flagxoff(a0)	* Clear flagxoff
		bra	sertxq			* Done
*
serrx2:		cmpi.b	#$13,d0			* Is this an XOFF ?
		bne	serrx3			* Jump if not
*
		move.b	#$FF,flagxoff(a0)	* Set flagxoff
		bra	sertxq			* Done
*
serrx3:		move.w	ibuftl(a0),d1		* Get tail index in d1
		bsr.w	wrapin			* Adjust for wraparound
		cmp.w	ibufhd(a0),d1		* Head = tail ?
		beq	seribf			* If so, we drop the character
*
		move.l	ibuf(a0),a2		* Get buffer address
		move.b	d0,0(a2,d1)		* Stash byte in buffer
		move.w	d1,ibuftl(a0)		* Save updated tail index
		bsr	rsilen			* Get length of buffer used
		cmp.w	ibufhi(a0),d2		* Hit high water mark ?
		bne	serrx4			* Jump if not
*
		btst.b	#1,linedisc(a0)		* RTS/CTS mode set ?
		bne	sertxq			* Done if so
*
		btst.b	#0,linedisc(a0)		* XON/XOFF mode set ?
		beq	serrx4			* Jump if not
*
		tst.b	flagxon(a0)		* XOFF already sent ?
		bne	serrx4			* Jump if so
*
		move.b	#$FF,flagxon(a0)	* Set the flag
		move.b	#$13,d1			* Send an XOFF
		bsr.w	serput			* ...
*
		.page
*
serrx4:		btst	#1,linedisc(a0)		* RTS/CTS mode set ?
		beq	sertxq			* Done if not
*
		bsr	rtson			* Turn on RTS
		bra	sertxq
*
* Handle transmitter interrupt
*
sertx:		btst.b	#1,linedisc(a0)		* RTS/CTS mode set ?
		bne	sertx2			* If so, go check CTS
*
		btst.b	#0,linedisc(a0)		* XON/XOFF mode set ?
		beq	sertx1			* Jump if not
*
		tst.b	flagxoff(a0)		* Check flagxoff
		bne	serctq			* Done if set
*
sertx1:		move.w	obufhd(a0),d2		* Head index to d2
		cmp.w	obuftl(a0),d2		* Compare to tail index
		beq	serctq			* Done if buffer empty
*
		bsr	wrapout			* Adjust pointer for wraparound
		move.l	obuf(a0),a2		* Get buffer base address
		move.b	0(a2,d2),ACIA_TDR(a1)	* Send byte on its way
		move.w	d2,obufhd(a0)		* Save updated head index
		bra	serctq			* Done
*
sertx2:		btst.b	#5,csr(a0)		* CTS set in csr ?
		beq	serctq			* If not, go check for CTS int
*
		bra	sertx1			* CTS was set, go transmit
*
seribf:		move.b	d0,erbyte(a0)		* Log dropped character
		addq.w	#1,ibfct(a0)		* ...
		bra	sertxq			* Go check Tx interrupt
*
		.page
*
		.ifne	BUCHLA
*
* midint -- Process an interrupt from MIDI-1 or MIDI-2
* ------    ------------------------------------------
midint:		move.b	ACIA_ISR(a1),isr(a0)	* Get and save ISR
		move.b	ACIA_CSR(a1),csr(a0)	* Get and save CSR
*
		btst.b	#7,isr(a0)		* Was int for this device ?
		beq	midintx			* Jump if not
*
midchk:		btst.b	#1,isr(a0)		* FRM/OVR/BRK error ?
		bne	miderr			* Jump if so
*
		btst.b	#0,isr(a0)		* Receiver interrupt ?
		bne	midrx			* Jump if so
*
midtxq:		btst.b	#6,isr(a0)		* Transmitter interrupt ?
		bne	midtx			* Jump if so
*
midintx:	rts				* Return to caller
*
miderr:		addq.w	#1,errct(a0)		* Update error count
		move.b	ACIA_RDR(a1),erbyte(a0)	* Get error byte
		rts
*
* Handle receiver interrupt
*
midrx:		move.b	ACIA_RDR(a1),d0		* Read data from ACIA
		move.w	ibuftl(a0),d1		* Get tail index in d1
		bsr.w	wrapin			* Adjust for wraparound
		cmp.w	ibufhd(a0),d1		* Head = tail ?
		beq	midibf			* If so, we drop the character
*
		move.l	ibuf(a0),a2		* Get buffer address
		move.b	d0,0(a2,d1)		* Stash byte in buffer
		move.w	d1,ibuftl(a0)		* Save updated tail index
		bra	midtxq			* Done  (go check tx int)
*
		.page
*
* Handle transmitter interrupt
*
midtx:		move.w	obufhd(a0),d2		* Head index to d2
		cmp.w	obuftl(a0),d2		* Compare to tail index
		beq	midintx			* Done if buffer empty
*
		bsr	wrapout			* Adjust pointer for wraparound
		move.l	obuf(a0),a2		* Get buffer base address
		move.b	0(a2,d2),ACIA_TDR(a1)	* Send byte on its way
		move.w	d2,obufhd(a0)		* Save updated head index
		bra	midintx			* Done
*
midibf:		move.b	d0,erbyte(a0)		* Log dropped character
		addq.w	#1,ibfct(a0)		* ...
		bra	midtxq			* Go check Tx interrupt
*
		.endc
*
		.page
*
* setexec -- Set an exception vector
* -------    -----------------------
setexec:	move.w	4(a7),d0		* Get vector number
		lsl.w	#2,d0			* .. times 4
		sub.l	a0,a0			* Clear a0
		lea	0(a0,d0),a0		* Get address of old vector
		move.l	(a0),d0			* Move old vector to d0
		move.l	6(a7),d1		* Pick up new vector
		bmi	setexec1		* Don't set if = -1
*
		move.l	d1,(a0)			* Set new vector
*
setexec1:	rts				* Return to caller
*
* piorec -- Get pointer to iorec structure
* ------    ------------------------------
piorec:		moveq.l	#0,d1			* Clear out d1
		move.w	4(a7),d1		* Get device number
		move.w	sr,-(a7)		* Save status register
		ori.w	#IPL7,sr		* Set IPL = 7  (no ints)
		lea	iortab,a2		* Get base address of table
		asl.l	#2,d1			* Device # times 4 for index
		move.l	0(a2,d1),d0		* Get iorec address from table
		move.w	(a7)+,sr		* Restore status register
		rts				* Return to caller
*
		.page
*
* setport -- Set ACIA parameters  (unit, mode, baud, CFR0, CFR1)
* -------    ---------------------------------------------------
setport:	moveq.l	#0,d1			* Clear out d1
		move.w	4(a7),d1		* Get device number
		asl.l	#3,d1			* Times 8 for index
		ori.w	#IPL7,sr		* Set IPL = 7  (no ints)
		lea	aciatab,a2		* Get base of table
		move.l	0(a2,d1),d0		* Get iorec address
		move.l	4(a2,d1),d2		* Get ACIA address
		movea.l	d0,a0			* Setup a0 = iorec address
		movea.l	d2,a1			* Setup a1 = ACIA address
		tst.w	6(a7)			* Change mode ?
		bmi	setpt1			* Jump if not
*
		move.b	7(a7),linedisc(a0)	* Set line discipline  (mode)
*
setpt1:		tst.w	8(a7)			* Change baud rate ?
		bmi	setpt2			* Jump if not
*
		moveq.l	#0,d1			* Clear out d1
		move.w	8(a7),d1		* Get baud rate index
		lea	brtable,a2		* Get base of baud rate table
		move.b	0(a2,d1),d2		* Get baud rate code from table
		move.b	cfr0(a0),d0		* Get current CFR0
		andi.w	#$0070,d0		* Mask off old baud rate code
		or.w	d2,d0			* OR in new baud rate code
		move.b	d0,cfr0(a0)		* Update CFR0 in table
		move.b	d0,ACIA_CFR(a1)		* Update hardware
*
setpt2:		tst.w	10(a7)			* Change CFR0 ?
		bmi	setpt3			* Jump if not
*
		move.b	11(a7),cfr0(a0)		* Update CFR0 in table
		move.b	cfr0(a0),ACIA_CFR(a1)	* Update CFR0 in hardware
*
setpt3:		tst.w	12(a7)			* Change CFR1 ?
		bmi	setpt4			* Jump if not
*
		bset.b	#7,13(a7)		* Force D7 = 1 in argument
		move.b	13(a7),cfr1(a0)		* Update CFR1 in table
		move.b	cfr1(a0),ACIA_CFR(a1)	* Update CFR1 in hardware
*
setpt4:		rts				* Return to caller
*
		.page
*
* aciainit -- Initialize an ACIA port
* --------    -----------------------
aciainit:	move.w	#IORECLN,d0		* Setup byte count for move
		move.l	a0,-(a7)		* Save iorec base
*
aciai_1:	move.b	(a2)+,(a0)+		* Move the default to the iorec
		dbf	d0,aciai_1		* ...
*
		move.l	(a7)+,a0		* Restore iorec base
		move.b	cfr0(a0),ACIA_CFR(a1)	* Setup CFR0
		move.b	cfr1(a0),ACIA_CFR(a1)	* Setup CFR1
		move.b	ACIA_RDR(a1),d0		* Clear RDR
		move.b	#$7F,ACIA_IER(a1)	* Disable all interrupts
		btst.b	#1,linedisc(a0)		* Are we in RTS/CTS mode ?
		bne	aciai_2			* Jump if so
*
		move.b	#$C3,ACIA_IER(a1)	* Enable TDRE, RDRF
		bra	aciai_3			* Go return
*
aciai_2:	move.b	#$E3,ACIA_IER(a1)	* Enable interrupts we want
*
aciai_3:	rts				* Return to caller
*
		.page
*
		.ifne	BUCHLA
*
* fastcopy -- Copy disk sector quickly
* --------    ------------------------
fastcopy:	move.l	4(a7),a0		* Get source pointer
		move.l	8(a7),a1		* Get destination pointer
		move.w	$3F,d0			* 512 bytes  (63+1)*8
*
fastcpy1:	move.b	(a0)+,(a1)+		* Move 8 bytes
		move.b	(a0)+,(a1)+		* ...
		move.b	(a0)+,(a1)+		* ...
		move.b	(a0)+,(a1)+		* ...
		move.b	(a0)+,(a1)+		* ...
		move.b	(a0)+,(a1)+		* ...
		move.b	(a0)+,(a1)+		* ...
		move.b	(a0)+,(a1)+		* ...
		dbra	d0,fastcpy1		* Loop until all bytes are moved
*
		rts				* Return to caller
*
		.page
*
* _hdvini -- Drive initialization
* -------    --------------------
_hdvini:	link	a6,#-18			* Reserve space on stack
		movem.l	d3-d7/a3-a5,-(a7)	* Preserve registers
		move.l	#300,maxactim		* maxactim = 300 * 20ms
		clr.w	d0			* Put zeros in ...
		move.w	d0,nflops		* ... nflops
		move.w	d0,dskmode		* ... dskmode
		move.w	d0,-2(a6)		* Start with drive A
		bra	hdvilp			* ...
*
hdvilp1:	movea.l	#dskmode,a0		* Get dskmode address in a0
		movea.w	-2(a6),a1		* Drive number in a1
		adda.l	a1,a0			* Point at dskmode for drive
		clr.b	(a0)			* Clear flag in dskmode
		clr.w	-(a7)			* Push arguments onto stack
		clr.w	-(a7)			* ...
		clr.w	-(a7)			* ...
		move.w	-2(a6),-(a7)		* Push drive number onto stack
		clr.l	-(a7)			* ... filler
		clr.l	-(a7)			* ... filler
		jsr	flopini			* Initialize drive
		adda.l	#16,a7			* Cleanup stack
		move.w	d0,-(a7)		* Save error code on stack
		movea.w	-2(a6),a0		* Get drive number
		adda.l	a0,a0			* ... times 2
		adda.l	#dskerrs,a0		* Add base of dskerrs
		move.w	(a7)+,(a0)		* Move error code off of stack
		bne	hdvind			* Jump if drive not present
*
		addq.w	#1,nflops		* Update number of drives
		or.l	#3,drvbits		* Setup drive bits
*
hdvind:		addq.w	#1,-2(a6)		* Increment drive number
*
hdvilp:		cmp.w	#2,-2(a6)		* See if it's 2 yet
		blt	hdvilp1			* Loop until it is
*
		movem.l	(a7)+,d3-d7/a3-a5	* Restore registers
		unlk	a6			* Release temporary stack space
		rts				* Return to caller
*
* drvmap -- get drive map
* ------    -------------
drvmap:		move.l	drvbits,d0		* Get drive bits
		rts				* Return to caller
*
		.page
*
* getbpb -- get BIOS parameter block
* ------    ------------------------
getbpb:		link	a6,#-12			* Reserve space for temporaries
		movem.l	d5-d7/a4-a5,-(a7)	* Save registers on stack
		cmp.w	#2,8(a6)		* Check drive number
		blt	gbpb1			* Jump if OK
*
		clr.l	d0			* Set drive number to 0
		bra	gbpber			* Go flag error
*
gbpb1:		move.w	8(a6),d0		* Get drive number
		asl.w	#5,d0			* ... times 32
		ext.l	d0			* ... extend sign
		move.l	d0,a5			* Move to a5
		add.l	#drvbpbs,a5		* Add base of drive bpb are
		move.l	a5,a4			* Save in a4
*
gbpbrds:	move.w	#1,(a7)			* floprd count = 1,
		clr.w	-(a7)			* ... side = 0,
		clr.w	-(a7)			* ... track = 0,
		move.w	#1,-(a7)		* ... sector = 1,
		move.w	8(a6),-(a7)		* ... drive #,
		clr.l	-(a7)			* ... filler
		move.l	#buffer,-(a7)		* ... buffer address,
		jsr	floprd			* Go read the sector
		add.l	#16,a7			* Cleanup stack
		move.l	d0,-12(a6)		* Save error code
		tst.l	-12(a6)			* ... and test it
		bge	gbpb2			* Jump if OK
*
		move.w	8(a6),(a7)		* Put drive number on stack
		move.l	-12(a6),d0		* Get error code
		move.w	d0,-(a7)		* ... and put it on the stack
		jsr	criter			* Go do critical error routine
		addq.l	#2,a7			* Cleanup stack
		move.l	d0,-12(a6)		* Save error code
*
gbpb2:		move.l	-12(a6),d0		* Get error code
		cmp.l	#RETRYIT,d0		* Magic number for retry ?
		beq	gbpbrds			* Re-read if so
*
		tst.l	-12(a6)			* Test error code
		bge	gbpb3			* Jump if OK
*
		clr.l	d0			* Set code in d0
		bra	gbpber			* Go set error code
*
		.page
*
gbpb3:		move.l	#buffer+11,(a7)		* Bytes per sector
		bsr	itom			* Convert 8086 to 68K format
		move.w	d0,d7			* Save bytes per sector
		beq	gbpb4			* Jump if zero
*
		move.b	buffer+13,d6		* Sectors per cluster
		ext.w	d6			* ... sign extended
		and.w	#$FF,d6			* ... and trimmed
		bne	gbpb5			* Jump if non-zero
*
gbpb4:		clr.l	d0			* Set error code
		bra	gbpber			* ...
*
gbpb5:		move.w	d7,(a4)			* Set recsize
		move.w	d6,2(a4)		* Set clsiz
		move.l	#buffer+22,(a7)		* Convert sectors per fat
		bsr	itom			* ...
		move.w	d0,8(a4)		* Set fsiz
		move.w	8(a4),d0		* Get fsiz
		addq.w	#1,d0			* ... plus 1
		move.w	d0,10(a4)		* Set fatrec
		move.w	(a4),d0			* Get recsize
		muls.w	2(a4),d0		* ... times clsiz
		move.w	d0,4(a4)		* Set clsizb
		move.l	#buffer+17,(a7)		* Convert number of dir ents
		bsr	itom			* ...
		asl.w	#5,d0			* ... times 32
		ext.l	d0			* ... sign extended
		divs.w	(a4),d0			* ... / recsize
		move.w	d0,6(a4)		* Set rdlen
		move.w	10(a4),d0		* Get fatrec
		add.w	6(a4),d0		* ... + rdlen
		add.w	8(a4),d0		* ... + fsiz
		move.w	d0,12(a4)		* Set datrec
		move.l	#buffer+19,(a7)		* Get number of sectors
		bsr	itom			* ... convert
		sub.w	12(a4),d0		* ... - datrec
		ext.l	d0			* ... sign extended
		divs.w	2(a4),d0		* ... / clsiz
		move.w	d0,14(a4)		* Set numcl
*
		.page
*
		move.l	#buffer+26,(a7)		* Convert number of heads
		bsr	itom			* ...
		move.w	d0,20(a5)		* Set dnsides
		move.l	#buffer+24,(a7)		* Convert sectors per track
		bsr	itom			* ...
		move.w	d0,24(a5)		* Set dspt
		move.w	20(a5),d0		* Get dnsides
		muls.w	24(a5),d0		* ... * dspt
		move.w	d0,22(a5)		* Set dspc
		move.l	#buffer+28,(a7)		* Convert number of hidden sects
		bsr	itom			* ...
		move.w	d0,26(a5)		* Set dhidden
		move.l	#buffer+19,(a7)		* Convert sectors on disk
		bsr	itom			* ...
		ext.l	d0			* ... sign extended
		divs.w	22(a5),d0		* ... / dspc
		move.w	d0,18(a5)		* Set dntracks
		clr.w	d7			* Counter = 0
		bra	gbpblpt			* Jump to end of loop
*
gbpb6:		move.l	a5,a0			* Get buffer pointer
		move.w	d7,a1			* Loop count to a1
		add.l	a1,a0			* ... + bpb address
		move.w	d7,a1			* Loop count to a1
		add.l	#buffer,a1		* ... + buffer address
		move.b	8(a1),28(a0)		* Copy a s/n byte
		addq.w	#1,d7			* Update loop count
*
gbpblpt:	cmp.w	#3,d7			* Moved 3 bytes ?
		blt	gbpb6			* Loop if not
*
		.page
*
		move.l	#cdev,a0		* Address of cdev table
		move.w	8(a6),a1		* ... plus drive number
		add.l	a1,a0			* ... to a0
		move.l	#wpstatus,a1		* Address of wpstatus table
		move.w	8(a6),a2		* ... plus drive number
		add.l	a2,a1			* ... to a1
		move.b	(a1),(a0)		* Move wpstatus to cdev
		beq	gbpb7
*
		moveq.l	#1,d0			* Set status = "Uncertain"
		bra	gbpb8
*
gbpb7:		clr.w	d0			* Set status = "Unchanged"
*
gbpb8:		move.l	#dskmode,a1		* Update dskmode table
		move.w	8(a6),a2		* ...
		add.l	a2,a1			* ...
		move.b	d0,(a1)			* ...
		move.l	a5,d0			* Setup to return bpb pointer
*
gbpber:		tst.l	(a7)+			* Pop garbage off top of stack
		movem.l	(a7)+,d6-d7/a4-a5	* Restore registers
		unlk	a6
		rts
*
		.page
*
* mediach -- check for media change
* -------    ----------------------
mediach:	link	a6,#0			* Create scratch on stack
		movem.l	d6-d7/a5,-(a7)		* Save registers on stack
		cmp.w	#2,8(a6)		* Check drive number
		blt	media1			* Jump if OK
*
		moveq.l	#ERR15,d0		* Error -- "unknown device"
		bra	media2			* ...
*
media1:		move.w	8(a6),d7		* Get drive number in d7
		movea.w	d7,a5			* Point into dskmode table
		add.l	#dskmode,a5		* ...
		cmp.b	#2,(a5)			* Definitely changed ?
		bne	media3			* Jump if not
*
		moveq.l	#2,d0			* Setup to return "Changed"
		bra	media2			* Done -- go return to caller
*
media3:		move.l	#wplatch,a0		* Check wplatch for drive
		tst.b	0(a0,d7)		* ...
		beq	media4			* Jump if not set
*
		move.b	#1,(a5)			* Set dskmode to "Uncertain"
*
media4:		move.l	_hz_200,d0		* Get time in d0
		movea.w	d7,a1			* Calculate acctim table address
		add.l	a1,a1			* ... = drive # * 4
		add.l	a1,a1			* ...
		add.l	#acctim,a1		* ... + acctim base address
		move.l	(a1),d1			* Get acctim for drive
		sub.l	d1,d0			* Subtract from current time
		cmp.l	maxactim,d0		* Timed out ?
		bge	media5			* Jump if so
*
		clr.w	d0			* Setup to return "Unchanged"
		bra	media2
*
media5:		move.b	(a5),d0			* Return status from dskmode
		ext.w	d0
*
media2:		tst.l	(a7)+			* Pop extra stack word
		movem.l	(a7)+,d7/a5		* Restore registers
		unlk	a6
		rts				* Return to caller
*
		.page
*
* rwabs -- read / write sector(s)
* -----    ----------------------
rwabs:		link	a6,#-4			* Reserve stack for temps
		movem.l	d4-d7/a5,-(a7)		* Save registers on stack
		cmp.w	#2,18(a6)		* Drive number
		blt	rwabs1
*
		moveq.l	#ERR15,d0		* ERROR -- "unknown device"
		bra	rwabser
*
rwabs1:		move.w	18(a6),d6		* Save drive number
		cmp.w	#2,8(a6)		* Check rwflag
		bge	rwabs7
*
		move.w	d6,d0
		asl.w	#5,d0
		ext.l	d0
		move.l	d0,a5
		add.l	#drvbpbs,a5
		move.w	d6,(a7)
		bsr	mediach			* Test for media change
		move.w	d0,d7			* Save status
		cmp.w	#2,d7			* Disk changed ?
		bne	rwabs2			* Jump if not
*
		moveq.l	#ERR14,d0		* ERROR -- "media changed"
		bra	rwabser			* ...
*
rwabs2:		cmp.w	#1,d7			* Disk possibly changed ?
		bne	rwabs7
*
rwabs3:		move.w	#1,(a7)			* floprd count = 1,
		clr.w	-(a7)			* ... side = 0,
		clr.w	-(a7)			* ... track = 0,
		move.w	#1,-(a7)		* ... sector = 1,
		move.w	d6,-(a7)		* ... drive number,
		clr.l	-(a7)			* ... filler,
		move.l	#buffer,-(a7)		* ... buffer address,
		jsr	floprd			* Read boot sector
		add.l	#16,a7			* Cleanup stack
		move.l	d0,-4(a6)		* Save error number
		tst.l	-4(a6)			* ... and test it
		bge	rwabs11			* Jump if OK
*
		.page
*
		move.w	d6,(a7)
		move.l	-4(a6),d0
		move.w	d0,-(a7)
		jsr	criter			* Call critical error code
		addq.l	#2,a7			* Cleanup stack
		move.l	d0,-4(a6)
*
rwabs11:	move.l	-4(a6),d0
		cmp.l	#RETRYIT,d0
		beq	rwabs3			* Read again
*
		tst.l	-4(a6)			* Test error number
		bge	rwabs4			* OK ?
*
		move.l	-4(a6),d0
		bra	rwabser			* ERROR
*
rwabs4:		clr.w	d7			* Clear media change status
		bra	rwabs10
*
rwabs6:		move.l	#buffer,a0		* Address of sector buffer to a0
		move.b	8(a0,d7),d0		* Serial number
		ext.w	d0
		move.b	28(a5,d7),d1		* Compare to old one
		ext.w	d1
		cmp.w	d1,d0
		beq	rwabs5			* Jump if they're the same
*
		moveq.l	#ERR14,d0		* ERROR -- "media changed"
		bra	rwabser
*
rwabs5:		addq.w	#1,d7			* Next byte of serial number
*
rwabs10:	cmp.w	#3,d7			* All 3 bytes checked ?
		blt	rwabs6			* Loop if not
*
		.page
*
		move.w	d6,a0			* Point at wplatch for drive
		add.l	#wplatch,a0		* ...
		move.w	d6,a1			* Point at wpstatus for drive
		add.l	#wpstatus,a1		* ...
		move.b	(a1),(a0)		* Move wpstatus to wplatch
		bne	rwabs9			* Jump if set
*
		move.w	d6,a0			* Set dskmode to "Unchanged"
		add.l	#dskmode,a0		* ...
		clr.b	(a0)			* ...
*
rwabs7:		tst.w	nflops			* Test nflops
		bne	rwabs8
*
		moveq.l	#ERR02,d0		* ERROR -- "drive not ready"
		bra	rwabser
*
rwabs8:		cmp.w	#1,8(a6)		* Check rwflag
		ble	rwabs9
*
		subq.w	#2,8(a6)		* Adjust to 0|1
*
rwabs9:		move.w	14(a6),(a7)		* Setup count,
		move.w	d6,-(a7)		* ... drive,
		move.w	16(a6),-(a7)		* ... recno,
		move.l	10(a6),-(a7)		* ... buffer,
		move.w	8(a6),-(a7)		* ... rwflag
		bsr	floprw			* Call floprw
*
		add.l	#10,a7			* Cleanup stack
*
rwabser:	tst.l	(a7)+			* Pop stack scratch
		movem.l	(a7)+,d5-d7/a5		* Restore registers
		unlk	a6			* Release temporaries
		rts				* Return to caller
*
		.page
*
* floprw -- Floppy read / write driver
* ------    --------------------------
floprw:		link	a6,#-6			* Reserve space for temps
		movem.l	d2-d7/a5,-(a7)		* Save registers
		move.w	16(a6),d0		* Drive number to d0
		asl.w	#5,d0			* Shift ( *32 )
		ext.l	d0			* Sign extend it, too
		move.l	d0,a5			* Result in a5
		add.l	#drvbpbs,a5		* Add base of BPB table
		btst	#0,13(a6)		* Buffer address even ?
		bne	floprw01		* Jump if not
*
		clr.w	d0			* Clear odd flag
		bra	floprw02		* Go save the flag
*
floprw01:	moveq.l	#1,d0			* Set odd flag
*
floprw02:	move.w	d0,-2(a6)		* Save it
		tst.w	22(a5)			* dspc set ?
		bne	floprw20		* Jump if so
*
		moveq.l	#9,d0			* Assume 9
		move.w	d0,22(a5)		* ... and make it the default
		move.w	d0,24(a5)		* ... for dspt and dspc
*
floprw20:	bra	floprw18
*
floprw03:	tst.w	-2(a6)			* Is odd flag set ?
		beq	floprw04		* Jump if not
*
		move.l	#buffer,d0		* Point to sector buffer
		bra	floprw05
*
floprw04:	move.l	10(a6),d0		* Get buffer address
*
floprw05:	move.l	d0,-6(a6)		* ... and save
		move.w	14(a6),d6		* Get recno  (lsn)
		ext.l	d6			* Extend it to long
		divs.w	22(a5),d6		* Divide by dspc for track #
		move.w	14(a6),d4		* Get recno
		ext.l	d4			* Extend to long
		divs.w	22(a5),d4		* Divide by dspc
		swap	d4			* Use remainder as sector number
		cmp.w	24(a5),d4		* Compare with dspt
		bge	floprw06		* Greater than or equal ?
*
		clr.l	d5			* Use side 0
		bra	floprw07
*
floprw06:	moveq.l	#1,d5			* Use side 1
		sub.w	24(a5),d4		* Subtract dspt from sector #
*
		.page
*
floprw07:	tst.w	-2(a6)			* Is odd flag set ?
		beq	floprw08		* Jump if not
*
		moveq.l	#1,d3			* Set counter to 1
		bra	floprw10
*
floprw08:	move.w	24(a5),d0		* Get dspt
		sub.w	d4,d0			* Subtract sector number
		cmp.w	18(a6),d0		* Compare with number of sectors
		bge	floprw09		* Jump if greater than or eq to
*
		move.w	24(a5),d3		* Get dspt
		sub.w	d4,d3			* Subtract sector number
		bra	floprw10
*
floprw09:	move.w	18(a6),d3		* Count = number of sectors
*
floprw10:	addq.w	#1,d4			* Adjust sector number (1 org)
*
floprw11:	tst.w	8(a6)			* Test r/w flag
		beq	floprw14		* Jump if it's a read
*
		move.l	-6(a6),d0		* Get buffer pointer
		cmp.l	10(a6),d0		* See if it's = buffer addr
		beq	floprw12		* Jump if so
*
		move.l	-6(a6),(a7)		* Get source address
		move.l	10(a6),-(a7)		* Destination address
		jsr	fastcopy		* Copy the sector
		addq.l	#4,a7			* Cleanup stack
*
floprw12:	move.w	d3,(a7)			* Number of sectors
		move.w	d5,-(a7)		* Side number
		move.w	d6,-(a7)		* Track number
		move.w	d4,-(a7)		* Sector number
		move.w	16(a6),-(a7)		* Drive number
		clr.l	-(a7)			* Filler
		move.l	-6(a6),-(a7)		* Buffer address
		jsr	flopwr			* Write sector
		add.l	#16,a7			* Cleaup stack
		move.l	d0,d7			* Save error code in d7
		tst.l	d7			* Errors ?
		bne	floprw13		* Jump if so
*
		.page
*
		tst.w	fverify			* Verify after write ?
		beq	floprw13		* Jump if not
*
		move.w	d3,(a7)			* Number of sectors
		move.w	d5,-(a7)		* Side number
		move.w	d6,-(a7)		* Track number
		move.w	d4,-(a7)		* Sector number
		move.w	16(a6),-(a7)		* Drive number
		clr.l	-(a7)			* Filler
		move.l	#buffer,-(a7)		* Buffer address
		jsr	flopver			* Verify sector(s)
		add.l	#16,a7			* Cleanup stack
		move.l	d0,d7			* Save error code in d7
		tst.l	d7			* Errors ?
		bne	floprw13		* Jump if so
*
		move.l	#buffer,(a7)		* Address of sector buffer
		bsr	itom			* Convert first word of buffer
		tst.w	d0			* Bad sectors ?
		beq	floprw13		* Jump if not
*
		moveq.l	#ERR16,d7		* ERROR -- "Bad sectors"
*
floprw13:	bra	floprw15
*
		.page
*
floprw14:	move.w	d3,(a7)			* Number of sectors
		move.w	d5,-(a7)		* Side number
		move.w	d6,-(a7)		* Track number
		move.w	d4,-(a7)		* Sector number
		move.w	16(a6),-(a7)		* Drive number
		clr.l	-(a7)			* Filler
		move.l	-6(a6),-(a7)		* Sector buffer
		jsr	floprd			* Read floppy sector(s)
		add.l	#16,a7			* Cleanup stack
		move.l	d0,d7			* Save error code in d7
		move.l	-6(a6),d0		* User buffer address
		cmp.l	10(a6),d0		* Is it the desired buffer ?
		beq	floprw15		* Jump if so
*
		move.l	10(a6),(a7)		* Push source address
		move.l	-6(a6),-(a7)		* Push destination address
		jsr	fastcopy		* Copy the sector
		addq.l	#4,a7			* Cleanup stack
*
floprw15:	tst.l	d7			* Error code set ?
		bge	floprw16		* Jump if not
*
		move.w	16(a6),(a7)		* Push drive number
		move.l	d7,d0			* Error code
		move.w	d0,-(a7)		* ... onto stack
		jsr	criter			* Call critical error handler
		addq.l	#2,a7			* Cleanup stack
		move.l	d0,d7			* Get error code
*
floprw16:	cmp.l	#RETRYIT,d7		* Re-try ?
		beq	floprw11		* Jump if so
*
		tst.l	d7			* Check error code
		bge	floprw17		* Jump if no error
*
		move.l	d7,d0			* Error code is the result
		bra	floprw19
*
		.page
*
floprw17:	move.w	d3,d0			* Sector counter to d0
		ext.l	d0			* Sign extend
		moveq.l	#9,d1			* Times 512
		asl.l	d1,d0			* ...
		add.l	d0,10(a6)		* Update buffer address
		add.w	d3,14(a6)		* Update sector number
		sub.w	d3,18(a6)		* Update sector count
*
floprw18:	tst.w	18(a6)			* More sectors ?
		bne	floprw03		* Jump if so
*
		clr.l	d0			* Set "no error" as result
*
floprw19:	tst.l	(a7)+			* Test top of stack
		movem.l	(a7)+,d3-d7/a5		* Restore registers
		unlk	a6			* Release stack space
		rts				* Return to caller
*
		.page
*
* itom -- Convert 8086 integer format to 680000 integer format
* ----    ----------------------------------------------------
itom:		link	a6,#-4			* Reserve stack for temps
		move.l	8(a6),a0		* Address of the number to a0
		move.b	1(a0),d0		* Get high byte
		ext.w	d0			* Make it a word
		and.w	#$FF,d0			* Isolate bits 0..7
		asl.w	#8,d0			* Shift in bits 8..15
		move.l	8(a6),a1		* Address of the number to a1
		move.b	(a1),d1			* Get low byte
		ext.w	d1			* Extend to word
		and.w	#$FF,d1			* Isolate bits 0..7
		or.w	d1,d0			* Combine with bits 8..15
		unlk	a6			* Release stack space
		rts				* Return to caller
*
		.page
*
* flopini -- Initialize floppy drive
* -------    -----------------------
flopini:	lea	dsb0,a1			* Point to dsb for drive A
		tst.w	12(a7)			* Drive A wanted ?
		beq	flopin01		* Jump if so
*
		lea	dsb1,a1			* Drive B dsb address to a1
*
flopin01:	move.w	seekrate,2(a1)		* Get seek rate
		moveq.l	#ERR01,d0		* Set default error number
		clr.w	0(a1)			* Set track number to 0
		bsr	floplock		* Setup floppy parameters
		bsr	select			* Select drive and side
		move.w	#$FF00,0(a1)
		bsr	restore			* Restore the drive
		moveq.l	#6,d7
		bsr	hseek1
		bne	flopin02
*
		bsr	restore			* Restore the drive
		beq	flopok			* Jump if no error
*
flopin02:	bra	flopfail		* Jump to error handler
*
		.page
*
* floprd -- Read floppy
* ------    -----------
floprd:		bsr	change			* Disk changed ?
		moveq.l	#ERR11,d0		* Set to "read error"
		bsr	floplock		* Set parameters
*
floprd01:	bsr	select			* Select drive and side
		bsr	go2track		* Position on track
		bne	floprd02		* Try again on error
*
		move.w	#ERR01,deferr		* Set default error
		move.l	edma(a5),a0		* Get final DMA address
		move.l	cdma(a5),a2		* Get first DMA address
		move.l	#$40000,d7		* Set timeout counter
		move.b	#FL_RM,DSKCMD(a6)	* Start readng the disk
*
floprd03:	btst	#0,DSKCMD(a6)		* Disk busy yet ?
		bne	floprd05		* Jump if so
*
		subq.l	#1,d7			* Decrement timeout counter
		beq	floprd04		* Jump if timed out
*
		bra	floprd03		* Go try for busy again
*
floprd05:	move.l	#$40000,d7		* Reset timeout counter
		bra	floprd09		* Go join main read loop
*
floprd07:	subq.l	#1,d7			* Decrement timeout counter
		beq	floprd04		* Jump if timed out
*
		btst	#0,DSKCMD(a6)		* Done yet ?
		beq	floprd08		* Jump if so
*
floprd09:	btst	#1,DSKCMD(a6)		* DRQ set ?
		beq	floprd07		* Jump if not
*
		move.b	DSKDAT(a6),(a2)+	* Read the byte into the buffer
		cmp.l	a0,a2			* Done reading ?
		bne	floprd07		* Jump if not
*
		.page
*
floprd08:	move.b	DSKCMD(a6),d0		* Read final status into d0
		and.b	#$1C,d0			* Mask for error bits
		beq	flopok			* Jump if no errors
*
		bsr	errbits			* Determine error code
*
floprd02:	cmp.w	#1,retrycnt(a5)		* Re-try ?
		bne	floprd06		* Jump if not
*
		bsr	reseek			* Home and repeat the seek
*
floprd06:	subq.w	#1,retrycnt(a5)		* Update re-try count
		bpl	floprd01		* Re-try if count still >0
*
		bra	flopfail		* Failure
*
floprd04:	move.w	#ERR02,curerr(a5)	* Set error code "not ready"
		bsr	rs1772			* Reset the WD1772
		bra	floprd02		* See if it can be retried
*
		.page
*
* errbits -- Develop error code for floppy I/O error
* -------    ---------------------------------------
errbits:	moveq.l	#ERR13,d1		* Write protect ?
		btst	#6,d0			* ...
		bne	errbits1		* Jump if so
*
		moveq.l	#ERR08,d1		* Record not found ?
		btst	#4,d0			* ...
		bne	errbits1		* Jump if so
*
		moveq.l	#ERR04,d1		* CRC error ?
		btst	#3,d0			* ...
		bne	errbits1		* Jump if so
*
		moveq.l	#ERR12,d1		* Lost data ?
		btst	#2,d0			* ...
		bne	errbits1		* Jump if so
*
		move.w	deferr(a5),d1		* Use default error
*
errbits1:	move.w	d1,curerr(a5)		* ... as curerr
		rts				* Return to caller
*
		.page
*
* flopwr -- Floppy write
* ------    ------------
flopwr:		bsr	change			* Check for disk change
		moveq.l	#ERR10,d0		* Set write error
		bsr	floplock		* Set parameters
		move.w	csect(a5),d0		* Get sector number in d0
		subq.w	#1,d0			* ... -1
		or.w	ctrack(a5),d0		* OR with track number
		or.w	cside(a5),d0		* OR with side number
		bne	flopwr01		* Jump if not boot sector
*
		moveq.l	#2,d0			* Media change
		bsr	setdmode		* Set to "unsure"
*
flopwr01:	bsr	select			* Select drive and side
		bsr	go2track		* Seek
		bne	flopwr06		* Re-try on error
*
flopwr02:	move.w	#ERR01,curerr(a5)	* Set for default error
		move.l	cdma(a5),a2		* Get I/O address
		move.l	#$40000,d7		* Set timeout counter
		move.w	ctrack(a5),d0		* Get track number
		cmpi.w	#40,d0			* See if we need precomp
		bcs	flopwr10		* Jump if not
*
		move.b	#FL_WS,DSKCMD(a6)	* Write with precomp
		bra	flopwr08		* Go join main write loop
*
flopwr10:	move.b	#FL_WS+FL_NC,DSKCMD(a6)	* Write  (no precomp)
*
		.page
*
flopwr08:	btst	#0,DSKCMD(a6)		* Busy yet ?
		bne	flopwr09		* Jump if so
*
		subq.l	#1,d7			* Decrement tiemout count
		bne	flopwr08		* Jump if not timed out
*
flopwr13:	bsr	rs1772			* Reset WD1772
		bra	flopwr05		* See if we can retry
*
flopwr11:	btst	#0,DSKCMD(a6)		* Done ?
		beq	flopwr04		* Jump if so
*
flopwr09:	btst	#1,DSKCMD(a6)		* DRQ set ?
		bne	flopwr12		* Jump if so
*
		subq.l	#1,d7			* Decrement timeout counter
		beq	flopwr13		* Jump if timed out
*
		bra	flopwr11		* Jump if not
*
flopwr12:	move.b	(a2)+,DSKDAT(a6)	* Write byte to floppy
		bra	flopwr11		* Go get the next one
*
		.page
*
flopwr04:	move.b	DSKCMD(a6),d0		* Get floppy status
		bsr	errbits			* Set error code
		btst	#6,d0			* Write protect error ?
		bne	flopfail		* Fail if so
*
		and.b	#$5C,d0			* Mask error bits
		bne	flopwr05		* Jump if an error occurred
*
		addq.w	#1,csect(a5)		* Update sector number
		add.l	#512,cdma(a5)		* Update DMA address
		subq.w	#1,ccount(a5)		* Decrement sector count
		beq	flopok			* Jump if zero -- done
*
		bsr	select1			* Set sector and DMA pointer
		bra	flopwr02		* Write next sector
*
flopwr05:	cmp.w	#1,retrycnt(a5)		* Second re-try ?
		bne	flopwr07		* Jump if not
*
flopwr06:	bsr	reseek			* Restore and re-seek
*
flopwr07:	subq.w	#1,retrycnt(a5)		* Update re-try count
		bpl	flopwr01		* Re-try if still >0
*
		bra	flopfail		* ... else fail
*
		.page
*
* flopfmt -- Format a track on the floppy
* -------    ----------------------------
flopfmt:	cmp.l	#FMAGIC,22(a7)		* Is the magic right ?
		bne	flopfail		* Fail immediately if not
*
		bsr	change			* Check for disk change
		moveq.l	#ERR01,d0		* Set default error number
		bsr	floplock		* Set parameters
		bsr	select			* Select drive and side
		move.w	14(a7),spt(a5)		* Set sectors per track,
		move.w	20(a7),interlv(a5)	* ... interleave,
		move.w	26(a7),virgin(a5)	* ... initial data,
		moveq.l	#2,d0			* Disk changed
		bsr	setdmode		* ...
		bsr	hseek			* Seek
		bne	flopfail		* Fail if seek error occurs
*
		move.w	ctrack(a5),0(a1)	* Current track to DSB
		move.w	#ERR01,curerr(a5)	* Default to curerr
		bsr	fmtrack			* Format the track
		bne	flopfail		* Fail if error occurs
*
		move.w	spt(a5),ccount(a5)	* Use spt as ccount
		move.w	#1,csect(a5)		* Start with sector 1
		bsr	verify1			* Verify the sector
		move.l	cdma(a5),a2		* Get bad sector list
		tst.w	(a2)			* Any bad ones ?
		beq	flopok			* Jump if not
*
		move.w	#ERR16,curerr(a5)	* ERROR -- "bad sectors"
		bra	flopfail
*
		.page
*
* fmtrack -- Do the dirty work of formatting a track
* -------    ---------------------------------------
fmtrack:	move.w	#ERR10,deferr(a5)	* Set default error number
		move.w	#1,d3			* Start with sector 1
		move.l	cdma(a5),a2		* Get track buffer address
		move.w	#60-1,d1		* Set count = 60
		move.b	#$4E,d0			* Write $4E as initial gap
		bsr	wmult			* ...
*
fmtrak01:	move.w	d3,d4			* Save sector number in d4
*
fmtrak02:	move.w	#12-1,d1		* Set count = 12
		clr.b	d0			* Write $00 as sync
		bsr	wmult			* ...
		move.w	#3-1,d1			* Set count = 3
		move.b	#$F5,d0			* Write $F5's
		bsr	wmult			* ...
		move.b	#$FE,(a2)+		* Write $FE (address mark)
		move.b	ctrack+1,(a2)+		* Write track #
		move.b	cside+1,(a2)+		* Write side #
		move.b	d4,(a2)+		* Write sector #
		move.b	#2,(a2)+		* Write sector size code
		move.b	#$F7,(a2)+		* Write checksum code
		move.w	#22-1,d1		* Set count = 22
		move.b	#$4E,d0			* Write gap
		bsr	wmult			* ...
		move.w	#12-1,d1		* Set count = 12
		clr.b	d0			* Write $00 as sync
		bsr	wmult			* ...
		move.w	#3-1,d1			* Set count = 3
		move.b	#$F5,d0			* Write $F5's
		bsr	wmult			* ...
		move.b	#$FB,(a2)+		* Write DAM
		move.w	#256-1,d1		* Set count = 256 words
*
		.page
*
fmtrak03:	move.b	virgin(a5),(a2)+	* Write virgin data in buffer
		move.b	virgin+1(a5),(a2)+	* ...
		dbf	d1,fmtrak03		* ...
*
		move.b	#$F7,(a2)+		* Write CRC code
		move.w	#40-1,d1		* Set count = 40
		move.b	#$4E,d0			* Write gap
		bsr	wmult			* ...
		add.w	interlv(a5),d4		* Add interleave to sector #
		cmp.w	spt(a5),d4		* Check against spt
		ble	fmtrak02		* Jump if not >
*
		addq.w	#1,d3			* Starting sector +1
		cmp.w	interlv(a5),d3
		ble	fmtrak01
*
		move.w	#1401-1,d1		* Set count = 1401
		move.b	#$4E,d0			* Write final gap
		bsr	wmult			* ...
		move.l	cdma(a5),a2		* Get buffer address
		move.l	#$40000,d7		* Set timeout count
		move.w	ctrack(a5),d0		* Get track number
		cmpi.w	#40,d0			* Do we need precomp ?
		bcs	fmtrak08		* Jump if not
*
		move.b	#FL_WT,DSKCMD(a6)	* Start format with precomp
		bra	fmtrak07		* Go join main format loop
*
fmtrak08:	move.b	#FL_WT+FL_NC,DSKCMD(a6)	* Start format without precomp
*
		.page
*
fmtrak07:	btst	#0,DSKCMD(a6)		* Busy yet ?
		bne	fmtrak09		* Jump if so
*
fmtrak04:	subq.l	#1,d7			* Decrement timeout counter
		bne	fmtrak07		* Jump if not timed out
*
fmtrak05:	bsr	rs1772			* Reset WD1772
		moveq.l	#1,d7			* Set error flag
		rts				* Return to caller
*
fmtrak11:	btst	#0,DSKCMD(a6)		* Done ?
		beq	fmtrak06		* Jump if so
*
fmtrak09:	btst	#1,DSKCMD(a6)		* DRQ set ?
		bne	fmtrak10		* Jump if so
*
		subq.l	#1,d7			* Decrement timeout coutner
		beq	fmtrak05		* Jump if timed out
*
		bra	fmtrak11		* Go check again
*
fmtrak10:	move.b	(a2)+,DSKDAT(a6)	* Write a byte of format
		bra	fmtrak11		* Go wait for DRQ
*
fmtrak06:	move.b	DSKCMD(a6),d0		* Get final status
		bsr	errbits			* Set possible error code
		andi.b	#$44,d0			* Check error bits
		rts				* Return to caller with status
*
* wmult -- write multiple bytes into the track buffer
* -----    ------------------------------------------
wmult:		move.b	d0,(a2)+		* Move a byte into the buffer
		dbf	d1,wmult		* Loop until we're done
*
		rts				* Return to caller
*
		.page
*
* flopver -- verify sector(s) on a floppy
* -------    ----------------------------
flopver:	bsr	change			* Check for disk change
		moveq.l	#ERR11,d0		* Set default to "read error"
		bsr	floplock		* Set parameters
		bsr	select			* Select drive and side
		bsr	go2track		* Seek
		bne	flopfail		* Jump if seek error
*
		bsr	verify1			* Verify the data
		bra	flopok			* Done
*
* verify1 -- Verify the data for a sector
* -------    ----------------------------
verify1:	move.w	#ERR11,deferr(a5)	* Set default = "read error"
		move.l	cdma(a5),a2		* Pointer for bad sector list
		add.l	#512,cdma(a5)		* Update by length of a sector
*
verify01:	move.w	#2,retrycnt(a5)		* Set retrycnt for 2 attempts
		move.w	csect(a5),d7		* Get sector nubmer
		move.b	d7,DSKSEC(a6)		* Send to WD1772
*
verify02:	moveq.l	#0,d6			* Clear bad compare flag
		move.l	cdma(a5),a0		* Get compare address
		move.l	#$40000,d7		* Set timeout counter
		move.b	#FL_RS,DSKCMD(a6)	* Start the read
*
verify08:	btst	#0,DSKCMD(a6)		* Busy yet ?
		bne	verify09		* Jump if so
*
		subq.l	#1,d7			* Decrement timeout counter
		beq	verify13		* Jump if timed out
*
		bra	verify08		* Wait for busy some more
*
verify09:	move.l	#$40000,d7		* Reset timeout counter
		bra	verify11		* Go join main compare loop
*
verify10:	subq.l	#1,d7			* Decrement timeout counter
		beq	verify13		* Jump if timed out
*
		btst	#0,DSKCMD(a6)		* Done yet ?
		beq	verify12		* Jump if so
*
		.page
*
verify11:	btst	#1,DSKCMD(a6)		* DRQ set ?
		beq	verify10		* Jump if not
*
		move.b	DSKDAT(a6),d0		* Read data
		bra	verify10		* Continue reading
*
verify12:	move.b	DSKCMD(a6),d0		* Get final status
		bsr	errbits			* Determine error code
		and.b	#$1C,d0			* Mask RNF, CRC, LOST
		bne	verify06		* Jump to re-try if any set
*
verify05:	addq.w	#1,csect(a5)		* Update sector number
		subq.w	#1,ccount(a5)		* Update sector count
		bne	verify01		* Jump if more to do
*
		sub.l	#512,cdma(a5)		* Reset DMA pointer
		clr.w	(a2)			* Set NIL in bad sector list
		rts				* Return to caller
*
verify06:	cmp.w	#1,retrycnt(a5)		* Is it the 2nd attempt ?
		bne	verify07		* Jump if not
*
		bsr	reseek			* Restore and re-seek
*
verify07:	subq.w	#1,retrycnt(a5)		* Decrement retry count
		bpl	verify02		* Re-try if still positive
*
		move.w	csect(a5),(a2)+		* Add to bad sector list
		bra	verify05		* Go do next sector
*
verify13:	move.w	#ERR02,curerr(a5)	* Set timeout error code
		bsr	rs1772			* Reset WD1772
		bra	verify06		* See if we can retry it
*
		.page
*
* flopvbl -- Floppy VBL timer interrupt handling
* -------    -----------------------------------
flopvbl:	movem.l	d0-d1/a0-a1/a5-a6,-(a7)	* Save registers
		sub.l	a5,a5			* Clear a5 for use as RAM base
		lea	FLOPPY,a6		* Base of floppy regs
		st	motoron(a5)		* Set motoron flag
		move.l	frclock(a5),d0		* Get interrupt count
		move.b	d0,d1			* ...
		and.b	#7,d1			* ... MOD 8
		bne	flopvb02		* Jump if not 8th int
*
		move.w	sr,-(a7)		* Save interrupt level
		ori.w	#IPL7,sr		* Mask off interrupts
		clr.w	deslflag(a5)		* Clear deselect flag
		move.b	mc1iorec+cfr1,d0	* Get port status
		ori.b	#1,d0			* Set drive select bit
		move.b	d0,MC1ACIA+ACIA_CFR	* Send drive select to port
		move.b	d0,mc1iorec+cfr1	* Store updated port status
		move.w	(a7)+,sr		* Restore interrupt level
		lea	wpstatus(a5),a0		* Point at wpstatus for drive
		lea	dsb0(a5),a1		* Point at dsb0
		move.b	DSKCMD(a6),d0		* Read WD1772 status
		btst	#6,d0			* Test write protect bit
		sne	(a0)			* ... and save
*
		.page
*
flopvb02:	move.w	wpstatus(a5),d0		* Get wpstatus of drive A
		or.w	d0,wplatch(a5)		* OR into wplatch
		tst.w	deslflag(a5)		* Floppies already deselected ?
		bne	flopvb03		* Jump if so
*
		move.b	DSKCMD(a6),d0		* Read WD1772 status
		btst	#7,d0			* Motor-on bit set ?
		bne	flopvb04		* Don't de-select if it is
*
		move.w	sr,-(a7)		* Save sr
		ori.w	#IPL7,sr		* Disable interrupts
		move.b	mc1iorec+cfr1,d0	* Get cfr1 image
		bclr	#0,d0			* Clear d0  (drive select)
		move.b	d0,mc1iorec+cfr1	* Save new value
		move.b	d0,MC1ACIA+ACIA_CFR	* Send to hardware
		move.w	(a7)+,sr		* Restore sr  (enable ints)
		move.w	#1,deslflag(a5)		* Set the deselect flag
*
flopvb03:	clr.w	motoron(a5)		* Clear motoron flag
*
flopvb04:	movem.l	(a7)+,d0-d1/a0-a1/a5-a6	* Restore registers
		rts				* Return to caller
*
		.page
*
* floplock -- Setup floppy parameters
* --------    -----------------------
floplock:	movem.l	d3-d7/a3-a6,flpregs	* Save registers
		move.w	sr,flpsrsv		* Save sr
		ori.w	#IPL7,sr		* Disable ints
		sub.l	a5,a5			* Clear a5
		lea	FLOPPY,a6		* Base of floppy registers
		st	motoron(a5)		* Set motoron flag
		move.w	d0,deferr(a5)		* Set default error code
		move.w	d0,curerr(a5)		* ... current error code
		move.w	#1,flock(a5)		* Disable flopvbl routine
		move.l	8(a7),cdma(a5)		* Set cdma
		move.w	16(a7),cdev(a5)		* ... cdev
		move.w	18(a7),csect(a5)	* ... csect
		move.w	20(a7),ctrack(a5)	* ... ctrack
		move.w	22(a7),cside(a5)	* ... cside
		move.w	24(a7),ccount(a5)	* ... ccount
		move.w	#2,retrycnt(a5)		* ... retrycnt
		lea	dsb0(a5),a1		* Address of dsb0
		tst.w	cdev(a5)		* Drive A ?
		beq	floplk01		* Jump if so
*
		lea	dsb1(a5),a1		* Drive B, use dsb1
*
floplk01:	moveq.l	#0,d7			* Clear out d7
		move.w	ccount(a5),d7		* Get ccount
		lsl.w	#8,d7			* ... * sector length  (512)
		lsl.w	#1,d7			* ...
		move.l	cdma(a5),a0		* Get DMA start address
		add.l	d7,a0			* Add length of sectors
		move.l	a0,edma(a5)		* Set edma  (end DMA address)
		tst.w	0(a1)			* Check current track
		bpl	floplk03		* Jump if >= 0
*
		bsr	select			* Select the drive
		clr.w	0(a1)			* Set current track to 0
		bsr	restore			* Restore drive to track 0
		beq	floplk03		* Jump if OK
*
		moveq.l	#10,d7			* Seek out to track 10
		bsr	hseek1			* ...
		bne	floplk02		* Jump if an error occurred
*
		bsr	restore			* Try the restore again
		beq	floplk03		* Jump if OK
*
floplk02:	move.w	#$FF00,0(a1)		* Recalibrate error
*
floplk03:	rts				* Return to caller
*
		.page
*
* flopfail -- Floppy I/O failure exit
* --------    -----------------------
flopfail:	moveq.l	#1,d0			* Set media change to "unsure"
		bsr	setdmode		* ...
		move.w	curerr(a5),d0		* Get curerr
		ext.l	d0			* ...
		bra	flopok1			* Go set end status
*
* flopok -- Floppy I/O success exit
* ------    -----------------------
flopok:		clr.l	d0			* Set error code = 0  (no error)
*
flopok1:	move.l	d0,-(a7)		* Push code onto stack
		move.w	0(a1),d7		* Get track number
		move.b	d7,DSKDAT(a6)		* Send to WD1772
		move.w	#FL_SK,d6		* Seek command to d6
		bsr	flopcmds		* Send command
		move.w	cdev(a5),d0		* Get drive number
		lsl.w	#2,d0			* Use as index
		lea	acctim(a5),a0		* ... into acctim
		move.l	_hz_200(a5),0(a0,d0)	* Set last access time
		cmp.w	#1,nflops(a5)
		bne	flopok01
*
		move.l	_hz_200(a5),4(a0)	* Time for other drive
*
flopok01:	move.w	flpsrsv,sr		* Restore sr  (enable ints)
		move.l	(a7)+,d0		* Restore error number to d0
		movem.l	flpregs,d3-d7/a3-a6	* Restore registers
		clr.w	flock			* Release flopvbl routine
		rts				* Return to caller
*
		.page
*
* hseek -- seek to track in ctrack
* -----    -----------------------
hseek:		move.w	ctrack,d7		* Get ctrack
*
* hseek1 -- seek to track in d7
* ------    -------------------
hseek1:		move.w	#ERR10,curerr		* Set curerr = "seek error"
		move.b	d7,DSKDAT(a6)		* Send track number to WD1772
		move.w	#FL_SK,d6		* Seek command
		bra	flopcmds		* Go send command to WD1772
*
* reseek -- Restore, then re-seek
* ------    ---------------------
reseek:		move.w	#ERR10,curerr		* Set curerr = "seek error"
		bsr	restore			* Restore
		bne	reseek01		* Jump if error occurred
*
		clr.w	0(a1)			* Clear current track to 0
		move.b	#0,DSKTRK(a6)		* Send track to WD1772
		move.b	#5,DSKDAT(a6)		* Setup to seek to track 5
		move.w	#FL_SK,d6		* Seek command
		bsr	flopcmds		* Seek
		bne	reseek01		* Jump if error
*
		move.w	#5,0(a1)		* Update current track = 5
*
* go2track -- seek with verify
* --------    ----------------
go2track:	move.w	#ERR10,curerr(a5)	* Set curerr = "seek error"
		move.w	ctrack(a5),d7		* Put track # in d7
		move.b	d7,DSKDAT(a6)		* Send track to WD1772
		moveq.l	#FL_SV,d6		* Seek/verify command
		bsr	flopcmds		* Send command to floppy
		bne	reseek01		* Jump if error occurred
*
		move.w	ctrack(a5),0(a1)	* Update track number in dsb
		and.b	#$18,d7			* Test for RNF, CRC, LOST
*
reseek01:	rts				* Return to caller
*
		.page
*
* restore -- Restore to track 0
* -------    ------------------
restore:	clr.w	d6			* Restore command
		bsr	flopcmds		* Send to WD1772
		bne	restor01		* Jump if an error occurred
*
		btst	#2,d7			* Check track 0 bit
		eori	#0004,sr		* Invert Z flag in 680000 sr
		bne	restor01		* Jump if not track 0
*
		clr.w	0(a1)			* Set track = 0 in dsb
*
restor01:	rts				* Return to caller
*
* flopcmds -- Send command to WD1772 floppy controller
* --------    ----------------------------------------
flopcmds:	move.w	2(a1),d0		* Get seek rate from dsb
		and.b	#$3,d0			* ...
		or.b	d0,d6			* OR into command word
		move.l	#$40000,d7		* Set timeout counter
		btst	#7,DSKCMD(a6)		* Motor on ?
		bne	flopcm01		* Jump if so
*
		move.l	#$60000,d7		* Set longer timeout count
*
flopcm01:	move.b	d6,DSKCMD(a6)		* Write command from d6
*
		move.l	#$1000,d0		* Set initial busy timeout in d0
*
flopcm04:	btst	#0,DSKCMD(a6)		* Controller busy yet ?
		bne	flopcm02		* Jump if so
*
		subq.l	#1,d0			* Decrement timeout counter
		beq	flopcm03		* Jump if timed out
*
		bra	flopcm04		* Wait for busy some more
*
flopcm02:	subq.l	#1,d7			* Decrement timeout counter
		beq	flopcm03		* Jump if timed out
*
		btst	#0,DSKCMD(a6)		* WD1772 done ?
		bne	flopcm02		* Jump if not
*
		clr.l	d7			* Clear out upper bits of d7
		move.b	DSKCMD(a6),d7		* Read status into LS byte of d7
		clr.l	d6			* Clear error flag in d6
		rts				* Return to caller
*
flopcm03:	bsr	rs1772			* Reset WD1772  (end transfer)
		moveq.l	#1,d6			* Set error flag in d6
		rts				* Return to caller
*
		.page
*
* rs1772 -- Reset WD1772 floppy controller
* ------    ------------------------------
rs1772:		move.b	#FL_FR,DSKCMD(a6)	* Send reset to WD1772
		move.w	#15,d7			* Set delay count
*
rs1772a:	dbf	d7,rs1772a		* Delay a while
		rts				* Return to caller
*
* select -- Select drive and side
* ------    ----------------------
select:		move.w	sr,-(a7)		* Save sr on stack
		ori.w	#IPL7,sr		* Disable interrupts
		clr.w	deslflag(a5)		* Clear deselect flag
		move.b	mc1iorec+cfr1,d0	* Get current MC1 cfr1 image
		ori.b	#1,d0			* Select drive  (we only have 1)
		move.b	d0,mc1iorec+cfr1	* Store updated image
		move.b	d0,MC1ACIA+ACIA_CFR	* Send it to the MC1 ACIA
		move.w	cside(a5),d0		* Get side number  (LS bit)
		and.w	#1,d0			* Mask off garbage bits
		andi.b	#$FE,mc2iorec+cfr1	* Mask off D0 in MC2 cfr1 image
		or.b	mc2iorec+cfr1,d0	* OR side with mc2iorec cfr1
		move.b	d0,mc2iorec+cfr1	* Store updated cfr1 image
		move.b	d0,MC2ACIA+ACIA_CFR	* Send it to the MC2 ACIA
		move.w	(a7)+,sr		* Restore interrupts
		move.w	0(a1),d7		* Get track from dsb
		move.b	d7,DSKTRK(a6)		* Send it to the WD1772
		clr.b	tmpdma(a5)		* Clear bits 31..24 of tmpdma
*
select1:	move.w	csect(a5),d7		* Get sector number
		move.b	d7,DSKSEC(a6)		* Send it to the WD1772
		move.l	cdma(a5),a0		* Setup transfer address in a0
		rts				* Return to caller
*
		.page
*
* change -- Check for disk change status
* ------    ----------------------------
change:		cmp.w	#1,nflops		* Check # of floppies
		bne	change02
*
		move.w	16(a7),d0		* Drive number to d0
		cmp.w	disknum,d0
		beq	change01		* Jump if =
*
		move.w	d0,-(a7)		* Stack drive number
		move.w	#ERR17,-(a7)		* Stack error code
		bsr	criter			* Do critical error routine
		addq.w	#4,a7			* Cleanup stack
		move.w	#$FFFF,wplatch		* Status for drives = "unsure"
		move.w	16(a7),disknum		* Save disk number
*
change01:	clr.w	16(a7)			* Clear disk number
*
change02:	rts				* Return to caller
*
* setdmode -- Set drive change mode
* --------    ---------------------
setdmode:	lea	dskmode,a0		* Point at disk mode table
		move.b	d0,-(a7)		* Save mode on stack
		move.w	cdev(a5),d0		* Get drive number
		move.b	(a7)+,0(a0,d0)		* Set drive mode / cleanup stack
		rts				* Return to caller
*
		.page
*
* bootload -- Load boot sector
* --------    ----------------
bootload:	link	a6,#0			* Link stack frames
		movem.l	d6-d7,-(a7)		* Save registers
		jsr	_hdvini			* Init drive
		tst.w	nflops			* See if any connected
		beq	bootld01		* Jump if drive there
*
		moveq.l	#1,d0			* "couldn't load"
		bra	bootld02
*
bootld01:	moveq.l	#2,d0			* "no drive connected"
*
bootld02:	move.w	d0,d7			* Save possible error in d7
		tst.w	nflops
		beq	bootld04
*
		cmp.w	#2,bootdev
		bge	bootld04
*
		move.w	#1,(a7)			* Sector count = 1,
		clr.w	-(a7)			* ... side = 0,
		clr.w	-(a7)			* ... track = 0,
		move.w	#1,-(a7)		* ... sector = 1,
		move.w	bootdev,-(a7)		* ... drive = bootdev,
		clr.l	-(a7)			* ... filler,
		move.l	#buffer,-(a7)		* ... buffer address
		jsr	floprd			* Read the sector
		add.l	#16,a7			* Cleanup stack
		tst.l	d0			* Error ?
		bne	bootld03		* Jump if not
*
		clr.w	d7
		bra	bootld04
*
bootld03:	tst.b	wpstatus
		bne	bootld04
*
		moveq.l	#3,d0			* "unreadable"
		bra	bootld07
*
bootld04:	tst.w	d7
		beq	bootld05
*
		move.w	d7,d0			* Get old error code
		bra	bootld07
*
		.page
*
bootld05:	move.w	#256,(a7)		* Set count for 256 words
		move.l	#buffer,-(a7)		* Point at buffer
		bsr	chksum			* Calculate checksum
		addq.l	#4,a7			* Cleanup stack
		cmp.w	#$1234,d0		* Boot sector checksum ?
		bne	bootld06		* Jump if not
*
		clr.w	d0			* Set flag for OK
		move.w	bootdev,booted		* Save last boot device
		bra	bootld07
*
bootld06:	moveq.l	#4,d0			* "not valid boot sector"
*
bootld07:	tst.l	(a7)+
		movem.l	(a7)+,d7		* Restore registers
		unlk	a6
		rts				* Return to caller
*
		.page
*
* chksum -- caluculate checksum
* ------    -------------------
chksum:		link	a6,#0
		movem.l	d6-d7,-(a7)		* Preserve registers
		clr.w	d7			* Clear sum
		bra	chksum02		* Jump into loop
*
chksum01:	move.l	8(a6),a0		* Get buffer address
		move.w	(a0),d0			* Get word
		add.w	d0,d7			* Add to checksum in d7
		addq.l	#2,8(a6)		* Update buffer address
*
chksum02:	move.w	12(a6),d0		* Get word count
		subq.w	#1,12(a6)		* Decrement it
		tst.w	d0			* Done ?
		bne	chksum01		* Loop if not
*
		move.w	d7,d0			* Put result in d0
		tst.l	(a7)+
		movem.l	(a7)+,d7		* Restore registers
		unlk	a6
		rts				* Return to caller
*
		.page
*
* mult32 -- 32 bit signed multiply
* ------    ----------------------
mult32:		link	a6,#-4
		clr.w	d2
		tst.l	8(a6)
		bge	mult32a
*
		neg.l	8(a6)
		addq.w	#1,d2
*
mult32a:	tst.l	12(a6)
		bge	mult32b
*
		neg.l	12(a6)
		addq.w	#1,d2
*
mult32b:	move.w	10(a6),d0
		mulu	14(a6),d0
		move.l	d0,-4(a6)
		move.w	8(a6),d0
		mulu	14(a6),d0
		move.w	12(a6),d1
		mulu	10(a6),d1
		add.w	d1,d0
		add.w	-4(a6),d0
		move.w	d0,-4(a6)
		move.l	-4(a6),d0
		btst	#0,d2
		beq	mult32c
*
		neg.l	d0
*
mult32c:	unlk	a6
		rts
*
		.page
*
* rand -- Generate a random number
* ----    ------------------------
rand:		link	a6,#-4			* Reserve stack for temps
		tst.l	rseed			* See if the seed is zero
		bne	rand01			* Jump if not
*
		move.l	_hz_200,d0		* Pick up the 200 Hz clock
		moveq.l	#16,d1			* Shift it left
		asl.l	d1,d0			* ...
		or.l	_hz_200,d0		* OR in current 200 Hz clock
		move.l	d0,rseed		* Use that as the seed
*
rand01:		move.l	#$BB40E62D,-(a7)	* Put PI on the stack
		move.l	rseed,-(a7)		* ... and rseed, too
		bsr	mult32			* Multiply them
		addq.l	#8,a7			* Cleanup stack
		addq.l	#1,d0			* Add 1 to the result
		move.l	d0,rseed		* Save as new seed
		move.l	rseed,d0
		asr.l	#8,d0			* Make it a 24 bit number
		and.l	#$FFFFFF,d0		* ...
		unlk	a6			* Release stack
		rts				* Return to caller
*
		.page
*
* protobt -- Generate a prototype boot sector
* -------    --------------------------------
protobt:	link	a6,#-6
		movem.l	d5-d7/a5,-(a7)		* Save registers
		tst.w	18(a6)			* Test execflg
		bge	protbt03		* Jump if set
*
		move.w	#256,(a7)		* Count = 256 words
		move.l	8(a6),-(a7)		* Address of buffer
		bsr	chksum			* Calculate checksum
		addq.l	#4,a7			* Cleanup stack
		cmp.w	#$1234,d0		* Boot checksum ?
		beq	protbt01		* Jump if so
*
		clr.w	d0			* Not executable
		bra	protbt02
*
protbt01:	moveq.l	#1,d0			* Executable
*
protbt02:	move.w	d0,18(a6)		* Set execflg
*
protbt03:	tst.l	12(a6)			* Serial number ?
		blt	protbt07		* Jump if not to be changed
*
		move.l	12(a6),d0		* Get it into d0
		cmp.l	#$FFFFFF,d0		* > $FFFFFF
		ble	protbt04		* Jump if not
*
		bsr	rand			* Generate a random number
		move.l	d0,12(a6)		* Save as s/n
*
protbt04:	clr.w	d7			* Clear counter
		bra	protbt06		* Enter move loop
*
		.page
*
protbt05:	move.l	12(a6),d0		* Get s/n
		and.l	#$FF,d0			* Isolate low 8 bits
		move.w	d7,a1			* Point to next byte
		add.l	8(a6),a1		* ...
		move.b	d0,8(a1)		* Write byte in buffer
		move.l	12(a6),d0		* Get s/n
		asr.l	#8,d0			* Shift right 8 bits
		move.l	d0,12(a6)		* Save shifted value
		addq.w	#1,d7			* Update counter
*
protbt06:	cmp.w	#3,d7			* See if we're done
		blt	protbt05		* Loop if not
*
protbt07:	tst.w	16(a6)			* Diskette size ?
		blt	protbt10		* Jump if not to be changed
*
		move.w	16(a6),d6		* Get diskette size
		muls.w	#19,d6			* ... times 19 as pointer
		clr.w	d7			* Clear counter
		bra	protbt09		* Go move in BPB
*
protbt08:	move.w	d7,a0			* Get counter
		add.l	8(a6),a0		* Add base of buffer
		move.w	d6,a1			* Get BPB pointer
		add.l	#pbpbtab,a1		* Add base of prototype BPB's
		move.b	(a1),11(a0)		* Copy BPB data
		addq.w	#1,d6			* Update pointer
		addq.w	#1,d7			* Update counter
*
protbt09:	cmp.w	#19,d7			* Done ?
		blt	protbt08		* Loop if not
*
protbt10:	clr.w	-6(a6)
		move.l	8(a6),-4(a6)
		bra	protbt12
*
		.page
*
protbt11:	move.l	-4(a6),a0		* Get buffer pointer
		move.w	(a0),d0			* Get word from buffer
		add.w	d0,-6(a6)		* Sum for checksum
		addq.l	#2,-4(a6)		* Point at next word
*
protbt12:	move.l	8(a6),d0		* Get buffer address
		add.l	#$1FE,d0		* Plus sector length
		cmp.l	-4(a6),d0		* Done ?
		bhi	protbt11		* Loop if not
*
		move.w	#$1234,d0		* Checksum for boot sector
		sub.w	-6(a6),d0		* Subtract checksum for buffer
		move.l	-4(a6),a1
		move.w	d0,(a1)			* Store checksum in buffer
		tst.w	18(a6)			* Check execflg
		bne	protbt13		* Boot sector to be executable ?
*
		move.l	-4(a6),a0		* Mung checksum so it's not
		addq.w	#1,(a0)
*
protbt13:	tst.l	(a7)+
		movem.l	(a7)+,d6-d7/a5		* Restore registers
		unlk	a6			* Release stack
		rts				* Return to caller
*
		.endc
*
* criter -- Critical error handler
* ------    ----------------------
criter:		move.l	critvec,-(a7)		* Put error vector on stack
		moveq.l	#-1,d0			* Set default error return
		rts				* "Return" to handler
*
		.page
*
		.ifne	BUCHLA
*
* api_int -- Analog processor interrupt handler
* -------    ----------------------------------
api_int:	movem.l	d0-d1/a0-a1,-(a7)	* Save registers
		clr.l	d0			* Read input port
		move.b	ANALOG,d0		* ... into d0[7..0]
		move.b	d0,api_inp		* ... and api_inp
		btst	#7,d0			* Check for signal number flag
		bne	api_s0a			* Jump if signal number read
*
		lea	api_tv,a0		* Get api_tv base in a0
		clr.w	d1			* Get api_sv in d1
		move.b	api_sv,d1		* ...
		cmpi.b	#6,d1			* Check range
		bgt	api_sve			* Jump if out of range
*
		lsl.w	#2,d1			* Develop jump address
		move.l	0(a0,d1.W),a1		* ... from api_tv in a1
		jmp	(a1)			* Jump to state handler
*
api_s0:		btst	#7,d0			* Check for signal number flag
		beq	api_ret			* Jump if not signal number
*
api_s0a:	clr.b	api_val			* Clear value byte
		andi.b	#$7F,d0			* Mask off signal number flag
		cmpi.b	#82,d0			* See if it's in range
		bgt	api_err			* Jump if out of range (GT 82)
*
		move.b	d0,api_sig		* Store signal number
		beq	api_pub			* Jump if it was "all off"
*
		lea	api_tab,a0		* Get sv table base
		move.b	0(a0,d0.W),api_sv	* Set sv for next time
*
api_ret:	movem.l	(a7)+,d0-d1/a0-a1	* Restore registers
		rte				* Return to interrupted code
*
api_err:	move.b	d0,api_bug		* Catch the bug
		clr.b	api_sv			* Force state zero
		bra	api_ret			* Go back to interrupted code
*
api_sve:	move.b	d1,api_svb		* Catch the bug
		clr.b	api_sv			* Force state zero
		bra	api_ret			* Go back to interrupted code
*
		.page
api_s1:		btst	#0,d0			* See if it takes a value
		beq	api_s1a			* Jump if not
*
		move.b	#2,api_sv		* Set sv for state 2
		bra	api_ret			* Go return from interrupt
*
api_s1a:	clr.b	api_val			* Clear value byte
		bra	api_pub			* Go output to fifo
*
api_s2:		ori.b	#$80,d0			* Set status = 1
		move.b	d0,api_val		* Set value byte
		bra	api_pub			* Go output to fifo
*
api_s3:		ror.b	#1,d0			* Set status in value byte
		andi.b	#$80,d0			* ...
		move.b	d0,api_val		* ...
		move.b	#4,api_sv		* Set sv for state 4
		bra	api_ret			* Go return from interrupt
*
api_s4:		or.b	d0,api_val		* Set value byte
		bra	api_pub			* Go output to fifo
*
api_s5:		ror.b	#1,d0			* Set status in value byte
		andi.b	#$80,d0			* ...
*
api_s6:		move.b	d0,api_val		* Set value byte
*
api_pub:	movea.l	api_fi,a0		* Get fifo input pointer in a0
		move.w	api_sig,(a0)+		* Store new input
		cmpa.l	#api_fe,a0		* Wrap around ?
		blt	api_pb1			* Jump if not
*
		movea.l	#api_fum,a0		* Wrap input pointer to start
*
api_pb1:	move.l	a0,api_fi		* Update input pointer
		cmpa.l	api_fo,a0		* Buffer full ?
		bne	api_pb3			* Jump if not
*
		movea.l	api_fo,a1		* Get fifo output pointer in a1
		adda.l	#2,a1			* Increment it
		cmpa.l	#api_fe,a1		* Wrap around ?
		blt	api_pb2			* Jump if not
*
		movea.l	#api_fum,a1		* Wrap output pointer to start
*
api_pb2:	move.l	a1,api_fo		* Set new output pointer
*
api_pb3:	clr.b	api_sv			* Set state to 0
		bra	api_ret			* Return from interrupt
*
		.page
*
* api_get -- Get analog input
* -------    ----------------
api_get:	move.w	sr,-(a7)		* Save status register
		ori.w	#IPL3,sr		* Disable ANALOG I/O interrupts
		movea.l	api_fo,a0		* Get fifo output pointer
		cmpa.l	api_fi,a0		* See if fifo is empty
		bne	api_g1			* Jump if not
*
		moveq.l	#-1,d0			* Set empty-fifo value
		move.w	(a7)+,sr		* Restore interrupt mask
		rts				* Return to caller
*
api_g1:		clr.l	d0			* Get fifo value
		move.w	(a0)+,d0		* ...
		cmpa.l	#api_fe,a0		* See if pointer wrapped
		blt	api_g2			* Jump if not
*
		movea.l	#api_fum,a0		* Wrap the pointer around
*
api_g2:		move.l	a0,api_fo		* Update pointer
		move.w	(a7)+,sr		* Restore interrupt mask
		rts				* Return to caller
*
* api_zap -- clear analog I/O FIFO
* -------    ---------------------
api_zap:	move.w	sr,-(a7)		* Save status register
		ori.w	#IPL3,sr		* Disable ANALOG I/O interrupts
		move.l	#api_fum,api_fi		* Clear analog processor fifo
		move.l	#api_fum,api_fo		* ...
		move.w	(a7)+,sr		* Restore interrupt mask
		rts
*
		.endc
*
		.page
*************************************************************************
*									*
*			Constant definitions				*
*			--------------------				*
*************************************************************************
*
		.even
*
		.ifne	BUCHLA
*
* Buchla 700 tables:
*
* t13tab -- Trap-13 pointer table -- Primary BIOS functions
* ------    -----------------------------------------------
t13tab:		dc.w	12			* Number of Trap-13 routines
*
		dc.l	nullrts			* 0 - (getmpb)
		dc.l	bconstat		* 1 - bconstat
		dc.l	bconin			* 2 - bconin
		dc.l	bconout			* 3 - bconout
		dc.l	rwabs			* 4 - rwabs
		dc.l	setexec			* 5 - setexec
		dc.l	nullrts			* 6 - (tickcal)
		dc.l	getbpb			* 7 - getbpb
		dc.l	bcostat			* 8 - bcostat
		dc.l	mediach			* 9 - mediach
		dc.l	drvmap			* 10 - drvmap
		dc.l	nullrts			* 11 - (shift)
*
		.page
*
* t14tab -- Trap-14 pointer table -- Extended BIOS functions (Buchla 700 only)
* ------    ------------------------------------------------------------------
t14tab:		dc.w	10			* Number of trap14 routines
*
		dc.l	piorec			* 0 - Get iorec address
		dc.l	setport			* 1 - Set ACIA parameters
		dc.l	floprd			* 2 - Read floppy
		dc.l	flopwr			* 3 - Write floppy
		dc.l	flopfmt			* 4 - Format floppy
		dc.l	flopver			* 5 - Verify floppy
		dc.l	protobt			* 6 - Create prototype boot sec.
		dc.l	rand			* 7 - Generate random number
		dc.l	api_get			* 8 - Get analog input
		dc.l	api_zap			* 9 - Clear analog FIFO
*
		.endc
*
		.even
*
		.ifeq	BUCHLA
*
* NASA 3D Helmet tables:
*
* t13tab -- Trap-13 pointer table -- Primary BIOS functions
* ------    -----------------------------------------------
t13tab:		dc.w	12			* Number of Trap-13 routines
*
		dc.l	nullrts			* 0 - (getmpb)
		dc.l	bconstat		* 1 - bconstat
		dc.l	bconin			* 2 - bconin
		dc.l	bconout			* 3 - bconout
		dc.l	nullrts			* 4 - (rwabs)
		dc.l	setexec			* 5 - setexec
		dc.l	nullrts			* 6 - (tickcal)
		dc.l	nullrts			* 7 - (getbpb)
		dc.l	bcostat			* 8 - bcostat
		dc.l	nullrts			* 9 - (mediach)
		dc.l	nullrts			* 10 - (drvmap)
		dc.l	nullrts			* 11 - (shift)
*
		.page
*
* t14tab -- Trap-14 pointer table -- Extended BIOS functions (Buchla 700 only)
* ------    ------------------------------------------------------------------
t14tab:		dc.w	10			* Number of trap14 routines
*
		dc.l	piorec			* 0 - Get iorec address
		dc.l	setport			* 1 - Set ACIA parameters
		dc.l	nullrts			* 2 - (Read floppy)
		dc.l	nullrts			* 3 - (Write floppy)
		dc.l	nullrts			* 4 - (Format floppy)
		dc.l	nullrts			* 5 - (Verify floppy)
		dc.l	nullrts			* 6 - (Create prototype boot sec.)
		dc.l	nullrts			* 7 - (Generate random number)
		dc.l	nullrts			* 8 - (Get analog input)
		dc.l	nullrts			* 9 - (Clear analog FIFO)
*
		.endc
*
		.page
*
* cdt01 -- Character device dispatch table #1 -- input status
* -----    --------------------------------------------------
cdt01:		dc.l	sr1ist			* 0 - PRT -- Printer
		dc.l	sr2ist			* 1 - AUX -- Serial-2
		dc.l	sr2ist			* 2 - CON -- Console
*
		.ifne	BUCHLA
*
		dc.l	mc1ist			* 3 - MC1 -- MIDI-1
		dc.l	mc2ist			* 4 - MC2 -- MIDI-2
*
		.endc
*
* cdt02 -- Character device dispatch table #2 -- input
* -----    -------------------------------------------
cdt02:		dc.l	sr1inp			* 0 - PRT -- Printer
		dc.l	sr2inp			* 1 - AUX -- Serial-2
		dc.l	sr2inp			* 2 - CON -- Console
*
		.ifne	BUCHLA
*
		dc.l	mc1inp			* 3 - MC1 -- MIDI-1
		dc.l	mc2inp			* 4 - MC2 -- MIDI-2
*
		.endc
*
* cdt03 -- Character device dispatch table #3 -- output
* -----    --------------------------------------------
cdt03:		dc.l	sr1out			* 0 - PRT -- Printer
		dc.l	sr2out			* 1 - AUX -- Serial-2
		dc.l	sr2out			* 2 - CON -- Console
		.ifne	BUCHLA
*
		dc.l	mc1out			* 3 - MC1 -- MIDI-1
		dc.l	mc2out			* 4 - MC2 -- MIDI-2
*
		.endc
*
* cdt04 -- Character device dispatch table #4 -- output status
* -----    ---------------------------------------------------
cdt04:		dc.l	sr1ost			* 0 - PRT -- Printer
		dc.l	sr2ost			* 1 - AUX -- Serial-2
		dc.l	sr2ost			* 2 - CON -- Console
		.ifne	BUCHLA
*
		dc.l	mc1ost			* 3 - MC1 -- MIDI-1
		dc.l	mc2ost			* 4 - MC2 -- MIDI-2
*
		.endc
*
		.page
*
* iorec defaults:
* ---------------
* Serial-1:
* ---------
sr1dflt:	dc.l	sr1ibuf
		dc.w	SR1IBS
		dc.w	0,0
		dc.w	SR1IBS/4
		dc.w	SR1IBS-(SR1IBS/4)
		dc.l	sr1obuf
		dc.w	SR1OBS
		dc.w	0,0
		dc.w	SR1OBS/4
		dc.w	SR1OBS-(SR1OBS/4)
		dc.b	SR1CFR0,SR1CFR1
		dc.b	0,0
		dc.b	SR1DISC,0
		dc.b	0,0
		dc.w	0,0
*
* Serial-2:
* ---------
sr2dflt:	dc.l	sr2ibuf
		dc.w	SR2IBS
		dc.w	0,0
		dc.w	SR2IBS/4
		dc.w	SR2IBS-(SR2IBS/4)
		dc.l	sr2obuf
		dc.w	SR2OBS
		dc.w	0,0
		dc.w	SR2OBS/4
		dc.w	SR2OBS-(SR2OBS/4)
		dc.b	SR2CFR0,SR2CFR1
		dc.b	0,0
		dc.b	SR2DISC,0
		dc.b	0,0
		dc.w	0,0
*
		.page
*
		.ifne	BUCHLA
*
* MIDI-1:
* -------
mc1dflt:	dc.l	mc1ibuf
		dc.w	MC1IBS
		dc.w	0,0
		dc.w	MC1IBS/4
		dc.w	MC1IBS-(MC1IBS/4)
		dc.l	mc1obuf
		dc.w	MC1OBS
		dc.w	0,0
		dc.w	MC1OBS/4
		dc.w	MC1OBS-(MC1OBS/4)
		dc.b	MC1CFR0,MC1CFR1
		dc.b	0,0
		dc.b	MC1DISC,0
		dc.b	0,0
		dc.w	0,0
*
* MIDI-2:
* -------
mc2dflt:	dc.l	mc2ibuf
		dc.w	MC2IBS
		dc.w	0,0
		dc.w	MC2IBS/4
		dc.w	MC2IBS-(MC2IBS/4)
		dc.l	mc2obuf
		dc.w	MC2OBS
		dc.w	0,0
		dc.w	MC2OBS/4
		dc.w	MC2OBS-(MC2OBS/4)
		dc.b	MC2CFR0,MC2CFR1
		dc.b	0,0
		dc.b	MC2DISC,0
		dc.b	0,0
		dc.w	0,0
*
		.endc
*
		.page
*
* brtable -- Baud rate setup table
* -------    ---------------------
brtable:	dc.b	BR_19K2		* 0 - 19200 baud
		dc.b	BR_9600		* 1 -  9600 baud
		dc.b	BR_4800		* 2 -  4800 baud
		dc.b	BR_3600		* 3 -  3600 baud
		dc.b	BR_2400		* 4 -  2400 baud
		dc.b	BR_7200		* 5 -  7200 baud  (2000 on Atari)
		dc.b	BR_1800		* 6 -  1800 baud
		dc.b	BR_1200		* 7 -  1200 baud
		dc.b	BR_600		* 8 -   600 baud
		dc.b	BR_300		* 9 -   300 baud
*
* iortab -- iorec pointer table
* ------    -------------------
iortab:		dc.l	sr1iorec	* 0 - PRT Serial-1 Printer
		dc.l	sr2iorec	* 1 - AUX Serial-2 Host
		dc.l	sr2iorec	* 2 - CON Serial-2 Console
*
		.ifne	BUCHLA
*
		dc.l	mc1iorec	* 3 - MC1 MIDI-1   MIDI port #1
		dc.l	mc2iorec	* 4 - MC2 MIDI-2   MIDI port #2
*
		.endc
*
* aciatab -- iorec and ACIA pointer table
* -------    ----------------------------
aciatab:	dc.l	sr1iorec,SR1ACIA	* 0 - PRT Serial-1
		dc.l	sr2iorec,SR2ACIA	* 1 - AUX Serial-2
		dc.l	sr2iorec,SR2ACIA	* 2 - CON Serial-2
*
		.ifne	BUCHLA
*
		dc.l	mc1iorec,MC1ACIA	* 3 - MC1 MIDI-1
		dc.l	mc2iorec,MC2ACIA	* 4 - MC2 MIDI-2
*
		.endc
*
		.page
*
		.ifne	BUCHLA
*
* pbpbtab -- Prototype BPB data table  (BPS..NHID)  19 bytes per entry
* -------    ---------------------------------------------------------
*
* 0 -- 40 tracks, single sided (180K)
*
pbpbtab:	dc.b	$00,$02,$01,$01,$00,$02,$40,$00
		dc.b	$68,$01,$FC,$02,$00,$09,$00,$01
		dc.b	$00,$00,$00
*
* 1 -- 40 tracks, double sided (360K)
*
		dc.b	$00,$02,$02,$01,$00,$02,$70,$00
		dc.b	$D0,$02,$FD,$02,$00,$09,$00,$02
		dc.b	$00,$00,$00
*
* 2 -- 80 tracks, single sided (360K)
*
		dc.b	$00,$02,$02,$01,$00,$02,$70,$00
		dc.b	$D0,$02,$F8,$05,$00,$09,$00,$01
		dc.b	$00,$00,$00
*
* 3 -- 80 tracks, double sided (720K)
*
		dc.b	$00,$02,$02,$01,$00,$02,$70,$00
		dc.b	$A0,$05,$F9,$05,$00,$09,$00,$02
		dc.b	$00,$00,$00
*
* VSDD initialization table
*
		.even
*
vsddtab:	dc.w	$8252		* R0	Mode word 0
		dc.w	$E474		* R1	Mode word 1
		dc.w	$0006		* R2	Register window base
		dc.w	$0100		* R3	Data window base
		dc.w	$0000		* R4	Data length mask
		dc.w	$0000		* R5	Data segment base
		dc.w	$0001		* R6	Priority access count
		dc.w	$0040		* R7	ODT base
		dc.w	$0080		* R8	AT base
		dc.w	$0010		* R9	CLT base
		dc.w	$0011		* R10	CG bases
		dc.w	$0000		* R11	AT counter  (R/O)
*
		dc.w	$0C08		* R12	HC0 =  3, VC0 =   8
		dc.w	$140A		* R13	HC1 =  5, VC1 =  10
		dc.w	$9568		* R14	HC2 = 37, VC2 = 360
		dc.w	$A16A		* R15	HC3 = 40, VC3 = 362
*
vsddit01:	dc.w	$8253		* R0 with UCF = 1, DEN = 0
vsddit02:	dc.w	$825B		* R0 with UCF = 1, DEN = 1
*
		.page
*
* analog processor input state table
*
api_tab:	dc.b	0,1,1,1,1,1,1,1,1,1	* 0..9
		dc.b	1,1,1,1,1,1,1,1,1,1	* 10..19
		dc.b	1,1,1,1,1		* 20..24
		dc.b	3,3,3,3,3		* 25..29
		dc.b	3,3,3,3,3,3,3,3,3	* 30..38
		dc.b	5			* 39
		dc.b	5,5,5,5,5,5,5,5,5,5	* 40..49
		dc.b	5,5,5			* 50..52
		dc.b	3,3,3,3,3,3,3		* 53..59
		dc.b	5,5,5,5,5,5,5,5,5,5	* 60..69
		dc.b	5,5,5			* 70..72
		dc.b	6,6,6,6,6,6,6		* 73..79
		dc.b	6,6,6			* 80..82
*
		.even
*
* analog processor state transfer vector
*
api_tv:		dc.l	api_s0,api_s1,api_s2,api_s3
		dc.l	api_s4,api_s5,api_s6
*
		.endc
*
		.page
*************************************************************************
* 									*
* 			RAM storage definitions				*
*			-----------------------				*
*************************************************************************
*
		.bss
		.even
*
* RAM data area
* -------------
* WARNING:
* --------
* The variables from timevec through rsarea are permanently assigned so
* that we can link to certain bios variables without having to link to the
* bios object file itself.  Some of these get defined in hwdefs.s and in
* hwdefs.s so beware of changing them.
*
timevec:	ds.l	1		* System timer trap vector
critvec:	ds.l	1		* Critical error handler vector
termvec:	ds.l	1		* Process terminate hook vector
resvec3:	ds.l	1		* Reserved vector 3
resvec4:	ds.l	1		* Reserved vector 4
resvec5:	ds.l	1		* Reserved vector 5
resvec6:	ds.l	1		* Reserved vector 6
resvec7:	ds.l	1		* Reserved vector 7
*
fc_sw:		ds.w	1		* Frame clock switch (<0=dn, 0=off, >0=up)
fc_val:		ds.l	1		* Frame clock value  (0..FCMAX)
*
_wzcrsh:	ds.w	1		* Crash area: flag for ROMP
_crshsr:	ds.w	1		* Crash area: SR
_crshpc:	ds.l	1		* Crash area: PC
_crshsp:	ds.l	1		* Crash area: SP
_crshus:	ds.l	1		* Crash area: USP
_crshvc:	ds.l	1		* Crash area: vector # in MS byte
_crshrg:	ds.l	16		* Crash area: registers
_crshst:	ds.w	16		* Crash area: top 16 words of stack
*
* The area from biosram to SSTACK-1 is cleared on reset.
* ------------------------------------------------------
biosram		.equ	*		* Start of BIOS RAM variables
*
		.ifne	BUCHLA
*
_hz_1k:		ds.l	1		* 1000 Hz clock
_hz_200:	ds.l	1		* 200 Hz clock
frclock:	ds.l	1		* 50 Hz clock
t1count:	ds.w	1		* Timer 1 count
t2count:	ds.w	1		* Timer 2 count
t3count:	ds.w	1		* Timer 3 count
*
seekrate:	ds.w	1		* Seek rate
cdev:		ds.w	1		* Current drive
ctrack:		ds.w	1		* Current track
csect:		ds.w	1		* Current sector
cside:		ds.w	1		* Current side
ccount:		ds.w	1		* Current sector count
spt:		ds.w	1		* Sectors per track
interlv:	ds.w	1		* Sector interleave count
virgin:		ds.w	1		* Initial formatting data
deferr:		ds.w	1		* Default error number
curerr:		ds.w	1		* Current error number
*
cdma:		ds.l	1		* Current DMA address
edma:		ds.l	1		* Ending DMA address
tmpdma:		ds.l	1		* Temporary DMA address
*
		.endc
*
rseed:		ds.l	1		* Random number seed
*
savptr:		ds.l	1		* Pointer to register save area
*
_rsflag:	ds.l	1		* Register save area overflow flag
		ds.l	18*32		* Register save area  (32 levels)
rsarea:		ds.l	1		* Dummy long word at top of save area
*
* ***** end of the permanently assigned bios variables *****
*
		.page
*
		.ifne	BUCHLA
*
acctim:		ds.l	2		* Accumulated disk time table
maxactim:	ds.l	1		* Maximum acctim value
hdv_init:	ds.l	1		* Disk init vector
hdv_bpb:	ds.l	1		* Disk get bpb vector
hdv_rw:		ds.l	1		* Disk r/w vector
hdv_boot:	ds.l	1		* Disk boot vector
hdv_mchg:	ds.l	1		* Disk media change vector
drvbits:	ds.l	1		* Drive map bits
dskbufp:	ds.l	1		* Disk buffer pointer
		.page
nflops:		ds.w	1		* Number of drives
disknum:	ds.w	1		* Current disk number
booted:		ds.w	1		* Most recent boot device or -1
flock:		ds.w	1		* Floppy semaphore
fverify:	ds.w	1		* Floppy verify flag
*
tdiv1:		ds.w	1		* Timer divider 1  (divides _hz_1k)
tdiv2:		ds.w	1		* Timer divider 2  (divides _hz_200)
*
retrycnt:	ds.w	1		* Re-try count
wpstatus:	ds.w	1		* Write protect status table
wplatch:	ds.w	1		* Write protect latch table
bootdev:	ds.w	1		* Boot device number
*
motoron:	ds.w	1		* Motor-on flag
deslflag:	ds.w	1		* Drive deselect flag
*
		.endc
*
flpsrsv:	ds.w	1		* Status register save area
flpregs:	ds.l	16		* Register save area
*
		.ifne	BUCHLA
*
dsb0:		ds.l	1		* Drive A DSB
dsb1:		ds.l	1		* Drive B DSB
*
dskmode:	ds.b	2		* Disk change mode table
dskerrs:	ds.w	2		* Disk error code table
drvbpbs:	ds.w	16*2		* Disk BPB save area
*
		.endc
*
		.page
*
sr1iorec:	ds.b	IORECLN		* Serial-1 iorec structure
sr2iorec:	ds.b	IORECLN		* Serial-2 iorec structure
*
		.ifne	BUCHLA
*
mc1iorec:	ds.b	IORECLN		* MIDI-1 iorec structure
mc2iorec:	ds.b	IORECLN		* MIDI-2 iorec structure
*
		.endc
*
sr1ibuf:	ds.b	SR1IBS		* Serial-1 input buffer
sr1obuf:	ds.b	SR1OBS		* Serial-1 output buffer
sr2ibuf:	ds.b	SR2IBS		* Serial-2 input buffer
sr2obuf:	ds.b	SR2OBS		* Serial-2 output buffer
*
		.ifne	BUCHLA
*
mc1ibuf:	ds.b	MC1IBS		* MIDI-1 input buffer
mc1obuf:	ds.b	MC1OBS		* MIDI-1 output buffer
mc2ibuf:	ds.b	MC2IBS		* MIDI-2 input buffer
mc2obuf:	ds.b	MC2OBS		* MIDI-2 output buffer
*
api_inp:	ds.b	1		* Analog processor input byte
api_bug:	ds.b	1		* Analog processor signal # "bug trap"
api_sv:		ds.b	1		* Analog processor state
api_svb:	ds.b	1		* Analog processor state "bug trap"
*
		.even
*
api_sig:	ds.b	1		* Analog signal number
api_val:	ds.b	1		* Analog value
*
api_fi:		ds.l	1		* Analog processor FIFO input pointer
api_fo:		ds.l	1		* Analog processor FIFO output pointer
api_fum:	ds.w	APISIZE		* Analog processor FIFO
api_fe		.equ	*		* End of analog processor FIFO
*
		.endc
*
		.even
*
basepage:	ds.l	64		* Pseudo base page
*
buffer:		ds.b	1024		* Default disk buffer
*
biostop		.equ	*		* End of BIOS RAM
*
		.end
