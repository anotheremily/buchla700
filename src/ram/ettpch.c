/*
   =============================================================================
	ettpch.c -- MIDAS tuning table pitch field handlers
	Version 6 -- 1987-12-21 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"
#include "charset.h"

#include "midas.h"
#include "tundsp.h"

/* functions defined elsewhere */

extern short cnvp2c ();

/* variables defined elsewhere */

#if	DEBUGIT
extern short debugsw;
#endif

extern short stcrow, stccol, cents;

extern short tdbox[][8];

extern short tuntab[];

extern char dspbuf[];
extern char sfdsp[];

extern unsigned *tunob;

/* 
*/

/*
   =============================================================================
	loc2key() -- convert a screen location to a key number or field code
   =============================================================================
*/

short
loc2key (row, col)
     short row, col;
{
  if (row EQ 24)
    {

      if ((col > 6) AND (col < 39))
	return (-1);		/* -1 = name */
      else if (col > 52)
	return (-9);		/* -9 = table # */
      else
	return (-10);		/* -10 = not a field */

    }
  else if (col < 12)
    return (row);		/* 0..23 = key 0..23 */
  else if ((col > 11) AND (col < 22))
    return (row + 24);		/* 24..47 = key 24..47 */
  else if ((col > 21) AND (col < 32))
    return (row + 48);		/* 48..71 = key 48..71 */
  else if ((col > 31) AND (col < 42))
    return (row + 72);		/* 72..95 = key 72..95 */
  else if ((col > 41) AND (col < 53))
    return (row + 96);		/* 96..119 = key 96..119 */
  else if ((col > 52) AND (row < 8))
    return (row + 120);		/* 120..127 = key 120..127 */
  else if (col > 52)
    {

      if ((row EQ 9) OR (row EQ 10))
	return (-2);		/* -2 = transpose and copy */
      else if (row EQ 12)
	return (-3);		/* -3 = increment */
      else if (row EQ 14)
	return (-4);		/* -4 = interpolate */
      else if (row EQ 16)
	return (-5);		/* -5 = undo */
      else if (row EQ 18)
	return (-6);		/* -6 = value */
      else if (row EQ 20)
	return (-7);		/* -7 = store */
      else if (row EQ 22)
	return (-8);		/* -8 = retrieve */
      else
	return (-10);		/* -10 = not a field */
    }
}

/* 
*/

/*
   =============================================================================
	et_tpch() -- load the edit buffer
   =============================================================================
*/

short
et_tpch (nn)
     short nn;
{
  register short key, val;

  key = loc2key (stcrow, stccol);

  if (key < 0)
    return (FAILURE);

  val = tuntab[key];
  cnvc2p (ebuf, (val >> 1));
  ebflag = TRUE;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_tpch() -- unload (parse) edit buffer
   =============================================================================
*/

short
ef_tpch (nn)
     short nn;
{
  register short key;

  ebflag = FALSE;
  key = loc2key (stcrow, stccol);

  if (key < 0)
    return (FAILURE);

  if (cnvp2c ()EQ FAILURE)
    return (FAILURE);

  modtun ();
  tuntab[key] = cents << 1;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_tpch() -- (re)display the field
   =============================================================================
*/

short
rd_tpch (nn)
     short nn;
{
  register short val, key, n;

  n = nn & 0xFF;
  key = loc2key (cfetp->frow, cfetp->flcol);
  val = tuntab[key];

  cnvc2p (dspbuf, (val >> 1));

  dspbuf[0] += '0';
  dspbuf[1] += 'A';
  dspbuf[2] = sfdsp[dspbuf[2] - 7];
  dspbuf[3] += '0';
  dspbuf[4] += '0';
  dspbuf[5] = '\0';

  vbank (0);
  vcputsv (tunob, 64,
	   ((val EQ 320) OR (val EQ 21920))
	   ? TDMKEYC : tdbox[n][4], tdbox[n][5],
	   cfetp->frow, cfetp->flcol, dspbuf, 14);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_tpch() -- handle new data entry
   =============================================================================
*/

short
nd_tpch (nn, k)
     short nn, k;
{
  register short col, n;

  n = nn & 0xFF;

  if (stccol < 11)
    col = stccol - 6;
  else if (stccol < 21)
    col = stccol - 16;
  else if (stccol < 31)
    col = stccol - 26;
  else if (stccol < 41)
    col = stccol - 36;
  else if (stccol < 52)
    col = stccol - 47;
  else
    col = stccol - 58;

  switch (col)
    {

    case 0:

      ebuf[0] = k;
      dspbuf[0] = k + '0';
      break;

    case 1:

      if (k GT 6)
	return (FAILURE);

      ebuf[1] = k;
      dspbuf[0] = k + 'A';
      break;
/* 
*/
    case 2:

      if (k EQ 7)
	{			/* blank */

	  ebuf[2] = k;
	  dspbuf[0] = sfdsp[0];
	  break;

	}
      else if (k EQ 8)
	{			/* flat */

	  ebuf[2] = k;
	  dspbuf[0] = sfdsp[1];
	  break;

	}
      else if (k EQ 9)
	{			/* sharp */

	  ebuf[2] = k;
	  dspbuf[0] = sfdsp[2];
	  break;

	}
      else
	return (FAILURE);
/* 
*/
    case 3:
    case 4:

      ebuf[col] = k;
      dspbuf[0] = k + '0';
      break;
    }

  dspbuf[1] = '\0';

  vbank (0);
  vcputsv (tunob, 64, TDENTRY, tdbox[n][5], stcrow, stccol, dspbuf, 14);

  if (col EQ 4)
    return (SUCCESS);

  advtcur ();
  return (SUCCESS);
}
