/*
   =============================================================================
	chksec.c -- section operation support functions
	Version 18 -- 1988-07-16 -- D.N. Lynx Crowe
   =============================================================================
*/

#undef	DEBUGGER		/* define to enable debug trace */

#include "stddefs.h"
#include "debug.h"
#include "score.h"
#include "scfns.h"
#include "secops.h"
#include "secdefs.h"

extern short ancmsw;
extern short grptran;

extern short grpmode[];
extern short grpstat[];
extern short grptmap[];

extern short varmode[][16];

short cmslens[N_ETYPES] = {	/* score event sizes for copy / merge */

  0,				/*  0 - EV_NULL         not copied or merged */
  0,				/*  1 - EV_SCORE        not copied or merged */
  0,				/*  2 - EV_SBGN         not copied or merged */
  0,				/*  3 - EV_SEND         not copied or merged */
  6,				/*  4 - EV_INST */
  5,				/*  5 - EV_NBEG */
  5,				/*  6 - EV_NEND */
  5,				/*  7 - EV_STOP */
  5,				/*  8 - EV_INTP */
  6,				/*  9 - EV_TMPO */
  6,				/* 10 - EV_TUNE */
  6,				/* 11 - EV_GRP */
  6,				/* 12 - EV_LOCN */
  6,				/* 13 - EV_DYN */
  6,				/* 14 - EV_ANVL */
  6,				/* 15 - EV_ANRS */
  6,				/* 16 - EV_ASGN */
  6,				/* 17 - EV_TRNS */
  5,				/* 18 - EV_REPT */
  5,				/* 19 - EV_PNCH */
  5,				/* 20 - EV_PRES */
  0,				/* 21 - EV_FINI         not copied or merged */
  5,				/* 22 - EV_CPRS */
  5,				/* 23 - EV_BAR */
  5				/* 24 - EV_NEXT */
};

/* 
*/

char cmgtags[N_ETYPES] = {	/* score event group sensitivity tags */

  FALSE,			/*  0 - EV_NULL         not copied or merged */
  FALSE,			/*  1 - EV_SCORE        not copied or merged */
  FALSE,			/*  2 - EV_SBGN         not copied or merged */
  FALSE,			/*  3 - EV_SEND         not copied or merged */
  TRUE,				/*  4 - EV_INST */
  TRUE,				/*  5 - EV_NBEG */
  TRUE,				/*  6 - EV_NEND */
  FALSE,			/*  7 - EV_STOP */
  FALSE,			/*  8 - EV_INTP */
  FALSE,			/*  9 - EV_TMPO */
  FALSE,			/* 10 - EV_TUNE */
  TRUE,				/* 11 - EV_GRP */
  TRUE,				/* 12 - EV_LOCN */
  TRUE,				/* 13 - EV_DYN */
  TRUE,				/* 14 - EV_ANVL */
  TRUE,				/* 15 - EV_ANRS */
  FALSE,			/* 16 - EV_ASGN */
  TRUE,				/* 17 - EV_TRNS */
  FALSE,			/* 18 - EV_REPT */
  FALSE,			/* 19 - EV_PNCH */
  FALSE,			/* 20 - EV_PRES */
  FALSE,			/* 21 - EV_FINI         not copied or merged */
  TRUE,				/* 22 - EV_CPRS */
  FALSE,			/* 23 - EV_BAR */
  FALSE				/* 24 - EV_NEXT */
};

/* 
*/

char cmgtype[N_ETYPES] = {	/* score event group types for copy / merge */

  0,				/*  0 - EV_NULL         not copied or merged */
  0,				/*  1 - EV_SCORE        not copied or merged */
  0,				/*  2 - EV_SBGN         not copied or merged */
  0,				/*  3 - EV_SEND         not copied or merged */
  0,				/*  4 - EV_INST */
  1,				/*  5 - EV_NBEG */
  1,				/*  6 - EV_NEND */
  0,				/*  7 - EV_STOP */
  0,				/*  8 - EV_INTP */
  0,				/*  9 - EV_TMPO */
  0,				/* 10 - EV_TUNE */
  0,				/* 11 - EV_GRP */
  0,				/* 12 - EV_LOCN */
  0,				/* 13 - EV_DYN */
  0,				/* 14 - EV_ANVL */
  0,				/* 15 - EV_ANRS */
  0,				/* 16 - EV_ASGN */
  0,				/* 17 - EV_TRNS */
  0,				/* 18 - EV_REPT */
  0,				/* 19 - EV_PNCH */
  0,				/* 20 - EV_PRES */
  0,				/* 21 - EV_FINI         not copied or merged */
  1,				/* 22 - EV_CPRS */
  0,				/* 23 - EV_BAR */
  0				/* 24 - EV_NEXT */
};

