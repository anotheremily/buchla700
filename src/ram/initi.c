/*
   =============================================================================
	initi.c -- MIDAS-VII - instrument editor support functions
	Version 2 -- 1989-12-19 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGPS		0	/* define non-zero to debug pntsel() */

#include "stddefs.h"
#include "fpu.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "graphdef.h"

#include "midas.h"
#include "instdsp.h"

extern int setipl ();

extern unsigned tofpu (), fromfpu ();

extern short s_inst[], vce2trg[], vce2grp[];

extern short curfunc;		/* current function number */
extern short curinst;		/* current instrument number */
extern short curpnt;		/* current point number (absolute) */
extern short curvce;		/* current voice number */
extern short cxval;		/* graphics cursor x value */
extern short editsw;		/* edit mode switch */
extern short initcfg;		/* initialization cfg value */
extern short npts;		/* number of points in function */
extern short pecase;		/* point edit case variable */
extern short pntsv;		/* point selection state variable */
extern short subj;		/* edited point number (relative) */
extern short temax;		/* time upper limit */
extern short temin;		/* time lower limit */

extern char *funcndx[256][2];	/* function header pointers */

extern short idbox[][8];	/* display box parameters */
extern short instmod[12];	/* intrument data modified */

extern short fnoff[];		/* instrument fn to FPU fn table */

extern struct instpnt *pntptr;	/* pointer to edited point */

extern struct instdef idefs[NINST];	/* current instrument definitions */
extern struct instdef vbufs[12];	/* voice instrument buffers */

/* 
*/

#include "dfltins.h"		/* short dfltins[]; default instrument */
#include "ptoftab.h"		/* short ptoftab[]; pitch to frequency table */

short finival[NFINST] = {	/* initial function values */

  FPUVAL (500),			/*  0: Freq 1 */
  FPUVAL (500),			/*  1: Freq 2 */
  FPUVAL (500),			/*  2: Freq 3 */
  FPUVAL (500),			/*  3: Freq 4 */
  FPUVAL (500),			/*  4: Filter / Resonance */
  FPUVAL (500),			/*  5: Location */

  FPUVAL (0),			/*  6: Index 1 */
  FPUVAL (0),			/*  7: Index 2 */
  FPUVAL (0),			/*  8: Index 3 */
  FPUVAL (0),			/*  9: Index 4 */
  FPUVAL (0),			/* 10: Index 5 */
  FPUVAL (0),			/* 11: Index 6 */

  FPUVAL (0)			/* 12: Level */
};

short expbit[16] = {		/* FPU time exponent bit expansion table */

  0x0001,			/* 0 */
  0x0002,			/* 1 */
  0x0004,			/* 2 */
  0x0008,			/* 3 */
  0x0010,			/* 4 */
  0x0020,			/* 5 */
  0x0040,			/* 6 */
  0x0080,			/* 7 */
  0x0100,			/* 8 */
  0x0200,			/* 9 */
  0x0400,			/* 10 */
  0x0800,			/* 11 */
  0x1000,			/* 12 */
  0x2000,			/* 13 */
  0x4000,			/* 14 */
  0x8000			/* 15 */
};

/* 
*/

/*
   =============================================================================
	segtime() -- determine time to a point using a new segment time
   =============================================================================
*/

short
segtime (pn, ptime)
     short pn;
     unsigned ptime;
{
  register struct instpnt *pp;
  register short i;
  register unsigned tacc;

  pp = &vbufs[curvce].idhpnt[vbufs[curvce].idhfnc[curfunc].idfpt1];
  tacc = 0;

  for (i = 0; i < pn; i++)
    tacc += fromfpu (pp++->iptim);

  tacc += fromfpu (ptime);

  return (tacc);
}

/*
   =============================================================================
	setseg() -- set the time to a segment
   =============================================================================
*/

