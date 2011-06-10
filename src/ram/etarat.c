/*
   =============================================================================
	etarat.c -- MIDAS assignment editor - phase shifter rate field
	Version 4 -- 1987-12-19 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "asgdsp.h"

extern unsigned *asgob;

extern short stcrow, stccol;
extern short ps_rate;

extern short adbox[][8];

extern char dspbuf[];

/* 
*/

/*
   =============================================================================
	et_arat() -- load the edit buffer
   =============================================================================
*/

short
et_arat (n)
     short n;
{
  sprintf (ebuf, "%02d", ps_rate);
  ebflag = TRUE;

  return (SUCCESS);
}

/*
   =============================================================================
	ef_arat() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_arat (n)
     short n;
{
  register short i, tmpval;

  ebuf[2] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;
  tmpval = 0;

  for (i = 0; i < 2; i++)	/* convert from ASCII to binary */
    tmpval = (tmpval * 10) + (ebuf[i] - '0');

  ps_rate = tmpval;
  sendval (2, 0, ((ps_rate * 10) << 5));
  modasg ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_arat() -- (re)display the field
   =============================================================================
*/

short
rd_arat (nn)
     short nn;
{
  register short n;

  n = nn & 0xFF;
  sprintf (dspbuf, "%02.2d", ps_rate);

  vbank (0);
  vcputsv (asgob, 64, adbox[n][4], adbox[n][5],
	   adbox[n][6] + 2, adbox[n][7] + 8, dspbuf, 14);

  return (SUCCESS);
}

/*
   =============================================================================
	nd_arat() -- handle new data entry
   =============================================================================
*/

short
nd_arat (nn, k)
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

  vbank (0);
  vcputsv (asgob, 64, AK_ENTRY, adbox[n][5], stcrow, stccol, dspbuf, 14);

  advacur ();
  return (SUCCESS);
}
