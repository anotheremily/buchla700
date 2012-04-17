/*
   =============================================================================
	ettval.c -- MIDAS tuning editor -- value field handlers
	Version 2 -- 1987-12-09 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"
#include "charset.h"

#include "midas.h"
#include "tundsp.h"

extern unsigned *tunob;

extern short stcrow, stccol, tunval, tunmod;

extern short tdbox[][8];

extern char dspbuf[];

/* 
*/

/*
   =============================================================================
	modtun() -- indicate that the tuning table was modified
   =============================================================================
*/

modtun ()
{
  if (! tunmod)
    {

      tunmod = TRUE;
      tdswin (6);
    }
}

/* 
*/

/*
   =============================================================================
	et_tval() -- load the edit buffer
   =============================================================================
*/

short
et_tval (n)
     short n;
{
  register short tv;
  register char ts;

  tv = (tunval < 0 ? -tunval : tunval) >> 1;
  ts = tunval < 0 ? '-' : '+';

  sprintf (ebuf, "%c%04d", ts, tv);
  ebflag = TRUE;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_tval() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_tval (n)
     short n;
{
  register short *ov;
  register short i, tmpval, tv;
  register char ts;

  ebuf[5] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;

  tmpval = 0;

  for (i = 1; i < 5; i++)	/* convert from ASCII to binary */
    tmpval = (tmpval * 10) + (ebuf[i] - '0');

  if (ebuf[0] == '-')
    tunval = -tmpval << 1;
  else
    tunval = tmpval << 1;

  tv = (tunval < 0 ? -tunval : tunval) >> 1;
  ts = tunval < 0 ? '-' : '+';
  sprintf (dspbuf, "Val %c%04d", ts, tv);

  vbank (0);
  vcputsv (tunob, 64, tdbox[n][4], tdbox[n][5], 18, 54, dspbuf, 14);

  modtun ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_tval() -- (re)display the field
   =============================================================================
*/

short
rd_tval (nn)
     short nn;
{
  register short tv, n;
  register char ts;

  n = nn & 0xFF;

  tv = (tunval < 0 ? -tunval : tunval) >> 1;
  ts = tunval < 0 ? '-' : '+';

  sprintf (dspbuf, "Val %c%04d", ts, tv);

  vbank (0);
  vcputsv (tunob, 64, tdbox[n][4], tdbox[n][5], 18, 54, dspbuf, 14);
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_tval() -- handle new data entry
   =============================================================================
*/

short
nd_tval (nn, k)
     short nn;
     register short k;
{
  register short ec, n;

  n = nn & 0xFF;
  ec = stccol - cfetp->flcol;	/* setup edit buffer column */

  if (ec == 0)
    {

      if (k == 8)
	{

	  ebuf[0] = '-';
	  ebuf[5] = '\0';

	  dspbuf[0] = '-';
	  dspbuf[1] = '\0';

	}
      else if (k == 9)
	{

	  ebuf[0] = '+';
	  ebuf[5] = '\0';

	  dspbuf[0] = '+';
	  dspbuf[1] = '\0';

	}
      else
	{

	  return (FAILURE);
	}
/* 
*/
    }
  else
    {

      ebuf[ec] = k + '0';
      ebuf[5] = '\0';

      dspbuf[0] = k + '0';
      dspbuf[1] = '\0';
    }

  vbank (0);
  vcputsv (tunob, 64, TDENTRY, tdbox[n][5], stcrow, stccol, dspbuf, 14);

  advtcur ();
  return (SUCCESS);
}
