* ------------------------------------------------------------------------------
* serintr.s -- MIDAS-VII serial I/O subroutines
* Version 4 -- 1988-12-13 -- D.N. Lynx Crowe
*
* These subroutines replace those in bios.s in order to add support for
* MIDAS-VII foot pedal and pulse inputs, and pulse outputs.
*
* WARNING:  The code below uses addresses in the bios RAM area.  These addresses
* correspond to those in PROMS dated 1988-06-20 et seq.  If the bios is changed,
* the addresses marked with <== may have to be changed.
* ------------------------------------------------------------------------------
*
		.text
*
		.xdef	_setsio
*
		.xdef	_foot1
		.xdef	_foot2
		.xdef	_pulse1
		.xdef	_pulse2
*
		.xdef	serintr
*
		.xdef	serint
		.xdef	midint
*
		.xdef	wrapin
		.xdef	wrapout
*
		.xdef	serput
		.xdef	midput
*
		.xdef	rtschk
*
		.xdef	rtson
		.xdef	rtsoff
*
* ==============================================================================
*
* The following addresses, marked by <==, are bios version dependent:
*
RAM		.equ	$00000400	* Beginning of system RAM area	<==
*
SR1IOREC	.equ	RAM+$0AB0	* Serial-1 iorec structure	<==
SR2IOREC	.equ	RAM+$0AD8	* Serial-2 iorec structure	<==
MC1IOREC	.equ	RAM+$0B00	* MIDI-1 iorec structure	<==
MC2IOREC	.equ	RAM+$0B28	* MIDI-2 iorec structure	<==
*
* End of bios version dependent addresses.
*
* ==============================================================================
*
		.page
*
SERVECT		.equ	$000074		* Level 5 interrupt autovector address
*
IPL7		.equ	$0700		* IPL 7 value for sr
*
* ACIA I/O Addresses:
* -------------------
SR1ACIA		.equ	$3A8001		* Serial-1 ACIA base address
SR2ACIA		.equ	$3A8009		* Serial-2 ACIA base address
MC1ACIA		.equ	$3AC001		* MIDI-1 ACIA base address
MC2ACIA		.equ	$3AC009		* MIDI-2 ACIA base address
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
* serintr -- Serial (Serial-1, Serial-2, MIDI-1, MIDI-2) interrupt handler
* -------    -------------------------------------------------------------
serintr:	movem.l	d0-d3/a0-a2,-(a7)	* Save registers
		lea	SR1IOREC,a0		* Point at Serial-1 iorec
		lea	SR1ACIA,a1		* Point at Serial-1 ACIA
		movea.l	_foot1,a2		* Point at foot sw. 1 processor
		bsr	serint			* Go process (possible) int.
*
		lea	SR2IOREC,a0		* Point at Serial-2 iorec
		lea	SR2ACIA,a1		* Point at Serial-2 ACIA
		movea.l	_foot2,a2		* Point at foot sw. 2 processor
		bsr	serint			* Go process (possible) int.
*
		lea	MC1IOREC,a0		* Point at MIDI-1 iorec
		lea	MC1ACIA,a1		* Point at MIDI-1 ACIA
		movea.l	_pulse1,a2		* Point at pulse 1 processor
		bsr	midint			* Go process (possible) int.
*
		lea	MC2IOREC,a0		* Point at MIDI-2 iorec
		lea	MC2ACIA,a1		* Point at MIDI-2 ACIA
		movea.l	_pulse2,a2		* Point at pulse 2 processor
		bsr	midint			* Go process (possible) int.
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
serintx:	btst.b	#4,isr(a0)		* DCD interrupt ?
		bne	calldcd			* Jump if so
*
serdone:	rts				* Return to caller
*
calldcd:	move.b	csr(a0),d0		* Get CSR interrupt status
		btst.l	#4,d0			* Check DCD input  (0 = active)
		bne	calldcd0		* Jump if line was inactive
*
		moveq.l	#1,d0			* Set footswitch status to TRUE
		bra	calldcd1		* ...
*
calldcd0:	moveq.l	#0,d0			* Set footswitch status to FALSE
*
calldcd1:	move.w	d0,-(a7)		* Call the footswitch processor
		jsr	(a2)			* ... (*footX)(status)
		tst.w	(a7)+			* ...
		rts				* Return to caller
*
		.page
*
* Handle serial I/O port error
*
sererr:		addq.w	#1,errct(a0)		* Update error count
		move.b	ACIA_RDR(a1),erbyte(a0)	* Get error byte
		rts				* Return to caller
*
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
		bsr	wrapout			* Adjust pointer for wraparound
		move.l	obuf(a0),a2		* Get buffer base in a2
		move.b	0(a2,d2),ACIA_TDR(a1)	* Send byte on its way
		move.w	d2,obufhd(a0)		* Save updated head index
		bra	serintx			* Done
*
		.page
*
* Handle receiver interrupt
*
serrx:		btst.b	#1,linedisc(a0)		* RTS/CTS mode set ?
		beq	serrx1			* Jump if not
*
		bsr	rtsoff			* Turn off RTS
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
		bsr	wrapin			* Adjust for wraparound
		cmp.w	ibufhd(a0),d1		* Head = tail ?
		beq	seribf			* If so, we drop the character
