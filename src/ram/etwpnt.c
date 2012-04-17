/*
   =============================================================================
	etwpnt.c -- waveshape editor - point number field handlers
	Version 8 -- 1987-12-11 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "wsdsp.h"

extern short advwcur (), wdswin (), newws ();

extern unsigned *waveob;

extern short stccol, curwpnt;

extern short wdbox[][8];

extern char dspbuf[];

/* 
*/

/*
   =============================================================================
	et_wpnt() -- load the edit buffer
   =============================================================================
*/

short
et_wpnt (n)
     short n;
{
  sprintf (ebuf, "%03d", curwpnt);
  ebflag = TRUE;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_wpnt() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_wpnt (n)
     short n;
{
  register short i, tmpval;

  ebuf[3] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;

  tmpval = 0;

  for (i = 0; i < 3; i++)	/* convert from ASCII to binary */
    tmpval = (tmpval * 10) + (ebuf[i] - '0');

  if (tmpval >= NUMWPNT)
    return (FAILURE);

  curwpnt = tmpval;
  newws ();
  wdswin (4);
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_wpnt() -- (re)display the field
   =============================================================================
*/

short
rd_wpnt (nn)
     short nn;
{
  register short n;

  n = nn & 0xFF;
  sprintf (dspbuf, "%03d", curwpnt);

  vbank (0);
  vcputsv (waveob, 64, wdbox[n][4], wdbox[n][5],
	   wdbox[n][6], wdbox[n][7] + WPNT_OFF, dspbuf, 14);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_wpnt() -- handle new data entry
   =============================================================================
*/

short
nd_wpnt (nn, k)
     short nn;
     register short k;
{
  register short ec, n;

  n = nn & 0xFF;
  ec = stccol - cfetp->flcol;	/* setup edit buffer column */
  ebuf[ec] = k + '0';
  ebuf[3] = '\0';

  dspbuf[0] = k + '0';
  dspbuf[1] = '\0';

  vbank (0);

  vcputsv (waveob, 64, WS_ENTRY, wdbox[n][5],
	   wdbox[n][6], stccol, dspbuf, 14);

  advwcur ();

  return (SUCCESS);
}
