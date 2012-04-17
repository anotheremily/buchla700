/*
   =============================================================================
	idselbx.c -- instrument definition box selection functions
	Version 46 -- 1988-09-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "graphdef.h"

#include "midas.h"
#include "instdsp.h"

extern short (*itxput) ();

extern short lseg (), dswin (), tsplot4 (), vbfill4 (), enterit (), vtdisp ();
extern short advicur (), bspicur ();

extern short drawfn (), selpnt (), ttox (), vtoy ();

extern unsigned timeto (), exp_c ();

/* 
*/

extern short cursbox, cxval, cyval;
extern short curvce, curfunc, curpnt;
extern short hitbox, hitcx, hitcy, idnamsw, idimsw, idtdat;
extern short pntsv, pecase, stccol, subj, submenu;

extern unsigned temin, temax;

extern struct selbox *csbp, *curboxp;
extern struct instdef vbufs[];
extern struct instdef idefs[];

extern struct instpnt *pntptr;

extern short idbox[][8];
extern short instmod[];

extern char *vtdeptr;

extern char *idbxlbl[];
extern char idhlbl[];
extern char bfs[];

extern unsigned *instob;

/* forward references */

short bx_null (), idfnbox ();

/* 
*/

struct selbox idboxes[] = {

  {1, 1, 84, 26, 0, idfnbox},	/*  0: Freq 1 */
  {86, 1, 169, 26, 1, idfnbox},	/*  1: Freq 2 */
  {171, 1, 254, 26, 2, idfnbox},	/*  2: Freq 3 */
  {256, 1, 339, 26, 3, idfnbox},	/*  3: Freq 4 */
  {341, 1, 424, 26, 4, idfnbox},	/*  4: Filter / Resonance */
  {426, 1, 509, 26, 5, idfnbox},	/*  5: Location */

  {1, 28, 84, 54, 6, idfnbox},	/*  6: Index 1 */
  {86, 28, 169, 54, 7, idfnbox},	/*  7: Index 2 */
  {171, 28, 254, 54, 8, idfnbox},	/*  8: Index 3 */
  {256, 28, 339, 54, 9, idfnbox},	/*  9: Index 4 */
  {341, 28, 424, 54, 10, idfnbox},	/* 10: Index 5 */
  {426, 28, 509, 54, 11, idfnbox},	/* 11: Index 6 */

  {1, 56, 509, 208, 12, idfnbox},	/* 12: Level */

  {1, 210, 110, 236, 0x0000, enterit},	/* 13: Source- Mlt */
  {112, 210, 142, 236, 0x0000, enterit},	/* 14: Pt */
  {144, 210, 206, 236, 0x0000, enterit},	/* 15: Time */
  {208, 210, 366, 236, 0x0000, enterit},	/* 16: Value */
  {368, 210, 509, 236, 0x0000, enterit},	/* 17: Action */

  {1, 238, 131, 348, 0x0000, idfnbox},	/* 18: Algorithm */
  {133, 238, 267, 251, 0x0000, idfnbox},	/* 19: Voice & Inst */
  {269, 238, 379, 306, 0x0000, enterit},	/* 20: Oscillators */
  {381, 238, 509, 348, 0x0000, idfnbox},	/* 21: Waveshape */

  {133, 308, 379, 348, 0x0000, enterit},	/* 22: Variables */

  {133, 252, 267, 306, 19, idfnbox},	/* 23: Name & Comments */

  {0, 0, 0, 0, 0, FN_NULL}	/* end of table */
};

/* 
*/

/*
   =============================================================================
	idtxput() -- put out instrument menu text for the instrument display
   =============================================================================
*/

idtxput (row, col, txt, tag)
     short row, col;
     char *txt;
     short tag;
{
  if (v_regs[5] & 0x0180)
    vbank (0);

  tsplot4 (instob, 64, CFBX19, row, col, txt, 14);
}