/* 
*/

short ehdlist[N_ETYPES] = {	/* hplist update type table */

  -1,				/*  0 - EV_NULL */
  -1,				/*  1 - EV_SCORE */
  EH_SBGN,			/*  2 - EV_SBGN */
  EH_SEND,			/*  3 - EV_SEND */
  EH_INST,			/*  4 - EV_INST */
  -1,				/*  5 - EV_NBEG */
  -1,				/*  6 - EV_NEND */
  -1,				/*  7 - EV_STOP */
  EH_INTP,			/*  8 - EV_INTP */
  EH_TMPO,			/*  9 - EV_TMPO */
  EH_TUNE,			/* 10 - EV_TUNE */
  EH_GRP,			/* 11 - EV_GRP */
  EH_LOCN,			/* 12 - EV_LOCN */
  EH_DYN,			/* 13 - EV_DYN */
  -1,				/* 14 - EV_ANVL */
  EH_ANRS,			/* 15 - EV_ANRS */
  EH_ASGN,			/* 16 - EV_ASGN */
  EH_TRNS,			/* 17 - EV_TRNS */
  -1,				/* 18 - EV_REPT */
  -1,				/* 19 - EV_PNCH */
  -1,				/* 20 - EV_PRES */
  -1,				/* 21 - EV_FINI */
  -1,				/* 22 - EV_CPRS */
  -1,				/* 23 - EV_BAR */
  -1				/* 24 - EV_NEXT */
};

/* 
*/

/*
   =============================================================================
	oktocm() -- see if an event is OK to copy or merge

	returns:

		TRUE	OK to copy or merge
		FALSE	don't copy or merge
   =============================================================================
*/

short
oktocm (ep)
     register struct s_entry *ep;
{
  register short et, grp, mode;

  et = 0x007F & ep->e_type;	/* get event type */

  if (0 EQ cmslens[et])		/* is this type not copied ? */
    return (FALSE);		/* return FALSE if so */

  if (NOT cmgtags[et])		/* is it group sensitive ? */
    return (TRUE);		/* return TRUE if not */

  /* get group number */

  grp = 0x000F & (cmgtype[et] ? ep->e_data2 : ep->e_data1);

  if (NOT grpstat[grp])		/* group enabled ? */
    return (FALSE);		/* can't do it if not */

  if ((et EQ EV_ANRS) OR (et EQ EV_ANVL))	/* analog */
    mode = ancmsw ?
      varmode[0x0007 & (ep->e_data1 >> 4)][grp] :
      (grptmap[grp] EQ - 1 ? 0 : 2);
  else				/* other */
    mode = grptmap[grp] EQ - 1 ? 0 : 2;

  /* return TRUE if in record mode */

  return ((mode EQ 2) ? TRUE : FALSE);
}

/* 
*/

/*
   =============================================================================
	oktode() -- see if an non-note event is OK to delete

	returns:

		TRUE	OK to delete
		FALSE	don't delete
   =============================================================================
*/

short
oktode (ep)
     register struct s_entry *ep;
{
  register short et, grp, mode;

  et = 0x007F & ep->e_type;	/* get event type */

  if ((et EQ EV_NBEG) OR (et EQ EV_NEND))	/* don't delete notes */
    return (FALSE);

  if (NOT cmgtags[et])		/* is event group sensitive ? */
    return (TRUE);		/* return TRUE if not */

  /* get group number */

  grp = 0x000F & (cmgtype[et] ? ep->e_data2 : ep->e_data1);

  if (NOT grpstat[grp])		/* enabled ? */
    return (FALSE);		/* can't do it if not */

  if ((et EQ EV_ANRS) OR (et EQ EV_ANVL))	/* analog */
    mode = ancmsw ? varmode[7 & (ep->e_data1 >> 4)][grp] : grpmode[grp];
  else				/* other */
    mode = grpmode[grp];

  /* return TRUE if  in record mode */

  return ((mode EQ 2) ? TRUE : FALSE);
}

