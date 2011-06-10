/*
   =============================================================================
	gcurpos.c -- MIDAS cursor arrow functions and arrow object definitions
	Version 16 -- 1988-03-11 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddvars.h"

#include "midas.h"

#define	YOFF	15		/* y pixel offset */
#define	XOFF	14		/* x pixel offset */

extern short sgcsw;

/* 4 bit pixel patterns */

#define	AW_0	0x0000
#define	AW_1	0xF000
#define	AW_2	0x0F00
#define	AW_3	0xFF00
#define	AW_4	0x00F0
#define	AW_5	0xF0F0
#define	AW_6	0x0FF0
#define	AW_7	0xFFF0
#define	AW_8	0x000F
#define	AW_9	0xF00F
#define	AW_A	0x0F0F
#define	AW_B	0xFF0F
#define	AW_C	0x00FF
#define	AW_D	0xF0FF
#define	AW_E	0x0FFF
#define	AW_F	0xFFFF

extern int objclr (), SetPri (), vbank ();

extern unsigned exp_c ();

extern short stcrow, stccol;

/* 
*/

static int ar_ule[] = {		/* upper left even pixel arrow */

  AW_F, AW_0, AW_0, AW_0,	/* 0 */
  AW_C, AW_0, AW_0, AW_0,	/* 1 */
  AW_A, AW_0, AW_0, AW_0,	/* 2 */
  AW_9, AW_0, AW_0, AW_0,	/* 3 */
  AW_0, AW_8, AW_0, AW_0,	/* 4 */
  AW_0, AW_4, AW_0, AW_0,	/* 5 */
  AW_0, AW_2, AW_0, AW_0,	/* 6 */
  AW_0, AW_1, AW_0, AW_0,	/* 7 */
  AW_0, AW_0, AW_8, AW_0,	/* 8 */
  AW_0, AW_0, AW_7, AW_8,	/* 9 */
  AW_0, AW_0, AW_6, AW_0,	/* 10 */
  AW_0, AW_0, AW_5, AW_E,	/* 11 */
  AW_0, AW_0, AW_5, AW_8,	/* 12 */
  AW_0, AW_0, AW_1, AW_4,	/* 13 */
  AW_0, AW_0, AW_1, AW_2,	/* 14 */
  AW_0, AW_0, AW_0, AW_0	/* 15 */
};

static int ar_ulo[] = {		/* upper left odd pixel arrow */

  AW_7, AW_8, AW_0, AW_0,	/* 0 */
  AW_6, AW_0, AW_0, AW_0,	/* 1 */
  AW_5, AW_0, AW_0, AW_0,	/* 2 */
  AW_4, AW_8, AW_0, AW_0,	/* 3 */
  AW_0, AW_4, AW_0, AW_0,	/* 4 */
  AW_0, AW_2, AW_0, AW_0,	/* 5 */
  AW_0, AW_1, AW_0, AW_0,	/* 6 */
  AW_0, AW_0, AW_8, AW_0,	/* 7 */
  AW_0, AW_0, AW_4, AW_0,	/* 8 */
  AW_0, AW_0, AW_3, AW_C,	/* 9 */
  AW_0, AW_0, AW_3, AW_0,	/* 10 */
  AW_0, AW_0, AW_2, AW_F,	/* 11 */
  AW_0, AW_0, AW_2, AW_C,	/* 12 */
  AW_0, AW_0, AW_0, AW_A,	/* 13 */
  AW_0, AW_0, AW_0, AW_9,	/* 14 */
  AW_0, AW_0, AW_0, AW_0	/* 15 */
};

/* 
*/

static int ar_ure[] = {		/* upper right even pixel arrow */

  AW_0, AW_0, AW_1, AW_E,	/* 0 */
  AW_0, AW_0, AW_0, AW_6,	/* 1 */
  AW_0, AW_0, AW_0, AW_A,	/* 2 */
  AW_0, AW_0, AW_1, AW_2,	/* 3 */
  AW_0, AW_0, AW_2, AW_0,	/* 4 */
  AW_0, AW_0, AW_4, AW_0,	/* 5 */
  AW_0, AW_0, AW_8, AW_0,	/* 6 */
  AW_0, AW_1, AW_0, AW_0,	/* 7 */
  AW_0, AW_2, AW_0, AW_0,	/* 8 */
  AW_3, AW_C, AW_0, AW_0,	/* 9 */
  AW_0, AW_C, AW_0, AW_0,	/* 10 */
  AW_F, AW_4, AW_0, AW_0,	/* 11 */
  AW_3, AW_4, AW_0, AW_0,	/* 12 */
  AW_5, AW_0, AW_0, AW_0,	/* 13 */
  AW_9, AW_0, AW_0, AW_0,	/* 14 */
  AW_0, AW_0, AW_0, AW_0	/* 15 */
};

