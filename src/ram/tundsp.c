/*
   =============================================================================
	tundsp.c -- MIDAS tuning table editor
	Version 23 -- 1988-11-28 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "hwdefs.h"
#include "biosdefs.h"
#include "stddefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "charset.h"
#include "fields.h"

#include "midas.h"
#include "tundsp.h"

#define	TUN_VAL	100		/* default value for tunval in cents */

extern short stcrow, stccol, cxval, cyval;

extern short curtun;		/* current tuning table */
extern short submenu;		/* submenu cursor switch */
extern short tunmod;		/* tuning table modified */
extern short tunval;		/* tuning table generator value */
extern short ttcmdsv;		/* tuning table editing state variable */
extern short tdnamsw;		/* tuning editor displaying typewriter */

extern short oldtun[];		/* previous tuning table for undo */
extern short tuntab[];		/* current tuning table */
extern short tunlib[][128];	/* tuning table library */

extern short panlkey[];		/* local key tunings */
extern short lclkmap[];		/* local key to MIDI key map */

extern char tuncurn[];		/* current tuning table name */
extern char tunname[][32];	/* tuning table names */
extern char sfdsp[];

extern unsigned *obj0, *obj2;

extern char bfs[];
extern char *tdbxlb[];

extern short tdbox[][8];

extern unsigned *tunob;

extern struct octent *tdoct;

/* 
*/

short tunpal[16][3] = {		/* color palette */

  {0, 0, 0},			/* 0 */
  {3, 3, 3},			/* 1 */
  {0, 0, 0},			/* 2 */
  {3, 3, 3},			/* 3 */
  {1, 1, 0},			/* 4 */
  {1, 0, 1},			/* 5 */
  {0, 1, 1},			/* 6 (was 0, 1, 0) */
  {0, 1, 1},			/* 7 (was 0, 1, 0) */
  {0, 0, 1},			/* 8 (was 0, 0, 2) */
  {0, 2, 3},			/* 9 (was 0, 3, 0) */
  {2, 2, 2},			/* 10 */
  {2, 3, 3},			/* 11 */
  {3, 3, 0},			/* 12 */
  {3, 3, 0},			/* 13 */
  {3, 0, 0},			/* 14 */
  {0, 0, 3}			/* 15 */
};

			/*   12345678901234567890123456789012   */
static char dfltnam[] = "Local 3rds + MIDI 12 tone scale ";

char *tdlabl[] = {

  "C", "#", "D", "#", "E", "F", "#", "G",
  "#", "A", "#", "B", "C", "#", "D", "#",
  "E", "F", "#", "G", "#", "A", "#", "B"
};

/* 
*/

/*
   =============================================================================
	gettun() -- retrieve a tuning table from the tuning table library
   =============================================================================
*/

gettun (n)
     short n;
{
  memcpyw (tuntab, tunlib[n], 128);
  memcpy (tuncurn, tunname[n], 32);
  curtun = n;
  tunmod = FALSE;
}

/*
   =============================================================================
	puttun() -- store a tuning table in the tuning table library
   =============================================================================
*/

puttun (n)
     short n;
{
  memcpyw (tunlib[n], tuntab, 128);
  memcpy (tunname[n], tuncurn, 32);
  tunmod = FALSE;
}

/* 
*/

/*
   =============================================================================
	inittt() -- initialize tuning table to equal tempered 12 tone scale
   =============================================================================
*/

inittt (n)
     short n;
{
  register short i;

  for (i = 0; i < 128; i++)
    tunlib[n][i] = ((i < 21) ? 160 : (i > 108) ? 10960 :
		    (160 + ((i - 12) * 100))) << 1;

  for (i = 0; i < 24; i++)
    tunlib[n][lclkmap[i]] = panlkey[i] << 1;

  strcpy (tunname[n], dfltnam);
}


/*
   =============================================================================
	inittl() -- initialize tuning table library
   =============================================================================
*/

inittl ()
{
  register short i;

  for (i = 0; i < NTUNS; i++)
    inittt (i);

  tunval = TUN_VAL << 1;
  gettun (0);
  memcpyw (oldtun, tuntab, 128);
}

