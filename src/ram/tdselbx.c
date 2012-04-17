/*
   =============================================================================
	tdselbx.c -- tuning editor box selection functions
	Version 14 -- 1988-12-08 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "graphdef.h"

#include "midas.h"
#include "tundsp.h"

extern short enterit (), loc2key (), advtcur (), bsptcur (), vtdisp (),
nokey ();

extern unsigned exp_c ();

extern short hitbox, cxval, cyval, hitcx, hitcy, submenu;
extern short tunmod, curtun, tdnamsw;
extern short ttcmdsv, ttsel1, ttsel2, ttsel3;

extern short tdbox[][8];

extern struct selbox *csbp, *curboxp;

extern unsigned *tunob;

extern short tuntab[];
extern short oldtun[];

extern char tuncurn[];
extern char vtlin1[], vtlin2[], vtlin3[];

/* forward references */

short bx_null (), tdfnbox ();

/* 
*/

struct selbox tdboxes[] = {

  {0, 0, 95, 335, 0, tdfnbox},	/*  0: keys   0..23 */
  {96, 0, 175, 335, 1, tdfnbox},	/*  1: keys  24..47 */
  {176, 0, 255, 335, 2, tdfnbox},	/*  2: keys  48..71 */
  {256, 0, 335, 335, 3, tdfnbox},	/*  3: keys  72..95 */
  {336, 0, 423, 335, 4, tdfnbox},	/*  4: keys  96..119 */
  {424, 0, 511, 118, 5, tdfnbox},	/*  5: keys 120..127 */
  {424, 119, 511, 349, 6, tdfnbox},	/*  6: commands */
  {0, 336, 423, 349, 7, tdfnbox},	/*  7: name */

  {0, 0, 0, 0, 0, FN_NULL}	/* end of table */
};

/* 
*/

/*
   =============================================================================
	endttyp() -- end function for virtual typewriter
   =============================================================================
*/

endttyp ()
{
  tdnamsw = FALSE;
  submenu = FALSE;
  tdswin (0);
  tdswin (1);
  tdswin (2);
  tdswin (6);
}

/*
   =============================================================================
	tcoladj() -- adjust select column to be a display column
   =============================================================================
*/

short
tcoladj (icol)
     short icol;
{
  if (icol < 12)
    return (6);
  else if (icol < 22)
    return (16);
  else if (icol < 32)
    return (26);
  else if (icol < 42)
    return (36);
  else if (icol < 53)
    return (47);
  else
    return (58);
}

/* 
*/

/*
   =============================================================================
	tdhilit() -- highlight selected tuning entries
   =============================================================================
*/

tdhilit (from, to)
     short from, to;
{
  register short i, row, col, box;

  if (to < from)
    {

      i = to;
      to = from;
      from = i;
    }
/* 
*/
  for (i = from; i <= to; i++)
    {

      if (i < 24)
	{

	  row = i;
	  col = 6;
	  box = 0;

	}
      else if (i < 48)
	{

	  row = i - 24;
	  col = 16;
	  box = 1;

	}
      else if (i < 72)
	{

	  row = i - 48;
	  col = 26;
	  box = 2;

	}
      else if (i < 96)
	{

	  row = i - 72;
	  col = 36;
	  box = 3;

	}
      else if (i < 120)
	{

	  row = i - 96;
	  col = 47;
	  box = 4;

	}
      else
	{

	  row = i - 120;
	  col = 58;
	  box = 5;
	}

      dsttval (row, col, tuntab[i], TDSELD, tdbox[box][5]);
    }
}

/* 
*/

/*
   =============================================================================
	tdfnbox() -- tuning display box hit processor
   =============================================================================
*/

