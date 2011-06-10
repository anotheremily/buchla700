/*
   =============================================================================
	stcpos.c -- text cursor positioning for the score display
	Version 19 -- 1989-11-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "hwdefs.h"
#include "memory.h"
#include "fields.h"

#include "midas.h"
#include "scdsp.h"

#define	STCOBJ		obj3

#define	SDLINE		4	/* dividing line color - analog stuff */

extern unsigned exp_c ();

extern unsigned *STCOBJ;	/* text cursor address in VSDD RAM */

extern short cxval, cyval;	/* score cursor x,y */
extern short stcrow, stccol;	/* score cursor row,col */
extern short sctctab[10][64];	/* score background color table */

/* 
*/

short sctccon[10][64] = {	/* score background color constants */

  /* 0 - row  0 */

  {SDBG00, SDBG00, SDBG01, SDBG01, SDBG01, SDBG01, SDBG01, SDBG01,
   SDBG01, SDBG00, SDBG00, SDBG00, SDBG00, SDBG00, SDBG00, SDBG00,
   SDBG00, SDBG00, SDBG00, SDBG00, SDBG00, SDBG00, SDBG01, SDBG01,
   SDBG01, SDBG01, SDBG01, SDBG01, SDBG01, SDBG02, SDBG02, SDBG02,

   SDBG02, SDBG02, SDBG02, SDBG02, SDBG02, SDBG02, SDBG02, SDBG02,
   SDBG02, SDBG02, SDBG02, SDBG02, SDBG02, SDBG02, SDBG02, SDBG02,
   SDBG02, SDBG02, SDBG02, SDBG02, SDBG02, SDBG02, SDBG02, SDBG03,
   SDBG03, SDBG03, SDBG03, SDBG03, SDBG03, SDBG03, SDBG03, SDBG03},

  /* 1 - row 17 */

  {SDBG04, SDBG04, SDBG04, SDBG04, SDBG04, SDBG04, SDBG04, SDBG04,
   SDBG04, SDBG04, SDBG04, SDBG04, SDBG04, SDBG04, SDBG05, SDBG05,
   SDBG05, SDBG05, SDBG05, SDBG05, SDBG05, SDBG06, SDBG06, SDBG06,
   SDBG06, SDBG06, SDBG06, SDBG06, SDBG06, SDBG06, SDBG06, SDBG07,

   SDBG07, SDBG07, SDBG07, SDBG07, SDBG07, SDBG07, SDBG07, SDBG07,
   SDBG08, SDBG08, SDBG08, SDBG08, SDBG08, SDBG08, SDBG08, SDBG08,
   SDBG08, SDBG08, SDBG09, SDBG09, SDBG09, SDBG09, SDBG09, SDBG09,
   SDBG09, SDBG10, SDBG10, SDBG10, SDBG10, SDBG10, SDBG10, SDBG10},

  /* 2 - row 18 */

  {SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,

   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11},
/* 
*/
  /* 3 - row 19 */

  {SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,

   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11},

  /* 4 - row 20 */

  {SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,

   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11},

  /* 5 - row 21 */

  {SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,

   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11,
   SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11, SDBG11},
/* 
*/
  /* 6 - row 22 */

  {SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,
   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,
   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,
   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,

   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,
   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,
   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,
   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12},

  /* 7 - row 23 */

  {SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,
   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,
   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,
   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,

   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,
   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,
   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12,
   SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12, SDBG12},

  /* 8 - row 24 */

  {SDBG13, SDBG13, SDBG13, SDBG13, SDBG13, SDBG13, SDBG13, SDBG13,
   SDBG13, SDBG13, SDBG13, SDBG13, SDBG13, SDBG13, SDBG13, SDBG13,
   SDBG13, SDBG13, SDBG13, SDBG13, SDBG13, SDBG13, SDBG13, SDBG13,
   SDBG13, SDBG13, SDBG13, SDBG14, SDBG14, SDBG14, SDBG14, SDBG14,

   SDBG14, SDBG14, SDBG14, SDBG14, SDBG14, SDBG14, SDBG14, SDBG14,
   SDBG14, SDBG14, SDBG14, SDBG14, SDBG14, SDBG14, SDBG14, SDBG14,
   SDBG14, SDBG14, SDBG14, SDBG14, SDBG14, SDBG14, SDBG14, SDBG14,
   SDBG14, SDBG15, SDBG15, SDBG15, SDBG15, SDBG15, SDBG15, SDBG15},

  /* 9 - special - bottom of row 21 - top is entry 5 */

  {SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE,
   SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE,
   SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE,
   SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE,

   SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE,
   SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE,
   SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE,
   SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE, SDLINE}
};

