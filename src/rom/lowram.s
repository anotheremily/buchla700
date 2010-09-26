* ------------------------------------------------------------------------------
* lowram.s -- create a GEMDOS compatible basepage for the Buchla 700
* Version 10 -- 1988-03-28 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
* Provides a GEMDOS compatible basepage and calls start_ to
* get things rolling for applications on the Buchla 700.
*
* Also sets the stack pointer to the high end of RAM.
*
* WARNING:  This code ONLY works for programs loaded into RAM.
* ------------------------------------------------------------------------------
* We expect the following to be the relative layout of memory:
*
*	lowram		load address  (this chunk of code)
*	 ...
*	p_lowtpa	start of 'TPA' / basepage address
*	 ...
*	start_		first module  (starts 'text' area)
*	 ...
*	 ...			-text-
*	 ...
*	_etext		end of 'text' area + 1
*	basedat		start of 'data' area  (will usually be _etext)
*	 ...
*	 ...			-data-
*	 ...
*	_edata		end of 'data' area + 1
*	basebss		start of 'bss' area   (will usually be _edata)
*	 ...
*	 ...			-BSS-
*	 ...
*	_end		end of actual 'bss' area + 1
*	 ...
*	 ...			-heap-
*	 ...
*	HI_RAM-$400	end of pseudo 'bss' area  (cleared by fsmain.s)
*	 ...
*	 ...			-stack margin-
*	 ...
*	HI_RAM		end of RAM + 1,  initial stack address
* ------------------------------------------------------------------------------
* The startup code in fsmain.s uses the bss length to know how much RAM
* to clear.  We give it HI_RAM-$400-basebss, and put the stack at HI_RAM.
* The startup code clears from p_bbase through p_bbase+p_blen-1, inclusive.
* This keeps fsmain.s from wiping itself out when it zaps the bss area.
*
* The startup code (start_) in fsmain.s is entered with the address of
* the basepage passed as a long word parameter on the top of the stack.
* ------------------------------------------------------------------------------
		.text
*
		.page
*
* Globals defined here:
*
		.xdef	LOWRAM
		.xdef	_Lo_RAM
		.xdef	_Hi_RAM
*
		.xdef	basebss
		.xdef	basedat
		.xdef	p_lowtpa
		.xdef	p_env
*
		.xdef	_p_tlen
		.xdef	_p_dlen
*
* Globals referred to here:
*
		.xref	start_			* entry point in fsmain.s
		.xref	_panic			* entry point in fsmain.s
*
		.xref	_edata			* supplied by loader
		.xref	_etext			* supplied by loader
*
* ------------------------------------------------------------------------------
* Memory setup:
* ------------
* The equate for HI_RAM must be set to match the memory configuration used.
*
* 	For a  512K system, _Hi_RAM = $080000
* 	For a 1024K system, _Hi_RAM = $100000
*
_Hi_RAM		.equ	$100000		* highest RAM address + 1
*
		.page
*
* LOWRAM -- Startup code for Buchla 700 application programs
* ------    ------------------------------------------------
_Lo_RAM:
LOWRAM:		lea	basebss,a0		* setup bss base
		move.l	a0,p_bbase		* ...
		lea	_Hi_RAM-$400,a1		* setup faked bss length
		suba.l	a0,a1			* ...
		move.l	a1,p_blen		* ...
*
		lea	basedat,a0		* setup data base
		move.l	a0,p_dbase		* ...
		lea	_edata,a1		* setup data length
		suba.l	a0,a1			* ...
		move.l	a1,p_dlen		* ...
*
		lea	start_,a0		* setup text base
		move.l	a0,p_tbase		* ...
		lea	_etext,a1		* setup text length
		suba.l	a0,a1			* ...
		move.l	a1,p_tlen		* ...
*
		lea	_Hi_RAM,a7		* setup stack pointer
*
		lea	p_lowtpa,a0		* setup TPA base
		move.l	a0,p_lowtpa		* ...
		move.l	a7,p_hitpa		* setup high TPA
*
		move.l	#p_lowtpa,-(a7)		* Pass basepage address
		jsr	start_			* Start things going
		addq.l	#4,a7			* Clean up stack
*
		move.l	#pmsg,-(a7)		* OOPS:  panic()
		jsr	_panic			* ... shouldn't ever return
		addq.l	#4,a7
*
hstop:		stop	#$2000			* "Die, sucker!"
		bra	hstop
*
		.page
*
* The mess below is the basepage.  It defines the start of the pseudo 'TPA'.
* This MUST live in the 'text' segment for things to work.
*
p_lowtpa:	dc.l	0			* low address of TPA
p_hitpa:	dc.l	0			* high address + 1 of TPA
p_tbase:	dc.l	0			* low address of text segment
p_tlen:		dc.l	0			* length of text segment
p_dbase:	dc.l	0			* low address of data segment
p_dlen:		dc.l	0			* length of data segment
p_bbase:	dc.l	0			* low address of BSS segment
p_blen:		dc.l	0			* length of BSS segment
p_dta:		dc.l	0			* pointer to DTA
p_parent:	dc.l	0			* pointet to parent basepage
		dc.l	0
p_env:		dc.l	0			* pointer to environment string
		ds.l	20
p_cmdlin:	dc.b	0			* command line tail image
		ds.b	127
*
BPLEN		.equ	*-p_lowtpa		* MUST be $0100 (or we goofed)
*
_p_tlen		.equ	p_tlen			* text length for MIDAS-VII
_p_dlen		.equ	p_dlen			* data length for MIDAS-VII
*
* ------------------------------------------------------------------------------
		.data
* ------------------------------------------------------------------------------
		.even
*
basedat:	dc.b	'***** data *****'		* start of data
*
pmsg:		dc.b	'returned from start_()',0	* panic() message
*
* ------------------------------------------------------------------------------
		.bss
* ------------------------------------------------------------------------------
		.even
*
basebss:	ds.l	1				* start of BSS
*
		.end