setseg (pn, ptime)
     short pn;
     unsigned ptime;
{
  register short i;
  register unsigned tacc;
  register struct instpnt *pp;

  pp = &vbufs[curvce].idhpnt[vbufs[curvce].idhfnc[curfunc].idfpt1];
  tacc = 0;

  for (i = 0; i < pn; i++)
    tacc += fromfpu (pp++->iptim);

  pp->iptim = tofpu (ptime - tacc);
}

/* 
*/

/*
   =============================================================================
	timeto() -- find the time at a point in a function
   =============================================================================
*/

short
timeto (fn, pj)
     short fn, pj;
{
  register struct instdef *ip;
  register struct instpnt *pt;
  register short tf, pm, pn;

  ip = &vbufs[curvce];
  pn = ip->idhfnc[fn].idfpt1;
  pt = &ip->idhpnt[pn];
  pm = pn + pj;
  tf = 0;

  while (pn LE pm)
    {

      tf += fromfpu (pt->iptim);
      ++pt;
      ++pn;
    }

  return (tf);
}

/* 
*/

/*
   =============================================================================
	pntsel() -- setup editing limits for subj in the current function
   =============================================================================
*/

pntsel ()
{
  register struct idfnhdr *fp;

  fp = &vbufs[curvce].idhfnc[curfunc];

  curpnt = fp->idfpt1 + subj;
  pntptr = &vbufs[curvce].idhpnt[curpnt];

  npts = fp->idfpif;

  if (npts EQ 1)
    {				/* detemine editing case */

      pecase = 0;		/* single point */
      temin = 1;
      temax = 32767;

    }
  else if (subj EQ (npts - 1))
    {

      pecase = 1;		/* last point */
      temin = timeto (curfunc, subj - 1);
      temax = 32767;

    }
  else
    {

      pecase = 2;		/* interior point */
      temin = timeto (curfunc, subj - 1);
      temax = timeto (curfunc, subj + 1);
    }

#if DEBUGPS
  printf ("pntsel(): curpnt=%d, subj=%d, fp=0x%08lx, pntptr=0x%08lx\r\n",
	  curpnt, subj, fp, pntptr);
  printf ("pntsel(): npts=%d, pecase=%d, temin=%d, temax=%d\r\n",
	  npts, pecase, temin, temax);
#endif
}

/* 
*/

/*
   =============================================================================
	vtoy() -- convert a value to a y coordinate for display
   =============================================================================
*/

short
vtoy (val, window)
     short val, window;
{
  register short yval;

  if (val GT 1000)
    val = 1000;

  yval = 56 + (((1000 - val) * 14) / 100);

  if (window < 12)
    return ((short) (idbox[window][1] + (((yval - 56) * 100L) / 540L)));
  else
    return (yval);
}

/* 
*/

/*
   =============================================================================
	ttox() -- convert a time to an x coordinate for display
   =============================================================================
*/

short
ttox (time, window)
     unsigned time;
     short window;
{
  register short xval;

  if (time < 128)		/* 0..127 */
    xval = 8 + (short) (((time * 1000L) / 2309L));
  else if (time < 256)		/* 128..255 */
    xval = 64 + (short) ((((time - 128) * 1000L) / 2309L));
  else if (time < 512)		/* 256..511 */
    xval = 120 + (short) ((((time - 256) * 1000L) / 4636L));
  else if (time < 1024)		/* 512..1023 */
    xval = 176 + (short) ((((time - 512) * 1000L) / 9290L));
  else if (time < 2048)		/* 1024..2047 */
    xval = 232 + (short) ((((time - 1024) * 1000L) / 18600L));
  else if (time < 4096)		/* 2048..4095 */
    xval = 288 + (short) ((((time - 2048) * 1000L) / 37218L));
  else if (time < 8192)		/* 4096..8191 */
    xval = 344 + (short) ((((time - 4096) * 1000L) / 74454L));
  else if (time < 16384)	/* 8192..16383 */
    xval = 400 + (short) ((((time - 8192) * 1000L) / 148927L));
  else if (time < (unsigned) 32768)	/* 16384..32767 */
    xval = 456 + (short) ((((time - 16384) * 1000L) / 309113L));
  else
    xval = 509;

  if (xval > 509)
    xval = 509;

  if (window < 12)
    return ((short) (idbox[window][0] + (((xval - 8) * 100L) / 599L)));
  else
    return (xval);
}

