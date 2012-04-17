/*
   =============================================================================
	uslice.c -- update score display slice
	Version 32 -- 1988-09-28 -- D.N. Lynx Crowe
   =============================================================================
*/

#undef	DEBUGGER

#define	DEBUGIT		0

#include "debug.h"

#include "stddefs.h"
#include "slice.h"
#include "hwdefs.h"
#include "score.h"
#include "vsdd.h"
#include "graphdef.h"

#include "midas.h"
#include "scdsp.h"
#include "scfns.h"

#define	BARCOLOR	0x3333

/* variables defined elsewhere */

#if DEBUGIT
extern short debugsw;
#endif

extern unsigned scrl;

extern short sbase;
extern short sd;
extern short soffset;

extern unsigned *consl;
extern unsigned *cursl;
extern unsigned *nxtsl;
extern unsigned *prvsl;
extern unsigned *saddr;

extern unsigned slices[];

extern short gtctab[12];	/* group to color table */

extern struct gdsel *gdfsep;	/* gdsel freechain pointer */

extern struct gdsel *gdstbc[NGDSEL];	/* group status list heads */
extern struct gdsel *gdstbn[NGDSEL];	/* group status list heads */
extern struct gdsel *gdstbp[NGDSEL];	/* group status list heads */

extern struct gdsel gdfsl[MAXFSL];	/* gdsel pool */

/* 
*/

/* initialized data */

short lintab1[] = {		/* note top line table */

  205, 205, 201, 197, 197, 193, 193, 189, 185, 185, 181, 181,
  177, 177, 173, 169, 169, 165, 165, 161, 157, 157, 153, 153,
  149, 149, 145, 141, 141, 137, 137, 133, 129, 129, 125, 125,
  121, 121, 117, 113, 113, 109, 109, 105, 101, 101, 97, 97,
  93, 93, 89, 85, 85, 81, 81, 77, 73, 73, 69, 69,
  65, 65, 61, 57, 57, 53, 53, 49, 45, 45, 41, 41,
  37, 37, 33, 29, 29, 25, 25, 21, 17, 17, 13, 13,
  9, 9, 5, 1
};

short lintab2[] = {		/* note top line table - N_FLAT */

  205, 201, 201, 197, 193, 193, 189, 189, 185, 181, 181, 177,
  177, 173, 173, 169, 165, 165, 161, 161, 157, 153, 153, 149,
  149, 145, 145, 141, 137, 137, 133, 133, 129, 125, 125, 121,
  121, 117, 117, 113, 109, 109, 105, 105, 101, 97, 97, 93,
  93, 89, 89, 85, 81, 81, 77, 77, 73, 69, 69, 65,
  65, 61, 61, 57, 53, 53, 49, 49, 45, 41, 41, 37,
  37, 33, 33, 29, 25, 25, 21, 21, 17, 13, 13, 9,
  9, 5, 5, 1
};

short nttab1[] = {		/* forward code transition table */

  0,				/* 0 */
  2,				/* 1 */
  3,				/* 2 */
  3,				/* 3 */
  5,				/* 4 */
  3,				/* 5 */
  0				/* 6 */
};

short nttab2[] = {		/* backward code transition table */

  0,				/* 0 */
  0,				/* 1 */
  1,				/* 2 */
  3,				/* 3 */
  0,				/* 4 */
  4,				/* 5 */
  0				/* 6 */
};

/* 
*/

short pxtbl[][4] = {		/* pixel mask table for notes */

  {0x0000, 0x0000, 0x0000, 0},	/* 0 */
  {0x0000, 0x0000, 0xFFFF, 0},	/* 1 */
  {0x0000, 0xFFFF, 0xFFFF, 0},	/* 2 */
  {0xFFFF, 0xFFFF, 0xFFFF, 0},	/* 3 */
  {0xFFFF, 0x0000, 0x0000, 0},	/* 4 */
  {0xFFFF, 0xFFFF, 0x0000, 0},	/* 5 */
  {0xFFFF, 0xFFFF, 0xFFFF, 0}	/* 6 */
};

short epxtbl[][16] = {		/* pixel mask table for events */

  {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,	/* 0 */
   0xFFFF, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000,
   0, 0, 0, 0},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 1 */
   0x0000, 0xFFFF, 0xFFFF, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000,
   0, 0, 0, 0},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 2 */
   0x0000, 0x0000, 0x0000, 0xFFFF,
   0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
   0, 0, 0, 0},

  {0xFFFF, 0xFFFF, 0xFFFF, 0x0000,	/* 3 */
   0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000,
   0, 0, 0, 0},

  {0x0000, 0x0000, 0x0000, 0xFFFF,	/* 4 */
   0xFFFF, 0xFFFF, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000,
   0, 0, 0, 0},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 5 */
   0x0000, 0x0000, 0xFFFF, 0xFFFF,
   0xFFFF, 0x0000, 0x0000, 0x0000,
   0, 0, 0, 0},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 6 */
   0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0xFFFF, 0xFFFF, 0xFFFF,
   0, 0, 0, 0}
};

