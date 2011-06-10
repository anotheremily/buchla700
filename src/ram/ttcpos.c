/*
   =============================================================================
	ttcpos.c -- virtual typewriter cursor positioning
	Version 9 -- 1988-03-08 -- D.N. Lynx Crowe
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
#include "libdsp.h"

#define	CW_0	0x0000
#define	CW_F	0xFFFF

extern unsigned exp_c ();

extern short vtcrow;		/* virtual typewriter cursor row */
extern short vtccol;		/* virtual typewriter cursor column */

/* 
*/

static short ttcur[] = {

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
	ttcini() -- initialize typewriter cursor
   =============================================================================
*/

ttcini (color)
     unsigned color;
{
  if ((v_regs[5] & 0x0180) NE 0x0100)
    vbank (1);

  andcopy (v_tcur, ttcur, exp_c (color), 64);
}

/*
   =============================================================================
	ttcpos() -- position the typewriter cursor at ('row', 'col')
   =============================================================================
*/

ttcpos (row, col)
     register short row, col;
{
  register short yrow, xcol;
  register struct octent *op;

  if (v_regs[5] & 0x0180)	/* point at the control bank */
    vbank (0);

  yrow = row * 14;		/* get cursor display position */
  xcol = col << 3;

  op = &v_obtab[TTCURS];	/* point at v_obtab entry */

  v_odtab[TTCPRI][0] |= V_BLA;	/* blank the object */
  objclr (TTCPRI);		/* turn off the old location */

  op->objx = xcol;		/* update v_obtab entry */
  op->objy = yrow;
  op->odtw1 = 0x0400 | (0x03FF & (xcol >> 1));

  SetPri (TTCURS, TTCPRI);	/* turn on the new location */

  vtcrow = row;			/* update cursor position variables */
  vtccol = col;
}
