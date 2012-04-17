/*
   =============================================================================
	enterit.c -- MIDAS data entry and cursor support functions
	Version 40 -- 1989-12-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "m7ver.h"
#include "stddefs.h"
#include "biosdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "hwdefs.h"
#include "memory.h"
#include "fields.h"
#include "fpu.h"
#include "macros.h"
#include "panel.h"
#include "curpak.h"

#include "midas.h"
#include "instdsp.h"
#include "score.h"
#include "scdsp.h"
#include "scfns.h"
#include "wsdsp.h"

#if	DEBUGIT
extern short debugsw;

short debugen = 1;
#endif

extern short (*cx_key) (), (*cy_key) (), (*cx_upd) (), (*cy_upd) ();
extern short (*xy_up) (), (*xy_dn) (), (*not_fld) ();
extern short (*x_key) (), (*e_key) (), (*m_key) (), (*d_key) ();
extern short (*premove) (), (*pstmove) (), (*curtype) ();

extern short *cratex;
extern short *cratey;

extern short asig, aval, astat, aflag;
extern short xkstat, ykstat, xkcount, ykcount;
extern short cmtype, cmfirst, xycntr, curpos;
extern short cxrate, cyrate, cxval, cyval;
extern short ncvwait, nchwait, chwait, cvwait, cvtime, chtime;
extern short stcrow, stccol, runit, submenu, vtcrow, vtccol;
extern short trkball, tkctrl, txflag, tyflag;
extern short curhold, hcwval, thcwval, tvcwval, vcwval;

extern short sigtab[128][2];

extern short crate0[];

short syrate = SMYRATE;		/* smooth scroll vertical rate */

short LastRow = -1;
short LastCol = -1;

/* 
*/

/*
   =============================================================================
	enterit() -- standard data entry (ENTER) function  
   =============================================================================
*/

enterit ()
{
  if (! astat)		/* only on closures */
    return;

#if	DEBUGIT
  if (debugsw && debugen)
    printf ("enterit():  ENTRY  row = %d  col = %d  curfet =$%lX\n",
	    stcrow, stccol, curfet);
#endif

  if (infield (stcrow, stccol, curfet))
    {				/* in a field ? */

      cfetp = infetp;		/* set fet pointer */

      if ((cfetp) && (! ebflag))
	(*cfetp->ebto) (cfetp->ftags);	/* load ebuf */

      if (cfetp)
	(*cfetp->ebfrom) (cfetp->ftags);	/* process ebuf */

      if (cfetp)
	(*cfetp->redisp) (cfetp->ftags);	/* redisplay */

      ebflag = FALSE;
    }
}

/* 
*/

/*
   =============================================================================
	nokey() -- null key function
   =============================================================================
*/

short
nokey ()
{
  return (FALSE);
}

/* 
*/

/*
   =============================================================================
	cmvgen() -- general cursor mover
   =============================================================================
*/

cmvgen ()
{
  register short nc, newrow, newcol, newpos;

  (*premove) ();		/* handle PRE-MOVE functions */

  nc = (*curtype) ();		/* get new CURSOR TYPE wanted */

  newrow = YTOR (cyval);	/* setup new row */
  newcol = XTOC (cxval);	/* setup new column */

  if (cmtype != nc)
    {				/* if changed ... */

      nchwait = curhold;	/* set hold time */
      ncvwait = curhold;

      LastRow = -1;		/* reset last position */
      LastCol = -1;
    }

  /* see if we've got a new text cursor position */

  if ((newrow != LastRow) || (newcol != LastCol))
    newpos = TRUE;
  else
    newpos = FALSE;

  /* setup horizontal and vertical timer counts */

  chtime = (nc == CT_GRAF) ? hcwval : thcwval;
  cvtime = (nc == CT_GRAF) ? vcwval : ((nc == CT_SMTH) ? syrate : tvcwval);

  switch (nc)
    {				/* switch on new cursor type */

    case CT_GRAF:		/* GRAPHIC CURSOR */

      if (cmtype == CT_SCOR)
	{			/* change from score text */

	  stcoff ();		/* turn off text */
	  sgcon ();		/* turn on graphics */
	}

      cmtype = nc;		/* set cursor type */
      gcurpos (cxval, cyval);	/* position cursor */
      break;

/* 
*/
    case CT_TEXT:		/* TEXT CURSOR */

      cmtype = nc;		/* set cursor type */

      if (newpos)
	itcpos (newrow, newcol);	/* position cursor */

      break;

    case CT_VIRT:		/* VIRTUAL TYPEWRITER CURSOR */

      cmtype = nc;		/* set cursor type */
      ttcpos (vtcrow, vtccol);	/* position cursor */
      break;

    case CT_SCOR:		/* SCORE TEXT CURSOR */

      if (cmtype == CT_GRAF)	/* change from graphics */
	sgcoff ();		/* turn off graphics */

      cmtype = nc;		/* set cursor type */

      if (newpos)
	stcpos (newrow, newcol);	/* position cursor */

      break;

    case CT_SMTH:		/* SMOOTH SCROLL TEXT CURSOR */

      cmtype = nc;		/* set cursor type */

      if (newpos)
	ctcpos (newrow, newcol);	/* position cursor */

      break;

    case CT_MENU:		/* SUBMENU CURSOR */

      cmtype = nc;		/* set cursor type */
      mtcpos (vtcrow, vtccol);	/* position cursor */
      break;
    }

  LastRow = newrow;
  LastCol = newcol;

  (*pstmove) ();		/* handle POST-MOVE functions */
}

/* 
*/

/*
   =============================================================================
	crxrate() -- calculate cursor X rate
   =============================================================================
*/

