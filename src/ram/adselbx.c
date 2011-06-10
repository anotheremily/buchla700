/*
   =============================================================================
	adselbx.c -- assignment editor box selection functions
	Version 30 -- 1988-12-08 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "hwdefs.h"
#include "fields.h"
#include "vsdd.h"
#include "graphdef.h"

#include "midas.h"
#include "asgdsp.h"

#if	DEBUGIT
extern short debugsw;
#endif

#define	ATWROW	10		/* typewriter row */
#define	ATWCOL	1		/* typewriter column */
#define	ATDECOL	1		/* data entry base column */

extern short enterit (), adswin (), advacur (), bspacur ();
extern short nokey (), vtdisp (), adpoint ();

extern short (*itxput) ();
extern short (*point) ();

extern short admctl;
extern short adnamsw;
extern short asgfks;
extern short asghit;
extern short asgmod;
extern short auxctl;
extern short curasg;
extern short cxval;
extern short cyval;
extern short hitbox;
extern short hitcx;
extern short hitcy;
extern short lastam;
extern short submenu;

extern short adbox[][8];

extern short grp2prt[12][2];
extern short ins2grp[12];
extern short asgkble[];
extern short key2grp[];
extern short mctlnum[];

extern struct selbox *csbp, *curboxp;

extern char bfs[];
extern char caname[];
extern char vtlin1[], vtlin2[], vtlin3[];

extern unsigned *asgob;

/* forward references */

short bx_null (), adfnbox ();

/* 
*/

struct selbox adboxes[] = {

  {1, 0, 94, 27, 0, adfnbox},	/*  0:  curasg, caname */
  {1, 28, 94, 40, 1, adfnbox},	/*  1:  asgtab[curasg] */
  {1, 56, 94, 68, 2, enterit},	/*  2:  curmop */
  {1, 42, 94, 54, 3, enterit},	/*  3:  prgchan */
  {96, 1, 190, 138, 4, enterit},	/*  4:  vce2grp[voice] */
  {192, 1, 302, 138, 5, adfnbox},	/*  5:  mctlnum[srcvar] */
  {304, 1, 510, 138, 6, adfnbox},	/*  6:  grp2prt[group][pt|ch] */
  {1, 140, 510, 348, 7, adfnbox},	/*  7:  key2grp[key] */
  {1, 70, 38, 82, 8, adfnbox},	/*  8:  auxctl */
  {40, 70, 94, 82, 9, adfnbox},	/*  9:  curtun */
  {1, 84, 94, 138, 10, enterit},	/* 10:  ps_intn, ps_rate, ps_dpth */

  {0, 0, 0, 0, 0, FN_NULL}	/* end of table */
};

/* 
*/

/*
   =============================================================================
	endatyp() -- end function for virtual typewriter
   =============================================================================
*/

endatyp ()
{
  adnamsw = FALSE;
  submenu = FALSE;

  if (admctl EQ - 1)
    adswin (7);			/* refresh key map window */
  else
    admenu (admctl);		/* refresh menu window */
}

/* 
*/

/*
   =============================================================================
	admtxt() -- output text to the submenu
   =============================================================================
*/

admtxt (row, col, txt, tag)
     short row, col;
     char *txt;
     short tag;
{
  if (v_regs[5] & 0x0180)
    vbank (0);

  vcputsv (asgob, 64, ACFBX07, ACBBX07, row + 10, col, txt, 14);
}

/* 
*/

/*
   =============================================================================
	admenu() -- put up or take down an assignment display submenu
   =============================================================================
*/

admenu (n)
     short n;
{
  register short i;

  if (n GE 0)
    {				/* put up a submenu */

      itxput = admtxt;		/* setup output function */

      if (v_regs[5] & 0x0180)
	vbank (0);

      vbfill4 (asgob, 128, adbox[7][0], adbox[7][1],
	       adbox[7][2], adbox[7][3], exp_c (adbox[7][5]));

      switch (n)
	{			/* fill the window */

	case 0:		/* instruments */

	  showim ();		/* show the instruments */
	  break;

	case 1:		/* assignments */

	  showam (lastam);	/* show the assignments */
	  settc (0, 9);		/* position the cursor */
	  break;

	case 2:		/* tunings */

	  showtm ();		/* show the tunings */
	  settc (5, 10);	/* position the cursor */
	  break;

	default:		/* eh ? */

	  break;
	}

    }
  else
    {				/* take down the submenu */

      admctl = n;		/* set new menu type */
      adswin (7);		/* refresh the window */
    }

  point = adpoint;

  if (v_regs[5] & 0x0180)
    vbank (0);

  lseg (0, 0, 0, 349, AK_BORD);	/* fix left edge of screen */
  lseg (511, 0, 511, 349, AK_BORD);	/* fix right edge of screen */

  admctl = n;			/* set new menu type */
}

