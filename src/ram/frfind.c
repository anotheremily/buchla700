/*
   =============================================================================
	frfind.c -- the frame finder, and other pointer pushers
	Version 17 -- 1988-07-28 -- D.N. Lynx Crowe

	struct s_entry *
	ep_adj(sep, sdir, tval)
	struct s_entry *sep;
	int sdir;
	long tval;

		Returns a pointer to the event chain at the time 'tval'
		starting from 'sep' in the direction 'sdir'.
		The right end of the chain is returned when 'sdir' EQ 0,
		and the left end is returned when 'sdir' NE 0.

	struct s_entry *
	frfind(tval, sdir)
	long tval;
	int sdir;

		Returns a pointer to the event chain at the time 'tval'
		in the current score in the direction 'sdir', or E_NULL
		if the current score is empty.
		The right end of the chain is returned when 'sdir' EQ 0,
		and the left end is returned when 'sdir' NE 0.

	struct s_entry *
	findev(ep, te, et, d1, d2)
	struct s_entry *ep;
	long te;
	short et, d1, d2;

		Searches the event chain starting at 'ep' for an event at
		a time of 'te' with:  a type of 'et', e_data1 EQ 'd1',
		and e_data2 EQ 'd2'.  The values of 'd1' or 'd2' may be -1,
		in which case e_data1 or e_data2 will be assumed to match.
		Returns a pointer to the desired event if it is found, or
		E_NULL if no event in the chain matches the criteria given.

	struct s_entry *
	ehfind(et, te, d1, d2)
	short et;
	long te;
	short d1, d2;

		Searches the event header chain starting for an event at
		a time of 'te' with:  a type of 'et', e_data1 EQ 'd1',
		and e_data2 EQ 'd2'.  The values of 'd1' or 'd2' may be -1,
		in which case e_data1 or e_data2 will be assumed to match.
		Returns a pointer to the desired event if it is found, or
		E_NULL if no event in the chain matches the criteria given.
   =============================================================================
*/

#define	DEBUGIT		0
#define	CHECKP		0

#include "stddefs.h"
#include "score.h"
#include "scfns.h"

#if	DEBUGIT
extern short verbose, testing;
#endif

extern short insmode;

/* 
*/

/*
   =============================================================================
	ep_adj(sep, sdir, tval) -- Returns a pointer to the event chain
	at the time 'tval' starting from 'sep' in the direction 'sdir'.
   =============================================================================
*/

