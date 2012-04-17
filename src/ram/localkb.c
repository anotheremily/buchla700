/*
   =============================================================================
	localkb.c -- local keyboard processing
	Version 39 -- 1988-11-28 -- D.N. Lynx Crowe

	List with the pr -e4 option to expand tabs to 4 spaces instead of 8.
   =============================================================================
*/

#include "stddefs.h"
#include "graphdef.h"
#include "glcdefs.h"
#include "glcfns.h"
#include "hwdefs.h"
#include "lcdline.h"
#include "vsdd.h"
#include "panel.h"
#include "smdefs.h"
#include "slice.h"
#include "macros.h"

#include "midas.h"
#include "score.h"
#include "scdsp.h"
#include "scfns.h"

#define	LCL_PRT		3	/* 1-origin local keyboard port number */

/* 
*/

extern short (*point) ();

extern short ancmsw;
extern short angroup;
extern short asmode;
extern short astat;
extern short aval;
extern short ctrsw;
extern short curasg;
extern short curtun;
extern short curvce;
extern short editsw;
extern short gomode;
extern short gtmsel;
extern short insmode;
extern short ismode;
extern short ldpass;
extern short lksel;
extern short lstbgnc;
extern short lstendc;
extern short lstflag;
extern short oldpk;
extern short pkctrl;
extern short recsw;
extern short stepenb;
extern short stepint;
extern short stepwgt;

extern char keystat[];
extern char trgtab[];

extern short grpmode[];
extern short grpstat[];
extern short grptmap[];
extern short ldmap[];
extern short prstab[];
extern short tuntab[];
extern short veltab[];
extern short vce2grp[];
extern short vce2trg[];

extern short grp2prt[][2];
extern short ldbox[][8];
extern short varmode[][16];

extern struct gdsel *gdstbc[];

extern struct n_entry *lstbgns[NLSTENTS];
extern struct n_entry *lstends[NLSTENTS];

/* 
*/

short lclkmap[24] = {		/* local key to MIDI key number map table */

  0, 1, 2, 3, 4, 5, 6,		/*  1..7 */
  7, 8, 9, 10, 11, 12, 13,	/*  8..14 */
  114, 115, 116, 117, 118, 119, 120,	/* 15..21 */
  121, 122, 123			/* 22..24 */
};

short panlkey[24] = {		/* default tunings, in cents, for local keys */

  560, 960, 1360, 1760, 2160, 2560, 2960, 3360,
  3760, 4160, 4560, 4960, 5360, 5760, 6160, 6560, 6960,
  7360, 7760, 8160, 8560, 8960, 9360, 9760
};

/* 
*/

short stepfrm[4][17] = {	/* step mode frame counts */

  /*  1    1    1    1    1    1          */
  /* --   --   --    -    -    -          */
  /* 64   32   16    8    4    2    1     */

  {3, 4, 10, 22, 46, 94, 190,	/* legato - normal */
   7, 16, 34, 70, 142, 286,	/* legato - dotted */
   3, 6, 14, 30},		/* legato - triplets */

  {2, 4, 8, 16, 32, 64, 128,	/* normal - normal */
   6, 12, 24, 48, 96, 192,	/* normal - dotted */
   3, 5, 10, 20},		/* normal - triplets */

  {2, 3, 4, 6, 10, 16, 24,	/* stacato - normal */
   4, 5, 8, 12, 20, 30,		/* stacato - dotted */
   3, 4, 5, 8},			/* stacato - triplets */

  {3, 6, 12, 24, 48, 96, 192,	/* interval - normal */
   9, 18, 36, 72, 144, 288,	/* interval - dotted */
   4, 8, 16, 32}		/* interval - triplets */
};

/* 
*/

/*
   =============================================================================
	setleds() -- set LEDs according to pkctrl, etc.
   =============================================================================
*/

setleds ()
{
  register short i;

  for (i = 0; i < 24; i++)	/* turn off all LEDs */
    io_leds = 0x0080 | i;

  switch (pkctrl)
    {

    default:
    case PK_PFRM:		/* performance */
    case PK_INST:		/* instrument */
    case PK_LOAD:		/* load */

      return;

    case PK_NOTE:		/* note entry */

      if (stepenb)
	io_leds = 0x01;

      io_leds = 6 - stepwgt;
      io_leds = 7 + stepint;
      return;

    case PK_ASGN:		/* assignment table */

      if (curasg == 0)
	return;

      io_leds = (curasg - 1) % 20;	/* indicate current assignment */
      return;
/* 
*/
    case PK_GOTO:		/* go to */

      if (gomode == GO_SECT)
	{			/* indicate active sections */

	  for (i = 0; i < 20; i++)
	    if (E_NULL != seclist[curscor][i])
	      io_leds = i;

	}
      else
	{			/* indicate active scores */

	  for (i = 0; i < 20; i++)
	    if (E_NULL != scores[i])
	      io_leds = i;
	}

      return;

    case PK_LIBR:

      for (i = 0; i < 20; i++)
	if (E_NULL != scores[i])
	  io_leds = i;

      return;
    }
}

