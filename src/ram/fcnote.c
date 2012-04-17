/*
   =============================================================================
	fcnote.c -- MIDAS-VII note edit -- find complete note
	Version 1 -- 1988-05-17 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "score.h"
#include "scfns.h"
#include "neddefs.h"

#define	TO_LFT		(TO_BAK + 1)

extern long ctime;		/* time at cursor */

/*
   =============================================================================
	fcnote() -- find complete note

	Arguments:

		grp		group number
		tnote		note number

		ctime		cursor time to search from

	Returns:

		E_NULL		couldn't find the note
		ptr		pointer to the begin event for the note

		p_nbeg		pointer to note begin event
		p_nend		pointer to note end event
		t_note		duration of the note
   =============================================================================
*/

/* 
*/

struct n_entry *
fcnote (grp, tnote)
     register short grp, tnote;
{
  register struct n_entry *bp, *ep;
  register short en, eg, et;
  long t_left;

  /* setup initial search parameters */

  bp = (struct n_entry *) ep_adj (p_cur, 0, ctime);	/* cursor loc */
  t_left = t_cur - TO_LFT;	/* time at left edge of screen */
  p_nbeg = (struct n_entry *) E_NULL;	/* no begin yet */
  p_nend = (struct n_entry *) E_NULL;	/* no end yet */
  t_note = 0L;			/* no duration yet */

  FOREVER
  {				/* scan left from cursor */

    et = 0x007F & bp->e_type;
    en = bp->e_note;
    eg = bp->e_group;

    if ((bp->e_time < t_left) || (et == EV_SCORE))
      {

	/* done -- can't see begin,  or note not there */

	return (E_NULL);

      }
    else if ((et == EV_NEND) && (en == tnote) && (eg == grp))
      {

	/* done -- hit note end first -- notes overlap */

	return (E_NULL);
/* 
*/
      }
    else if ((et == EV_NBEG) && (en == tnote) && (eg == grp))
      {

	/* found note begin -- possible note starting at bp */

	ep = bp->e_fwd;		/* scan to right of begin */

	FOREVER
	{			/* scan right from note begin */

	  et = 0x007F & ep->e_type;	/* event type */
	  en = ep->e_note;	/* note */
	  eg = ep->e_group;	/* group */

	  if ((et == EV_NBEG) && (en == tnote) && (eg == grp))
	    {

	      /* hit note begin first -- done -- notes overlap */

	      return (E_NULL);
/* 
*/
	    }
	  else if ((et == EV_NEND) && (en == tnote) && (eg == grp))
	    {

	      /* hit note end -- done -- found complete note */

	      p_nbeg = bp;	/* note begin */
	      p_nend = ep;	/* note end */
	      t_note = ep->e_time - bp->e_time;
	      return (bp);

	    }
	  else if (et == EV_FINI)
	    {

	      /* hit score end -- done -- can't find end */

	      return (E_NULL);
	    }

	  ep = ep->e_fwd;	/* scan right */

	}			/* end FOREVER */

      }				/* end if */

    bp = bp->e_bak;		/* scan left */

  }				/* end FOREVER */
}