/*
   =============================================================================
	showim() -- show the instrument menu
   =============================================================================
*/

showim ()
{
  register short row, col, ins;

  ins = 0;

  for (col = 1; col < 46; col += 22)
    {

      (*itxput) (0, col, "No Instrument", 0);

      for (row = 1; row < 15; row++)
	{

	  if (ins >= NINST)
	    break;

	  sprintf (bfs, "%02d %-16.16s", ins, idefs[ins].idhname);
	  (*itxput) (row, col, bfs, 1);
	  ++ins;
	}
    }
}

/* 
*/

/*
   =============================================================================
	cdnicur() -- instrument definition virtual typewriter -- cursor down
   =============================================================================
*/

cdnicur ()
{
  register struct instdef *ip;

  ip = &vbufs[curvce];

  switch (idtdat)
    {

    case 0:
      idtdat = 1;
      vtdeptr = ip->idhcom1;
      settc (19, stccol);
      return;

    case 1:
      idtdat = 2;
      vtdeptr = ip->idhcom2;
      settc (20, stccol);
      return;

    case 2:
      idtdat = 3;
      vtdeptr = ip->idhcom3;
      settc (21, stccol);
      return;

    case 3:
      idtdat = 0;
      vtdeptr = ip->idhname;
      settc (18, stccol);
      return;
    }
}

/* 
*/

/*
   =============================================================================
	cupicur() -- instrument definition virtual typewriter -- cursor up
   =============================================================================
*/

cupicur ()
{
  register struct instdef *ip;

  ip = &vbufs[curvce];

  switch (idtdat)
    {

    case 0:
      idtdat = 3;
      vtdeptr = ip->idhcom3;
      settc (21, stccol);
      return;

    case 1:
      idtdat = 0;
      vtdeptr = ip->idhname;
      settc (18, stccol);
      return;

    case 2:
      idtdat = 1;
      vtdeptr = ip->idhcom1;
      settc (19, stccol);
      return;

    case 3:
      idtdat = 2;
      vtdeptr = ip->idhcom2;
      settc (20, stccol);
      return;
    }
}

/* 
*/

/*
   =============================================================================
	edfunc() -- setup to edit a function
   =============================================================================
*/

short
edfunc (n)
     short n;
{
  register unsigned cx;
  register struct instdef *ip;

  ip = &vbufs[curvce];

  /* first, put level in selected window */

  cx = exp_c (ID_LVLC);		/* label background = level background */

  vbank (0);
  vbfill4 (instob, 128, idbox[n][0], idbox[n][1],
	   idbox[n][2], idbox[n][3], cx);

  tsplot4 (instob, 64,
	   ((ip->idhfnc[12].idftmd & I_TM_KEY) ? idbox[n][4] : ID_INST),
	   idbox[n][6], idbox[n][7], idbxlbl[12], 14);

  if (n != 12)			/* if not editing the level, draw it in the label */
    drawfn (12, 0, ID_CLVL, n);

/* 
*/
  /* next, put the level and selected functions in the level window */

  vbank (0);			/* clear the edit/level  box */
  vbfill4 (instob, 128, idbox[12][0], idbox[12][1],
	   idbox[12][2], idbox[12][3], cx);

  /* label the function */

  tsplot4 (instob, 64,
	   ((ip->idhfnc[n].idftmd & I_TM_KEY) ? idbox[n][4] : ID_INST),
	   idbox[12][6], idbox[12][7], idbxlbl[n], 14);

  /* scale -- x labels */

  tsplot4 (instob, 64, TGRID, 14, 0, idhlbl, 14);

  /* scale -- y labels */

  tsplot4 (instob, 64, TGRID, 14, 0, "\300", 14);
  tsplot4 (instob, 64, TGRID, 12, 0, "\302", 14);
  tsplot4 (instob, 64, TGRID, 10, 0, "\304", 14);
  tsplot4 (instob, 64, TGRID, 8, 0, "\306", 14);
  tsplot4 (instob, 64, TGRID, 6, 0, "\310", 14);
  tsplot4 (instob, 64, TGRID, 4, 0, "\312", 14);

  lseg (7, 56, 7, 196, LGRID);	/* draw the scale */
  lseg (7, 196, 509, 196, LGRID);

  /* plot functions:  first level, then selected function */

  if (n != 12)			/* if not editing level, draw it as a reference */
    drawfn (12, 0, ID_CLVL, 12);

  drawfn (n, pntsv, ID_CLIN, 12);

  curfunc = n;			/* make n the current function */

  if (curfunc == 4)
    {

      tsplot4 (instob, 64,
	       ((ip->idhfnc[n].idftmd & I_TM_KEY) ?
		idbox[12][4] : ID_INST),
	       idbox[12][6] + 1, idbox[12][7] + 4, "Res", 14);

      rd_ires (12);
    }

  return (TRUE);
}