/* 
*/

/*
   =============================================================================
	uslice(slice, mask1, mask2, gdstb)

	Updates 'slice[]' using 'mask1' and 'mask2' for the pixel mask
	and the slice mask, respectively, and 'gdstb[]' for the group table.

	Note transitions are based on the value of global variable 'sd',
	which is zero for forward, and non-zero for backward scrolling.

	Returns count of active groups and event priority levels.

	Note colors come from 'gtctab[]'.

	This code must be very fast or the display bogs down the system.
   =============================================================================
*/

short
uslice (slice, mask1, mask2, gdstb)
     unsigned *slice;
     register unsigned mask1, mask2;
     struct gdsel *gdstb[];
{
  register unsigned *pxptr, *slptr;
  register struct gdsel *gdsep;
  register unsigned i, ncolor, w;
  unsigned *ncptr, *nctabp;
  struct gdsel **gdstp, *gdprv, *gdnxt;
  short wrote;
  short *lintab;

  DB_ENTR ("uslice()");

  /* initialize note color, note control, and group control pointers */

  gdstp = gdstb;		/* group control table */
  nctabp = sd ? nttab2 : nttab1;	/* note transition table */
  ncptr = gtctab;		/* group to color table */
  wrote = 0;			/* reset count of active groups */

  lintab = (ac_code == N_SHARP) ? lintab1 : lintab2;	/* line table */

  if (gdstb[NGDSEL - 1])
    {				/* check for a bar marker */

      slptr = slice;		/* point at the slice */
      ncolor = mask1 & BARCOLOR;	/* setup bar write variable */

      for (i = 211; i--;)	/* draw the bar */
	*slptr++ = (*slptr & mask2) | ncolor;

      gdstb[NGDSEL - 1] = (struct gdsel *) NULL;	/* clear flag */
    }

/* 
*/

  for (i = 0; i < 12; i++)
    {				/* scan the group control table ... */

      ncolor = mask1 & *ncptr++;	/* get note color mask */
      gdprv = (struct gdsel *) gdstp;	/* setup previous pointer */
      gdsep = *gdstp++;		/* setup current pointer */

      if (gdsep)
	{			/* ... process each active group ... */

	  ++wrote;		/* indicate slice was updated */

	  do
	    {			/* ... for each note played by the group ... */

	      /* setup slice and pixel pointers */

	      slptr = slice + lintab[gdsep->note];
	      pxptr = &pxtbl[gdsep->code][0];

	      /* update the slice */

	      *slptr++ = (*slptr & mask2) | (*pxptr++ & ncolor);
	      *slptr++ = (*slptr & mask2) | (*pxptr++ & ncolor);
	      *slptr = (*slptr & mask2) | (*pxptr++ & ncolor);

	      gdnxt = gdsep->next;	/* set next pointer */

	      /* update and check update note status code */

	      if (0 == (gdsep->code = nctabp[gdsep->code]))
		{

		  /* if it's zero, delete the element */

		  gdprv->next = gdnxt;
		  gdsep->next = gdfsep;
		  gdfsep = gdsep;

		}
	      else
		gdprv = gdsep;

	      gdsep = gdnxt;	/* set pointer for next pass */

	    }
	  while (gdsep);
	}
    }

/* 
*/
  for (; i < (NGDSEL - 1); i++)
    {

      gdprv = (struct gdsel *) gdstp;	/* setup previous pointer */
      gdsep = *gdstp++;		/* setup current pointer */

      if (gdsep)
	{			/* ... process each active event priority ... */

	  ++wrote;		/* indicate slice was updated */

	  do
	    {			/* ... for each event of this priority  ... */

	      /* setup slice and pixel pointers */

	      slptr = slice + 212;
	      pxptr = &epxtbl[gdsep->code][0];

	      /* get event color */

	      ncolor = mask1 & gdsep->note;

	      /* update the slice */

	      if (w = *pxptr++)	/* 212 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 213 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 214 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 215 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 216 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 217 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 218 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 219 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 220 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 221 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 222 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr)	/* 223 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      gdnxt = gdsep->next;	/* set next pointer */
	      gdprv->next = gdnxt;	/* delete the element */
	      gdsep->next = gdfsep;
	      gdfsep = gdsep;
	      gdsep = gdnxt;	/* set pointer for next pass */

	    }
	  while (gdsep);
	}
    }

  DB_EXIT (wrote ? "uslice - slice written" : "uslice - no write");

  return (wrote);
}

