/*
   =============================================================================
	etscor.c -- score field handlers
	Version 7 -- 1988-08-16 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "fields.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "stddefs.h"
#include "score.h"

#include "midas.h"
#include "scdsp.h"

extern unsigned *obj8;

extern short curscor;
extern short insmode;
extern short stccol;

extern char dspbuf[];

/*
   =============================================================================
	et_scor() -- load edit buffer
   =============================================================================
*/

short
et_scor ()
{
  sprintf (ebuf, "%02.2d", curscor + 1);
  ebflag = TRUE;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_scor() -- parse edit buffer
   =============================================================================
*/

short
ef_scor ()
{
  register short ival;

  ebuf[2] = '\0';
  ival = ((ebuf[0] - '0') * 10) + (ebuf[1] - '0');

  ebflag = FALSE;

  if ((ival GT N_SCORES) OR (ival EQ 0))
    return (FAILURE);

  if (insmode)
    {

      icancel ();
      dsimode ();
    }

  selscor (ival - 1);
  sdwins ();
  return (SUCCESS);
}

/*
   =============================================================================
	rd_scor() -- redisplay field
   =============================================================================
*/

short
rd_scor ()
{
  sprintf (dspbuf, "%02.2d", curscor + 1);

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputs (obj8, 8, 7, dspbuf, SDW13ATR);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_scor() -- data entry
   =============================================================================
*/

short
nd_scor (n, k)
     short n, k;
{
  register short ec;

  ec = stccol - cfetp->flcol;
  ebuf[ec] = k + '0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 8, stccol, k + '0', SDW13DEA);
  advscur ();

  return (SUCCESS);
}