/* 
*/

/*
   =============================================================================
	tt_trcp() -- transpose and copy tuning table values
   =============================================================================
*/

tt_trcp (start, finish, dest)
     short start, finish, dest;
{
  register short i;
  register long v;

  memcpyw (oldtun, tuntab, 128);	/* preserve old table for undo */

  if (start > finish)
    {

      for (i = finish; ((i LE start) AND (dest < 128)); i++)
	{

	  /* reverse copy */

	  v = oldtun[i] + (long) tunval;	/* transpose */

	  if (v GT (long) PITCHMAX)	/* limit */
	      v = (long) PITCHMAX;
	  else if (v LT (long) PITCHMIN)
	      v = (long) PITCHMIN;

	  tuntab[dest++] = (short) v;	/* store the value */
	}

    }
  else
    {

      for (i = start; ((i LE finish) AND (dest < 128)); i++)
	{

	  /* forward copy */

	  v = oldtun[i] + (long) tunval;	/* transpose */

	  if (v GT (long) PITCHMAX)	/* limit */
	      v = (long) PITCHMAX;
	  else if (v LT (long) PITCHMIN)
	      v = (long) PITCHMIN;

	  tuntab[dest++] = (short) v;	/* store the value */
	}
    }

  tunmod = TRUE;
}

/* 
*/

/*
   =============================================================================
	tt_intp() -- interpolate tuning table values
   =============================================================================
*/