/* 
*/

/*
   =============================================================================
	oktodg() -- see if a note event is OK to delete

	returns:

		TRUE	OK to delete
		FALSE	don't delete
   =============================================================================
*/

short
oktodg (ep)
     register struct s_entry *ep;
{
  register short et, grp;

  et = 0x007F & ep->e_type;	/* get event type */

  if ((et NE EV_NBEG) AND	/* is it a note begin ... */
      (et NE EV_NEND))		/* ... or a note end ? */
    return (FALSE);		/* return FALSE if not */

  grp = ep->e_data2;		/* get group number */

  if ((grpmode[grp] EQ 2) AND	/* is group in record mode ... */
      grpstat[grp])		/* ... and enabled ? */
    return (TRUE);		/* return TRUE if so */
  else
    return (FALSE);		/* return FALSE if not */
}

/* 
*/

/*
   =============================================================================
	chksec() -- check for a valid section

	returns:

		0	section OK  (both ends found in proper order)
		1	can't find the begin event
		2	can't find the end event
		3	section end occurs before section begin
   =============================================================================
*/

short
chksec (ns)
     register short ns;
{
  register struct s_entry *ep;

  DB_ENTR ("chksec");

  /* find section beginning in seclist */

  if (E_NULL EQ (p_sbgn = seclist[curscor][ns]))
    {

      secopok = FALSE;
      p_send = E_NULL;
      DB_EXIT ("chksec - 1");
      return (1);
    }

  t_sbgn = p_sbgn->e_time;	/* save section start time */

/* 
*/
  /* find section end by scanning hplist */

  ep = hplist[curscor][EH_SEND];

  while (ep)
    {

      if (((ep->e_type & 0x007F) EQ EV_SEND) AND (ep->e_data1 EQ ns))
	break;

      ep = ep->e_up;
    }

  if (E_NULL EQ (p_send = ep))
    {				/* error if not found */

      secopok = FALSE;
      DB_EXIT ("chksec - 2");
      return (2);
    }

  /* get section end and check that it occurs after section begin */

  if (t_sbgn GE (t_send = ep->e_time))
    {

      secopok = FALSE;
      DB_EXIT ("chksec - 3");
      return (3);
    }

  t_sect = 1 + t_send - t_sbgn;	/* calculate section time */

  DB_EXIT ("chksec - 0");
  return (0);
}

/* 
*/

/*
   =============================================================================
	sizesec() -- find the size of a section

	returns:	number of storage units required

	Assumes that the section passed chksec() and that p_sbgn and p_send
	are valid.  Fails badly if not.
   =============================================================================
*/

long
sizesec ()
{
  register long ec;
  register short et;
  register struct s_entry *sbp, *sep;

  DB_ENTR ("sizesec");

  ec = 0L;			/* start with zero length */
  sbp = p_sbgn->e_fwd;		/* point to first event to copy */
  sep = p_send;			/* point at section end */

  while (sbp NE sep)
    {				/* scan up to section end ... */

      et = 0x007F & sbp->e_type;	/* get event type */

      if (oktocm (sbp))		/* if it's one we want ... */
	ec += cmslens[et];	/* ... add its length */

      sbp = sbp->e_fwd;		/* point at next event */
    }

#ifdef	DEBUGGER
  if (ec)
    DB_CMNT ("sizesec - non-null copy");
  else
    DB_CMNT ("sizesec - null copy");
#endif

  DB_EXIT ("sizesec");
  return (ec);			/* return number of longs required */
}

/* 
*/

/*
   =============================================================================
	edelta() -- adjust times in events above 'btime' by 'delta'

	ep	pointer to event to start adjusting at
	btime	base time
	delta	adjustment factor added to event times
   =============================================================================
*/

edelta (ep, btime, delta)
     register struct s_entry *ep;
     register long btime, delta;
{
  DB_ENTR ("edelta");

  while ((ep->e_type & 0x007F) NE EV_FINI)
    {				/* for each event */

      if (ep->e_time > btime)	/* in range ? */
	ep->e_time += delta;	/* add delta */

      ep = ep->e_fwd;		/* point at next event */
    }

  DB_EXIT ("edelta");
}

/* 
*/

