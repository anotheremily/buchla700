/*
   =============================================================================
	wsdsp.c -- MIDAS waveshape editor display driver
	Version 35 -- 1988-09-09 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "memory.h"

#include "fpu.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "graphdef.h"
#include "charset.h"
#include "fields.h"

#include "midas.h"
#include "instdsp.h"
#include "wsdsp.h"

/* things defined elsewhere */

extern unsigned exp_c ();

extern int (*point) ();

extern unsigned *obj0;

extern char bfs[];

extern char *wdbxlb0[];
extern char *wdbxlb1[];

extern short wdbox[][8];

extern struct instdef vbufs[];

/* 
*/

extern short curinst;
extern short curvce;
extern short curwave;		/* current waveshape library slot */
extern short curwdth;		/* current waveshape cursor width */
extern short curwfnl;		/* current waveshape final value */
extern short curwhrm;		/* current waveshape harmonic number */
extern short curwhrv;		/* current waveshape harmonic value */
extern short curwpnt;		/* current waveshape point number */
extern short curwoff;		/* current waveshape offset value */
extern short curwslt;		/* current waveshape instrument slot */
extern short cxval;
extern short cyval;
extern short lstwoff;
extern short lstwpnt;
extern short stccol;
extern short stcrow;
extern short wpntsv;		/* waveshape point selection state variable */
extern short wplast;
extern short wvlast;

extern short vmtab[NUMHARM];	/* harmonic table */
extern short wsbuf[NUMWPCAL];	/* waveshape generation buffer */

extern long vknm[NUMHARM][NUMWPCAL];	/* scaled harmonics */

extern unsigned *waveob;	/* waveshape display object pointer */

extern short wsnmod[12][2];	/* waveshape number or data modified */

extern struct wstbl wslib[NUMWAVS];	/* waveshape library */

extern struct octent *wdoct;	/* object control table pointer */

/* forward references */

short wdpoint ();

/* 
*/

short wavpal[16][3] = {		/* waveshape display color palette */

  {0, 0, 0},			/* 0 */
  {3, 3, 3},			/* 1 */
  {2, 2, 2},			/* 2 */
  {0, 1, 1},			/* 3 (was 0, 1, 0) */
  {1, 0, 1},			/* 4 */
  {0, 1, 1},			/* 5 (was 0, 1, 0) */
  {2, 1, 2},			/* 6 */
  {0, 3, 0},			/* 7 */
  {2, 0, 0},			/* 8 */
  {2, 0, 2},			/* 9 */
  {0, 0, 0},			/* 10 */
  {2, 3, 3},			/* 11 */
  {3, 3, 0},			/* 12 */
  {3, 0, 0},			/* 13 */
  {0, 0, 1},			/* 14 (was 0, 0, 2) */
  {0, 0, 3}			/* 15 */
};

/* 
*/

/*
   =============================================================================
	advwcur() -- advance the waveshape display text cursor
   =============================================================================
*/

advwcur ()
{
  register short newcol;

  newcol = stccol + 1;

  if (newcol <= cfetp->frcol)
    itcpos (stcrow, newcol);
}

/* 
*/

/*
   =============================================================================
	hdraw(h) -- draw harmonic 'h' for the current waveshape
   =============================================================================
*/

hdraw (hv, h)
     register short *hv;
     short h;
{
  register short bc, bx, by, j;

  bx = (h << 4) + 4;

  if (hv[h] < 0)
    {

      by = WBOFF - ((-hv[h] * WBSF1) / WBSF2);
      bc = WBCN;

    }
  else
    {

      by = WBOFF - ((hv[h] * WBSF1) / WBSF2);
      bc = WBCP;
    }

  for (j = 0; j < 8; j++)
    {

      lseg (bx, WBOFF, bx, by, bc);
      ++bx;
    }
}

/* 
*/

/*
   =============================================================================
	dsws() -- display the current waveshape
   =============================================================================
*/

