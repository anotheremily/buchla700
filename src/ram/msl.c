/*
   =============================================================================
	msl.c -- midas main scan loop
	Version 102 -- 1989-11-14 -- D.N. Lynx Crowe

	List with pr -e4 option to expand tabs to 4 spaces instead of 8.
   =============================================================================
*/

#define	DEBUGIT		0	/* enable debug code */

#define	OLDTIME		0	/* use old tempo time calculations */

#include "stddefs.h"
#include "biosdefs.h"
#include "graphdef.h"
#include "glcfns.h"
#include "glcdefs.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "hwdefs.h"
#include "memory.h"
#include "cmeta.h"
#include "fields.h"
#include "fpu.h"
#include "macros.h"
#include "patch.h"
#include "smdefs.h"
#include "sclock.h"
#include "scwheel.h"
#include "slice.h"
#include "timers.h"
#include "wordq.h"

#include "midas.h"
#include "instdsp.h"
#include "score.h"
#include "scdsp.h"
#include "scfns.h"
#include "wsdsp.h"
#include "asgdsp.h"

#define	LCL_PRT		3	/* 1-origin local keyboard port number */

#if	DEBUGIT
extern short debugsw;

short debugms = 1;
#endif

/* 
*/

/* variables defined elsewhere */

extern short (*cx_key) ();
extern short (*cy_key) ();
extern short (*d_key) ();
extern short (*e_key) ();
extern short (*m_key) ();
extern short (*x_key) ();

extern
PFS (*swpt)[];

     extern short aflag;
     extern short amplval;
     extern short ancmsw;
     extern short angroup;
     extern short asig;
     extern short astat;
     extern short aval;
     extern short clkctl;
     extern short clkrun;
     extern short ctrsw;
     extern short cxrate;
     extern short cxval;
     extern short cyrate;
     extern short cyval;
     extern short dsp_ok;
     extern short editss;
     extern short editsw;
     extern short lampsw;
     extern short ndisp;
     extern short nxtflag;
     extern short pchsw;
     extern short recsw;
     extern short runit;
     extern short swback;
     extern short swctrl;
     extern short swdelta;
     extern short swdir;
     extern short swfiin;
     extern short swflag;
     extern short swlast;
     extern short swndx;
     extern short swstop;
     extern short swthr;
     extern short swtime;
     extern short swwait;
     extern short sd;
     extern short se;
     extern short stcrow;
     extern short stccol;
     extern short tglclk;
     extern short tglpch;
     extern short timemlt;
     extern short tmpomlt;
     extern short tmpoval;
     extern short tuneval;
     extern short vtpcol;
     extern short vtprow;

/* 
*/

     extern unsigned short *obj8;

     extern long afi;
     extern long lcdtime;
     extern long swcount;
     extern long swrate;
     extern long swrmin;
     extern long swtemp;

     extern short grpmode[];
     extern short grpstat[];
     extern short prstab[];
     extern short simled[];
     extern short swfifo[NSWFIFO];
     extern short tmultab[];
     extern short vce2trg[];
     extern short veltab[];

     extern char trgtab[];

     extern short grp2prt[][2];
     extern short sigtab[][2];
     extern short varmode[][16];

     extern struct gdsel *gdstbc[];

     extern struct wordq ptefifo;

     unsigned short fifoval;

/* 
*/

/* 
   =============================================================================
		clk_ped() -- process clock on/off toggle pedal
   =============================================================================
*/

clk_ped (stat)
     short stat;
{
  if (stat)
    tglclk = TRUE;
}

/* 
   =============================================================================
		pch_ped() -- process punch in/out toggle pedal
   =============================================================================
*/

pch_ped (stat)
     short stat;
{
  if (stat && pchsw)
    tglpch = TRUE;
}

/* 
*/

/*
   =============================================================================
	msl() -- MIDAS main scan loop
   =============================================================================
*/

