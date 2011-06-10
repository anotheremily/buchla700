/*
   ============================================================================
	regs.h -- register structure for Buchla 700 ROMP debug facility
	Version 6 -- 1987-06-15 -- D.N. Lynx Crowe
	(c) Copyright 1987 -- D.N. Lynx Crowe

	Defines the registers saved on the stack when a breakpoint occurs.
   ============================================================================
*/

struct regs
{

  long d_reg[8];		/* Data registers d0..d7 */

  char *a_reg[8];		/* Address registers */

  unsigned short reg_fill,	/* Filler to keep long alignment */
    reg_sr,			/* Status register */
   *reg_pc;			/* Program counter */
};