/* 
*/

/*
   =============================================================================
	selpnt() -- select a point for editing
   =============================================================================
*/

short
selpnt ()
{
  register struct idfnhdr *fp;

  register short i, xv, np;

  short lo_x, lo_x_pt, hi_x, hi_x_pt;

  fp = &vbufs[curvce].idhfnc[curfunc];
  np = fp->idfpif;
  lo_x_pt = 0;
  lo_x = ttox (timeto (curfunc, lo_x_pt), 12);
  hi_x_pt = np - 1;
  hi_x = ttox (timeto (curfunc, hi_x_pt), 12);

  for (i = 0; i < np; i++)
    {

      xv = ttox (timeto (curfunc, i), 12);

      if (xv LE cxval)		/* largest x LE cxval */
	if (xv GE lo_x)
	  {

	    lo_x = xv;
	    lo_x_pt = i;
	  }

      if (xv GE cxval)		/* smallest x GE cxval */
	if (xv LT hi_x)
	  {

	    hi_x = xv;
	    hi_x_pt = i;
	  }
    }

  if (lo_x EQ hi_x)
    return (lo_x_pt);

  if ((cxval - lo_x) LT (hi_x - cxval))
    return (lo_x_pt);
  else
    return (hi_x_pt);
}

/* 
*/

/*
   =============================================================================
	setinst() -- setup for editing a new instrument
   =============================================================================
*/

setinst ()
{
  register struct instdef *ip;
  register struct idfnhdr *fp;

  curfunc = 12;			/* current function = 12 = level */
  subj = 0;			/* current edited point = 0 (first point) */
  pntsv = 0;			/* point selection state = unselected */
  pecase = 0;			/* point selection case = 0 (first point) */

  ip = &vbufs[curvce];		/* instrument definition pointer */
  fp = &ip->idhfnc[curfunc];	/* function pointer */

  curpnt = subj + fp->idfpt1;	/* current point in function */
  pntptr = &ip->idhpnt[curpnt];	/* current point pointer */

  newws ();			/* setup waveshape variables */
}

/* 
*/

/*
   =============================================================================
	newinst() -- select a new instrument
   =============================================================================
*/

newinst (inst)
     short inst;
{
  curinst = inst;
  s_inst[curvce] = inst;
  setinst ();
}

/*
   =============================================================================
	newvce() -- select a new voice
   =============================================================================
*/

newvce (voice)
     short voice;
{
  curvce = voice;
  curinst = s_inst[curvce];
  setinst ();
}

/* 
*/

/*
   =============================================================================
	setivce() -- set instrument definition for a voice
   =============================================================================
*/

setivce (voice)
     short voice;
{
  register short i, j, k, oldi;
  register struct instdef *ip;

  ip = &vbufs[voice];

  oldi = setipl (FPU_DI);	/* disable FPU interrupts */

/* +++++++++++++++++++++++ FPU interrupts disabled ++++++++++++++++++++++++++ */

  for (j = 0; j < NFINST; j++)
    {				/* set funcndx[] entries */

      k = (voice << 4) + fnoff[j];
      funcndx[k][0] = &ip->idhfnc[j];
      funcndx[k][1] = ip->idhpnt;
    }

  /* set waveshapes */

  memcpyw ((io_fpu + FPU_OWST + (voice << 9) + 0x0100 + 1),
	   ip->idhwvaf, NUMWPNT);

  memcpyw ((io_fpu + FPU_OWST + (voice << 9) + 1), ip->idhwvbf, NUMWPNT);

  setipl (oldi);		/* restore interrupts */

/* ++++++++++++++++++++++++++ Interrupts restored +++++++++++++++++++++++++++ */

  instmod[voice] = FALSE;	/* instrument matches library */
}

