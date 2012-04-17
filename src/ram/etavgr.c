/*
   =============================================================================
	etavgr.c -- assignment editor - voice to group field handlers
	Version 9 -- 1988-03-18 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "asgdsp.h"

#if	DEBUGIT
extern short debugsw;
#endif

extern char *numblk ();

extern unsigned *asgob;

extern short stcrow, stccol;

extern short adbox[][8];

extern char dspbuf[];

extern short vce2grp[12];	/* voice to group table */
extern short ins2grp[12];

/* 
*/

/*
   =============================================================================
	et_avgr() -- load the edit buffer
   =============================================================================
*/

short
et_avgr (nn)
     short nn;
{
  register short grp;
  char buf[4];

  grp = 0x00FF & (nn >> 8);

  sprintf (ebuf, "%s", numblk (buf, vce2grp[grp]));
  ebflag = TRUE;

#if	DEBUGIT
  if (debugsw)
    printf ("et_avgr($%04.4X):  ebuf=[%s]\n", nn, ebuf);
#endif

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_avgr() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_avgr (nn)
     short nn;
{
  register short tmpval, vce, i;

  vce = 0x00FF & (nn >> 8);

  ebuf[2] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;
  tmpval = 0;

#if	DEBUGIT
  if (debugsw)
    printf ("ef_avgr($%04.4X):  ebuf=[%s]\n", nn, ebuf);
#endif

  if ((ebuf[0] == ' ') && (ebuf[1] == ' '))
    {

      tmpval = -1;

    }
  else
    {

      for (i = 0; i < 2; i++)
	{

	  if (ebuf[i] == ' ')
	    ebuf[i] = '0';

	  tmpval = (tmpval * 10) + (ebuf[i] - '0');
	}

      if ((tmpval == 0) || (tmpval > 12))
	return (FAILURE);
    }

  vce2grp[vce] = tmpval;

  if (tmpval > 0)
    execinst (vce, (ins2grp[tmpval - 1] & 0x00FF), 1);

  modasg ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_avgr() -- (re)display the field
   =============================================================================
*/

short
rd_avgr (nn)
     short nn;
{
  register short n, grp;
  char buf[4];

  n = 0x00FF & nn;
  grp = 0x00FF & (nn >> 8);

  vbank (0);
  vcputsv (asgob, 64, adbox[n][4], adbox[n][5],
	   cfetp->frow, cfetp->flcol, numblk (buf, vce2grp[grp]), 14);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_avgr() -- handle new data entry
   =============================================================================
*/

short
nd_avgr (nn, k)
     short nn;
     register short k;
{
  register short n, col;

  n = nn & 0xFF;
  col = stccol - cfetp->flcol;

  ebuf[col] = k + '0';
  ebuf[2] = '\0';

#if	DEBUGIT
  if (debugsw)
    printf ("nd_avgr($%04.4X, %d):  cfetp=$%08.8lX, col=%d, ebuf=[%s]\n",
	    nn, k, cfetp, col, ebuf);
#endif

  dspbuf[0] = k + '0';
  dspbuf[1] = '\0';

  vbank (0);
  vcputsv (asgob, 64, AK_ENTRY, adbox[n][5], stcrow, stccol, dspbuf, 14);

  advacur ();
  return (SUCCESS);
}
