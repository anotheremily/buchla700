/*
   =============================================================================
	etaccn.c -- assignment editor - source controller number field handlers
	Version 7 -- 1988-03-19 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "asgdsp.h"

extern char *numblk ();

extern unsigned *asgob;

extern short stcrow, stccol;

extern short adbox[][8];

extern char dspbuf[];

extern short mctlnum[4];	/* MIDI controller number table (-1, 00..99) */

/* 
*/

/*
   =============================================================================
	et_accn() -- load the edit buffer
   =============================================================================
*/

short
et_accn (n)
     short n;
{
  register short ctl;

  char buf[4];

  ctl = 0x00FF & (n >> 8);

  numblk (ebuf, (mctlnum[ctl] & 0x00FF));
  ebflag = TRUE;

  return (SUCCESS);
}

/*
   =============================================================================
	ef_accn() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_accn (n)
     short n;
{
  register short tmpval, ctl, i;

  ctl = 0x00FF & (n >> 8);

  ebuf[2] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;
  tmpval = 0;

  if ((ebuf[0] EQ ' ') AND (ebuf[1] EQ ' '))
    {

      tmpval = -1;

    }
  else
    {

      for (i = 0; i < 2; i++)
	{

	  if (ebuf[i] EQ ' ')
	    ebuf[i] = '0';

	  tmpval = (tmpval * 10) + (ebuf[i] - '0');
	}
    }

  if (mctlnum[ctl] EQ - 1)
    mctlnum[ctl] = tmpval;
  else
    mctlnum[ctl] = (mctlnum[ctl] & 0xFF00) | tmpval;

  modasg ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_accn() -- (re)display the field
   =============================================================================
*/

short
rd_accn (nn)
     short nn;
{
  register short n, ctl;
  char buf[4];

  n = 0x00FF & nn;
  ctl = 0x00FF & (nn >> 8);

  vbank (0);
  vcputsv (asgob, 64, adbox[n][4], adbox[n][5],
	   cfetp->frow, cfetp->flcol,
	   numblk (buf, (mctlnum[ctl] & 0x00FF)), 14);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_accn() -- handle new data entry
   =============================================================================
*/

short
nd_accn (nn, k)
     short nn;
     register short k;
{
  register short n;

  n = nn & 0xFF;
  ebuf[stccol - cfetp->flcol] = k + '0';
  ebuf[2] = '\0';

  dspbuf[0] = k + '0';
  dspbuf[1] = '\0';

  vbank (0);
  vcputsv (asgob, 64, AK_ENTRY, adbox[n][5], stcrow, stccol, dspbuf, 14);

  advacur ();
  return (SUCCESS);
}