/* 
*/

/*
   =============================================================================
	adfnbox() -- assignment display box hit processor
   =============================================================================
*/

short
adfnbox (n)
     short n;
{
  register short i, grp, key, line;
  short row, col;

  row = hitcy / 14;
  col = hitcx >> 3;

  switch (hitbox)
    {

    case 0:			/* asignment number or name */

      if ((NOT adnamsw) AND (row EQ 0))
	{			/* menu or table */

	  if ((col GE 1) AND (col LE 7))
	    {			/* menu */

	      if (admctl NE 1)
		{

		  lastam = 0;
		  admenu (1);	/* put up menu */

		}
	      else
		{

		  if (lastam EQ 0)
		    {		/* show page 2 */

		      lastam = 60;
		      admenu (1);

		    }
		  else
		    {		/* take down menu */

		      lastam = 0;
		      admenu (-1);
		    }
		}

	      return (SUCCESS);

	    }
	  else
	    {			/* table */

	      return (enterit ());	/* data entry */
	    }
	}

      if (adnamsw)
	{			/* typewriter is up */

	  if (vtyper ())
	    {

	      if (NOT asgmod)
		{

		  asgmod = TRUE;
		  adswin (0);
		}
	    }
/* 
*/
	}
      else
	{			/* setup the typewriter */

	  vbank (0);
	  vbfill4 (asgob, 128, CTOX (ATWCOL), RTOY (ATWROW),
		   CTOX (ATWCOL + 30) - 1, RTOY (ATWROW + 3) - 1,
		   exp_c (ACBBX04));

	  tsplot4 (asgob, 64, ACFBX04, ATWROW, ATWCOL, vtlin1, 14);
	  tsplot4 (asgob, 64, ACFBX04, ATWROW + 1, ATWCOL, vtlin2, 14);
	  tsplot4 (asgob, 64, ACFBX04, ATWROW + 2, ATWCOL, vtlin3, 14);

	  point = adpoint;	/* draw a border ... */

	  lseg (CTOX (ATWCOL) - 1, RTOY (ATWROW) - 1,	/* up, lt */
		CTOX (ATWCOL + 30), RTOY (ATWROW) - 1,	/* up, rt */
		AK_BORD);

	  lseg (CTOX (ATWCOL + 30), RTOY (ATWROW) - 1,	/* up, rt */
		CTOX (ATWCOL + 30), RTOY (ATWROW + 3) - 1,	/* dn, rt */
		AK_BORD);

	  lseg (CTOX (ATWCOL + 30), RTOY (ATWROW + 3) - 1,	/* dn, rt */
		CTOX (1) - 1, RTOY (ATWROW + 3) - 1,	/* dn, lt */
		AK_BORD);

	  lseg (CTOX (1) - 1, RTOY (ATWROW + 3) - 1,	/* dn, lt */
		CTOX (1) - 1, RTOY (ATWROW) - 1,	/* up, lt */
		AK_BORD);

	  vtsetup (asgob, vtdisp, ATDECOL,
		   caname,
		   ATWROW, ATWCOL,
		   advacur, bspacur, nokey, nokey, endatyp,
		   adbox[n][4], adbox[n][5]);

	  adnamsw = TRUE;
	  submenu = TRUE;
	}

      return (SUCCESS);
/* 
*/
    case 1:

      if ((col GE 1) AND (col LE 4))
	{			/* store */

	  if (curasg EQ 0)
	    return (FAILURE);

	  putasg (curasg);
	  adswin (0);
	  return (SUCCESS);

	}
      else if ((col GE 6) AND (col LE 10))
	{			/* retrieve */

	  getasg (curasg);
	  awins ();
	  return (SUCCESS);
	}

    case 5:			/* toggle controller omni mode */

      if (col EQ 25)
	{

	  if ((row LT 5) OR (row GT 8))
	    return (FAILURE);

	  mctlnum[row - 5] ^= GTAG1;

	  if (v_regs[5] & 0x0180)
	    vbank (0);

	  bfs[0] = row - 5 + '2';
	  bfs[1] = '\0';

	  tsplot4 (asgob, 64,
		   (mctlnum[row - 5] & GTAG1) ?
		   AK_MODC : adbox[n][4], row, adbox[n][7], bfs, 14);

	  return (SUCCESS);

	}
      else
	{

	  return (enterit ());
	}
/* 
*/
    case 6:			/* instruments, groups, ports, channels */

      if (((col EQ 41) OR (col EQ 42) OR
	   (col EQ 54) OR (col EQ 55)) AND (row EQ 3))
	{

	  /* instrument menu */

	  if (admctl NE 0)
	    admenu (0);		/* put up menu */
	  else
	    admenu (-1);	/* take down menu */

	  return (SUCCESS);

	}
      else if (col EQ 39)
	{			/* groups 1..6 */

	  if ((row LT 4) OR (row GT 9))
	    return (FAILURE);

	  /* toggle "legato" mode */

	  grp = row - 4;

	  ins2grp[grp] ^= GTAG1;

	  bfs[0] = '1' + grp;
	  bfs[1] = '\0';

	  if (v_regs[5] & 0x0180)
	    vbank (0);

	  tsplot4 (asgob, 64,
		   ((ins2grp[grp] & GTAG1) ? AK_MODC : adbox[n][4]),
		   row, col, bfs, 14);

	  return (SUCCESS);
/* 
*/
	}
      else if (col EQ 52)
	{			/* groups 7..12 */

	  if ((row LT 4) OR (row GT 9))
	    return (FAILURE);

	  /* toggle "legato" mode */

	  grp = row + 2;

	  ins2grp[grp] ^= GTAG1;

	  bfs[0] = grp + ((grp > 8) ? 163 : '1');
	  bfs[1] = '\0';

	  if (v_regs[5] & 0x0180)
	    vbank (0);

	  tsplot4 (asgob, 64,
		   ((ins2grp[grp] & GTAG1) ? AK_MODC : adbox[n][4]),
		   row, col, bfs, 14);

	  return (SUCCESS);

	}
      else
	{

	  return (enterit ());	/* data entry */
	}
/* 
*/
    case 7:			/* assignment map */

      if (admctl NE - 1)
	return (FAILURE);

      if (asghit)
	{

	  if (asghit EQ (hitcy / 14))
	    {			/* previous hit ? */

	      /* toggle selected key status */

	      grp = asghit - 11;
	      line = (14 * grp) + AK_BASE;
	      key = 0;

	      for (i = 0; i < 88; i++)
		if ((hitcx GE (asgkble[i] + 24)) AND
		    (hitcx LE (asgkble[i] + (i EQ 87 ? 31 : 28))))
		  key = i + 1;

	      if (key-- EQ 0)
		return (FAILURE);

	      if (key LT asgfks)
		{

		  i = asgfks;
		  asgfks = key;
		  key = i;
		}

	      for (i = asgfks; i LE key; i++)
		key2grp[i] ^= (0x0001 << grp);

	      drawk2g (grp);
	      asghit = 0;

	      if (NOT asgmod)
		{

		  asgmod = TRUE;
		  adswin (0);
		}

	    }
	  else
	    {

	      /* clear the selection */

	      drawk2g (asghit - 11);
	      asghit = 0;
	    }
/* 
*/
	}
      else
	{

	  /* make initial selection */

	  asghit = hitcy / 14;
	  grp = asghit - 11;

	  if (grp2prt[grp][0] NE 1)
	    {

	      asghit = 0;
	      return (FAILURE);
	    }

	  line = (14 * grp) + AK_BASE;
	  key = 0;

	  for (i = 0; i < 88; i++)
	    if ((hitcx GE (asgkble[i] + 24)) AND
		(hitcx LE (asgkble[i] + (i EQ 87 ? 31 : 28))))
	      key = i + 1;

	  if (key-- EQ 0)
	    {

	      asghit = 0;
	      return (FAILURE);
	    }

	  asgfks = key;

	  vbfill4 (asgob, 128, asgkble[key] + 24, line,
		   asgkble[key] + (key EQ 87 ? 31 : 28),
		   line + 3, exp_c (AK_SELC2));

	}

      return (SUCCESS);

    case 8:			/* aux ctl */

      setaux (NOT auxctl);
      adswin (8);
      return (SUCCESS);

    case 9:			/* tuning menu */

      if ((col GE 6) AND (col LE 8))
	{

	  if (admctl NE 2)
	    admenu (2);		/* put up menu */
	  else
	    admenu (-1);	/* take down menu */

	  return (SUCCESS);

	}
      else
	{

	  return (enterit ());	/* data entry */
	}
    }

  return (FAILURE);
}