/* 
*/

/*
   =============================================================================
	showpt() -- show the data about a point
   =============================================================================
*/

showpt (q)
     short q;
{
  register short ptx, pty, i, j, k;
  register struct instdef *ip;
  short ptc;

  if (q)
    dswin (14);			/* point number */

  dswin (15);			/* time */
  dswin (16);			/* value */
  dswin (17);			/* action */

  ip = &vbufs[curvce];
  j = ip->idhfnc[curfunc].idfpif;
  k = ip->idhfnc[curfunc].idfpt1;

  for (i = 0; i < j; i++)
    {

      ptx = ttox (timeto (curfunc, i), 12);
      pty = vtoy ((ip->idhpnt[k + i].ipval) >> 5, 12);
      ptc = ((ip->idhpnt[k + i].ipvsrc) || (ip->idhpnt[k + i].ipact) ?
	     ID_ACTP : ID_CPNT);

      drawpt (ptx, pty, ptc);
    }

  ptx = ttox (timeto (curfunc, subj), 12);
  pty = vtoy ((ip->idhpnt[k + subj].ipval) >> 5, 12);
  drawpt (ptx, pty, ID_SELD);
}

/* 
*/

/*
   =============================================================================
	endityp() -- exit instrument definition virtual typewriter
   =============================================================================
*/

endityp ()
{
  idnamsw = FALSE;
  submenu = FALSE;
  idvlblc ();
  dswin (19);
  dswin (22);
}

/*
   =============================================================================
	showfn() show the data about a function
   =============================================================================
*/

showfn ()
{
  dswin (13);			/* source multiplier */
  showpt (1);			/* point, time, value, action */
}

/*
   =============================================================================
	bx_null() -- null item selection processor
   =============================================================================
*/

short
bx_null ()
{
  return (TRUE);
}

/*
   =============================================================================
	iclrwin() -- clear an instrument editor window
   =============================================================================
*/

iclrwin (n)
     register short n;
{
  if (v_regs[5] & 0x0180)
    vbank (0);

  vbfill4 (instob, 128, idbox[n][0], idbox[n][1], idbox[n][2], idbox[n][3],
	   exp_c (idbox[n][5]));
}

/* 
*/

/*
   =============================================================================
	idfnbox() -- instrument display function box hit processor
   =============================================================================
*/

