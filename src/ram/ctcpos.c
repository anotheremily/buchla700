/*
   =============================================================================
	ctcpos.c -- character text cursor positioning
	Version 12 -- 1989-11-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "hwdefs.h"
#include "macros.h"
#include "patch.h"

#include "midas.h"

short ctcsw;			/* text cursor status */

extern short CurLine;		/* current patch display line */
extern short cxval, cyval;	/* patch text cursor x, y */
extern short stcrow, stccol;	/* patch text cursor row,col */
extern short vtcrow, vtccol;	/* menu text cursor row,col */

extern unsigned *obj9;		/* patch cursor object pointer */
extern unsigned *obj11;		/* menu cursor object pointer */

short mtcoldc;			/* previous cursor column location */
short mtcoldr;			/* previous cursor row location */

/* 
*/

/*
   =============================================================================
	ctcpos() -- position the patch text cursor at ('row', 'col')
   =============================================================================
*/

ctcpos (row, col)
     register short row, col;
{
  register short nrow;

  if (ctcsw)
    {

      if (v_regs[5] & 0x0180)	/* point at the control bank */
	vbank (0);

      nrow = CurLine + 7;

      if (stcrow == DATAROW)	/* turn off old cursor */
	vclrav (obj9, nrow, stccol, C_ULINE, 48);

      if (row == DATAROW)	/* turn on new cursor */
	vsetav (obj9, nrow, col, C_ULINE, 48);
    }

  stcrow = row;			/* update cursor position */
  stccol = col;
}

/* 
*/

/*
   =============================================================================
	ctcoff() -- turn off the patch text cursor
   =============================================================================
*/

ctcoff ()
{
  if (v_regs[5] & 0x0180)	/* point at the control bank */
    vbank (0);

  if (stcrow == DATAROW)	/* turn off cursor */
    vclrav (obj9, CurLine + 7, stccol, C_ULINE, 48);

  ctcsw = FALSE;
}

/*
   =============================================================================
	ctcon() -- turn on the patch text cursor
   =============================================================================
*/

ctcon ()
{
  if (v_regs[5] & 0x0180)	/* point at the control bank */
    vbank (0);

  if (stcrow == DATAROW)
    {				/* turn on cursor */

      ctcsw = TRUE;
      vsetav (obj9, CurLine + 7, stccol, C_ULINE, 48);
    }
}

/* 
*/

/*
   =============================================================================
	mtcpos() -- position the menu text cursor at ('row', 'col')
   =============================================================================
*/

mtcpos (row, col)
     register short row, col;
{
  if (v_regs[5] & 0x0180)	/* point at the control bank */
    vbank (0);

  if (inrange (mtcoldr, 19, 23))
    {				/* turn off old cursor */

      vclrav (obj11, mtcoldr - 18, mtcoldc, C_ULINE, 64);

      mtcoldr = 0;		/* void old cursor location */
      mtcoldc = 0;
    }

  if (inrange (row, 19, 23))
    {				/* turn on new cursor */

      vsetav (obj11, row - 18, col, C_ULINE, 64);

      mtcoldr = row;		/* keep track of new cursor */
      mtcoldc = col;
    }

  vtcrow = row;			/* update cursor position */
  vtccol = col;
}

/*
   =============================================================================
	mtcoff() -- turn off the menu text cursor
   =============================================================================
*/

mtcoff ()
{
  if (v_regs[5] & 0x0180)	/* point at the control bank */
    vbank (0);

  if (inrange (mtcoldr, 19, 23))	/* turn off cursor */
    vclrav (obj11, mtcoldr - 18, mtcoldc, C_ULINE, 64);
}
