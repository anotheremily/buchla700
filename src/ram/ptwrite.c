/*
   =============================================================================
	ptwrite.c -- librarian - write patch functions
	Version 7 -- 1988-11-18 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGWE		0
#define	DEBUGPW		0

#include "stddefs.h"
#include "stdio.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "patch.h"

#include "midas.h"
#include "libdsp.h"

#if	(DEBUGPW|DEBUGWE)
extern short debugsw;
#endif

#if	DEBUGPW
short debugpw = 1;
#endif

#if	DEBUGWE
extern short debugwe;
#endif

extern long chksum ();

/* 
*/

char ptsizes[] = {

  0,				/* NULL - end of list */
  4,				/* PA_KEY  */
  4,				/* PA_TRG  */
  3,				/* PA_PLS  */
  3,				/* PA_LED  */
  4,				/* PA_SLIN */
  3,				/* PA_SCTL */
  2,				/* PA_TUNE */
  4,				/* PA_RSET */
  4,				/* PA_RADD */
  3,				/* PA_INST */
  6,				/* PA_OSC  */
  3,				/* PA_WAVA */
  3,				/* PA_WAVB */
  3,				/* PA_CNFG */
  5,				/* PA_LEVL */
  6,				/* PA_INDX */
  6,				/* PA_FREQ */
  5,				/* PA_FILT */
  5,				/* PA_FILQ */
  5,				/* PA_LOCN */
  5,				/* PA_DYNM */
  4,				/* PA_AUX  */
  4,				/* PA_RATE */
  4,				/* PA_INTN */
  4,				/* PA_DPTH */
  5				/* PA_VOUT */
};

/* 
*/

/*
   =============================================================================
	ptsizer() -- return number of bytes necessary for storing active patches
   =============================================================================
*/

long
ptsizer ()
{
  register short pp;
  register long nb;

  nb = 0L;

  if (pp = find1st ())
    {

      while (pp)
	{

	  nb += (ptsizes[PE_SPEC & patches[pp].paspec] + 4);
	  pp = findnxt (pp);
	}
    }

  if (nb)
    ++nb;			/* ... and one for the terminator */

#if	DEBUGPW
  if (debugsw AND debugpw)
    printf ("ptsizer():  %ld bytes required\n", nb);
#endif

  return (nb);
}

/* 
*/

/*
   =============================================================================
	ptwrite() -- store a patch table
   =============================================================================
*/

short
ptwrite (fp)
     register FILE *fp;
{
  register short pp;
  char cb, zero;

#if	DEBUGPW
  if (debugsw AND debugpw)
    printf ("ptwrite($%08lX):  entered\n", fp);
#endif

  zero = '\0';

  ldwmsg ("Busy -- Please stand by", (char *) 0L, "  writing patches",
	  LCFBX10, LCBBX10);

  if (pp = find1st ())
    {

      while (pp)
	{

#if	DEBUGWE
	  if (debugsw AND debugwe)
	    {

	      printf
		("ptwrite():  %3d  %04.4X %04.4X  %04.4X %04.4X  %04.4X %04.4X\n    ",
		 pp, patches[pp].defnum, patches[pp].stmnum,
		 patches[pp].paspec, patches[pp].pasuba, patches[pp].padat1,
		 patches[pp].padat2);
	    }
#endif

	  switch (cb = (PE_SPEC & patches[pp].paspec))
	    {

	    case PA_KEY:
	    case PA_TRG:

	      if (wr_ec (fp, &cb, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].defnum, 4L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].pasuba, 2L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].padat2 + 1, 1L))
		return (FAILURE);

	      break;
/* 
*/
	    case PA_PLS:
	    case PA_SCTL:

	      if (wr_ec (fp, &cb, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].defnum, 4L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].pasuba + 1, 1L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].padat2 + 1, 1L))
		return (FAILURE);

	      break;

	    case PA_LED:

	      if (wr_ec (fp, &cb, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].defnum, 4L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].pasuba + 1, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].padat1, 1L))
		return (FAILURE);

	      break;

	    case PA_SLIN:

	      if (wr_ec (fp, &cb, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].defnum, 4L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].pasuba + 1, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].padat1, 2L))
		return (FAILURE);

	      break;

	    case PA_TUNE:

	      if (wr_ec (fp, &cb, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].defnum, 4L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].padat1 + 1, 1L))
		return (FAILURE);

	      break;
/* 
*/
	    case PA_RSET:
	    case PA_RADD:

	      if (wr_ec (fp, &cb, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].defnum, 4L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].pasuba + 1, 1L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].padat1 + 1, 1L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].padat2 + 1, 1L))
		return (FAILURE);

	      break;

	    case PA_INST:
	    case PA_WAVA:
	    case PA_WAVB:
	    case PA_CNFG:

	      if (wr_ec (fp, &cb, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].defnum, 4L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].pasuba, 1L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].padat1 + 1, 1L))
		return (FAILURE);

	      break;

	    case PA_OSC:
	    case PA_INDX:
	    case PA_FREQ:

	      if (wr_ec (fp, &cb, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].defnum, 4L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].pasuba, 2L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].padat1 + 1, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].padat2, 2L))
		return (FAILURE);

	      break;
/* 
*/
	    case PA_LEVL:
	    case PA_FILT:
	    case PA_FILQ:
	    case PA_LOCN:
	    case PA_DYNM:

	      if (wr_ec (fp, &cb, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].defnum, 4L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].pasuba, 1L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].padat1 + 1, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].padat2, 2L))
		return (FAILURE);

	      break;

	    case PA_AUX:
	    case PA_RATE:
	    case PA_INTN:
	    case PA_DPTH:

	      if (wr_ec (fp, &cb, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].defnum, 4L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].padat1 + 1, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].padat2, 2L))
		return (FAILURE);

	      break;

	    case PA_VOUT:

	      if (wr_ec (fp, &cb, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].defnum, 4L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].pasuba + 1, 1L))
		return (FAILURE);

	      if (wr_ec (fp, (char *) &patches[pp].padat1 + 1, 1L))
		return (FAILURE);

	      if (wr_ec (fp, &patches[pp].padat2, 2L))
		return (FAILURE);

	      break;
/* 
*/
	    default:

	      return (FAILURE);
	    }

#if	DEBUGWE
	  if (debugsw AND debugwe)
	    printf ("\n\n");
#endif

	  pp = findnxt (pp);	/* find the next patch */
	}

      if (wr_ec (fp, &zero, 1L))	/* write the terminator */
	return (FAILURE);

    }
  else
    {

      return (FAILURE);		/* no patches to write */
    }

#if	DEBUGPW
  if (debugsw AND debugpw)
    printf ("ptwrite():  SUCCESS\n");
#endif

  return (SUCCESS);
}