short
idfnbox (n)
     short n;
{
  register short col, i, row;
  register struct instdef *ip;
  register char *ivtdptr;

  ip = &vbufs[curvce];

  row = hitcy / 14;
  col = hitcx >> 3;

/* 
*/
  if (hitbox == 12)
    {				/* process a hit in the level window */

      /* handle data entry special case for resonance in filter fn. */

      if ((curfunc == 4) && (row == 5) && (col == 62))
	return (enterit ());

      switch (pntsv)
	{			/* dispatch off of point select state */

	case 0:		/* nothing selected so far:  select a point */

	  subj = selpnt ();	/* decide on the point */
	  pntsel ();		/* make it current */
	  pntsv = 1;		/* say it's selected */

	  edfunc (curfunc);	/* update the display */
	  showpt (1);

	  /* set cursor to point */

	  cxval = ttox (timeto (curfunc, subj), 12);
	  cyval = vtoy ((pntptr->ipval >> 5), 12);
	  arcurs (ID_SELD);
	  gcurpos (cxval, cyval);
	  break;

	case 1:		/* point was selected:  unselect it */

	  pntsv = 0;
	  edfunc (curfunc);
	  arcurs (ID_NCUR);
	  gcurpos (cxval, cyval);
	  break;
	}

      return (TRUE);

    }
  else if (hitbox == 18)
    {				/* configuration */

      if ((row == 17) && (col >= 4) && (col <= 9))
	{

	  wcmenu (0);
	  return (TRUE);

	}
      else
	{

	  return (enterit ());
	}
/* 
*/
    }
  else if (hitbox == 19)
    {				/* voice & inst. */

      if ((col >= 26) && (col <= 29) && (row == 17))
	{

	  if (idimsw)
	    {			/* menu is up, take it down */

	      idimsw = FALSE;

	      if (v_regs[5] & 0x0180)
		vbank (0);

	      idbord ();
	      allwins ();

	    }
	  else
	    {			/* menu is down, put it up */

	      idimsw = TRUE;
	      itxput = idtxput;

	      if (v_regs[5] & 0x0180)
		vbank (0);

	      vbfill4 (instob, 128, 1, 1, 510, 208, exp_c (idbox[19][5]));

	      for (i = 13; i <= 18; i++)
		iclrwin (i);

	      for (i = 20; i <= 22; i++)
		iclrwin (i);

	      showim ();
	    }

	  return (SUCCESS);

	}
      else
	{

	  return (enterit ());	/* do data entry */
	}
/* 
*/
    }
  else if (hitbox == 21)
    {				/* waveshapes */

      if ((row == 17) && (col >= 48) && (col <= 54))
	{

	  wcmenu (1);
	  return (TRUE);

	}
      else
	{

	  return (enterit ());
	}

    }
  else if (hitbox == 23)
    {				/* process a hit in the name window */

      if (idnamsw)
	{			/* typewriter up - enter data */

	  vtyper ();
	  instmod[curvce] = TRUE;

	}
      else
	{			/* put up the typewriter */

	  /* set data pointer */

	  if (row == 18)
	    ivtdptr = ip->idhname;
	  else if (row == 19)
	    ivtdptr = ip->idhcom1;
	  else if (row == 20)
	    ivtdptr = ip->idhcom2;
	  else
	    ivtdptr = ip->idhcom3;

	  idtdat = row - 18;

	  idvtyp ();		/* display typewriter */
	  dswin (22);
	  idnamsw = TRUE;
	  submenu = TRUE;

	  vtsetup (instob, vtdisp, 17, ivtdptr, 22, 17,
		   advicur, bspicur, cupicur, cdnicur, endityp,
		   idbox[19][4], idbox[19][5]);
	}

      return (TRUE);
/* 
*/
    }
  else if (hitbox < 12)
    {				/* process a hit in a label window */

      pntsv = 0;		/* unselect previous point */

      arcurs (ID_NCUR);		/* set cursor color */
      gcurpos (cxval, cyval);

      if (n != curfunc)
	{			/* new function selected */

	  if (curfunc != 12)
	    dswin (curfunc);	/* stow old function */

	  edfunc (n);		/* select new function */

	}
      else
	{			/* reselect level */

	  dswin (curfunc);	/* stow old function */
	  edfunc (12);		/* select level */
	}

      subj = 0;			/* reset function selection */
      pntsel ();

      showfn ();		/* update display */

      setgc (ICURX, ICURY);	/* put cursor in center */
      return (TRUE);
    }

  return (FALSE);
}
