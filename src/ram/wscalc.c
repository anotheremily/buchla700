/*
   =============================================================================
	wscalc.c -- MIDAS-VII waveshape editor harmonic functions
	Version 9 -- 1988-09-09 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "macros.h"

#include "midas.h"
#include "wsdsp.h"

#define	WAVESMAX	1023
#define	WAVESMIN	1023

extern short curwhrm;

extern long hwave[NUMWPCAL];

extern short offsets[NUMWPCAL];
extern short vmtab[NUMHARM];
extern short wsbuf[NUMWPCAL];

extern long vknm[NUMHARM][NUMWPCAL];

#include "knmtab.h"		/* short knmtab[NUMHARM][NUMWPCAL]; */

/* 
*/

/*
   =============================================================================
	adj() -- adjust the coefficients in vknm[wshar][] for a new value
   =============================================================================
*/

adj (wshar)
     register short wshar;
{
  register short wspnt;
  register long harval;
  register short *kp;
  register long *vp;

  vp = &vknm[wshar][0];

  harval = vmtab[wshar];

  if (harval)
    {

      kp = &knmtab[wshar][0];

      for (wspnt = 0; wspnt < NUMWPCAL; wspnt++)
	*vp++ = *kp++ * harval;

    }
  else
    {

      for (wspnt = 0; wspnt < NUMWPCAL; wspnt++)
	*vp++ = 0;
    }
}

/* 
*/

/*
   =============================================================================
	wadj() -- adjust the coefficients for all harmonics
   =============================================================================
*/

wadj ()
{
  register short wshar;

  for (wshar = 0; wshar < NUMHARM; wshar++)
    adj (wshar);
}

/*
   =============================================================================
	clrwsa() -- clear waveshape table harmonic work areas
   =============================================================================
*/

clrwsa ()
{
  memsetw (offsets, 0, NUMWPCAL);
  memsetw (vknm, 0, (NUMHARM * NUMWPCAL) << 1);
  memsetw (vmtab, 0, NUMHARM);
}

/* 
*/

/*
   =============================================================================
	wscalc() -- calculate a waveshape from its harmonics and offsets
   =============================================================================
*/

wscalc ()
{
  register short wspnt, wshar;
  register long hfac, hmax, temp;

  hmax = WAVESMIN;		/* set minimum scaling value */

  for (wspnt = 0; wspnt < NUMWPCAL; wspnt++)
    {

      temp = 0;			/* sum up the harmonics */

      for (wshar = 0; wshar < NUMHARM; wshar++)
	temp += vknm[wshar][wspnt];

      /* add in the offsets */

      hwave[wspnt] = (temp / 100) + offsets[wspnt];

      /* adjust the maximum value seen */

      if ((temp = abs (hwave[wspnt])) > hmax)
	hmax = temp;
    }

  /* calculate the scale factor */

  hfac = ((long) WAVESMAX << 16) / hmax;

  /* scale the waveshape */

  for (wspnt = 0; wspnt < NUMWPCAL; wspnt++)
    wsbuf[wspnt] = (hwave[wspnt] * hfac) >> 16;
}