short
tdfnbox (n)
     short n;
{
  register short i, key, dcol, row, col;

  row = hitcy / 14;
  col = hitcx >> 3;

  switch (hitbox)
    {

    case 0:			/* pitch area */
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:

      switch (ttcmdsv)
	{			/* switch on editing state */

	case 0:		/* data entry */

	  enterit ();
	  return (SUCCESS);
/* 
*/
	case 1:		/* transpose and copy -- start */

	  ttsel1 = loc2key (row, col);

	  if (ttsel1 < 0)
	    {

	      ttcmdsv = 0;
	      tdswin (6);
	      return (SUCCESS);
	    }

	  ttcmdsv = 2;
	  dcol = tcoladj (col);
	  dsttval (row, dcol, tuntab[ttsel1], TDSELD, tdbox[hitbox][5]);

	  return (SUCCESS);

	case 2:		/* transpose and copy -- finish */

	  ttsel2 = loc2key (row, col);

	  if (ttsel2 < 0)
	    {

	      ttcmdsv = 0;
	      tdswin (6);
	      return (SUCCESS);
	    }

	  ttcmdsv = 3;
	  tdhilit (ttsel1, ttsel2);
	  return (SUCCESS);

	case 3:		/* transpose and copy -- dest */

	  ttsel3 = loc2key (row, col);

	  if (ttsel3 >= 0)
	    tt_trcp (ttsel1, ttsel2, ttsel3);

	  ttcmdsv = 0;

	  for (i = 0; i < 7; i++)
	    tdswin (i);

	  return (SUCCESS);
/* 
*/
	case 4:		/* increment -- from */

	  ttsel1 = loc2key (row, col);

	  if (ttsel1 < 0)
	    {

	      ttcmdsv = 0;
	      tdswin (6);
	      return (SUCCESS);
	    }

	  ttcmdsv = 5;
	  dcol = tcoladj (col);
	  dsttval (row, dcol, tuntab[ttsel1], TDSELD, tdbox[hitbox][5]);

	  return (SUCCESS);

	case 5:		/* increment -- to */

	  ttsel2 = loc2key (row, col);

	  if (ttsel2 >= 0)
	    tt_incr (ttsel1, ttsel2);

	  ttcmdsv = 0;

	  for (i = 0; i < 7; i++)
	    tdswin (i);

	  return (SUCCESS);
/* 
*/
	case 6:		/* interpolate -- from */

	  ttsel1 = loc2key (row, col);

	  if (ttsel1 < 0)
	    {

	      ttcmdsv = 0;
	      tdswin (6);
	      return (SUCCESS);
	    }

	  ttcmdsv = 7;
	  dcol = tcoladj (col);
	  dsttval (row, dcol, tuntab[ttsel1], TDSELD, tdbox[hitbox][5]);

	  return (SUCCESS);

	case 7:		/* interpolate -- to */

	  ttsel2 = loc2key (row, col);

	  if (ttsel2 >= 0)
	    tt_intp (ttsel1, ttsel2);

	  ttcmdsv = 0;

	  for (i = 0; i < 7; i++)
	    tdswin (i);

	  return (SUCCESS);
	}

      return (FAILURE);
/* 
*/
    case 6:			/* command, value, table # */

      switch (row)
	{

	case 9:		/* transpose and copy */
	case 10:

	  ttcmdsv = 1;
	  td_trcp (1);
	  return (SUCCESS);

	case 12:		/* increment */

	  ttcmdsv = 4;
	  td_incr (1);
	  return (SUCCESS);

	case 14:		/* interpolate */

	  ttcmdsv = 6;
	  td_intp (1);
	  return (SUCCESS);

	case 16:		/* undo */

	  ttcmdsv = 0;
	  memcpyw (tuntab, oldtun, 128);
	  twins ();
	  return (SUCCESS);
/* 
*/
	case 18:		/* data entry */
	case 24:

	  enterit ();
	  return (SUCCESS);

	case 20:		/* store */

	  if (curtun == 0)
	    return (FAILURE);

	  puttun (curtun);
	  memcpyw (oldtun, tuntab, 128);
	  tdswin (6);
	  return (SUCCESS);

	case 22:		/* retrieve */

	  gettun (curtun);
	  memcpyw (oldtun, tuntab, 128);
	  twins ();
	  return (SUCCESS);
	}

      return (FAILURE);

/* 
*/
    case 7:			/* tuning table name */

      if ((col < 7) || (col > 38))
	return (FAILURE);

      if (tdnamsw)
	{

	  vtyper ();
	  tunmod = TRUE;

	}
      else
	{

	  vbank (0);
	  vbfill4 (tunob, 128, CTOX (1), RTOY (21),
		   CTOX (31) - 1, RTOY (24) - 1, exp_c (TDTCURB));

	  tsplot4 (tunob, 64, TDTCHRC, 21, 1, vtlin1, 14);
	  tsplot4 (tunob, 64, TDTCHRC, 22, 1, vtlin2, 14);
	  tsplot4 (tunob, 64, TDTCHRC, 23, 1, vtlin3, 14);

	  vtsetup (tunob, vtdisp, 7, tuncurn, 21, 1,
		   advtcur, bsptcur, nokey, nokey, endttyp,
		   tdbox[n][4], tdbox[n][5]);

	  tdnamsw = TRUE;
	  submenu = TRUE;
	}

      return (SUCCESS);
    }

  return (FAILURE);
}