dsws (how)
     short how;
{
  register struct instdef *ip;
  register short *fv, *hv, *ov;
  register short i;
  short cx;

  cx = exp_c (wdbox[0][0]);

  ip = &vbufs[curvce];
  fv = curwslt ? &ip->idhwvbf : &ip->idhwvaf;
  ov = curwslt ? &ip->idhwvbo : &ip->idhwvao;
  hv = curwslt ? &ip->idhwvbh : &ip->idhwvah;

  point = wdpoint;

  if (v_regs[5] & 0x0180)
    vbank (0);

  if (how)
    vbfill4 (waveob, 128, wdbox[0][0], wdbox[0][1],
	     wdbox[0][2], wdbox[0][3], cx);

  lseg (1, 133, 510, 133, WZBC);	/* draw the zero line and ... */
  lseg (1, (WPOFF << 1), 510, (WPOFF << 1), WZBC);	/* ... bottom limit */

  for (i = 0; i < NUMHARM; i++)	/* draw the harmonics */
    hdraw (hv, i);

  for (i = 0; i < NUMWPNT; i++)
    {				/* draw the values */

      /* offset values */

      wdpoint (((2 * i) + 2),
	       (WPOFF - (((ov[i] >> 5) * WPSF1) / WPSF2)), WOVC);

      /* final values */

      wdpoint (((2 * i) + 2),
	       (WPOFF - (((fv[i] >> 5) * WPSF1) / WPSF2)), WFVC);
    }
}

/* 
*/

/*
   =============================================================================
	wdswin() -- display a window
   =============================================================================
*/

wdswin (n)
     register short n;
{
  register short cx, wval;
  register char wsgn;

  cx = wdbox[n][5];
  cx |= cx << 4;
  cx |= cx << 8;

  /* first, fill the box with the background color */

  if (v_regs[5] & 0x0180)
    vbank (0);

  vbfill4 (waveob, 128, wdbox[n][0], wdbox[n][1], wdbox[n][2],
	   wdbox[n][3], cx);

  /* put in the box label */

  tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6], wdbox[n][7], wdbxlb0[n], 14);

  tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6] + 1, wdbox[n][7],
	   wdbxlb1[n], 14);

/* 
*/
  switch (n)
    {				/* final text - overlays above stuff */

    case 0:			/* points and bars */

      dsws (0);
      return;

    case 2:			/* waveshape - voice - instrument - slot */

      sprintf (bfs, "%02d", curwave + 1);
      tsplot4 (waveob, 64,
	       exp_c (wsnmod[curvce][curwslt] ? WS_CHGC : wdbox[n][4]),
	       wdbox[n][6], wdbox[n][7] + WAVE_OFF, bfs, 14);

      sprintf (bfs, "%02d", curvce + 1);
      tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6],
	       wdbox[n][7] + WVCE_OFF, bfs, 14);

      sprintf (bfs, "%02d", curinst);
      tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6] + 1,
	       wdbox[n][7] + WINS_OFF, bfs, 14);

      sprintf (bfs, "%c", curwslt + 'A');
      tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6] + 1,
	       wdbox[n][7] + WSLT_OFF, bfs, 14);

      return;

/* 
*/
    case 4:			/* point - offset - width - final */

      sprintf (bfs, "%03d", curwpnt);
      tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6],
	       wdbox[n][7] + WPNT_OFF, bfs, 14);

      if (curwoff < 0)
	{

	  wval = -curwoff;
	  wsgn = '-';

	}
      else
	{

	  wval = curwoff;
	  wsgn = '+';
	}

      sprintf (bfs, "%c%04d", wsgn, wval);
      tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6],
	       wdbox[n][7] + WOFF_OFF, bfs, 14);

      if (curwdth == NUMWIDS)
	{

	  tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6] + 1,
		   wdbox[n][7], "Interp", 14);

	}
      else
	{

	  tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6] + 1,
		   wdbox[n][7], "Width", 14);

	  sprintf (bfs, "%01d", curwdth);
	  tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6] + 1,
		   wdbox[n][7] + WDTH_OFF, bfs, 14);
	}

      if (curwfnl < 0)
	{

	  wval = -curwfnl;
	  wsgn = '-';

	}
      else
	{

	  wval = curwfnl;
	  wsgn = '+';
	}

      sprintf (bfs, "%c%04d", wsgn, wval);
      tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6] + 1,
	       wdbox[n][7] + WFNL_OFF, bfs, 14);

      return;
/* 
*/
    case 5:			/* harmonic - value */

      sprintf (bfs, "%02d", curwhrm + 1);
      tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6],
	       wdbox[n][7] + WHRM_OFF, bfs, 14);

      if (curwhrv < 0)
	{

	  wval = -curwhrv;
	  wsgn = '-';

	}
      else
	{

	  wval = curwhrv;
	  wsgn = '+';
	}

      sprintf (bfs, "%c%03d", wsgn, wval);
      tsplot4 (waveob, 64, wdbox[n][4], wdbox[n][6] + 1,
	       wdbox[n][7] + WHRV_OFF, bfs, 14);

      return;

    }
}

