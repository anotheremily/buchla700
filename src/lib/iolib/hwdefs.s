* ------------------------------------------------------------------------------
* hwdefs.s --  External definitions of Buchla 700 I/O addresses
* Version 14 -- 1988-08-15 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
		.text
*
		.xdef	_io_time,_io_lcd,_io_ser,_io_midi
		.xdef	_io_disk,_io_tone,_io_leds,_io_kbrd
		.xdef	_io_vreg,_io_vraw,_io_vram,_io_fpu
		.xdef	_lcd_a0,_lcd_a1
*
		.xdef	_v_regs,_v_odtab,_v_actab
		.xdef	_v_ct0
		.xdef	_v_gt1
		.xdef	_v_score,_v_cgtab
*
		.xdef	_v_curs0,_v_curs1,_v_curs2,_v_curs3
		.xdef	_v_curs4,_v_curs5,_v_curs6,_v_curs7
		.xdef	_v_kbobj,_v_lnobj,_v_tcur
		.xdef	_v_win0
		.xdef	_v_cur
*
		.xdef	_fc_sw,_fc_val
*
* ------------------------------------------------------------------------------
*
* Hardware base addresses
* -----------------------
_io_fpu		.equ	$180000		* FPU base address
*
VB		.equ	$200000		* VSDD base address
*
_io_time	.equ	$3A0001		* Timer chip
_io_lcd		.equ	$3A4001		* LCD controller
_io_ser		.equ	$3A8001		* Serial ports  (RS232)
_io_midi	.equ	$3AC001		* MIDI ports
_io_disk	.equ	$3B0001		* Disk controller
_io_tone	.equ	$3B4001		* Sound generator chip
_io_leds	.equ	$3B8001		* LED driver
_io_kbrd	.equ	$3BC001		* Keyboard / panel processor
*
_lcd_a0		.equ	_io_lcd		* LCD port a0
_lcd_a1		.equ	_io_lcd+2	* LCD port a1
*
		.page
*
* Video definitions
* -----------------
_io_vreg	.equ	VB		* Relocated video registers after setup
_io_vraw	.equ	VB+$400		* Raw video registers at RESET
_io_vram	.equ	VB		* Video RAM base address
*
* Name			Offset		  Usage			       Bank
* -------		---------	  -------------------------    ----
_v_regs		.equ	VB		* Video registers	       0,1
*
_v_odtab	.equ	VB+128		* Object Descriptor Table	0
_v_actab	.equ	VB+256		* Access Table			0
_v_ct0		.equ	VB+1024		* Character Text-0		0
_v_gt1		.equ	VB+1304		* Graphics Text-1		0
_v_score	.equ	VB+8192		* Score object			0
_v_cgtab	.equ	VB+122880	* Character Generator Table	0
*
_v_curs0	.equ	VB+1024		* Cursor object 0  (arrow ULE)	1
_v_curs1	.equ	VB+1152		* Cursor object 1  (arrow ULO)	1
_v_curs2	.equ	VB+1280		* Cursor object 2  (arrow URE)	1
_v_curs3	.equ	VB+1408		* Cursor object 3  (arrow URO)	1
_v_curs4	.equ	VB+1536		* Cursor object 4  (arrow LLE)	1
_v_curs5	.equ	VB+1664		* Cursor object 5  (arrow LLO)	1
_v_curs6	.equ	VB+1792		* Cursor object 6  (arrow LRE)	1
_v_curs7	.equ	VB+1920		* Cursor object 7  (arrow LRO)	1
_v_tcur		.equ	VB+2048		* Typewriter cursor		1
_v_kbobj	.equ	VB+2880		* Keyboard object		1
_v_lnobj	.equ	VB+4672		* Line object			1
_v_cur		.equ	VB+6464		* Underline cursor		1
_v_win0		.equ	VB+16384	* Window-0 object		1
*
* BIOS RAM definitions
* --------------------
* WARNING: the following addresses must match those of the corresponding
* variables defined in bios.s or chaos is guaranteed.
*
_fc_sw		.equ	$420		* word - Frame counter switch
_fc_val		.equ	$422		* long - Frame counter value
*
		.end