/*
   =============================================================================
	madjsec() -- make a time-adjusted section copy

	Returns a pointer to the copy, or E_NULL if copy is empty.

	sbp	section begin pointer to section to copy
	btime	base time of copy

	Copy is bi-linked on e_fwd/e_bak, E_NULL terminated.

	Assumes that the section is complete and well ordered, and that
	we have enough free events to make the copy. Fails badly if not.
   =============================================================================
*/

struct s_entry *
madjsec (sbp, btime)
     register struct s_entry *sbp;
     register long btime;
{
  short ns, nv;
  register short grp, es, et;
  register struct s_entry *ep, *np;
  struct s_entry *cp;
  register long sat;

  DB_ENTR ("madjsec");

  np = ep = cp = E_NULL;	/* initialize np, ep and cp */
  ns = sbp->e_data1;		/* get section number */
  sat = sbp->e_time - btime;	/* get section adjust time */
  sbp = sbp->e_fwd;		/* advance to first event to copy */

/* 
*/

  /* copy up to section end */

  while (TRUE)
    {

      et = sbp->e_type & 0x007F;	/* get event type */

      /* we're done when we see the section end */

      if ((EV_SEND EQ et) AND (sbp->e_data1 EQ ns))
	break;

      if (oktocm (sbp))
	{			/* if event is copyable */

	  es = sbp->e_size;	/* get event size */
	  np = e_alc (es);	/* allocate event */
	  memcpyw (np, sbp, es << 1);	/* make copy */

	  if (cmgtags[et])
	    {			/* group sensitive ? */

	      grp = 0x000F & (cmgtype[et] ? np->e_data2 : np->e_data1);

	      if ((et EQ EV_NBEG) OR (et EQ EV_NEND))
		{

		  /* regroup */

		  np->e_data2 = (np->e_data2 & 0x00F0) | grptmap[grp];

		  /* transpose */

		  nv = np->e_data1 + grptran;

		  if (nv > 127)
		    nv = 127;
		  else if (nv < 0)
		    nv = 0;

		  np->e_data1 = nv;

		}
	      else if ((et EQ EV_ANRS) OR (et EQ EV_ANVL))
		{

		  /* regroup */

		  np->e_data1 = (np->e_data1 & 0x000F) | (grptmap[grp] << 4);

		}
	      else
		{

		  /* regroup */

		  if (cmgtype[et])
		    np->e_data2 = (np->e_data2 & 0x00F0) | grptmap[grp];
		  else
		    np->e_data1 = (np->e_data1 & 0x00F0) | grptmap[grp];
		}
	    }

/* 
*/
	  if (ep)
	    {			/* if copy started */

	      ep->e_fwd = np;	/* link new event to copy */

	    }
	  else
	    {			/* first event in chain */

	      DB_CMNT ("madjsec - making copy");
	      cp = np;		/* start the copy chain */
	    }

	  np->e_bak = ep;	/* link new event to previous one */
	  np->e_fwd = E_NULL;	/* terminate the copy chain */
	  np->e_time -= sat;	/* adjust the copied event time */
	  ep = np;		/* point ep at new event */
	}

      sbp = sbp->e_fwd;		/* point at next event to copy */
    }

  p_cbgn = cp;			/* save address of start of copy */
  p_cend = np;			/* save address of end of copy */

  if (cp)
    {

      DB_CMNT ("madjsec - copy made");
      t_cbgn = cp->e_time;	/* save start time of copy */
      t_cend = np->e_time;	/* save end time of copy */
    }

  DB_EXIT ("madjsec");
  return (cp);			/* return start address of copy */
}

/* 
*/

/*
   =============================================================================
	ehfix() -- put event headers in a copied section into hplist

	cbp	pointer to first event in copy
	cep	pointer to last event in copy
   =============================================================================
*/

ehfix (cbp, cep)
     register struct s_entry *cbp, *cep;
{
  register short et;

  for (;;)
    {				/* for each event from cbp to cep ... */

      et = 0x007F & cbp->e_type;	/* get event type */

      /* does event belong in hplist ? */

      if (-1 NE ehdlist[et])
	eh_ins (cbp, ehdlist[et]);	/* if so, update hplist */

      if (cbp EQ cep)		/* see if we're done */
	return;			/* return if so */

      cbp = cbp->e_fwd;		/* advance to the next event */
    }
}
