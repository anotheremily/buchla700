/*
   =============================================================================
	etagpt.c -- assignment editor - group to input port field handlers
	Version 5 -- 198-10-04 -- D.N. Lynx Crowe
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

extern char dspbuf[];
extern char *gprep[];

extern short key2grp[];

extern short adbox[][8];

extern short grp2prt[12][2];	/* group to port and channel table */
				/* port [0] = 0..3, channel [1] = -1, 1..16 */

/* 
*/

/*
   =============================================================================
	et_agpt() -- load the edit buffer
   =============================================================================
*/

short
et_agpt (n)
     short n;
{
  register short grp;

  grp = 0x00FF & (n >> 8);

  ebuf[0] = grp2prt[grp][0];
  ebuf[1] = '\0';

  ebflag = TRUE;

  return (SUCCESS);
}

/*
   =============================================================================
	ef_agpt() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_agpt (n)
     short n;
{
  register short tmpval, grp, i;

  grp = 0x00FF & (n >> 8);

  ebuf[1] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;

  grp2prt[grp][0] = ebuf[0];

  if (grp2prt[grp][0] NE 1)
    for (i = 0; i < 88; i++)
      key2grp[i] &= ~(0x0001 << grp);

  drawk2g (grp);

  modasg ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_agpt() -- (re)display the field
   =============================================================================
*/

short
rd_agpt (nn)
     short nn;
{
  register short n, grp;

  n = 0x00FF & nn;
  grp = 0x00FF & (nn >> 8);

  vbank (0);
  vcputsv (asgob, 64, adbox[n][4], adbox[n][5],
	   cfetp->frow, cfetp->flcol, gprep[grp2prt[grp][0]], 14);

  return (SUCCESS);
}

/*
   =============================================================================
	nd_agpt() -- handle new data entry
   =============================================================================
*/

short
nd_agpt (nn, k)
     short nn;
     register short k;
{
  register short n;

  n = nn & 0x00FF;

  if (k EQ 8)
    {				/* '-' */

      if (--ebuf[0] LT 0)
	ebuf[0] = 3;

    }
  else if (k EQ 9)
    {				/* '+' */

      if (++ebuf[0] GT 3)
	ebuf[0] = 0;

    }
  else
    {

      return (FAILURE);
    }

  dspbuf[0] = *gprep[ebuf[0]];
  dspbuf[1] = '\0';

  vbank (0);
  vcputsv (asgob, 64, AK_ENTRY, adbox[n][5], stcrow, stccol, dspbuf, 14);

  return (SUCCESS);
}