*
		.page
		move.l	ibuf(a0),a2		* Get buffer address
		move.b	d0,0(a2,d1)		* Stash byte in buffer
		move.w	d1,ibuftl(a0)		* Save updated tail index
		move.w	ibuftl(a0),d2		* Tail index to d2
		move.w	ibufhd(a0),d3		* Head index to d3
		cmp.w	d3,d2			* Head > Tail ?
		bhi	rsi_1			* Jump if not
*
		add.w	ibufsize(a0),d2		* Add buffer size to tail index
*
rsi_1:		sub.w	d3,d2			* Length = (adjusted)Tail - Head
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
		bsr	serput			* ...
*
serrx4:		btst	#1,linedisc(a0)		* RTS/CTS mode set ?
		beq	sertxq			* Done if not
*
		bsr	rtson			* Turn on RTS
		bra	sertxq			* Done
*
		.page
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
midintx:	btst	#4,isr(a0)		* DCD interrupt ?
		bne	mididcd			* Jump if so
*
mididone:	rts				* Return to caller
*
mididcd:	jmp	(a2)			* Exit through the DCD processor
*
miderr:		addq.w	#1,errct(a0)		* Update error count
		move.b	ACIA_RDR(a1),erbyte(a0)	* Get error byte
		rts
*
* Handle receiver interrupt
*
midrx:		move.b	ACIA_RDR(a1),d0		* Read data from ACIA
		move.w	ibuftl(a0),d1		* Get tail index in d1
		bsr	wrapin			* Adjust for wraparound
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
		.page
*
* serput -- Output a character to a serial port
* ------    -----------------------------------
serput:		move.w	sr,-(a7)		* Save status register
		ori.w	#IPL7,sr		* DISABLE INTERRUPTS
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
		move.w	(a7)+,sr		* RESTORE INTERRUPTS
		andi	#$FFFE,sr		* Clear carry flag = OK
		rts				* Return to caller
*
serpt_4:	bsr	serchk			* Check status on our way out
		bsr	rtschk			* Handle RTS protocol
		move.w	(a7)+,sr		* RESTORE INTERRUPTS
		ori	#$0001,sr		* Set carry flag = buffer full
		rts				* Return to caller
*
		.page
*
* midput -- Output to MIDI
* ------    --------------
midput:		move.w	sr,-(a7)		* Save status register
		ori.w	#IPL7,sr		* DISABLE INTERRUPTS
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
		move.w	(a7)+,sr		* RESTORE INTERRUPTS
		andi	#$FFFE,sr		* Clear carry flag = OK
		rts				* Return to caller
*
midpt_4:	bsr	midchk			* Check status on our way out
		move.w	(a7)+,sr		* RESTORE INTERRUPTS
		ori	#$0001,sr		* Set carry flag = buffer full
		rts				* Return to caller
*
		.page
*
* rtschk -- Check RTS mode and turn on RTS if it's enabled
* ------    ----------------------------------------------
rtschk:		btst	#1,linedisc(a0)		* RTS/CTS mode set ?
		beq	rtsexit			* Jump to exit if not
*
* rtson -- Turn on RTS line
* -----    ----------------
rtson:		move.b	cfr1(a0),d0		* Pick up CFR1 image
		bclr	#0,d0			* Turn on RTS line  (0 = on)
		bra	rtscmn			* Join common RTS code below
*
* rtsoff -- Turn off RTS line
* ------    -----------------
rtsoff:		move.b	cfr1(a0),d0		* Pick up CFR1 image
		bset	#0,d0			* Turn off RTS line  (1 = off)

rtscmn:		bset	#7,d0			* Make sure MS bit is set
		move.b	d0,cfr1(a0)		* Update CFR1 image
		move.b	d0,ACIA_CFR(a1)		* Send CFR to hardware
*
rtsexit:	rts				* Return to caller
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
* _setsio -- setsio() -- initialize serial I/O vectors and DCD interrupts
* -------    ------------------------------------------------------------
_setsio:	move.w	sr,-(a7)		* Preserve status register
		ori.w	#$IPL7,sr		* DISABLE INTERRUPTS
*
		lea	nulsiox,a0		* Get null return address
		move.l	a0,_foot1		* Initialize foot1 vector
		move.l	a0,_foot2		* Initialize foot2 vector
		move.l	a0,_pulse1		* Initialize pulse1 vector
		move.l	a0,_pulse2		* Initialize pulse2 vector
*
		lea	SR1ACIA,a1		* Point at Serial-1 ACIA
		move.b	#$90,ACIA_IER(a1)	* Enable DCD interrupts
*
		lea	SR2ACIA,a1		* Point at Serial-2 ACIA
		move.b	#$90,ACIA_IER(a1)	* Enable DCD interrupts
*
		lea	MC1ACIA,a1		* Point at MIDI-1 ACIA
		move.b	#$90,ACIA_IER(a1)	* Enable DCD interrupts
*
		lea	MC2ACIA,a1		* Point at MIDI-2 ACIA
		move.b	#$90,ACIA_IER(a1)	* Enable DCD interrupts
*
		lea	serintr,a0		* Initialize interrupt vector
		move.l	a0,SERVECT		* ... in processor RAM
*
		move.w	(a7)+,sr		* RESTORE INTERRUPTS
*
nulsiox:	rts				* Return to caller
*
		.bss
*
* DCD interrupt processor vectors
* -------------------------------
_foot1:		.ds.l	1			* short (*foot1)();
_foot2:		.ds.l	1			* short (*foot2)();
_pulse1:	.ds.l	1			* short (*pulse1)();
_pulse2:	.ds.l	1			* short (*pulse2)();
*
		.end
