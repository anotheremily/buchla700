/*
   =============================================================================
	vhinit.c -- initialize VSDD hardware
	Version 17 -- 1988-03-20 -- D.N. Lynx Crowe
	(c) Copyright 1987, 1988 -- D.N. Lynx Crowe

	VHinit()

		Setup VSDD registers, clear access table, clear object table,
		load character generator, set color lookup table defaults.
		Defines major video system variables, since the linker isn't
		smart enough to let me put them in a separate file.
   =============================================================================
*/

#define	FASTCHIP	1	/* non-zero if it's the fast VSDD chip */

#include "hwdefs.h"
#include "vsdd.h"
#include "memory.h"

#define	VREG(h,v)	((h<<10)|v)

extern	int	cgtable[];
extern	int	cg_rows;

struct octent	v_obtab[16];	/* object control table */

struct octent	*v_curob;	/* current v_obtab pointer */

int	v_nobj;			/* current object number */
int	v_obpri;		/* current object priority */

/*  */

/* initialized variables */

int	vr_data[] = {

	0x825B,		/* R0  -- Mode word 0 */
	0xC474,		/* R1  -- Mode word 1 */
	0x0006,		/* R2  -- Register window base, Control flags */
	0x0100,		/* R3  -- Data window base, X limit	(0x200000) */
	0x0000,		/* R4  -- Data length mask		(128K) */
	0x0000,		/* R5  -- Data segment base		(0x000000) */
	0x0001,		/* R6  -- Priority access count		(1) */
	0x0040,		/* R7  -- Object Descriptor Table base	(0x200080) */
	0x0080,		/* R8  -- Access Table base		(0x200100) */
	0x0010,		/* R9  -- Color Lookup Table base	(0x200020) */
	0x00FF,		/* R10 -- Character Generator bases	(0x21E000) */
	0x0000,		/* R11 -- Access Table address counter */

#if	FASTCHIP
	VREG( 3,   8),	/* R12 -- HC0 (HSYNC width)	VC0 (VSYNC width) */
	VREG( 5,  10),	/* R13 -- HC1 (AHZ start)	VC1 (AVZ start) */
	VREG(37, 360),	/* R14 -- HC2 (AHZ stop)	VC2 (AVZ stop) */
	VREG(40, 362)	/* R15 -- HC3 (HOR sweep)	VC3 (VRT sweep) */
#else
	VREG( 3,   8),	/* R12 -- HC0 (HSYNC width)	VC0 (VSYNC width) */
	VREG( 6,  10),	/* R13 -- HC1 (AHZ start)	VC1 (AVZ start) */
	VREG(38, 360),	/* R14 -- HC2 (AHZ stop)	VC2 (AVZ stop) */
	VREG(43, 361)	/* R15 -- HC3 (HOR sweep)	VC3 (VRT sweep) */
#endif
};

/*  */

/*
   =============================================================================
	VHinit() -- initialize the VSDD
   =============================================================================
*/

VHinit()
{

	/* select VSDD RAM bank 0 so we can access the control tables */

	vbank(0);

	/* set the video register values */

	memcpyw(v_regs, vr_data, 16);

	/* clear the access table to turn off all objects */

	memsetw(v_actab, 0xFFFF, 350);

	/* clear the object table */

	memsetw(v_odtab, 0, 64);

	/* move the character generator table to VSDD RAM */

	memsetw(v_cgtab, 0, 4096);
	memcpyw(v_cgtab, cgtable, 256 * cg_rows);
}
