/*
   =============================================================================
	itcpos.c -- text cursor positioning for the instrument display
	Version 12 -- 1989-12-19 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "hwdefs.h"
#include "memory.h"
#include "fields.h"

#include "midas.h"
#include "instdsp.h"

#define	CW_0	0x0000
#define	CW_F	0xFFFF

extern unsigned exp_c ();

extern short stcrow, stccol;	/* text cursor row,col */

extern unsigned *obj0;		/* cursor object pointer */

/* 
*/

static short itcur[] = {

  CW_0, CW_0, CW_0, CW_0,	/* 0 */
  CW_0, CW_0, CW_0, CW_0,	/* 1 */
  CW_0, CW_0, CW_0, CW_0,	/* 2 */
  CW_0, CW_0, CW_0, CW_0,	/* 3 */
  CW_0, CW_0, CW_0, CW_0,	/* 4 */
  CW_0, CW_0, CW_0, CW_0,	/* 5 */
  CW_0, CW_0, CW_0, CW_0,	/* 6 */
  CW_0, CW_0, CW_0, CW_0,	/* 7 */
  CW_0, CW_0, CW_0, CW_0,	/* 8 */
  CW_0, CW_0, CW_0, CW_0,	/* 9 */
  CW_0, CW_0, CW_0, CW_0,	/* 10 */
  CW_0, CW_0, CW_0, CW_0,	/* 11 */
  CW_F, CW_F, CW_0, CW_0,	/* 12 */
  CW_0, CW_0, CW_0, CW_0,	/* 13 */
  CW_0, CW_0, CW_0, CW_0,	/* 14 */
  CW_0, CW_0, CW_0, CW_0	/* 15 */
};

/* 
*/

/*
   =============================================================================
	itcini() -- initialize instrument text cursor
   =============================================================================
*/

itcini (color)
     unsigned color;
{
  if ((v_regs[5] & 0x0180) NE 0x0100)
    vbank (1);

  andcopy (v_cur, itcur, exp_c (color), 64);
}

/* 
*/

/*
   =============================================================================
	itcpos() -- position the instrument text cursor at ('row', 'col')
   =============================================================================
*/

itcpos (row, col)
     register short row, col;
{
  register short yrow, xcol;
  register struct octent *op;

  if (v_regs[5] & 0x0180)	/* point at the control bank */
    vbank (0);

  yrow = row * 14;		/* get cursor display position */
  xcol = col * 8;

  op = &v_obtab[TCURS];		/* point at v_obtab entry */
  setipl (VID_DI);		/* disable Vertical Interval interrupt */
  v_odtab[TCPRI][0] &= ~V_BLA;	/* blank the object */
  objclr (TCPRI);		/* turn off the old location */

  op->objx = xcol;		/* update v_obtab entry */
  op->objy = yrow;
  op->obase = &v_cur[0];
  op->odtw1 = 0x0400 | (0x03FF & (xcol >> 1));

  SetPri (TCURS, TCPRI);	/* turn on the new location */

  stcrow = row;			/* update cursor position */
  stccol = col;
}

/* 
*/

/*
   =============================================================================
	advicur() -- advance the instrument display text cursor
   =============================================================================
*/

advicur ()
{
  register short newcol;

  if (infield (stcrow, stccol, curfet))
    cfetp = infetp;
  else
    return;

  newcol = stccol + 1;

  if (newcol LE cfetp->frcol)
    itcpos (stcrow, newcol);
}

/*
   =============================================================================
	bspicur() -- backspace the instrument display text cursor
   =============================================================================
*/

bspicur ()
{
  register short newcol;

  if (infield (stcrow, stccol, curfet))
    cfetp = infetp;
  else
    return;

  newcol = stccol - 1;

  if (newcol GE cfetp->flcol)
    itcpos (stcrow, newcol);
}