static int ar_uro[] = {		/* upper right odd pixel arrow */

  AW_0, AW_0, AW_0, AW_F,	/* 0 */
  AW_0, AW_0, AW_0, AW_3,	/* 1 */
  AW_0, AW_0, AW_0, AW_5,	/* 2 */
  AW_0, AW_0, AW_0, AW_9,	/* 3 */
  AW_0, AW_0, AW_1, AW_0,	/* 4 */
  AW_0, AW_0, AW_2, AW_0,	/* 5 */
  AW_0, AW_0, AW_4, AW_0,	/* 6 */
  AW_0, AW_0, AW_8, AW_0,	/* 7 */
  AW_0, AW_1, AW_0, AW_0,	/* 8 */
  AW_1, AW_E, AW_0, AW_0,	/* 9 */
  AW_0, AW_6, AW_0, AW_0,	/* 10 */
  AW_7, AW_A, AW_0, AW_0,	/* 11 */
  AW_1, AW_A, AW_0, AW_0,	/* 12 */
  AW_2, AW_8, AW_0, AW_0,	/* 13 */
  AW_4, AW_8, AW_0, AW_0,	/* 14 */
  AW_0, AW_0, AW_0, AW_0	/* 15 */
};

/* 
*/

static int ar_lle[] = {		/* lower left even pixel arrow */

  AW_0, AW_0, AW_0, AW_0,	/* 0 */
  AW_0, AW_0, AW_1, AW_2,	/* 1 */
  AW_0, AW_0, AW_1, AW_4,	/* 2 */
  AW_0, AW_0, AW_5, AW_8,	/* 3 */
  AW_0, AW_0, AW_5, AW_E,	/* 4 */
  AW_0, AW_0, AW_6, AW_0,	/* 5 */
  AW_0, AW_0, AW_7, AW_8,	/* 6 */
  AW_0, AW_0, AW_8, AW_0,	/* 7 */
  AW_0, AW_1, AW_0, AW_0,	/* 8 */
  AW_0, AW_2, AW_0, AW_0,	/* 9 */
  AW_0, AW_4, AW_0, AW_0,	/* 10 */
  AW_0, AW_8, AW_0, AW_0,	/* 11 */
  AW_9, AW_0, AW_0, AW_0,	/* 12 */
  AW_A, AW_0, AW_0, AW_0,	/* 13 */
  AW_C, AW_0, AW_0, AW_0,	/* 14 */
  AW_F, AW_0, AW_0, AW_0	/* 15 */
};

static int ar_llo[] = {		/* lower left odd pixel arrow */

  AW_0, AW_0, AW_0, AW_0,	/* 0 */
  AW_0, AW_0, AW_0, AW_9,	/* 1 */
  AW_0, AW_0, AW_0, AW_A,	/* 2 */
  AW_0, AW_0, AW_2, AW_C,	/* 3 */
  AW_0, AW_0, AW_2, AW_F,	/* 4 */
  AW_0, AW_0, AW_3, AW_0,	/* 5 */
  AW_0, AW_0, AW_3, AW_C,	/* 6 */
  AW_0, AW_0, AW_4, AW_0,	/* 7 */
  AW_0, AW_0, AW_8, AW_0,	/* 8 */
  AW_0, AW_1, AW_0, AW_0,	/* 9 */
  AW_0, AW_2, AW_0, AW_0,	/* 10 */
  AW_0, AW_4, AW_0, AW_0,	/* 11 */
  AW_4, AW_8, AW_0, AW_0,	/* 12 */
  AW_5, AW_0, AW_0, AW_0,	/* 13 */
  AW_6, AW_0, AW_0, AW_0,	/* 14 */
  AW_7, AW_8, AW_0, AW_0	/* 15 */
};

/* 
*/

static int ar_lre[] = {		/* lower right even pixel arrow */

  AW_0, AW_0, AW_0, AW_0,	/* 0 */
  AW_9, AW_0, AW_0, AW_0,	/* 1 */
  AW_5, AW_0, AW_0, AW_0,	/* 2 */
  AW_3, AW_4, AW_0, AW_0,	/* 3 */
  AW_F, AW_4, AW_0, AW_0,	/* 4 */
  AW_0, AW_C, AW_0, AW_0,	/* 5 */
  AW_3, AW_C, AW_0, AW_0,	/* 6 */
  AW_0, AW_2, AW_0, AW_0,	/* 7 */
  AW_0, AW_1, AW_0, AW_0,	/* 8 */
  AW_0, AW_0, AW_8, AW_0,	/* 9 */
  AW_0, AW_0, AW_4, AW_0,	/* 10 */
  AW_0, AW_0, AW_2, AW_0,	/* 11 */
  AW_0, AW_0, AW_1, AW_2,	/* 12 */
  AW_0, AW_0, AW_0, AW_A,	/* 13 */
  AW_0, AW_0, AW_0, AW_6,	/* 14 */
  AW_0, AW_0, AW_1, AW_E	/* 15 */
};

