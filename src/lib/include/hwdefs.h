/*
   ============================================================================
	hwdefs.h -- Buchla 700 I/O addresses defined in hwdefs.s
	Version 12 -- 1988-08-15 -- D.N. Lynx Crowe
   ============================================================================
*/

/* hardware addresses */

extern	char	io_time[], io_lcd, io_ser, io_midi;
extern	char	io_disk, io_tone, io_leds, io_kbrd;
extern	char	lcd_a0, lcd_a1;

extern	unsigned	io_vreg[], io_vraw[], io_vram[], io_fpu[];


/* video memory allocations */

extern	unsigned	v_regs[], v_odtab[][4], v_actab[];
extern	unsigned	v_ct0[], v_gt1[], v_score[], v_cgtab[];

extern	unsigned	v_curs0[], v_curs1[], v_curs2[], v_curs3[];
extern	unsigned	v_curs4[], v_curs5[], v_curs6[], v_curs7[];
extern	unsigned	v_tcur[], v_kbobj[], v_lnobj[];
extern	unsigned	v_win0[], v_cur[];


/* stuff in the depths of the bios */

extern	short	fc_sw;

extern	long	fc_val;
