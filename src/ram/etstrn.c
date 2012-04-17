/*
   =============================================================================
	etstrn.c -- section edit group transposition field handlers
	Version 3 -- 1989-11-14 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "fields.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "stddefs.h"
#include "score.h"
#include "macros.h"

#include "midas.h"
#include "scdsp.h"

extern unsigned *obj8;

extern short grptran;
extern short sdmctl;
extern short vtccol;
extern short vtcrow;
extern short vtxval;
extern short vtyval;

extern char dspbuf[];

/*
   =============================================================================
	et_strn() -- load edit buffer
   =============================================================================
*/

short
et_strn ()
{
  sprintf (ebuf, "%c%02d", grptran < 0 ? '-' : '+', abs (grptran));

  ebflag = TRUE;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_strn() -- parse edit buffer
   =============================================================================
*/

short
ef_strn ()
{
  register short ival;

  ebuf[3] = '\0';
  ival = ((ebuf[1] - '0') * 10) + (ebuf[2] - '0');

  ebflag = FALSE;

  if ((ival != 0) && (ebuf[0] == '-'))
    ival = -ival;

  grptran = ival;

  return (SUCCESS);
}

/*
   =============================================================================
	rd_strn() -- redisplay field
   =============================================================================
*/

short
rd_strn ()
{
  sprintf (dspbuf, "%c%02d", grptran < 0 ? '-' : '+', abs (grptran));

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputs (obj8, 7, 22, dspbuf, SDMENUBG);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_strn() -- data entry
   =============================================================================
*/

short
nd_strn (n, k)
     short n, k;
{
  register short ec;

  if (sdmctl != 4)
    return (FAILURE);

  if ((vtccol >= 22) && (vtccol <= 24))
    ec = vtccol - 22;
  else
    return (FAILURE);

  if (ec == 0)
    {

      if (k == 8)
	k = '-';
      else if (k == 9)
	k = '+';
      else
	return (FAILURE);
    }
  else
    k += '0';

  ebuf[ec] = k;

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 7, vtccol, k, (SDBGMM | (SD_ENTR << 4)));

  if (vtccol < 24)
    {

      ++vtccol;
      vtxval = CTOX (vtccol);
      ttcpos (vtcrow, vtccol);
    }

  return (SUCCESS);
}