static int ar_lro[] = {		/* lower right odd pixel arrow */

  AW_0, AW_0, AW_0, AW_0,	/* 0 */
  AW_4, AW_8, AW_0, AW_0,	/* 1 */
  AW_2, AW_8, AW_0, AW_0,	/* 2 */
  AW_1, AW_A, AW_0, AW_0,	/* 3 */
  AW_7, AW_A, AW_0, AW_0,	/* 4 */
  AW_0, AW_6, AW_0, AW_0,	/* 5 */
  AW_1, AW_E, AW_0, AW_0,	/* 6 */
  AW_0, AW_1, AW_0, AW_0,	/* 7 */
  AW_0, AW_0, AW_8, AW_0,	/* 8 */
  AW_0, AW_0, AW_4, AW_0,	/* 9 */
  AW_0, AW_0, AW_2, AW_0,	/* 10 */
  AW_0, AW_0, AW_1, AW_0,	/* 11 */
  AW_0, AW_0, AW_0, AW_9,	/* 12 */
  AW_0, AW_0, AW_0, AW_5,	/* 13 */
  AW_0, AW_0, AW_0, AW_3,	/* 14 */
  AW_0, AW_0, AW_0, AW_F	/* 15 */
};

/* 
*/

static int *arrows[] = {	/* arrow pointer table */

  v_curs0,			/* 0 - upper left, even pixel */
  v_curs1,			/* 1 - upper left, odd pixel */

  v_curs2,			/* 2 - upper right, even pixel */
  v_curs3,			/* 3 - upper right, odd pixel */

  v_curs4,			/* 4 - lower left, even pixel */
  v_curs5,			/* 5 - lower left, odd pixel */

  v_curs6,			/* 6 - lower right, even pixel */
  v_curs7			/* 7 - lower right, odd pixel */
};

/* 
*/

/*
   =============================================================================
	gcurpos(xloc, yloc) -- position the graphic cursor at ('xloc', 'yloc')

	Also sets stcrow and stccol to keep them in sync.
   =============================================================================
*/

int
gcurpos (xloc, yloc)
     register int xloc, yloc;
{
  register int ab, xo, yo;
  register struct octent *op;

  if (xloc LT XCTR)
    {

      xo = 0;			/* arrow points left */

      if (yloc LT YCTR)
	{
	  ab = 0;
	  yo = 0;
	}			/* arrow points up */
      else
	{
	  ab = 4;
	  yo = -YOFF;
	}			/* arrow points down */

    }
  else
    {

      xo = -XOFF;		/* arrow points right */

      if (yloc LT YCTR)
	{
	  ab = 2;
	  yo = 0;
	}			/* arrow points up */
      else
	{
	  ab = 6;
	  yo = -YOFF;
	}			/* arrow points down */
    }

  if (v_regs[5] & 0x0180)
    vbank (0);

  v_odtab[GCPRI][0] |= V_BLA;
  objclr (GCPRI);

  op = &v_obtab[GCURS];
  op->objx = xloc + xo;
  op->objy = yloc + yo;
  op->obase = arrows[ab + (xloc & 0x0001)];
  op->odtw1 = 0x0400 | (0x03FF & (op->objx >> 1));

  SetPri (GCURS, GCPRI);

  stcrow = yloc / 14;
  stccol = xloc >> 3;
  return (ab + (xloc & 0x0001));
}

/* 
*/

/*
   =============================================================================
	andcopy() -- copy a pattern
   =============================================================================
*/

andcopy (to, from, with, len)
     register unsigned *to, *from, with;
     short len;
{
  register short i;

  for (i = len; i--;)
    *to++ = *from++ & with;
}

/*
   =============================================================================
	arcurs() -- setup the arrow cursor
   =============================================================================
*/

arcurs (icolor)
     unsigned icolor;
{
  register short color;

  color = exp_c (icolor);

  if ((v_regs[5] & 0x0180) NE 0x0100)
    vbank (1);

  andcopy (v_curs0, ar_ule, color, 64);	/* setup arrow cursor */
  andcopy (v_curs1, ar_ulo, color, 64);
  andcopy (v_curs2, ar_ure, color, 64);
  andcopy (v_curs3, ar_uro, color, 64);
  andcopy (v_curs4, ar_lle, color, 64);
  andcopy (v_curs5, ar_llo, color, 64);
  andcopy (v_curs6, ar_lre, color, 64);
  andcopy (v_curs7, ar_lro, color, 64);
}

/* 
*/

/*
   =============================================================================
	sgcon() -- set graphic cursor object "on"
   =============================================================================
*/

sgcon ()
{
  if (sgcsw)
    return;

  if (v_regs[5] & 0x0180)
    vbank (0);

  v_odtab[GCPRI][0] &= ~V_BLA;
  sgcsw = TRUE;
}

/*
   =============================================================================
	sgcoff() -- set graphic cursor object "off"
   =============================================================================
*/

sgcoff ()
{
  if (!sgcsw)
    return;

  if (v_regs[5] & 0x0180)
    vbank (0);

  v_odtab[GCPRI][0] |= V_BLA;
  sgcsw = FALSE;
}
