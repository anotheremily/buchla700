/*
   =============================================================================
	etatab.c -- assignment editor - assignment table number field handlers
	Version 9 -- 1988-08-22 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "asgdsp.h"

extern short adpoint ();

extern short (*point) ();

extern unsigned *asgob;

extern short asgmod;
extern short curasg;
extern short stccol;
extern short stcrow;

extern short adbox[][8];

extern char dspbuf[];

/* 
*/

/*
   =============================================================================
	et_atab() -- load the edit buffer
   =============================================================================
*/

short
et_atab (n)
     short n;
{
  sprintf (ebuf, "%02d", curasg);
  ebflag = TRUE;

  return (SUCCESS);
}

/*
   =============================================================================
	ef_atab() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_atab (n)
     short n;
{
  register short i, tmpval;

  ebuf[2] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;
  tmpval = 0;

  for (i = 0; i < 2; i++)	/* convert from ASCII to binary */
    tmpval = (tmpval * 10) + (ebuf[i] - '0');

  if (tmpval >= NASGS)
    return (FAILURE);

  curasg = tmpval;
  asgmod = TRUE;
  adswin (0);

  settc (2, 6);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_atab() -- (re)display the field
   =============================================================================
*/

short
rd_atab (nn)
     short nn;
{
  register short n;

  n = nn & 0xFF;
  sprintf (dspbuf, "%02.2d", curasg);

  point = adpoint;

  if (v_regs[5] & 0x0180)
    vbank (0);

  vcputsv (asgob, 64, (asgmod ? exp_c (AK_MODC) : adbox[n][4]), adbox[n][5],
	   cfetp->frow, cfetp->flcol, dspbuf, 14);

  lseg (0, 0, 128, 0, AK_BORD);

  return (SUCCESS);
}

/*
   =============================================================================
	nd_atab() -- handle new data entry
   =============================================================================
*/

short
nd_atab (nn, k)
     short nn;
     register short k;
{
  register short ec, n;

  n = nn & 0xFF;
  ec = stccol - cfetp->flcol;	/* setup edit buffer column */

  ebuf[ec] = k + '0';
  ebuf[2] = '\0';

  dspbuf[0] = k + '0';
  dspbuf[1] = '\0';

  point = adpoint;

  if (v_regs[5] & 0x0180)
    vbank (0);

  vcputsv (asgob, 64, AK_ENTRY, adbox[n][5], stcrow, stccol, dspbuf, 14);

  lseg (0, 0, 128, 0, AK_BORD);

  advacur ();
  return (SUCCESS);
}
