/*
   =============================================================================
	etwoff.c -- waveshape editor - offset value field handlers
	Version 16 -- 1987-12-22 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"
#include "charset.h"

#include "midas.h"
#include "instdsp.h"
#include "wsdsp.h"

extern short advwcur (), wdswin (), pntsup ();

extern unsigned *waveob;

extern short stcrow, stccol, lstwoff, curwoff, curvce, curwslt, curwdth;

extern short wsnmod[12][2];

extern short wdbox[][8];

extern char dspbuf[];

/* 
*/

/*
   =============================================================================
	et_woff() -- load the edit buffer
   =============================================================================
*/

short
et_woff (n)
     short n;
{
  register short hoff;
  register char hosgn;

  lstwoff = curwoff;

  if (curwoff < 0)
    {

      hosgn = '-';
      hoff = -curwoff;

    }
  else
    {

      hosgn = '+';
      hoff = curwoff;
    }

  sprintf (ebuf, "%c%04d", hosgn, hoff);
  ebflag = TRUE;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_woff() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_woff (n)
     short n;
{
  register short *ov;
  register short i, tmpval;

  ebuf[5] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;

  tmpval = 0;

  for (i = 1; i < 5; i++)	/* convert from ASCII to binary */
    tmpval = (tmpval * 10) + (ebuf[i] - '0');

  if (tmpval > 1023)
    return (FAILURE);

  if (ebuf[0] == '-')
    curwoff = -tmpval;
  else
    curwoff = tmpval;

  if (curwdth == NUMWIDS)
    wdintp ();
  else
    pntsup ();

  wdswin (0);
  wdswin (2);
  wdswin (4);
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_woff() -- (re)display the field
   =============================================================================
*/

short
rd_woff (nn)
     short nn;
{
  register short hoff, n;
  register char hosgn;

  n = nn & 0xFF;
  lstwoff = curwoff;

  if (curwoff < 0)
    {

      hosgn = '-';
      hoff = -curwoff;

    }
  else
    {

      hosgn = '+';
      hoff = curwoff;
    }

  sprintf (dspbuf, "%c%04d", hosgn, hoff);

  /* display the value */

  if (v_regs[5] & 0x0180)
    vbank (0);

  vcputsv (waveob, 64, wdbox[n][4], wdbox[n][5],
	   wdbox[n][6], wdbox[n][7] + WOFF_OFF, dspbuf, 14);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_woff() -- handle new data entry
   =============================================================================
*/

short
nd_woff (nn, k)
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

    }
  else
    {

      ebuf[ec] = k + '0';
      ebuf[5] = '\0';

      dspbuf[0] = k + '0';
      dspbuf[1] = '\0';
    }
/* 
*/
  if (v_regs[5] & 0x0180)
    vbank (0);

  vcputsv (waveob, 64, WS_ENTRY, wdbox[n][5], stcrow, stccol, dspbuf, 14);

  advwcur ();
  return (SUCCESS);
}