/* 
*/

/*
   =============================================================================
	wwins() -- display all waveshape editor windows
   =============================================================================
*/

wwins ()
{
  register short i;

  for (i = 0; i < 6; i++)
    wdswin (i);
}

/*
   =============================================================================
	wdpoint() -- plot a point for the lseg function
   =============================================================================
*/

wdpoint (x, y, pen)
     short x, y, pen;
{
  if (v_regs[5] & 0x0180)
    vbank (0);

  vputp (wdoct, x, y, pen);
}

/* 
*/

/*
   =============================================================================
	wdbord() -- draw the border for the waveshape display
   =============================================================================
*/

wdbord ()
{
  point = wdpoint;

  lseg (0, 0, 511, 0, WBORD);	/* outer border */
  lseg (511, 0, 511, 349, WBORD);
  lseg (511, 349, 0, 349, WBORD);
  lseg (0, 349, 0, 0, WBORD);

  lseg (0, 308, 511, 308, WBORD);	/* windows - H lines */
  lseg (511, 321, 0, 321, WBORD);

  lseg (175, 322, 175, 349, WBORD);	/* windows - V lines */
  lseg (231, 322, 231, 349, WBORD);
  lseg (399, 322, 399, 349, WBORD);
}

/* 
*/

/*
   =============================================================================
	clrws() -- initialize waveshape to null values
   =============================================================================
*/

clrws ()
{
  register struct instdef *ip;

  ip = &vbufs[curvce];

  if (curwslt)
    {

      memsetw (ip->idhwvbh, 0, NUMHARM);
      memsetw (ip->idhwvbo, 0, NUMWPNT);
      memsetw (ip->idhwvbf, 0, NUMWPNT);

    }
  else
    {

      memsetw (ip->idhwvah, 0, NUMHARM);
      memsetw (ip->idhwvao, 0, NUMWPNT);
      memsetw (ip->idhwvaf, 0, NUMWPNT);
    }

  clrwsa ();

  lstwpnt = wplast = curwpnt;

  lstwoff = wvlast = curwfnl = curwoff = 0;

  curwhrv = 0;

  updfpu ();			/* update the FPU */
  wsnmod[curvce][curwslt] = TRUE;	/* tag WS as modified */
}

/* 
*/

/*
   =============================================================================
	iniwslb() -- initialize waveshape library
   =============================================================================
*/

iniwslb ()
{
  register short i, j;

  memsetw (wsnmod, FALSE, (sizeof wsnmod) / 2);

  for (i = 0; i < NUMWAVS; i++)
    {

      for (j = 0; j < NUMWPNT; j++)
	{

	  wslib[i].final[j] = ((j + 1) << 8) ^ 0x8000;
	  wslib[i].offset[j] = ((j + 1) << 8) ^ 0x8000;
	}

      memsetw (wslib[i].harmon, 0, NUMHARM);	/* zero harmonics */
    }
}

/* 
*/

/*
   =============================================================================
	wsdsp() -- put up the waveshape display
   =============================================================================
*/

wsdsp ()
{
  waveob = &v_score[0];		/* setup object pointer */
  obj0 = &v_curs0[0];		/* setup cursor object pointer */
  wdoct = &v_obtab[WAVEOBJ];	/* setup object control table pointer */

  wpntsv = 0;			/* point selection state = unselected */
  newws ();			/* set editing variables */

  dswap ();			/* initialize display */

  if (v_regs[5] & 0x0180)
    vbank (0);

  memsetw (waveob, 0, 32767);
  memsetw (waveob + 32767L, 0, 12033);

  SetObj (WAVEOBJ, 0, 0, waveob, 512, 350, 0, 0, WAVEFL, -1);
  SetObj (0, 0, 1, obj0, 16, 16, WCURX, WCURY, OBFL_00, -1);

  arcurs (WDCURS);		/* setup arrow cursor object */
  itcini (WDCURS);		/* setup text cursor object */

  wdbord ();			/* draw the border */
  wwins ();

  SetPri (WAVEOBJ, WAVEPRI);
  SetPri (0, GCPRI);
  setgc (WCURX, WCURY);		/* display the graphic cursor */

  vsndpal (wavpal);		/* send the palette */
}