/* 
*/

/*
   =============================================================================
	localkb() -- local keyboard processing
   =============================================================================
*/

localkb (sig)
     short sig;
{
  register short i, trg, lclkey, key, vel;
  register struct s_entry *ep;
  short val, disptag;

  key = sig - 1;

  switch (pkctrl)
    {

    case PK_PFRM:		/* performance */

      lclkey = lclkmap[key];
      trg = ((LCL_PRT - 1) << 11) + lclkey;

      if (astat)
	{			/* key down */

	  prstab[trg] = SM_SCALE (aval);

	  if (keystat[key] == 0)
	    {			/* initial key closure */

	      trgtab[trg] |= M_KSTATE;
	      veltab[trg] = vel = SM_SCALE (64);
	      keystat[key] = 1;

	      stmproc (trg);	/* process as a patch stimulus */

	      if (editsw)
		{		/* edit mode */

		  execkey (trg, tuntab[lclkey], curvce, 0);

		}
	      else
		{		/* normal mode */

		  for (i = 0; i < 12; i++)
		    if ((grp2prt[i][0] == LCL_PRT) && (grp2prt[i][1] == 1))
		      {

			asgvce (i, LCL_PRT - 1, 0, lclkey, vel);
			ne_bgn (i, lclkey, vel);
		      }
		}
/* 
*/
	    }
	  else
	    {			/* pressure change */

	      val = SM_SCALE (aval);

	      for (i = 0; i < 12; i++)
		{

		  if ((grp2prt[i][0] == LCL_PRT) && (grp2prt[i][1] == 1))
		    {

		      if (newsv (i, SM_KPRS, val))
			{

			  if (recsw && grpstat[i] &&
			      (2 == (ancmsw ? varmode[5][i] : grpmode[i])))
			    {

			      if (E_NULL != (ep = e_alc (E_SIZE2)))
				{

				  ep->e_time = t_cur;
				  ep->e_type = EV_ANVL;
				  ep->e_data1 = 0x0050 | i;
				  ep->e_dn =
				    (struct s_entry *) ((long) val << 16);
				  p_cur =
				    e_ins (ep,
					   ep_adj (p_cur, 0, t_cur))->e_fwd;
				  ctrsw = TRUE;
				  se_disp (ep, D_FWD, gdstbc, 1);
				  ctrsw = FALSE;
				}

			    }
			  else if ((angroup - 1) == i)
			    {

			      dsanval (5);
			    }
			}
		    }
		}
	    }
/* 
*/
	}
      else
	{			/* key release */

	  keystat[key] = 0;
	  trgtab[trg] &= ~M_KSTATE;
	  prstab[trg] = 0;

	  if (! trgtab[trg])
	    {

	      for (i = 0; i < 12; i++)
		{

		  if (vce2trg[i] == trg)
		    {

		      vce2trg[i] = -1;
		      procpfl (trg);
		    }

		  if ((grp2prt[i][0] == LCL_PRT) && (grp2prt[i][1] == 1))
		    ne_end (trg, i);
		}

	      stmproc (0x8000 | trg);	/* process as a patch stimulus */
	    }
	}

      return;

/* 
*/
    case PK_NOTE:		/* note entry - interval and weight selection */

      if (astat)
	{			/* key down */

	  if (keystat[key] == 0)
	    {			/* closure */

	      keystat[key] = 1;

	      if (key == 0)
		{		/* delete last entry */

		  disptag = FALSE;

		  while (lstendc-- > 0)
		    {		/* note ends */

		      ep = lstends[lstendc];
		      lstends[lstendc] = (struct n_entry *) NULL;

		      if ((struct n_entry *) NULL == ep)
			continue;

		      if (ep == p_bak)
			p_bak = p_bak->e_bak;

		      if (ep == p_ctr)
			p_ctr = p_ctr->e_bak;

		      if (ep == p_cur)
			p_cur = p_cur->e_bak;

		      if (ep == p_fwd)
			p_fwd = p_fwd->e_bak;

		      e_del (e_rmv (ep));
		      disptag = TRUE;
		    }

		  lstendc = 0;
/* 
*/
		  while (lstbgnc-- > 0)
		    {		/* note begins */

		      ep = lstbgns[lstbgnc];
		      lstbgns[lstbgnc] = (struct n_entry *) NULL;

		      if ((struct n_entry *) NULL == ep)
			continue;

		      if (ep == p_bak)
			p_bak = p_bak->e_bak;

		      if (ep == p_ctr)
			p_ctr = p_ctr->e_bak;

		      if (ep == p_cur)
			p_cur = p_cur->e_bak;

		      if (ep == p_fwd)
			p_fwd = p_fwd->e_bak;

		      e_del (e_rmv (ep));
		      disptag = TRUE;
		    }

		  lstbgnc = 0;
		  lstflag = FALSE;

		  if (disptag)
		    {

		      if ((fc_val - stepfrm[3][stepint]) >= 0L)
			fc_val -= stepfrm[3][stepint];

		      sc_refr (fc_val);
		    }

		}
	      else if (key == 1)
		{		/* enable step */

		  stepenb = ! stepenb;
		  setleds ();

/* 
*/
		}
	      else if (key == 2)
		{		/* insert bar */

		  if (recsw)
		    {

		      if (E_NULL != (ep = e_alc (E_SIZE1)))
			{

			  ep->e_type = EV_BAR;
			  ep->e_time = t_cur;
			  p_cur =
			    e_ins (ep,
				   ep_adj (p_cur, 1, t_cur)->e_bak)->e_fwd;
			  sc_refr (fc_val);
			}
		    }

		}
	      else if (key == 3)
		{		/* insert a rest */

		  if ((fc_val + stepfrm[3][stepint]) < 0x00FFFFFEL)
		    fc_val += stepfrm[3][stepint];

		}
	      else if ((key >= 4) && (key <= 6))
		{

		  stepwgt = 6 - key;	/* select weight */
		  setleds ();

		}
	      else if ((key >= 7) && (key <= 23))
		{

		  stepint = key - 7;	/* select interval */
		  setleds ();
		}
	    }

	}
      else
	{			/* key up */

	  keystat[key] = 0;
	}

      return;
/* 
*/

    case PK_INST:		/* instrument selection */

      if (astat)
	{

	  if (keystat[key] == 0)
	    {

	      keystat[key] = 1;

	      if (key >= 20)
		return;

	      if (ismode == IS_LORC)
		selins (key + 1);
	      else
		selins (key + 21);
	    }

	}
      else
	{

	  keystat[key] = 0;
	}

      return;

    case PK_ASGN:		/* assignment table selection */

      if (astat)
	{

	  if (keystat[key] == 0)
	    {

	      keystat[key] = 1;

	      if ((asmode == 5) && (key >= 19))
		return;
	      else if (key >= 20)
		return;

	      selasg (key + 1 + ((asmode - 1) * 20));
	    }

	}
      else
	{

	  keystat[key] = 0;
	}

      return;
/* 
*/
    case PK_GOTO:		/* go to */

      if (astat)
	{

	  if (keystat[key] == 0)
	    {

	      keystat[key] = 1;

	      if (key >= 20)	/* limit key range */
		return;

	      if (gomode == GO_SECT)
		{		/* section */

		  if (E_NULL != (ep = seclist[curscor][key]))
		    {

		      if (insmode)
			{

			  icancel ();
			  dsimode ();
			}

		      sc_goto (fc_val = ep->e_time);
		      pkctrl = oldpk;
		      gomode = GO_NULL;
		      GLCcurs (G_ON);
		      GLCtext (0, 31, "Go To");
		      point = GLCplot;
		      lseg (GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 0);
		      GLCcurs (G_OFF);
		      setleds ();
		      return;
		    }
/* 
*/
		}
	      else
		{		/* score */

		  if (E_NULL != scores[key])
		    {

		      if (insmode)
			{

			  icancel ();
			  dsimode ();
			}

		      selscor (key);
		      pkctrl = oldpk;
		      gomode = GO_NULL;
		      GLCcurs (G_ON);
		      GLCtext (0, 31, "Go To");
		      point = GLCplot;
		      lseg (GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 0);
		      GLCcurs (G_OFF);
		      setleds ();
		      return;
		    }
		}
	    }

	}
      else
	{

	  keystat[key] = 0;
	}

      return;
/* 
*/
    case PK_LIBR:

      if (astat)
	{

	  if (keystat[key] == 0)
	    {

	      keystat[key] = 1;

	      if ((-1 != lksel) && (key < 20))
		{

		  ldpass = 2;

		  for (i = 0; i < N_SCORES; i++)
		    if (ldmap[i] == key)
		      {

			ldmap[i] = -1;
			dpy_scr (ldbox[1][4], i);
		      }

		  ldmap[lksel] = key;
		  dpy_scr (ldbox[1][4], lksel);
		  lksel = -1;
		}
	    }

	}
      else
	{

	  keystat[key] = 0;
	}

      return;
/* 
*/
    case PK_NGRP:

      if (astat)
	{

	  if (keystat[key] == 0)
	    {

	      keystat[key] = 1;

	      if ((-1 != gtmsel) && (key < 12))
		{

		  for (i = 0; i < 12; i++)
		    if (grptmap[i] == key)
		      {

			grptmap[i] = -1;
			dsgtme (i);
		      }

		  grptmap[gtmsel] = key;
		  dsgtmn (gtmsel, FALSE);
		  dsgtme (gtmsel);
		  gtmsel = -1;
		}
	    }

	}
      else
	{

	  keystat[key] = 0;
	}

      return;

    case PK_LOAD:

      if (astat)
	keystat[key] = 1;
      else
	keystat[key] = 0;

      return;
    }
}