msl ()
{
  register char *ioadr;
  register unsigned *fpu;
  register struct s_entry *ep;
  register short i, ti, val;
  register long rt;
  unsigned short chan, crel, oldsr, port, trg, trig, vel;
  short cxprev, cyprev, esi, newsig, oldclk, oldrec;
  long fctemp;

#if	DEBUGIT
  if (debugsw && debugms)
    printf ("msl():  ENTRY  ndisp=%d\n", ndisp);
#endif

  runit = TRUE;			/* set run state */

  while (runit)
    {

      dsp_ok = TRUE;		/* set display-OK flag for this pass */

      ioadr = &io_ser + 2L;	/* get edit switch status */
      esi = *ioadr & 0x0008;

      if (editss != esi)
	{			/* check for edit switch change */

	  editss = esi;

	  if (editss)		/* toggle edit state if it went hi */
	    editsw = ! editsw;
	}

      if (editsw)		/* update edit LED */
	io_leds = 0x9E;
      else
	io_leds = 0x1E;

      if ((! lampsw) && lcdtime)
	{

	  if (0 EQ-- lcdtime)
	    io_leds = 0x1F;	/* turn off the LCD backlight */
	}

      msm ();			/* scan the MIDI ports */
/* 
*/

      if (tglclk)
	{			/* check for clock on/off toggle */

	  oldsr = setsr (0x2700);	/* disable interrupts */
	  tglclk = FALSE;	/* cancel toggle flag */
	  setsr (oldsr);	/* enable interrupts */

	  clkset (! clkrun);	/* toggle clock mode */
	  dclkmd ();		/* update display */
	}

      if (tglpch)
	{			/* check for punch in/out toggle */

	  oldsr = setsr (0x2700);	/* disable interrupts */
	  tglpch = FALSE;	/* cancel toggle flag */
	  setsr (oldsr);	/* enable interrupts */

	  if ((ndisp == 2) && (v_regs[5] & 0x0180))
	    vbank (0);

	  for (i = 0; i < 12; i++)
	    {			/* scan the groups */

	      if (grpmode[i] == 1)
		{		/* stdby -> rec */

		  grpmode[i] = 2;

		  if (ndisp == 2)
		    vputc (obj8, 2, 6 + (i * 5), '*', simled[grpmode[i]]);

		}
	      else if (grpmode[i] == 2)
		{		/* rec -> play */

		  grpmode[i] = 0;

		  if (ndisp == 2)
		    vputc (obj8, 2, 6 + (i * 5), '*', simled[grpmode[i]]);
		}
	    }
	}

/* 
*/
      /* process stimulli from the patch stimulus fifo */

      if (getwq (&ptefifo, &fifoval) >= 0)
	{

	  crel = 0x8000 & fifoval;

	  trg = TRG_MASK & fifoval;
	  port = 0x0003 & (fifoval >> 11);
	  chan = 0x000F & (fifoval >> 7);
	  trig = 0x007F & fifoval;

	  veltab[trg] = vel = SM_SCALE (64);
	  prstab[trg] = 0;

	  if (crel)
	    {			/* release */

	      trgtab[trg] &= ~M_KSTATE;

	      for (i = 0; i < 12; i++)
		{

		  if (vce2trg[i] == trg)
		    {

		      vce2trg[i] = -1;
		      procpfl (trg);

		    }
		}

	      stmproc (fifoval);	/* do it as a patch stimulus */

	    }
	  else
	    {			/* closure */

	      trgtab[trg] |= M_KSTATE;

	      stmproc (fifoval);	/* do it as a patch stimulus */

	      for (i = 0; i < 12; i++)
		if ((grp2prt[i][0] == 1 + port) &&
		    (grp2prt[i][1] == 1 + chan))
		  asgvce (i, port, chan, trig, vel);
	    }
	}
/* 
*/
      if (-1L != (afi = XBIOS (X_ANALOG)))
	{			/* check panel inputs */

	  asig = (afi >> 8) & 0x007F;	/* signal number */
	  astat = (afi >> 7) & 0x0001;	/* status */
	  aval = afi & 0x007F;	/* value */

	  if (asig)
	    {			/* active signal */

	      aflag = TRUE;
	      newsig = astat && (! sigtab[asig][1]);

	      sigtab[asig][0] = aval;
	      sigtab[asig][1] = astat;

	    }
	  else
	    {			/* all keys up */

	      aflag = FALSE;
	      newsig = FALSE;

	      for (i = 0; i < 128; i++)
		sigtab[i][1] = 0;
	    }

/* 
*/
	  if (aflag)
	    {			/* anything changed ? */

	      if ((asig >= 1) && (asig <= 24))
		{

		  /* local keyboard performance key */

		  localkb (asig);

		}
	      else if ((asig >= 25) && (asig <= 38))
		{

		  if (astat)
		    lcd_on ();

		  if (! newsig)
		    doslide ();

		}
	      else if ((asig >= 39) && (asig <= 52))
		{

		  if (astat)
		    lcd_on ();

		  (*(*swpt)[asig - 39]) (astat, (asig - 39));

		}
	      else if ((asig >= 60) && (asig <= 69))
		{

		  (*d_key) (asig - 60);

/* 
*/
		}
	      else
		switch (asig)
		  {

		  case 53:	/* tablet x */

		    val = SM_SCALE (aval);

		    for (i = 0; i < 12; i++)
		      {

			if (grp2prt[i][0] == LCL_PRT)
			  {

			    if (newsv (i, SM_HTPW, val))
			      {

				if (recsw && grpstat[i] &&
				    (2
				     == (ancmsw ? varmode[0][i] :
					 grpmode[i])))
				  {

				    if (E_NULL != (ep = e_alc (E_SIZE2)))
				      {

					ep->e_time = t_cur;
					ep->e_type = EV_ANVL;
					ep->e_data1 = i;
					ep->e_dn =
					  (struct s_entry *) ((long) val <<
							      16);
					p_cur =
					  e_ins (ep,
						 ep_adj (p_cur, 0,
							 t_cur))->e_fwd;
					ctrsw = TRUE;
					se_disp (ep, D_FWD, gdstbc, 1);
					ctrsw = FALSE;
				      }

				  }
				else if ((angroup - 1) == i)
				  {

				    dsanval (0);
				  }
			      }
			  }
		      }

		    break;
/* 
*/
		  case 54:	/* tablet y */

		    val = SM_SCALE (aval);

		    for (i = 0; i < 12; i++)
		      {

			if (grp2prt[i][0] == LCL_PRT)
			  {

			    if (newsv (i, SM_VTMW, val))
			      {

				if (recsw && grpstat[i] &&
				    (2
				     == (ancmsw ? varmode[1][i] :
					 grpmode[i])))
				  {

				    if (E_NULL != (ep = e_alc (E_SIZE2)))
				      {

					ep->e_time = t_cur;
					ep->e_type = EV_ANVL;
					ep->e_data1 = 0x0010 | i;
					ep->e_dn =
					  (struct s_entry *) ((long) val <<
							      16);
					p_cur =
					  e_ins (ep,
						 ep_adj (p_cur, 0,
							 t_cur))->e_fwd;
					ctrsw = TRUE;
					se_disp (ep, D_FWD, gdstbc, 1);
					ctrsw = FALSE;
				      }

				  }
				else if ((angroup - 1) == i)
				  {

				    dsanval (1);
				  }
			      }
			  }
		      }

		    break;

		  case 55:	/* cursor x */

		    (*cx_key) ();
		    break;

		  case 56:	/* cursor y */

		    (*cy_key) ();
		    break;
/* 
*/
		  case 58:	/* longpot r */

		    val = SM_SCALE (aval);

		    for (i = 0; i < 12; i++)
		      {

			if (grp2prt[i][0] == LCL_PRT)
			  {

			    if (newsv (i, SM_LPBR, val))
			      {

				if (recsw && grpstat[i] &&
				    (2
				     == (ancmsw ? varmode[2][i] :
					 grpmode[i])))
				  {

				    if (E_NULL != (ep = e_alc (E_SIZE2)))
				      {

					ep->e_time = t_cur;
					ep->e_type = EV_ANVL;
					ep->e_data1 = 0x0020 | i;
					ep->e_dn =
					  (struct s_entry *) ((long) val <<
							      16);
					p_cur =
					  e_ins (ep,
						 ep_adj (p_cur, 0,
							 t_cur))->e_fwd;
					ctrsw = TRUE;
					se_disp (ep, D_FWD, gdstbc, 1);
					ctrsw = FALSE;
				      }

				  }
				else if ((angroup - 1) == i)
				  {

				    dsanval (2);
				  }
			      }
			  }
		      }

		    break;

/* 
*/
		  case 59:	/* scroll wheel */

		    wheel ();
		    break;

		  case 70:	/* X key */

		    (*x_key) ();
		    break;

		  case 71:	/* E key */

#if	DEBUGIT
		    if (debugsw && debugms)
		      printf ("msl():  -> e_key ($%lX)  astat=%d  ndisp=%d\n",
			      e_key, astat, ndisp);
#endif
		    (*e_key) ();

#if	DEBUGIT
		    if (debugsw && debugms)
		      printf
			("msl():  <- e_key ($%lX)  astat=%d  ndisp=%d  runit=%d\n",
			 e_key, astat, ndisp, runit);
#endif

		    break;

		  case 72:	/* M key */

		    (*m_key) ();
		    break;

		  case 73:	/* Tempo */

		    if (aval > 50)	/* dead band */
		      if (aval < 53)
			aval = 50;
		      else
			aval -= 2;

		    tmpomlt = aval > 100 ? 100 : aval;
#if	OLDTIME
		    ti = ((tmpomlt + 50) * tmpoval) / 100;
		    ti = (short) ((192000L / ti) - 1);
#else
		    ti = (tmpomlt + 50) * tmpoval;
		    ti = (short) ((19200000L / ti) - 1);
#endif
		    TIME_T2H = ti >> 8;
		    TIME_T2L = ti & 0x00FF;

		    if (tmpomlt == 50)
		      {		/* 0 */

			io_leds = 0x18;	/* green off */
			io_leds = 0x19;	/* red off */

		      }
		    else if (tmpomlt > 50)
		      {		/* hi */

			io_leds = 0x98;	/* green on */
			io_leds = 0x19;	/* red off */

		      }
		    else
		      {		/* lo */

			io_leds = 0x18;	/* green off */
			io_leds = 0x99;	/* red on */
		      }

		    break;
/* 
*/
		  case 74:	/* Time */

		    if (aval > 50)	/* dead band */
		      if (aval < 53)
			aval = 50;
		      else
			aval -= 2;

		    ti = aval > 100 ? 100 : aval;
		    timemlt = tmultab[ti];

		    if (ti == 50)
		      {		/* 0 */

			io_leds = 0x1A;	/* green off */
			io_leds = 0x1B;	/* red off */

		      }
		    else if (ti > 50)
		      {		/* hi */

			io_leds = 0x9A;	/* green on */
			io_leds = 0x1B;	/* red off */

		      }
		    else
		      {		/* lo */

			io_leds = 0x1A;	/* green off */
			io_leds = 0x9B;	/* red on */
		      }

		    break;

/* 
*/
		  case 75:	/* Tuning */

		    if (aval > 50)	/* dead band */
		      if (aval < 53)
			aval = 50;
		      else
			aval -= 2;

		    i = (aval > 100) ? 100 : aval;
		    tuneval = (i - 50) << 2;
		    settune ();

		    if (i == 50)
		      {

			io_leds = 0x1C;	/* green off */
			io_leds = 0x1D;	/* red off */

		      }
		    else if (i > 50)
		      {

			io_leds = 0x9C;	/* green on */
			io_leds = 0x1D;	/* red off */

		      }
		    else
		      {

			io_leds = 0x1C;	/* green off */
			io_leds = 0x9D;	/* red on */
		      }

		    break;
/* 
*/
		  case 76:	/* amplitude */

		    aval += aval >> 2;

		    if (aval > 127)
		      aval = 127;

		    amplval = (aval << 9) ^ 0x8000;
		    sendval (0, 0, amplval);
		    break;

		  case 77:	/* pedal 1 */

		    val = SM_SCALE (aval);

		    for (i = 0; i < 12; i++)
		      {

			if (grp2prt[i][0] == LCL_PRT)
			  {

			    if (newsv (i, SM_PED1, val))
			      {

				if (recsw && grpstat[i] &&
				    (2
				     == (ancmsw ? varmode[4][i] :
					 grpmode[i])))
				  {

				    if (E_NULL != (ep = e_alc (E_SIZE2)))
				      {

					ep->e_time = t_cur;
					ep->e_type = EV_ANVL;
					ep->e_data1 = 0x0040 | i;
					ep->e_dn =
					  (struct s_entry *) ((long) val <<
							      16);
					p_cur =
					  e_ins (ep,
						 ep_adj (p_cur, 0,
							 t_cur))->e_fwd;
					ctrsw = TRUE;
					se_disp (ep, D_FWD, gdstbc, 1);
					ctrsw = FALSE;
				      }

				  }
				else if ((angroup - 1) == i)
				  {

				    dsanval (4);
				  }
			      }
			  }
		      }

		    break;

/* 
*/
		  case 79:	/* cv 1 */

		    val = SM_SCALE (aval);

		    for (i = 0; i < 12; i++)
		      {

			if (grp2prt[i][0] == LCL_PRT)
			  {

			    if (newsv (i, SM_CTL1, val))
			      {

				if (recsw && grpstat[i] &&
				    (2
				     == (ancmsw ? varmode[3][i] :
					 grpmode[i])))
				  {

				    if (E_NULL != (ep = e_alc (E_SIZE2)))
				      {

					ep->e_time = t_cur;
					ep->e_type = EV_ANVL;
					ep->e_data1 = 0x0030 | i;
					ep->e_dn =
					  (struct s_entry *) ((long) val <<
							      16);
					p_cur =
					  e_ins (ep,
						 ep_adj (p_cur, 0,
							 t_cur))->e_fwd;
					ctrsw = TRUE;
					se_disp (ep, D_FWD, gdstbc, 1);
					ctrsw = FALSE;
				      }

				  }
				if ((angroup - 1) == i)
				  {

				    dsanval (3);
				  }
			      }
			  }
		      }

		    break;

		  }
	    }
	}
/* 
*/

#if	DEBUGIT
      if (debugsw && debugms && (! runit))
	printf ("msl():  end of asig cases -- dsp_ok = %d\n", dsp_ok);
#endif

      /* memory allocation changed ? */

      if ((ndisp == 2) && se_chg && dsp_ok)
	{

	  dsmem ();		/* display memory remaining */
	  se_chg = FALSE;
	}

      nxtflag = FALSE;		/* clear 'next score' flag */
      fctemp = fc_val;		/* sample the frame clock */

      if (t_cur != fctemp)
	{			/* see if frame clock changed */

	  if (t_cur < fctemp)
	    {			/* clock incremented */

	      if (se == D_BAK)	/* change direction ? */
		chgsef ();

	      sc_trek (fctemp);	/* track frame clock */

	    }
	  else
	    {			/* clock decremented */

	      if (se == D_FWD)	/* change direction ? */
		chgseb ();

	      sc_trek (fctemp);	/* track frame clock */
	    }

	  /* handle display update if there's time for it */

	}
      else if (dsp_ok && (t_ctr != t_cur))
	{

	  if (t_ctr < t_cur)
	    {			/* clock incremented */

	      if (sd == D_BAK)	/* change direction ? */
		chgsdf ();

	      sc_trak (t_ctr + 1);	/* track frame clock */

	    }
	  else
	    {			/* clock decremented */

	      if (sd == D_FWD)	/* change direction ? */
		chgsdb ();

	      sc_trak (t_ctr - 1);	/* track frame clock */
	    }
	}
/* 
*/
#if	DEBUGIT
      if (debugsw && debugms && (! runit))
	printf ("msl():  end of clock processing -- dsp_ok = %d\n", dsp_ok);
#endif

      /* handle 'next score' flag */

      if (nxtflag && (sd == D_FWD))
	{			/* switch scores ? */

	  oldrec = recsw;
	  oldclk = clkrun;

	  ti = curscor + 1;

	  if (ti >= N_SCORES)
	    ti = 0;

	  for (i = 0; i < N_SCORES; i++)
	    {

	      if (E_NULL != scores[ti])
		{

		  selscor (ti);
		  break;
		}

	      if (++ti >= N_SCORES)
		ti = 0;
	    }

	  clkset (oldclk);	/* restore clock mode */
	  dsclk ();
	  recsw = oldrec;	/* restore record/play mode */
	  dsrpmod ();
	  nxtflag = FALSE;	/* clear 'next score' flag */
	}

#if	DEBUGIT
      if (debugsw && debugms && (! runit))
	printf ("msl():  curproc\n");
#endif

      curproc ();		/* process wheel and ball */

#if	DEBUGIT
      if (debugsw && debugms && (! runit))
	printf ("msl():  seqproc\n");
#endif

      seqproc ();		/* process sequences */
    }

#if	DEBUGIT
  if (debugsw && debugms)
    printf ("msl():  EXIT  ndisp=%d\n", ndisp);
#endif
}
