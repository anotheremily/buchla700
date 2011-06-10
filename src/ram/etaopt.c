/*
   =============================================================================
	etaopt.c -- assignment editor - MIDI output port number field handlers
	Version 4 -- 1987-12-11 -- D.N. Lynx Crowe
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
extern short curmop;

extern short adbox[][8];

extern char dspbuf[];

extern char *gprep[];

/* 
*/

/*
   =============================================================================
	et_aopt() -- load the edit buffer
   =============================================================================
*/

short
et_aopt (n)
     short n;
{
  sprintf (ebuf, "%01.1d", curmop);
  ebflag = TRUE;

  return (SUCCESS);
}

/*
   =============================================================================
	ef_aopt() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_aopt (n)
     short n;
{
  register short tmpval;

  ebuf[1] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;

  tmpval = ebuf[0] - '0';

  if (tmpval GT 3)
    return (FAILURE);

  curmop = tmpval;
  modasg ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_aopt() -- (re)display the field
   =============================================================================
*/

short
rd_aopt (nn)
     short nn;
{
  register short n;

  n = nn & 0xFF;

  vbank (0);
  vcputsv (asgob, 64, adbox[n][4], adbox[n][5],
	   adbox[n][6], adbox[n][7] + 9, gprep[curmop], 14);

  return (SUCCESS);
}

/*
   =============================================================================
	nd_aopt() -- handle new data entry
   =============================================================================
*/

short
nd_aopt (nn, k)
     short nn;
     register short k;
{
  register short n;

  if ((k < 1) OR (k > 2))
    return (FAILURE);

  n = nn & 0xFF;
  ebuf[0] = k + '0';
  ebuf[1] = '\0';

  dspbuf[0] = k + '0';
  dspbuf[1] = '\0';

  vbank (0);
  vcputsv (asgob, 64, AK_ENTRY, adbox[n][5], stcrow, stccol, dspbuf, 14);

  return (SUCCESS);
}
