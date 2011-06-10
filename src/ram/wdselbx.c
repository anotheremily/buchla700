/*
   =============================================================================
	wdselbx.c -- waveshape editor box selection functions
	Version 27 -- 1988-09-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "graphdef.h"
#include "hwdefs.h"
#include "fpu.h"

#include "midas.h"
#include "instdsp.h"
#include "wsdsp.h"

extern short enterit (), wdswin ();

extern short curvce, curwdth, cursbox, hitbox, cxval, cyval, hitcx, hitcy;
extern short curwave, curwfnl, curwpnt, curwoff, curwhrm, curwhrv, curwslt;
extern short wdupdfl, wpntsv, wplast, wvlast, lstwoff, lstwpnt;

extern short offsets[NUMWPCAL];
extern short vmtab[NUMHARM];
extern short wsbuf[NUMWPCAL];

extern long vknm[NUMHARM][NUMWPCAL];

extern short wdbox[][8];

extern short wsnmod[12][2];

extern short crate1[], crate2[], *cratex, *cratey;

extern struct selbox *csbp, *curboxp;

extern struct instdef vbufs[];

extern struct wstbl wslib[];

/* forward references */

short bx_null (), wdfnbox ();

/* 
*/

struct selbox wdboxes[] = {

  {1, 1, 510, 307, 0, wdfnbox},	/*  0 */
  {1, 308, 510, 320, 1, wdfnbox},	/*  1 */
  {1, 322, 174, 348, 2, enterit},	/*  2 */
  {176, 322, 230, 348, 3, wdfnbox},	/*  3 */
  {232, 322, 398, 348, 4, enterit},	/*  4 */
  {400, 322, 510, 348, 5, enterit},	/*  5 */

  {0, 0, 0, 0, 0, FN_NULL}	/* end of table */
};

/*
   =============================================================================
	dsnewws() -- display a new waveshape
   =============================================================================
*/

dsnewws ()
{
  wdswin (0);
  wdswin (2);
  wdswin (4);
  wdswin (5);
}

/* 
*/

/*
   =============================================================================
	newws() -- setup editing for a new waveshape
   =============================================================================
*/

newws ()
{
  register short i;
  register struct instdef *ip;

  ip = &vbufs[curvce];

  if (curwslt)
    {

      for (i = 0; i < NUMWPNT; i++)
	{

	  offsets[i + 1] = ip->idhwvbo[i] >> 5;
	  wsbuf[i + 1] = ip->idhwvbf[i] >> 5;
	}

      memcpyw (vmtab, ip->idhwvbh, NUMHARM);

    }
  else
    {

      for (i = 0; i < NUMWPNT; i++)
	{

	  offsets[i + 1] = ip->idhwvao[i] >> 5;
	  wsbuf[i + 1] = ip->idhwvaf[i] >> 5;
	}

      memcpyw (vmtab, ip->idhwvah, NUMHARM);
    }

  offsets[0] = offsets[1];
  wsbuf[0] = wsbuf[1];

  curwhrv = vmtab[curwhrm];
  lstwoff = wvlast = curwoff = offsets[1 + curwpnt];
  curwfnl = wsbuf[curwpnt + 1];
  lstwpnt = wplast = curwpnt;

  memsetw (vknm, 0, (NUMHARM * NUMWPCAL) << 1);
  wadj ();
}

/* 
*/

/*
   =============================================================================
	wdfnbox() -- waveshape display box hit processor
   =============================================================================
*/

short
wdfnbox (n)
     short n;
{
  register short wval;
  register char wsgn;
  register short *fpuws;

  switch (n)
    {				/* switch off of window ID */

    case 0:			/* process a hit in the main window */

      switch (wpntsv)
	{

	case 0:		/* nothing selected so far:  select a point */

	  wpntsv = 1;
	  curwpnt = (cxval - 2) / 2;

	  if (curwpnt GE NUMWPNT)
	    curwpnt = NUMWPNT - 1;
	  else if (curwpnt < 0)
	    curwpnt = 0;

	  cratex = crate2;
	  cratey = crate2;
	  newws ();
	  cyval = WPOFF - ((curwoff * WPSF1) / WPSF2);
	  cxval = (curwpnt << 1) + 2;
	  arcurs (WS_GRAB);
	  gcurpos (cxval, cyval);
	  wplast = curwpnt;
	  wvlast = curwoff;
	  wdswin (4);
	  break;
/* 
*/
	case 1:		/* point was selected:  unselect it */

	  wpntsv = 0;
	  cratex = crate1;
	  cratey = crate1;

	  arcurs (WDCURS);
	  gcurpos (cxval, cyval);
	  break;

	case 2:		/* harmonic was selected:  unselect it */

	  wpntsv = 0;
	  cratex = crate1;
	  cratey = crate1;

	  arcurs (WDCURS);
	  settc (22, 1 + (curwhrm << 1));
	  break;
	}

      return (TRUE);

    case 1:			/* process a hit in the harmonic legend */

      wpntsv = 2;
      cratex = crate1;
      cratey = crate1;

      curwhrm = cxval >> 4;
      curwhrv = vmtab[curwhrm];

      cxval = (curwhrm << 4) + 8;

      if (curwhrv < 0)
	{

	  cyval = WBOFF - ((-curwhrv * WBSF1) / WBSF2);

	}
      else
	{

	  cyval = WBOFF - ((curwhrv * WBSF1) / WBSF2);
	}

      arcurs (WS_GRAB);
      gcurpos (cxval, cyval);
      wdswin (5);
      return (TRUE);
/* 
*/
    case 3:			/* process a hit in the store & fetch window */

      wsnmod[curvce][curwslt] = FALSE;

      if (cyval < 336)
	{			/* store ? */

	  memcpyw (&wslib[curwave],
		   curwslt ? vbufs[curvce].idhwvbf
		   : vbufs[curvce].idhwvaf, NUMHARM + (2 * NUMWPNT));

	  wdswin (2);

	}
      else
	{			/* retrieve */

	  memcpyw (curwslt ? vbufs[curvce].idhwvbf
		   : vbufs[curvce].idhwvaf,
		   &wslib[curwave], NUMHARM + (2 * NUMWPNT));

	  updfpu ();
	  newws ();
	  dsnewws ();
	}

      cratex = crate1;
      cratey = crate1;
      return (TRUE);

    default:			/* anywhere else is an error */

      cratex = crate1;
      cratey = crate1;
      return (FALSE);
    }
}