/* 
*/

/*
   =============================================================================
	stcclr() -- turn off all of the score text cursors
   =============================================================================
*/

stcclr ()
{
  register unsigned *obj;
  register short i, j;
  register unsigned k;

  if ((v_regs[5] & 0x0180) NE 0x0100)
    vbank (1);			/* select the correct bank */

  stcrow = 0;			/* leave score cursor row,col = 0,0 */
  stccol = 0;

  obj = STCOBJ;			/* intialize pointers */

  for (i = 0; i < 9; i++)
    {				/* clear each cursor row */

      for (j = 0; j < 64; j++)
	{			/* top cursor row */

	  k = exp_c (sctctab[i][j]);

	  *obj++ = k;
	  *obj++ = k;
	}

      for (j = 0; j < 64; j++)
	{			/* bottom cursor row */

	  k = exp_c (sctctab[i EQ 5 ? 9 : i][j]);

	  *obj++ = k;
	  *obj++ = k;
	}
    }
}

/* 
*/

/*
   =============================================================================
	stcoff() -- turn off text cursor for the score display
   =============================================================================
*/

stcoff ()
{
  register unsigned ccv, *tcp;
  register short tcrow;

  tcrow = stcrow ? (stcrow - 16) : 0;	/* get old object row */

  if ((v_regs[5] & 0x0180) NE 0x0100)
    vbank (1);

  tcp = STCOBJ + (tcrow << 8) + (stccol << 1);	/* object pointer */

  /* background color */

  ccv = exp_c (sctctab[tcrow][stccol]);

  *tcp++ = ccv;			/* turn off the text cursor */
  *tcp = ccv;
}

/* 
*/

/*
   =============================================================================
	stcpos() -- position the score text cursor at ('row', 'col')
   =============================================================================
*/

stcpos (row, col)
     short row, col;
{
  register unsigned *ccp, *tcp;
  register unsigned curson, ccv;
  register short ncrow, tcrow;

  tcrow = stcrow ? (stcrow - 16) : 0;	/* get old object row */
  ncrow = row ? (row - 16) : 0;	/* get new object row */

  if ((v_regs[5] & 0x0180) NE 0x0100)
    vbank (1);			/* select the cursor bank */

  curson = exp_c (SDCURSR);	/* get cursor color */

  /* get old cursor background color */

  ccv = exp_c (sctctab[tcrow][stccol]);

  tcp = STCOBJ + (tcrow << 8) + (stccol << 1);	/* old cursor pointer */
  ccp = STCOBJ + (ncrow << 8) + (col << 1);	/* new cursor pointer */

  *tcp++ = ccv;			/* turn off the old cursor */
  *tcp = ccv;

  *ccp++ = curson;		/* turn on the new cursor */
  *ccp = curson;

  stcrow = row;			/* update cursor position */
  stccol = col;
}

/* 
*/

/*
   =============================================================================
	advscur() -- advance the score text cursor
   =============================================================================
*/

advscur ()
{
  register short newcol;

  if (infield (stcrow, stccol, curfet))
    cfetp = infetp;
  else
    return;

  newcol = stccol + 1;

  if (newcol LE cfetp->frcol)
    stcpos (stcrow, newcol);
}

/*
   =============================================================================
	stcset() -- setup the score text cursor color table
   =============================================================================
*/

stcset ()
{
  memcpyw (&sctctab[0][0], &sctccon[0][0], 640);
}
