/*
   =============================================================================
	scread.c -- librarian - read score functions
	Version 15 -- 1988-08-02 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "graphdef.h"
#include "stddefs.h"
#include "stdio.h"
#include "score.h"
#include "scfns.h"
#include "vsdd.h"

#include "midas.h"
#include "libdsp.h"

extern short scskip ();

extern long schksum;		/* score checksum */

extern short errno;		/* system error number */
extern short lderrsw;		/* librarian error message switch */
extern short lrasw;		/* "Content" switch */

extern short ldmap[];		/* librarian gather-read map */

extern struct s_entry *libsp;	/* librarian score insert pointer */

extern struct s_time stimes[N_SCORES][N_SECTS];

/* 
*/

/*
   =============================================================================
	scioerr() -- clear a partially built score after an I/O error
   =============================================================================
*/

scioerr (sn, ep)
     short sn;
     struct s_entry *ep;
{
  char scid[40];
  char erms[40];

  if (E_NULL != ep)
    e_del (ep);

  sc_clr (sn);
  clrlsel ();

  sprintf (scid, "  score %d", sn + 1);
  sprintf (erms, "  errno = %d", errno);

  ldermsg ("Couldn't read", scid, erms, LD_EMCF, LD_EMCB);
}

/*
   =============================================================================
	noevent() -- clear a partially built score after running out of space
   =============================================================================
*/

noevent (sn)
{
  char scid[24];

  sc_clr (sn);
  clrlsel ();

  sprintf (scid, "  score %d", sn + 1);

  ldermsg ("Couldn't read", scid, "  Ran out of space", LD_EMCF, LD_EMCB);
}

/* 
*/

/*
   =============================================================================
	scread() -- read a score
   =============================================================================
*/