/* 
*/

/*
   =============================================================================
	rslice(gdstb)

	Reverses the slice codes in 'gdstb' based the value of sd,
	which is zero for forward, and non-zero for backward scrolling.
   =============================================================================
*/

rslice (gdstb)
     struct gdsel *gdstb[];
{
  register struct gdsel *gdsep;
  register struct gdsel **gdstp;
  register unsigned *nctabp;
  register unsigned i, nc;
  struct gdsel *gdprv, *gdnxt;

  DB_ENTR ("rslice");

#if DEBUGIT
  if (debugsw)
    printf ("\nrslice($%lX):  sd = %s\n", gdstb, sd ? "BAK" : "FWD");
#endif

  /* initialize note control and group control pointers */

  nctabp = sd ? nttab2 : nttab1;	/* note control table pointer */
  gdstp = gdstb;		/* group control table pointer */

/* 
*/
  /* reverse note codes */

  for (i = 0; i < 12; i++)
    {				/* scan the group control table ... */

      gdprv = (struct gdsel *) gdstp;	/* setup previous pointer */
      gdsep = *gdstp++;		/* setup current pointer */

      if (gdsep)
	{			/* ... process each active group ... */

	  do
	    {			/* ... for each note played by the group ... */

	      gdnxt = gdsep->next;	/* set next pointer */

	      /* update and check update note status code */

	      if (0 == (gdsep->code = nctabp[nc = gdsep->code]))
		{

#if DEBUGIT
		  if (debugsw)
		    printf ("rslice:  note %d - %d -> %d\n",
			    gdsep->note, nc, gdsep->code);
#endif

		  /* if it's zero, delete the element */

		  gdprv->next = gdnxt;
		  gdsep->next = gdfsep;
		  gdfsep = gdsep;

		}
	      else
		{

#if DEBUGIT
		  if (debugsw)
		    printf ("rslice:  note %d - %d -> %d\n",
			    gdsep->note, nc, gdsep->code);
#endif

		  gdprv = gdsep;
		}

	      gdsep = gdnxt;	/* set pointer for next pass */

	    }
	  while (gdsep);
	}
    }

/* 
*/
  /* delete event codes */

  for (; i < NGDSEL - 1; i++)
    {

      gdprv = (struct gdsel *) gdstp;	/* setup previous pointer */
      gdsep = *gdstp++;		/* setup current pointer */

      if (gdsep)
	{			/* ... process each active event priority ... */

	  do
	    {			/* ... for each event of this priority  ... */

	      gdnxt = gdsep->next;	/* set next pointer */
	      gdprv->next = gdnxt;	/* delete the element */
	      gdsep->next = gdfsep;
	      gdfsep = gdsep;
	      gdsep = gdnxt;	/* set pointer for next pass */

	    }
	  while (gdsep);
	}
    }

  gdstb[NGDSEL - 1] = (struct gdsel *) NULL;	/* clear bar flag */

  DB_EXIT ("rslice");
}

/* 
*/

/*
   =============================================================================
	cslice(slice, mask1, mask2, gdstb)

	Updates 'slice[]' using 'mask1' and 'mask2' for the pixel mask
	and the slice mask, respectively, using 'gdstb[]' as the group table.

	Differs from uslice() in that no note gdstb events are deleted.
	Used for center slice updates.

	Note transitions are based on the value of global variable 'sd',
	which is zero for forward, and non-zero for backward scrolling.

	Returns count of active groups and event priority levels.

	Note colors come from 'gtctab[]'.

	This code has to be very fast.
   =============================================================================
*/

short
cslice (slice, mask1, mask2, gdstb)
     unsigned *slice;
     register unsigned mask1, mask2;
     struct gdsel *gdstb[];
{
  register unsigned *pxptr, *slptr;
  register struct gdsel *gdsep;
  register unsigned i, ncolor, w;
  unsigned *ncptr, *nctabp;
  struct gdsel **gdstp, *gdprv, *gdnxt;
  short wrote;
  short *lintab;

  DB_ENTR ("cslice()");

  /* initialize note color, note control, and group control pointers */

  gdstp = gdstb;		/* group control table */
  nctabp = sd ? nttab2 : nttab1;	/* note transition table */
  ncptr = gtctab;		/* group to color table */
  wrote = 0;			/* reset count of active groups */

  lintab = (ac_code == N_SHARP) ? lintab1 : lintab2;	/* line table */

  if (gdstb[NGDSEL - 1])
    {				/* check for a bar marker */

      slptr = slice;		/* point at the slice */
      ncolor = mask1 & BARCOLOR;	/* setup bar write variable */

      for (i = 211; i--;)	/* draw the bar */
	*slptr++ = (*slptr & mask2) | ncolor;

      gdstb[NGDSEL - 1] = (struct gdsel *) NULL;	/* clear flag */
    }

/* 
*/

