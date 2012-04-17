/*
   =============================================================================
	etaprg.c -- MIDAS assignment editor - program change channel field
	Version 3 -- 1987-12-21 -- D.N. Lynx Crowe
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

extern unsigned *asgob;

extern short stcrow, stccol;
extern short prgchan;

extern short adbox[][8];

extern char dspbuf[];

/* 
*/

/*
   =============================================================================
	et_aprg() -- load the edit buffer
   =============================================================================
*/

short
et_aprg (n)
     short n;
{
  sprintf (ebuf, "%02d", prgchan);
  ebflag = TRUE;

  return (SUCCESS);
}

/*
   =============================================================================
	ef_aprg() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_aprg (n)
     short n;
{
  register short i, tmpval;

  ebuf[2] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;
  tmpval = 0;

  for (i = 0; i < 2; i++)	/* convert from ASCII to binary */
    tmpval = (tmpval * 10) + (ebuf[i] - '0');

#if	DEBUGIT
  if (debugsw)
    printf ("ef_aprg($%04.4X):  ebuf=[%s], tmpval=%d\n", n, ebuf, tmpval);
#endif

  if ((tmpval == 0) || (tmpval > 16))
    return (FAILURE);

  prgchan = tmpval;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_aprg() -- (re)display the field
   =============================================================================
*/

short
rd_aprg (nn)
     short nn;
{
  register short n;

  n = nn & 0xFF;
  sprintf (dspbuf, "%02.2d", prgchan);

  vbank (0);
  vcputsv (asgob, 64, adbox[n][4], adbox[n][5],
	   adbox[n][6], adbox[n][7] + 8, dspbuf, 14);

  return (SUCCESS);
}

/*
   =============================================================================
	nd_aprg() -- handle new data entry
   =============================================================================
*/

short
nd_aprg (nn, k)
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
