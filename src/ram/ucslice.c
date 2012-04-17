/*
   =============================================================================
	ucslice.c -- update score display center slice
	Version 7 -- 1988-09-28 -- D.N. Lynx Crowe
   =============================================================================
*/

#undef	DEBUGGER

#include "debug.h"

#include "stddefs.h"
#include "slice.h"
#include "hwdefs.h"
#include "score.h"
#include "graphdef.h"
#include "vsdd.h"

#include "midas.h"
#include "scdsp.h"
#include "scfns.h"

/* variables defined elsewhere */

extern unsigned *saddr;		/* score object base address */

extern short ndisp;		/* current display number */
extern short sd;		/* scroll direction flag */
extern short soffset;		/* score object scroll offset */

extern short gtctab[];		/* group to color table */
extern short lintab1[];		/* note top line table - N_SHARP */
extern short lintab2[];		/* note top line table - N_FLAT */
extern short nbmasks[];		/* pixel mask table for slices */
extern short nttab1[];		/* gdsel code transition table - fwd */
extern short nttab2[];		/* gdsel code transition table - bak */

extern short pxtbl[][4];	/* pixel mask table - notes */
extern short epxtbl[][16];	/* pixel mask table - events */

extern struct gdsel *gdfsep;	/* gdsel freechain pointer */

extern struct gdsel *gdstbc[];	/* group status list heads */

/* 
*/

/*
   =============================================================================
	ucslice()

	Updates the center slice.

	Note transitions are based on the value of global variable 'sd',
	which is zero for forward, and non-zero for backward scrolling.

	Note colors come from 'gtctab[]'.

	This code must be very fast or the display bogs down the system.
   =============================================================================
*/

ucslice ()
{
  register unsigned *pxptr, *slptr;
  register struct gdsel *gdsep;
  register long lc;
  register unsigned i, ncolor, mask1, mask2;
  unsigned *ncptr, *nctabp;
  struct gdsel **gdstp, *gdprv, *gdnxt;
  short *lintab;

  DB_ENTR ("ucslice()");

  lc = 128L;
  mask1 = nbmasks[soffset];
  mask2 = ~mask1;

  /* initialize note color, note control, and group control pointers */

  gdstp = gdstbc;		/* group control table */
  nctabp = sd ? nttab2 : nttab1;	/* note transition table */
  ncptr = gtctab;		/* group to color table */

  lintab = (ac_code == N_SHARP) ? lintab1 : lintab2;	/* line table */

/* 
*/

  for (i = 0; i < 12; i++)
    {				/* scan the group control table ... */

      ncolor = mask1 & *ncptr++;	/* get note color mask */
      gdprv = (struct gdsel *) gdstp;	/* setup previous pointer */
      gdsep = *gdstp++;		/* setup current pointer */

      if (gdsep)
	{			/* ... process each active group ... */

	  do
	    {			/* ... for each note played by the group ... */

	      if (ndisp == 2)
		{		/* ... if display is up */

		  /* setup slice and pixel pointers */

		  slptr = saddr + 64L + ((long) lintab[gdsep->note] << 7);

		  pxptr = &pxtbl[gdsep->code][0];

		  /* update the slice */

		  *slptr = (*slptr & mask2) | (*pxptr++ & ncolor);
		  slptr += lc;

		  *slptr = (*slptr & mask2) | (*pxptr++ & ncolor);
		  slptr += lc;

		  *slptr = (*slptr & mask2) | (*pxptr++ & ncolor);

		}

	      gdnxt = gdsep->next;	/* set 'next element' pointer */

	      /* update and check element code */

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
  for (; i < NGDSEL; i++)
    {

      gdprv = (struct gdsel *) gdstp;	/* setup previous pointer */
      gdsep = *gdstp++;		/* setup current pointer */

      if (gdsep)
	{			/* ... process each active event priority ... */

	  do
	    {			/* ... for each event of this priority  ... */

	      if (ndisp == 2)
		{		/* ... if display is up */

		  /* setup slice and pixel pointers */

		  slptr = saddr + 27200L;
		  pxptr = &epxtbl[gdsep->code][0];

		  /* get event color */

		  ncolor = mask1 & gdsep->note;

		  /* update the slice */

		  if (*pxptr++)	/* 212 */
		    *slptr = (*slptr & mask2) | ncolor;

		  slptr += lc;

		  if (*pxptr++)	/* 213 */
		    *slptr = (*slptr & mask2) | ncolor;

		  slptr += lc;

		  if (*pxptr++)	/* 214 */
		    *slptr = (*slptr & mask2) | ncolor;

		  slptr += lc;

		  if (*pxptr++)	/* 215 */
		    *slptr = (*slptr & mask2) | ncolor;

		  slptr += lc;

		  if (*pxptr++)	/* 216 */
		    *slptr = (*slptr & mask2) | ncolor;

		  slptr += lc;

		  if (*pxptr++)	/* 217 */
		    *slptr = (*slptr & mask2) | ncolor;

		  slptr += lc;
/* 
*/
		  if (*pxptr++)	/* 218 */
		    *slptr = (*slptr & mask2) | ncolor;

		  slptr += lc;

		  if (*pxptr++)	/* 219 */
		    *slptr = (*slptr & mask2) | ncolor;

		  slptr += lc;

		  if (*pxptr++)	/* 220 */
		    *slptr = (*slptr & mask2) | ncolor;

		  slptr += lc;

		  if (*pxptr++)	/* 221 */
		    *slptr = (*slptr & mask2) | ncolor;

		  slptr += lc;

		  if (*pxptr++)	/* 222 */
		    *slptr = (*slptr & mask2) | ncolor;

		  slptr += lc;

		  if (*pxptr)	/* 223 */
		    *slptr = (*slptr & mask2) | ncolor;
		}

	      gdnxt = gdsep->next;	/* set next pointer */
	      gdprv->next = gdnxt;	/* delete the element */
	      gdsep->next = gdfsep;
	      gdfsep = gdsep;
	      gdsep = gdnxt;	/* set pointer for next pass */

	    }
	  while (gdsep);
	}
    }

  DB_EXIT ("ucslice");
}