short
scread (ns, fp)
     short ns;
     register FILE *fp;
{
  register struct s_entry *ep;
  register short ehdr, go, sn;
  long nbr, nev;
  char etype;
  char scid[40];
  char erms[40];

  sn = ldmap[ns];		/* map the score */

  if (-1 == sn)
    {				/* skip score if map = -1 */

      if (rd_ec (fp, &nbr, 4L))
	{			/* get length */

	  skperr (ns);
	  return (FAILURE);
	}

      if (nbr == - 1L)		/* done if it's null */
	return (SUCCESS);

      if (skp_ec (fp, 16L))
	{			/* skip the score name */

	  skperr (ns);
	  return (FAILURE);
	}

      return (scskip (fp, ns));	/* skip the rest of it */
    }

/* 
*/
  go = TRUE;

  sprintf (scid, "  Reading score %2d", ns + 1);
  sprintf (erms, "       as score %2d", sn + 1);

  ldwmsg (" Busy -- please stand by", scid, erms, LCFBX10, LCBBX10);

  if (rd_ec (fp, &nbr, 4L))
    {				/* get number of longs required */

      scioerr (sn, 0L);
      return (FAILURE);
    }

  if (nbr == - 1L)
    {				/* see if it's a null score marker */

      return (SUCCESS);
    }

  if (lrasw)			/* clear target if in append mode */
    sc_clr (sn);

  if (nbr > (nev = evleft ()))
    {				/* see if we have enough space */

      sprintf (scid, "  score %d - no space", sn + 1);
      sprintf (erms, "  Need %ld, Have %ld", nbr, nev);

      ldermsg ("Couldn't read", scid, erms, LD_EMCF, LD_EMCB);

      return (FAILURE);
    }

  if (rd_ec (fp, &scname[sn], 16L))
    {				/* score name */

      scioerr (sn, 0L);
      return (FAILURE);
    }

  if (rd_ec (fp, &stimes[sn], (long) (N_SECTS * 12)))
    {				/* section times */

      scioerr (sn, 0L);
      return (FAILURE);
    }

  if (rd_ec (fp, &etype, 1L))
    {				/* read score header event */

      scioerr (sn, 0L);
      return (FAILURE);
    }

  if (etype != EV_SCORE)
    {				/* complain if it's not a score event */

      sprintf (scid, "  score %d", sn + 1);

      ldermsg ("Bad score --", "  1st event is wrong",
	       scid, LD_EMCF, LD_EMCB);

      return (FAILURE);
    }

  if (E_NULL == (ep = e_alc (E_SIZE1)))
    {				/* allocate header space */

      noevent (sn);
      return (FAILURE);
    }

  libsp = ep;
  scores[sn] = ep;

  ep->e_type = EV_SCORE;

  if (rd_ec (fp, &ep->e_data1, 1L))
    {

      scioerr (sn, ep);
      return (FAILURE);
    }

  ep->e_fwd = ep;
  ep->e_bak = ep;
/* 
*/
  do
    {

      if (rd_ec (fp, &etype, 1L))
	{			/* get event type */

	  scioerr (sn, 0L);
	  return (FAILURE);
	}

      switch (etype)
	{			/* process event entry */

	case EV_BAR:		/* bar marker */
	case EV_STOP:		/* stop */
	case EV_NEXT:		/* next */

	  if (E_NULL == (ep = e_alc (E_SIZE1)))
	    {

	      noevent (sn);
	      return (FAILURE);
	    }

	  ep->e_type = etype;

	  if (rd_ec (fp, &ep->e_time, 4L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  libsp = e_ins (ep, libsp);
	  break;
/* 
*/
	case EV_SBGN:		/* section begin */

	  ehdr = EH_SBGN;
	  goto doit1;

	case EV_SEND:		/* section end */

	  ehdr = EH_SEND;
	  goto doit1;

	case EV_TMPO:		/* tempo */

	  ehdr = EH_TMPO;
	  goto doit1;

	case EV_TUNE:		/* tuning */

	  ehdr = EH_TUNE;
	  goto doit1;

	case EV_ASGN:		/* I/O assign */

	  ehdr = EH_ASGN;
/* 
*/
	doit1:

	  if (E_NULL == (ep = e_alc (E_SIZE2)))
	    {

	      noevent (sn);
	      return (FAILURE);
	    }

	  ep->e_type = etype;

	  if (rd_ec (fp, &ep->e_time, 4L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data1, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  libsp = e_ins (ep, libsp);
	  eh_ins (ep, ehdr);

	  if (etype == EV_SBGN)
	    seclist[sn][ep->e_data1] = ep;

	  break;
/* 
*/
	case EV_SCORE:		/* score begin */
	case EV_REPT:		/* repeat */
	case EV_PNCH:		/* punch in/out */

	  if (E_NULL == (ep = e_alc (E_SIZE1)))
	    {

	      noevent (sn);
	      return (FAILURE);
	    }

	  ep->e_type = etype;

	  if (rd_ec (fp, &ep->e_time, 4L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data1, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  libsp = e_ins (ep, libsp);
	  break;
/* 
*/
	case EV_NBEG:		/* note begin */
	case EV_NEND:		/* note end */

	  if (E_NULL == (ep = e_alc (E_SIZE1)))
	    {

	      noevent (sn);
	      return (FAILURE);
	    }

	  ep->e_type = etype;

	  if (rd_ec (fp, &ep->e_time, 4L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data1, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data2, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_dn, 2L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  libsp = e_ins (ep, libsp);
	  break;
/* 
*/
	case EV_PRES:		/* polyphonic pressure */

	  if (E_NULL == (ep = e_alc (E_SIZE1)))
	    {

	      noevent (sn);
	      return (FAILURE);
	    }

	  ep->e_type = etype;

	  if (rd_ec (fp, &ep->e_time, 4L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data1, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data2, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  libsp = e_ins (ep, libsp);
	  break;
/* 
*/
	case EV_CPRS:		/* channel pressure */

	  if (E_NULL == (ep = e_alc (E_SIZE1)))
	    {

	      noevent (sn);
	      return (FAILURE);
	    }

	  ep->e_type = etype;

	  if (rd_ec (fp, &ep->e_time, 4L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data1, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data2, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  libsp = e_ins (ep, libsp);
	  break;

/* 
*/
	case EV_INST:		/* instrument change */

	  ehdr = EH_INST;
	  goto doit2;

	case EV_INTP:		/* interpolate */

	  ehdr = EH_INTP;
	  goto doit2;

	case EV_GRP:		/* group status */

	  ehdr = EH_GRP;
	  goto doit2;

	case EV_LOCN:		/* location */

	  ehdr = EH_LOCN;
	  goto doit2;

	case EV_DYN:		/* dynamics */

	  ehdr = EH_DYN;
	  goto doit2;

	case EV_ANRS:		/* analog resolution */

	  ehdr = EH_ANRS;
/* 
*/
	doit2:

	  if (E_NULL == (ep = e_alc (E_SIZE2)))
	    {

	      noevent (sn);
	      return (FAILURE);
	    }

	  ep->e_type = etype;

	  if (rd_ec (fp, &ep->e_time, 4L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data1, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data2, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  libsp = e_ins (ep, libsp);
	  eh_ins (ep, ehdr);
	  break;
/* 
*/
	case EV_TRNS:		/* transposition */

	  if (E_NULL == (ep = e_alc (E_SIZE3)))
	    {

	      noevent (sn);
	      return (FAILURE);
	    }

	  ep->e_type = etype;

	  if (rd_ec (fp, &ep->e_time, 4L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data1, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_lft, 2L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  libsp = e_ins (ep, libsp);
	  eh_ins (ep, EH_INTP);
	  break;
/* 
*/
	case EV_ANVL:		/* analog value */

	  if (E_NULL == (ep = e_alc (E_SIZE2)))
	    {

	      noevent (sn);
	      return (FAILURE);
	    }

	  ep->e_type = etype;

	  if (rd_ec (fp, &ep->e_time, 4L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data1, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  ep->e_dn = 0L;

	  if (rd_ec (fp, &ep->e_dn, 2L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  libsp = e_ins (ep, libsp);
	  break;
/* 
*/
	case EV_FINI:		/* score end */

	  if (E_NULL == (ep = e_alc (E_SIZE1)))
	    {

	      noevent (sn);
	      return (FAILURE);
	    }

	  ep->e_type = etype;

	  if (rd_ec (fp, &ep->e_time, 4L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  if (rd_ec (fp, &ep->e_data1, 1L))
	    {

	      scioerr (sn, ep);
	      return (FAILURE);
	    }

	  libsp = e_ins (ep, libsp);
	  go = FALSE;
	}

    }
  while (go);

  return (SUCCESS);
}