short
tt_intp (from, to)
     short from, to;
{
  register short i, j, k, n;
  register long t;

  memcpyw (oldtun, tuntab, 128);	/* preserve old table for undo */

  if (from > to)
    {				/* adjust to and from for forward scan */

      i = from;
      from = to;
      to = i;
    }

  n = to - from;		/* get interval size */

  if (n < 2)
    return (FAILURE);

  k = tuntab[from];
  t = (((long) tuntab[to] - (long) k) << 16) / n;
  j = 1 + from;
  n--;

  for (i = 0; i < n; i++)
    tuntab[j++] = (short) ((t * (1 + i)) >> 16) + k;

  tunmod = TRUE;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	tt_incr() -- increment tuning table values
   =============================================================================
*/

short
tt_incr (from, to)
     short from, to;
{
  register short i;
  register long v;

  memcpyw (oldtun, tuntab, 128);	/* preserve old table for undo */

  if (from > to)
    {				/* adjust to and from for forward scan */

      i = from;
      from = to;
      to = i;

    }

  v = (long) oldtun[from];	/* initial value */

  if (from++ EQ to)		/* interval has to be at least 1 */
    return (FAILURE);

  for (i = from; i LE to; i++)
    {

      v += (long) tunval;	/* increment */

      if (v GT (long) PITCHMAX)	/* limit */
	  v = (long) PITCHMAX;
      else if (v LT (long) PITCHMIN)
	  v = (long) PITCHMIN;

      tuntab[i] = (short) v;	/* store the value */
    }

  tunmod = TRUE;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	td_trcp() -- display transpose select label
   =============================================================================
*/

td_trcp (mode)
     short mode;
{
  register unsigned cx;

  cx = exp_c (mode ? TDSELD : tdbox[6][4]);
  vbank (0);
  vcputsv (tunob, 64, cx, tdbox[6][5], 9, 54, "Transpose", 14);
  vcputsv (tunob, 64, cx, tdbox[6][5], 10, 54, "and Copy", 14);
}

/*
   =============================================================================
	td_incr() -- display increment select label
   =============================================================================
*/

td_incr (mode)
     short mode;
{
  register unsigned cx;

  cx = exp_c (mode ? TDSELD : tdbox[6][4]);
  vbank (0);
  vcputsv (tunob, 64, cx, tdbox[6][5], 12, 54, "Increment", 14);
}

/*
   =============================================================================
	td_intp() -- display interpolate select label
   =============================================================================
*/

td_intp (mode)
     short mode;
{
  register unsigned cx;

  cx = exp_c (mode ? TDSELD : tdbox[6][4]);
  vbank (0);
  vcputsv (tunob, 64, cx, tdbox[6][5], 14, 54, "Intrpolat", 14);
}

/* 
*/

/*
   =============================================================================
	advtcur() -- advance the tuning display text cursor
   =============================================================================
*/

advtcur ()
{
  register short newcol;

  if (infield (stcrow, stccol, curfet))
    cfetp = infetp;
  else
    return;

  newcol = stccol + 1;

  if (newcol LE cfetp->frcol)
    itcpos (stcrow, newcol);

  cxval = stccol * 8;
  cyval = stcrow * 14;
}

/*
   =============================================================================
	bsptcur() -- backspace the tuning display text cursor
   =============================================================================
*/

bsptcur ()
{
  register short newcol;

  if (infield (stcrow, stccol, curfet))
    cfetp = infetp;
  else
    return;

  newcol = stccol - 1;

  if (newcol GE cfetp->flcol)
    itcpos (stcrow, newcol);

  cxval = stccol * 8;
  cyval = stcrow * 14;
}

/* 
*/

/*
   =============================================================================
	dsttval() -- display a tuning table value
   =============================================================================
*/

dsttval (row, col, val, fg, bg)
     short row, col, val;
     unsigned fg, bg;
{
  register unsigned cfg, cbg;

  cfg = exp_c (fg);
  cbg = exp_c (bg);

  cnvc2p (bfs, (val >> 1));

  bfs[0] += '0';
  bfs[1] += 'A';
  bfs[2] = sfdsp[bfs[2] - 7];
  bfs[3] += '0';
  bfs[4] += '0';
  bfs[5] = '\0';

  vbank (0);
  vcputsv (tunob, 64, cfg, cbg, row, col, bfs, 14);
}

/* 
*/

/*
   =============================================================================
	tdswin() -- display a window
   =============================================================================
*/

tdswin (n)
     register short n;
{
  register short cx, i, tv;
  char ts;

  cx = exp_c (tdbox[n][5]);

  /* first, fill the box with the background color */

  vbank (0);
  vbfill4 (tunob, 128, tdbox[n][0], tdbox[n][1], tdbox[n][2],
	   tdbox[n][3], cx);

  /* put in the box label */

  tsplot4 (tunob, 64, tdbox[n][4], tdbox[n][6], tdbox[n][7], tdbxlb[n], 14);

/* 
*/
  switch (n)
    {				/* final text - overlays above stuff */

    case 0:			/* keys 0..23 */

      for (i = 0; i < 24; i++)
	{

	  tsplot4 (tunob, 64, TDLABEL, i, 1, tdlabl[i], 14);
	  sprintf (bfs, "%2d", 1 + i);
	  tsplot4 (tunob, 64, TDMKEYC, i, 3, bfs, 14);
	  dsttval (i, 6, tuntab[i],
		   ((tuntab[i] EQ 320) OR (tuntab[i] EQ 21920))
		   ? TDMKEYC : tdbox[n][4], tdbox[n][5]);
	}

      return;

    case 1:			/* keys 24..47 */

      for (i = 24; i < 48; i++)
	{

	  sprintf (bfs, "%2d", 1 + i);
	  tsplot4 (tunob, 64, TDMKEYC, i - 24, 13, bfs, 14);
	  dsttval (i - 24, 16, tuntab[i],
		   ((tuntab[i] EQ 320) OR (tuntab[i] EQ 21920))
		   ? TDMKEYC : tdbox[n][4], tdbox[n][5]);
	}

      return;

    case 2:			/* keys 48..71 */

      for (i = 48; i < 72; i++)
	{

	  sprintf (bfs, "%2d", 1 + i);
	  tsplot4 (tunob, 64, TDMKEYC, i - 48, 23, bfs, 14);
	  dsttval (i - 48, 26, tuntab[i],
		   ((tuntab[i] EQ 320) OR (tuntab[i] EQ 21920))
		   ? TDMKEYC : tdbox[n][4], tdbox[n][5]);
	}

      return;
/* 
*/
    case 3:			/* keys 72..95 */

      for (i = 72; i < 96; i++)
	{

	  sprintf (bfs, "%2d", 1 + i);
	  tsplot4 (tunob, 64, TDMKEYC, i - 72, 33, bfs, 14);
	  dsttval (i - 72, 36, tuntab[i],
		   ((tuntab[i] EQ 320) OR (tuntab[i] EQ 21920))
		   ? TDMKEYC : tdbox[n][4], tdbox[n][5]);
	}

      return;

    case 4:			/* keys 96..119 */

      for (i = 96; i < 120; i++)
	{

	  sprintf (bfs, "%3d", 1 + i);
	  tsplot4 (tunob, 64, TDMKEYC, i - 96, 43, bfs, 14);
	  dsttval (i - 96, 47, tuntab[i],
		   ((tuntab[i] EQ 320) OR (tuntab[i] EQ 21920))
		   ? TDMKEYC : tdbox[n][4], tdbox[n][5]);
	}

      return;

    case 5:			/* keys 120..127 */

      for (i = 120; i < 128; i++)
	{

	  sprintf (bfs, "%3d", 1 + i);
	  tsplot4 (tunob, 64, TDMKEYC, i - 120, 54, bfs, 14);
	  dsttval (i - 120, 58, tuntab[i],
		   ((tuntab[i] EQ 320) OR (tuntab[i] EQ 21920))
		   ? TDMKEYC : tdbox[n][4], tdbox[n][5]);
	}

      return;
/* 
*/
    case 6:

      td_trcp (0);
      td_incr (0);
      td_intp (0);
      tsplot4 (tunob, 64, tdbox[n][4], 16, 54, "Undo", 14);

      tv = (tunval GE 0 ? tunval : -tunval) >> 1;
      ts = tunval GE 0 ? '+' : '-';
      sprintf (bfs, "Val %c%04d", ts, tv);
      tsplot4 (tunob, 64, tdbox[n][4], 18, 54, bfs, 14);

      tsplot4 (tunob, 64, tdbox[n][4], 20, 54, "Store", 14);
      tsplot4 (tunob, 64, tdbox[n][4], 22, 54, "Retrieve", 14);

      tsplot4 (tunob, 64, tdbox[n][4], 24, 54, "Table #", 14);
      bfs[0] = curtun + '0';
      bfs[1] = '\0';
      tsplot4 (tunob, 64, tunmod ? TDCHGD : tdbox[n][4], 24, 61, bfs, 14);

      return;

    case 7:			/* tuning table name */

      tsplot4 (tunob, 64, tdbox[n][4], 24, 7, tuncurn, 14);
      return;
    }
}

/* 
*/

/*
   =============================================================================
	twins() -- display all tuning editor windows
   =============================================================================
*/

twins ()
{
  register short i;

  for (i = 0; i < 8; i++)
    tdswin (i);
}

/* 
*/

/*
   =============================================================================
	tundsp() -- put up the tuning display
   =============================================================================
*/

tundsp ()
{
  tunob = &v_score[0];		/* setup object pointer */
  obj0 = &v_curs0[0];		/* setup cursor object pointer */
  obj2 = &v_tcur[0];		/* setup typewriter object pointer */
  tdoct = &v_obtab[TUNOBJ];	/* setup object control table pointer */

  ttcmdsv = 0;			/* nothing selected */
  tdnamsw = FALSE;
  submenu = FALSE;

  dswap ();			/* initialize display */

  if (v_regs[5] & 0x0180)
    vbank (0);

  memsetw (tunob, 0, 32767);	/* clear the display */
  memsetw (tunob + 32767L, 0, 12033);

  SetObj (TUNOBJ, 0, 0, tunob, 512, 350, 0, 0, TUNFL, -1);
  SetObj (0, 0, 1, obj0, 16, 16, TDCURX, TDCURY, OBFL_00, -1);
  SetObj (TTCURS, 0, 1, obj2, 16, 16, 0, 0, TTCCFL, -1);

  arcurs (TDCURSR);		/* setup arrow cursor object */
  itcini (TDCURSR);		/* setup text cursor object */
  ttcini (TDTCURC);		/* setup typewriter cursor object */

  twins ();

  SetPri (TUNOBJ, TUNPRI);

  settc (YTOR (TDCURY), XTOC (TDCURX));	/* display the text cursor */

  vsndpal (tunpal);
}