short
crxrate (cv)
     register short cv;
{
  register short cs;

  if (cv >= xycntr)
    {

      cv -= xycntr;
      cs = 1;
      curpos = -cv;

    }
  else
    {

      cv = xycntr - cv;
      cs = 0;
      curpos = cv;
    }

  if (cv > 127)
    cv = 127;

  return (cs ? -cratex[cv] : cratex[cv]);
}

/* 
*/

/*
   =============================================================================
	cryrate() -- calculate cursor Y rate
   =============================================================================
*/

short
cryrate (cv)
     register short cv;
{
  register short cs;

  if (cv >= xycntr)
    {

      cv -= xycntr;
      cs = 1;
      curpos = -cv;

    }
  else
    {

      cv = xycntr - cv;
      cs = 0;
      curpos = cv;
    }

  if (cv > 127)
    cv = 127;

  return (cs ? -cratey[cv] : cratey[cv]);
}

/* 
*/

/*
   =============================================================================
	cmfix() -- cursor motion initial movement processing
   =============================================================================
*/

cmfix ()
{
  register short acx, acy, scx, scy;

  crxrate (sigtab[55][0]);	/* get cursor x value */
  acx = abs (curpos);

  cryrate (sigtab[56][0]);	/* get cursor y value */
  acy = abs (curpos);

  scx = sign (cxrate, 1);
  scy = sign (cyrate, 1);

  if (cmfirst)
    {				/* first motion ? */

      if (acx >= acy)
	{			/* vertical movement */

	  cyrate = 0;
	  cxrate = scx;
	  nchwait = curhold;
	  ncvwait = cvtime;

	}
      else
	{			/* horizontal movement */

	  cxrate = 0;
	  cyrate = scy;
	  ncvwait = curhold;
	  nchwait = chtime;
	}

      cmfirst = FALSE;

    }
  else
    {

      /* limit text movement to 1 axis */

      if (cmtype != CT_GRAF)
	if (acx >= acy)
	  cyrate = 0;
	else
	  cxrate = 0;
    }
}

/* 
*/

/*
   =============================================================================
	cxkstd() -- standard cursor x rate calculation
   =============================================================================
*/

cxkstd ()
{
  trkball = FALSE;
  tkctrl = FALSE;
  txflag = FALSE;
  tyflag = FALSE;

  if (astat)
    {				/* contact */

      if (xkstat == FALSE)
	{

	  if (xkcount)
	    {			/* debounce */

	      xkcount--;
	      return;
	    }

	  xkstat = TRUE;
	  chwait = 1;
	  nchwait = curhold;

	  if (ykstat)
	    (*xy_dn) ();
	}

      cxrate = -crxrate (aval);
/* 
*/
    }
  else
    {				/* release */

      if (xkstat && ykstat)
	(*xy_up) ();

      xkstat = FALSE;
      xkcount = 1;
      cxrate = 0;

      if (ykstat == FALSE)
	{

	  cyrate = 0;
	  ykcount = 1;
	  nchwait = chtime;
	  ncvwait = cvtime;
	  chwait = 1;
	  cvwait = 1;
	  cmfirst = TRUE;
	}
    }

  return;
}

/* 
*/

/*
   =============================================================================
	cykstd() -- standard cursor y rate calculation
   =============================================================================
*/

cykstd ()
{
  trkball = FALSE;
  tkctrl = FALSE;
  txflag = FALSE;
  tyflag = FALSE;

  if (astat)
    {				/* contact */

      if (ykstat == FALSE)
	{

	  if (ykcount)
	    {			/* debounce */

	      ykcount--;
	      return;
	    }

	  ykstat = TRUE;
	  cvwait = 1;
	  ncvwait = curhold;

	  if (xkstat)
	    (*xy_dn) ();
	}

      cyrate = cryrate (aval);
/* 
*/
    }
  else
    {				/* release */

      if (xkstat && ykstat)
	(*xy_up) ();

      ykstat = FALSE;
      ykcount = 1;
      cyrate = 0;

      if (xkstat == FALSE)
	{

	  cxrate = 0;
	  xkcount = 1;
	  nchwait = chtime;
	  ncvwait = cvtime;
	  chwait = 1;
	  cvwait = 1;
	  cmfirst = TRUE;
	}
    }

  return;
}

/* 
*/

/*
   =============================================================================
	stdmkey() -- standard M key processing
   =============================================================================
*/

stdmkey ()
{
  if (astat)
    {

      runit = FALSE;
      submenu = FALSE;
    }
}

/*
   =============================================================================
	stddkey() - standard data key processing
   =============================================================================
*/

stddkey ()
{
  if (infield (stcrow, stccol, curfet))
    {

      cfetp = infetp;		/* set field pointer */

      if (astat)
	{			/* only do action on closures */

	  if (!ebflag)
	    (*cfetp->ebto) (cfetp->ftags);	/* setup ebuf */

	  (*cfetp->datain) (cfetp->ftags, asig - 60);	/* enter */
	}

    }
  else
    {

      if (astat)
	(*not_fld) (asig - 60);	/* not in field */
    }
}

/* 
*/

/*
   =============================================================================
	cxgen() -- standard cursor x update processing
   =============================================================================
*/

cxgen ()
{
  cxval += cxrate;

  if (cxval > CXMAX)
    cxval = CXMAX;
  else if (cxval < 0)
    cxval = 0;

  return;
}

/*
   =============================================================================
	cygen() -- standard cursor y update processing
   =============================================================================
*/

cygen ()
{
  cyval += cyrate;

  if (cyval > CYMAX)
    cyval = CYMAX;
  else if (cyval < 0)
    cyval = 0;

  return;
}