struct s_entry *
ep_adj (sep, sdir, tval)
     register struct s_entry *sep;
     int sdir;
     register long tval;
{
  register struct s_entry *tep;

#if	DEBUGIT
  if (verbose)
    printf ("epadj($%08lX, %d, %ld):  sep->e_time=%ld\n",
	    sep, sdir, tval, sep->e_time);
#endif

#if CHECKP
  Pcheck (sep, "sep - ep_adj() entry");
#endif

  if (tval < 0)			/* return start of score for negative times */
    return (scores[curscor]);

  if (sdir)
    {				/* find left (earliest) end of chain */

      if (sep->e_time LT tval)
	{

	  while (E_NULL NE (tep = sep->e_fwd))
	    {

#if CHECKP
	      Pcheck (tep, "tep - ep_adj() L .1.");
#endif

#if	DEBUGIT
	      if (verbose AND testing)
		printf ("  .1. sep=$%08lX, tep=$%08lX\n", sep, tep);
#endif

	      if (sep->e_time LT tval)
		sep = tep;
	      else
		break;
	    }
	}

      while (E_NULL NE (tep = sep->e_bak))
	{

#if CHECKP
	  Pcheck (tep, "tep - ep_adj() L .2.");
#endif

#if	DEBUGIT
	  if (verbose AND testing)
	    printf ("  .2. sep=$%08lX, tep=$%08lX\n", sep, tep);
#endif

	  if ((tep->e_time LT tval) OR (tep->e_type EQ EV_SCORE))
	    {

#if	DEBUGIT
	      if (verbose)
		printf ("  .3. $%08lX returned\n", sep);
#endif
	      return (sep);
	    }

	  sep = tep;
	}

#if CHECKP
      Pcheck (tep, "tep - ep_adj() L .4.");
#endif

#if	DEBUGIT
      if (verbose)
	printf ("  .4. $%08lX returned\n", sep);
#endif
      return (sep);

/* 
*/

    }
  else
    {				/* find right (latest) end of chain */

      if (sep->e_time GT tval)
	{

	  while (E_NULL NE (tep = sep->e_bak))
	    {

#if	CHECKP
	      Pcheck (tep, "tep - ep_adj() R .5.");
#endif

#if DEBUGIT
	      if (verbose AND testing)
		printf ("  .5. sep=$%08lX, tep=$%08lX\n", sep, tep);
#endif

	      if ((sep->e_time LE tval) OR (sep->e_type EQ EV_SCORE))
		break;
	      else
		sep = tep;
	    }
	}

      while (E_NULL NE (tep = sep->e_fwd))
	{

#if CHECKP
	  Pcheck (tep, "tep - ep_adj() R .6.");
#endif

#if DEBUGIT
	  if (verbose AND testing)
	    printf ("  .6. sep=$%08lX, tep=$%08lX\n", sep, tep);
#endif

	  if (tep->e_time GT tval)
	    {

#if	DEBUGIT
	      if (verbose)
		printf ("  .7. $%08lX returned\n", sep);
#endif
	      return (sep);
	    }

	  sep = tep;
	}

#if CHECKP
      Pcheck (tep, "tep - ep_adj() R .8.");
#endif

#if DEBUGIT
      if (verbose)
	printf ("  .8. $%08lX returned\n", sep);
#endif
      return (sep);
    }
}

/* 
*/

/*
   =============================================================================
	frfind(tval, sdir) -- Returns a pointer to the event chain
	at the time 'tval' in the current score in the direction 'sdir',
	or E_NULL if the current score is empty.
   =============================================================================
*/

struct s_entry *
frfind (tval, sdir)
     register long tval;
     int sdir;
{
  register int i;
  register long t_min, dt;
  register struct s_entry *ep, *sep;

#if	DEBUGIT
  if (verbose)
    {

      printf ("frfind(%ld, %d):  searching\n", tval, sdir);
    }
#endif

#if CHECKP
  Pcheck (scp, "scp - frfind() - entry");
  Pcheck (p_fwd, "p_fwd - frfind() - entry");
  Pcheck (p_cur, "p_cur - frfind() - entry");
  Pcheck (p_bak, "p_bak - frfind() - entry");
#endif

  if (scp EQ E_NULL)
    {				/* NULL if no score selected */

#if	DEBUGIT
      if (verbose)
	printf ("frfind(%ld, %d):  found scp EQ E_NULL\n", tval, sdir);
#endif
      return (E_NULL);
    }

  if (tval < 0)
    return (ep_adj (scp, sdir, tval));

  if (p_cur->e_time EQ tval)
    {				/* at p_cur ? */

#if	DEBUGIT
      if (verbose)
	printf ("frfind():  found tval at p_cur\n");
#endif
      return (ep_adj (p_cur, sdir, tval));
    }

  if (p_fwd->e_time EQ tval)
    {				/* at p_fwd ? */

#if	DEBUGIT
      if (verbose)
	printf ("frfind():  found tval at p_fwd\n");
#endif
      return (ep_adj (p_fwd, sdir, tval));
    }

  if (p_bak->e_time EQ tval)
    {				/* at p_bak ? */

#if	DEBUGIT
      if (verbose)
	printf ("frfind():  found tval at p_bak\n");
#endif
      return (ep_adj (p_bak, sdir, tval));
    }

  t_min = (tval GT p_cur->e_time) ?	/* time from p_cur */
    (tval - p_cur->e_time) : (p_cur->e_time - tval);

  ep = p_cur;

  dt = (tval GT p_fwd->e_time) ?	/* time from p_fwd */
    (tval - p_fwd->e_time) : (p_fwd->e_time - tval);

  if (dt LT t_min)
    {				/* select shortest time */

      t_min = dt;
      ep = p_fwd;

#if	DEBUGIT
      if (verbose)
	printf ("frfind():  p_fwd dt=%ld\n", dt);
#endif
    }

/* 
*/

  dt = (tval GT p_bak->e_time) ?	/* time to p_bak */
    (tval - p_bak->e_time) : (p_bak->e_time - tval);

  if (dt LT t_min)
    {				/* select shortest time */

      t_min = dt;
      ep = p_bak;

#if	DEBUGIT
      if (verbose)
	printf ("frfind():  p_bak dt=%ld\n", dt);
#endif
    }

  if (NOT insmode)
    {

      for (i = 0; i < N_SECTS; i++)
	{			/* search section list */

	  if (E_NULL NE (sep = seclist[curscor][i]))
	    {

	      dt = (tval GT sep->e_time) ?	/* time to section */
		(tval - sep->e_time) : (sep->e_time - tval);

	      if (dt LT t_min)
		{		/* select shortest time */

		  t_min = dt;
		  ep = sep;

#if	DEBUGIT
		  if (verbose)
		    printf ("frfind():  section %d dt=%ld\n", i, dt);
#endif
		}
	    }
	}
    }

#if	CHECKP
  Pcheck (ep, "ep - frfind() - ep_adj()/exiting");
#endif

  return (ep_adj (ep, sdir, tval));	/* adjust the pointer */
}

