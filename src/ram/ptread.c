/*
   =============================================================================
	ptread.c -- librarian - read patch functions
	Version 5 -- 1988-11-18 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGRE		0
#define	DEBUGSP		0

#include "graphdef.h"
#include "stddefs.h"
#include "stdio.h"
#include "patch.h"
#include "vsdd.h"

#include "midas.h"
#include "libdsp.h"

#if	(DEBUGRE|DEBUGSP)
extern short debugsw;
#endif

#if	DEBUGRE
extern short debugre;
#endif

#if	DEBUGSP
short debugsp = 1;
#endif

extern short dpecpos;
extern short dpepred;
extern short dpesucc;
extern short errno;
extern short ptecpos;
extern short ptepred;
extern short ptesucc;

extern struct patch ptebuf;

/* 
*/

/*
   =============================================================================
	stashp() -- stash an incoming patch
   =============================================================================
*/

short
stashp ()
{
  register short c;
  register unsigned short np, stim;

  c = findpte ();

  if (c EQ 0)
    {				/* old patch -- just update it */

      memcpyw (&patches[ptecpos].defnum, &ptebuf.defnum, 6);
      patches[ptecpos].paspec |= PE_TBIT;	/* define it */

#if	DEBUGSP
      if (debugsw AND debugsp)
	{

	  printf ("stashp():  UPDATED\n");
	  SnapPTV ("stashp");
	}
#endif

      return (SUCCESS);
    }

  /* allocate a patch entry and fill it in */

  if (0 EQ (ptecpos = pt_alc ()))
    {

#if	DEBUGSP
      if (debugsw AND debugsp)
	printf ("stashp():  patch table FULL\n");
#endif
      return (FAILURE);		/* no patch entries left */
    }

  memcpyw (&patches[ptecpos].defnum, &ptebuf.defnum, 6);
  patches[ptecpos].paspec |= PE_TBIT;	/* define it */
  stim = TRG_MASK & ptebuf.stmnum;

  if (c EQ 1)
    {				/* new patch -- no STM entry yet */

      ptepred = 0;
      stmptr[stim] = ptecpos;
    }
/* 
*/
  /* put patch in STM chain */

  if (ptepred)
    {				/* predecessor exits */

      ptesucc = patches[ptepred].nextstm;

      patches[ptecpos].nextstm = ptesucc;
      patches[ptecpos].prevstm = ptepred;

      patches[ptepred].nextstm = ptecpos;

      if (ptesucc)
	patches[ptesucc].prevstm = ptecpos;

    }
  else
    {				/* no predecessor */

      patches[ptecpos].prevstm = 0;

      if (c EQ - 1)
	{

	  ptesucc = stmptr[stim];

	  patches[ptecpos].nextstm = ptesucc;

	  patches[ptesucc].prevstm = ptecpos;

	  stmptr[stim] = ptecpos;

	}
      else
	{

	  patches[ptecpos].nextstm = 0;
	}
    }
/* 
*/
  /* update DEF table */

  if (0 EQ (c = finddpe ()))
    {

#if	DEBUGSP
      if (debugsw AND debugsp)
	{

	  printf ("stashp():  defent already exists -- dpecpos = %d\n",
		  dpecpos);

	  printf ("stashp():  ENTERED\n");
	  SnapPTV ("stashp");
	}
#endif
      return (SUCCESS);		/* defent already exists */
    }

  if (0 EQ (dpecpos = dt_alc ()))
    {

#if	DEBUGSP
      if (debugsw AND debugsp)
	printf ("stashp():  defent table FULL\n");
#endif
      return (FAILURE);		/* no defents left */
    }

  defents[dpecpos].nextdef = 0;
  defents[dpecpos].stm = ptebuf.stmnum;
  defents[dpecpos].adspec = ptebuf.paspec;
  defents[dpecpos].adsuba = ptebuf.pasuba;
  defents[dpecpos].addat1 = ptebuf.padat1;

  np = TRG_MASK & ptebuf.defnum;

  if (c EQ 1)
    {

      dpepred = 0;
      defptr[np] = dpecpos;
    }
/* 
*/
  if (dpepred)
    {

      dpesucc = defents[dpepred].nextdef;
      defents[dpecpos].nextdef = dpesucc;
      defents[dpepred].nextdef = dpecpos;

    }
  else
    {

      if (c EQ - 1)
	{

	  dpesucc = defptr[np];
	  defents[dpecpos].nextdef = dpesucc;
	  defptr[np] = dpecpos;

	}
      else
	{

	  defents[dpecpos].nextdef = 0;
	}
    }

#if	DEBUGSP
  if (debugsw AND debugsp)
    {

      printf ("stashp():  new defent created -- dpecpos = %d\n", dpecpos);

      printf ("stashp():  ENTERED\n");
      SnapPTV ("stashp");
    }
#endif
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ptioerr() -- put up an I/O error message
   =============================================================================
*/

ptioerr ()
{
  char erms[40];

  clrlsel ();

  sprintf (erms, "  errno = %d", errno);

  ldermsg ("Couldn't read", " the patch table", erms, LD_EMCF, LD_EMCB);
}

/*
   =============================================================================
	nopatch() -- give an error message after running out of space
   =============================================================================
*/

nopatch ()
{
  clrlsel ();

  ldermsg ("Couldn't read", "  the patch table.", "  Ran out of space",
	   LD_EMCF, LD_EMCB);
}

/* 
*/

/*
   =============================================================================
	ptread() -- read a patch
   =============================================================================
*/

short
ptread (fp)
     register FILE *fp;
{
  register short go;
  char cb;

  go = TRUE;

  ldwmsg (" Busy -- please stand by", (char *) NULL, "  Reading patches",
	  LCFBX10, LCBBX10);

  for (;;)
    {

      voidpb ();		/* clear the patch buffer */

      ptecpos = ptepred = ptesucc = 0;

#if	DEBUGRE
      if (debugsw AND debugre)
	printf ("ptread():  reading\n");
#endif
      if (rd_ec (fp, &cb, 1L))
	{			/* get control byte */

	  ptioerr ();
	  return (FAILURE);
	}

      if (0 EQ cb)		/* if it's 0, we're done */
	break;

      ptebuf.paspec = PE_TBIT | (PE_SPEC & cb);

      if (rd_ec (fp, &ptebuf.defnum, 4L))	/* DEF and STM */
	return (FAILURE);
/* 
*/
      switch (cb)
	{

	case PA_KEY:
	case PA_TRG:

	  if (rd_ec (fp, &ptebuf.pasuba, 2L))
	    return (FAILURE);

	  if (rd_ec (fp, (char *) &ptebuf.padat2 + 1, 1L))
	    return (FAILURE);

	  break;

	case PA_PLS:
	case PA_SCTL:

	  if (rd_ec (fp, (char *) &ptebuf.pasuba + 1, 1L))
	    return (FAILURE);

	  if (rd_ec (fp, (char *) &ptebuf.padat2 + 1, 1L))
	    return (FAILURE);

	  break;

/* 
*/
	case PA_LED:

	  if (rd_ec (fp, (char *) &ptebuf.pasuba + 1, 1L))
	    return (FAILURE);

	  if (rd_ec (fp, &ptebuf.padat1, 1L))
	    return (FAILURE);

	  break;

	case PA_SLIN:

	  if (rd_ec (fp, (char *) &ptebuf.pasuba + 1, 1L))
	    return (FAILURE);

	  if (rd_ec (fp, &ptebuf.padat1, 2L))
	    return (FAILURE);

	  break;

	case PA_TUNE:

	  if (rd_ec (fp, (char *) &ptebuf.padat1 + 1, 1L))
	    return (FAILURE);

	  break;
/* 
*/
	case PA_RSET:
	case PA_RADD:

	  if (rd_ec (fp, (char *) &ptebuf.pasuba + 1, 1L))
	    return (FAILURE);

	  if (rd_ec (fp, (char *) &ptebuf.padat1 + 1, 1L))
	    return (FAILURE);

	  if (rd_ec (fp, (char *) &ptebuf.padat2 + 1, 1L))
	    return (FAILURE);

	  break;

	case PA_INST:
	case PA_WAVA:
	case PA_WAVB:
	case PA_CNFG:

	  if (rd_ec (fp, &ptebuf.pasuba, 1L))
	    return (FAILURE);

	  if (rd_ec (fp, (char *) &ptebuf.padat1 + 1, 1L))
	    return (FAILURE);

	  break;

	case PA_OSC:
	case PA_INDX:
	case PA_FREQ:

	  if (rd_ec (fp, &ptebuf.pasuba, 2L))
	    return (FAILURE);

	  if (rd_ec (fp, (char *) &ptebuf.padat1 + 1, 1L))
	    return (FAILURE);

	  if (rd_ec (fp, &ptebuf.padat2, 2L))
	    return (FAILURE);

	  break;
/* 
*/
	case PA_LEVL:
	case PA_FILT:
	case PA_FILQ:
	case PA_LOCN:
	case PA_DYNM:

	  if (rd_ec (fp, &ptebuf.pasuba, 1L))
	    return (FAILURE);

	  if (rd_ec (fp, (char *) &ptebuf.padat1 + 1, 1L))
	    return (FAILURE);

	  if (rd_ec (fp, &ptebuf.padat2, 2L))
	    return (FAILURE);

	  break;

	case PA_AUX:
	case PA_RATE:
	case PA_INTN:
	case PA_DPTH:

	  if (rd_ec (fp, (char *) &ptebuf.padat1 + 1, 1L))
	    return (FAILURE);

	  if (rd_ec (fp, &ptebuf.padat2, 2L))
	    return (FAILURE);

	  break;

	case PA_VOUT:

	  if (rd_ec (fp, (char *) &ptebuf.pasuba + 1, 1L))
	    return (FAILURE);

	  if (rd_ec (fp, (char *) &ptebuf.padat1 + 1, 1L))
	    return (FAILURE);

	  if (rd_ec (fp, &ptebuf.padat2, 2L))
	    return (FAILURE);

	  break;
/* 
*/
	default:

	  return (FAILURE);
	}

#if	DEBUGRE
      if (debugsw AND debugre)
	printf ("\n\n");
#endif

      if (stashp ())
	{			/* stash the patch */

	  nopatch (fp);
	  ptecpos = 0;
	  voidpb ();
	  return (FAILURE);
	}
    }

#if	DEBUGRE
  if (debugsw AND debugre)
    printf ("ptread():  terminator read -- end of patch file\n");
#endif

  ptecpos = 0;
  voidpb ();
  return (SUCCESS);
}