  for (i = 0; i < 12; i++)
    {				/* scan the group control table ... */

      ncolor = *ncptr++;	/* get note color */
      gdprv = (struct gdsel *) gdstp;	/* setup previous pointer */
      gdsep = *gdstp++;		/* setup current pointer */

      if (gdsep)
	{			/* ... process each active group ... */

#ifdef	DEBUGGER
	  sprintf (DBvar, "cslice - g=%d  gdsep=$%lX", i, gdsep);
	  DB_CMNT (DBvar);
#endif
	  ++wrote;		/* indicate slice was updated */

	  do
	    {			/* ... for each note played by the group ... */

	      /* setup slice and pixel pointers */

	      slptr = slice + lintab[gdsep->note];
	      pxptr = &pxtbl[gdsep->code][0];

	      /* update the slice */

	      *slptr++ = (*slptr & mask2) | (*pxptr++ & ncolor);
	      *slptr++ = (*slptr & mask2) | (*pxptr++ & ncolor);
	      *slptr = (*slptr & mask2) | (*pxptr++ & ncolor);

	      gdnxt = gdsep->next;	/* set nxt pointer */
	      gdprv = gdsep;	/* set prv pointer */
	      gdsep = gdnxt;	/* set pointer for next pass */

	    }
	  while (gdsep);
	}
    }

/* 
*/
  for (; i < (NGDSEL - 1); i++)
    {				/* scan the event priorities ... */

      gdprv = (struct gdsel *) gdstp;	/* setup previous pointer */
      gdsep = *gdstp++;		/* setup current pointer */

      if (gdsep)
	{			/* ... process each active event priority ... */

	  ++wrote;		/* indicate slice was updated */

	  do
	    {			/* ... for each event of this priority  ... */

	      /* setup slice and pixel pointers */

	      slptr = slice + 212;
	      pxptr = &epxtbl[gdsep->code][0];

	      ncolor = gdsep->note;	/* get event color */

	      /* update the slice */

	      if (w = *pxptr++)	/* 212 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 213 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 214 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 215 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 216 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 217 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 218 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 219 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 220 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 221 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr++)	/* 222 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      ++slptr;

	      if (w = *pxptr)	/* 223 */
		*slptr = (*slptr & mask2) | (w & ncolor);

	      gdnxt = gdsep->next;	/* set next pointer */
	      gdprv->next = gdnxt;	/* delete the element */
	      gdsep->next = gdfsep;
	      gdfsep = gdsep;
	      gdsep = gdnxt;	/* set pointer for next pass */

	    }
	  while (gdsep);
	}
    }

  DB_EXIT (wrote ? "cslice - slice written" : "cslice - no write");

  return (wrote);
}

/* 
*/

/*
   =============================================================================
	clrsctl() -- clear slice control list and pointers
   =============================================================================
*/

clrsctl ()
{
  register struct gdsel *gdsp, *gnxt, **gp;
  register short i;

  gdfsep = gdsp = &gdfsl[0];	/* initialize slice control pool */
  gnxt = &gdfsl[1];

  for (i = MAXFSL - 1; i--;)
    {

      gdsp->code = 0;
      gdsp->note = 0;
      gdsp->next = gnxt;

      gdsp++;
      gnxt++;
    }

  gdsp->code = 0;
  gdsp->note = 0;
  gdsp->next = (struct gdsel *) 0L;

  gp = gdstbc;			/* clear gdstbc */

  for (i = NGDSEL; i--;)
    *gp++ = (struct gdsel *) 0L;

  gp = gdstbn;			/* clear gdstbn */

  for (i = NGDSEL; i--;)
    *gp++ = (struct gdsel *) 0L;

  gp = gdstbp;			/* clear gdstbp */

  for (i = NGDSEL; i--;)
    *gp++ = (struct gdsel *) 0L;

  sbase = I_SBASE;		/* initialize score video RAM offset */
  soffset = I_OFFSET;		/* initialize scroll register offset */
  saddr = &v_score[sbase];	/* initialize score area base address */

  consl = &slices[672];		/* initialize constant slice pointer */
  prvsl = &slices[448];		/* initialize previous slice pointer */
  cursl = &slices[224];		/* initialize current slice pointer */
  nxtsl = &slices[0];		/* initialize next slice pointer */
/* 
*/
  {
    register unsigned *p1, *p2, *p3, sword;
    unsigned *p4;

    p1 = consl;			/* clear slices */
    p2 = nxtsl;
    p3 = cursl;
    p4 = prvsl;

    for (i = 224; i--;)
      {

	sword = *p1++;
	*p2++ = sword;
	*p3++ = sword;
	*p4++ = sword;
      }
  }
}