/* 
*/

/*
   =============================================================================
	findev(ep, te, et, d1, d2) -- Searches the event chain
	starting at 'ep' for an event at a time of 'te' with:
	a type of 'et', e_data1 EQ 'd1', and e_data2 EQ 'd2'.

	The values of 'd1' or 'd2' may be -1, in which case
	e_data1 or e_data2 will be assumed to match.

	Returns a pointer to the desired event if it is found, or
	'E_NULL' if no event in the chain matches the criteria given.
   =============================================================================
*/

struct s_entry *
findev (ep, te, et, d1, d2)
     struct s_entry *ep;
     register long te;
     register short et, d1, d2;
{
  register struct s_entry *tp;

  tp = ep_adj (ep, 1, te);	/* search from left end of chain */

  while (tp->e_time EQ te)
    {				/* check the time, ... */

      if ((tp->e_type EQ et) AND	/* ... e_type, */
	  ((d1 EQ - 1) OR (tp->e_data1 EQ d1)) AND	/* ... e_data1, */
	  ((d2 EQ - 1) OR (tp->e_data2 EQ d2)))	/* ... e_data2 */
	return (tp);		/* found the event */

      tp = tp->e_fwd;		/* search forward */
    }

  return (E_NULL);		/* event not found */
}

/* 
*/

/*
   =============================================================================
	ehfind(eh, te, d1, d2) -- Searches the event header chain
	for an event at a time of 'te' with:  a header type of 'eh',
	e_data1 EQ 'd1', and e_data2 EQ 'd2'.

	The value of 'te' may be -1, in which case e_time will
	be assumed to match.

	The values of 'd1' or 'd2' may be -1, in which case
	e_data1 or e_data2 will be assumed to match.

	Returns a pointer to the desired event if it is found, or
	'E_NULL' if no event in the chain matches the criteria given.
   =============================================================================
*/

struct s_entry *
ehfind (eh, te, d1, d2)
     register short eh;
     register long te;
     register short d1, d2;
{
  register struct s_entry *tp;

  tp = hplist[curscor][eh];	/* get head of chain */

  while (E_NULL NE tp)
    {				/* check each event ... */

      if (((te EQ - 1L) OR (tp->e_time EQ te)) AND	/* ... time, */
	  ((d1 EQ - 1) OR (tp->e_data1 EQ d1)) AND	/* ... e_data1, */
	  ((d2 EQ - 1) OR (tp->e_data2 EQ d2)))	/* ... e_data2 */
	return (tp);		/* found the event */

      tp = tp->e_up;		/* search up the chain */
    }

  return (tp);			/* event not found */
}