/* 
*/

/*
   =============================================================================
	modinst() -- handle instrument modifications
   =============================================================================
*/

modinst ()
{
  short f, i, grp, oldi;

  if (NOT instmod[curvce])
    {

      instmod[curvce] = TRUE;
      dswin (19);
    }

  if (NOT editsw)
    {

      if ((grp = vce2grp[curvce]) > 0)
	{

	  oldi = setipl (FPU_DI);	/* +++++ disable FPU interrupts */

	  for (i = 0; i < 12; i++)
	    {

	      if (vce2grp[i] EQ grp)
		{

		  memcpyw (&vbufs[i], &vbufs[curvce],
			   sizeof (struct instdef) / 2);

		  for (f = 0; f < 12; f++)
		    vbufs[i].idhfnc[f].idftmd &= ~I_ACTIVE;

		  s_inst[i] = curinst;
		  clrvce (i);
		  setivce (i);
		  dosync (i);
		  execkey (0, 0, i, 1);
		  vce2trg[i] = -1;
		}
	    }

	  setipl (oldi);	/* +++++ enable FPU interrupts */
	}
    }
}

/* 
*/

/*
   =============================================================================
	initi() -- initialize an instrument definition
   =============================================================================
*/

initi (ip)
     register struct instdef *ip;
{
  register short i, mintfpu, rb;

  memsetw (ip, 0, sizeof (struct instdef) / 2);

  memcpy (ip->idhname, "                ", MAXIDLN + 1);
  memcpy (ip->idhcom1, "                ", MAXIDLN + 1);
  memcpy (ip->idhcom2, "                ", MAXIDLN + 1);
  memcpy (ip->idhcom3, "                ", MAXIDLN + 1);

  ip->idhplft = NIPNTS - NFINST;
  ip->idhcfg = (initcfg >> 8) & 0x00FF;
  mintfpu = tofpu (1);

  for (i = 0; i < NFINST; i++)
    {				/* for each function ... */

      /* initialize the function header */

      rb = ((i < 4) AND (i NE 0)) ? 0 : I_NRATIO;
      ip->idhfnc[i].idfpif = 1;
      ip->idhfnc[i].idfpt1 = i;
      ip->idhfnc[i].idftmd = (I_TM_KEY | 0x0010) | rb;

      /* initialize the first point in the function */

      ip->idhpnt[i].iptim = mintfpu;
      ip->idhpnt[i].ipval = finival[i];
    }

  for (i = 0; i < NUMWPNT; i++)
    {

      ip->idhwvaf[i] = (ip->idhwvao[i] = 0x8000 ^ ((i + 1) << 8));
      ip->idhwvbf[i] = (ip->idhwvbo[i] = 0x8000 ^ ((i + 1) << 8));
    }
}

/* 
*/

/*
   =============================================================================
	initil() -- initialize instrument library
   =============================================================================
*/

initil ()
{
  register short i;

  setipl (FPU_DI);		/* disable FPU interrupts */

/* +++++++++++++++++++++++ FPU interrupts disabled ++++++++++++++++++++++++++ */

  fpuclr ();			/* clear the FPU */

  memcpyw (&idefs[0], dfltins, sizeof (struct instdef) / 2);

  for (i = 1; i < NINST; i++)	/* initialize each instrument */
    initi (&idefs[i]);

  memset (funcndx, 0, sizeof funcndx);

  for (i = 0; i < 12; i++)
    {				/* initialize voices */

      memcpyw (&vbufs[i], &idefs[0], sizeof (struct instdef) / 2);
      setivce (i);
    }

  setipl (FPU_EI);		/* enable FPU interrupts */

/* ++++++++++++++++++++++++++ Interrupts enabled ++++++++++++++++++++++++++++ */

  newvce (0);			/* setup editing for instrument 0, voice 0 */
  newinst (0);
}
