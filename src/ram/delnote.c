/*
   =============================================================================
	delnote.c -- MIDAS-VII note deletion
	Version 16 -- 1988-10-27 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "score.h"
#include "scfns.h"
#include "graphdef.h"
#include "vsdd.h"

#include "midas.h"
#include "scdsp.h"

#define	TO_LFT		(TO_BAK + 1)

extern short cflag;		/* accidental flag */
extern short cnote;		/* note value at cursor */
extern short cxval;		/* cursor x value */
extern short cyval;		/* cursor y value */
extern short lstflag;		/* last note entry list end flag */
extern short recsw;		/* record status */

extern long ctime;		/* time at cursor */

extern short grpmode[12];	/* group record modes */
extern short grpstat[12];	/* group status */
/* 
*/

/*
   =============================================================================
	delnote() -- delete a note
   =============================================================================
*/

short
delnote ()
{
  register struct n_entry *bp, *cp, *ep;
  register long t_left;
  register short tnote, grp;
  register short runtag, scantag;
  short disptag;

  disptag = FALSE;		/* nothing changed yet */

  if (NOT recsw)		/* must be in record mode */
    return (FAILURE);

  /* convert cursor position to note number and time */

  if (pix2mid (cxval, cyval))
    goto notnote;

  ++cflag;
  tnote = cnote;
  t_left = t_cur - TO_LFT;
  cp = (struct n_entry *) frfind (ctime, 0);

/* 
*/
  while (cflag--)
    {				/* handle both naturals and accidentals */

      for (grp = 0; grp < 12; grp++)
	{			/* for each group ... */

	  if ((grpstat[grp] EQ 0) OR	/* ... enabled and ... */
	      (grpmode[grp] NE 2))	/* ... in record mode: */
	    continue;

	  runtag = TRUE;
	  bp = cp;		/* start at cursor */

	  while (runtag)
	    {			/* scan left from cursor until: */

	      if ((bp->e_time LT t_left) OR (bp->e_type EQ EV_SCORE))
		{		/* left edge */

		  /* done -- nothing to delete */

		  runtag = FALSE;
		  continue;
/* 
*/
		}
	      else if ((bp->e_type EQ EV_NEND) AND
		       (bp->e_note EQ tnote) AND (bp->e_group EQ grp))
		{		/* note end */

		  /* done -- overlap */

		  runtag = FALSE;
		  continue;
/* 
*/

		}
	      else if ((bp->e_type EQ EV_NBEG) AND
		       (bp->e_note EQ tnote) AND (bp->e_group EQ grp))
		{		/* note begin */

		  /* possible deletion */

		  /* scan right from note begin until: */

		  ep = bp->e_fwd;
		  scantag = TRUE;

		  while (scantag)
		    {

		      /* note begin -- done -- overlap */

		      if ((ep->e_type EQ EV_NBEG) AND
			  (ep->e_note EQ tnote) AND (ep->e_group EQ grp))
			{

			  scantag = FALSE;
			  runtag = FALSE;
			  continue;
/* 
*/

			}
		      else if ((ep->e_type EQ EV_NEND) AND
			       (ep->e_note EQ tnote) AND (ep->e_group EQ grp))
			{

			  /* note end */

			  /* delete note end */

			  if (cp EQ ep)
			    cp = cp->e_bak;

			  if (p_ctr EQ ep)
			    p_ctr = p_ctr->e_bak;

			  if (p_bak EQ ep)
			    p_bak = p_bak->e_bak;

			  if (p_fwd EQ ep)
			    p_fwd = p_fwd->e_bak;

			  if (p_cur EQ ep)
			    p_cur = p_cur->e_bak;

			  e_del (e_rmv (ep));
/* 
*/

			  /* delete note begin */

			  if (cp EQ bp)
			    cp = cp->e_bak;

			  if (p_ctr EQ bp)
			    p_ctr = p_ctr->e_bak;

			  if (p_bak EQ bp)
			    p_bak = p_bak->e_bak;

			  if (p_fwd EQ bp)
			    p_fwd = p_fwd->e_bak;

			  if (p_cur EQ bp)
			    p_cur = p_cur->e_bak;

			  e_del (e_rmv (bp));

			  disptag = TRUE;
			  runtag = FALSE;
			  scantag = FALSE;
			  continue;
/* 
*/

			}
		      else if (ep->e_type EQ EV_FINI)
			{

			  /* score end */

			  /* delete note begin */

			  if (cp EQ bp)
			    cp = cp->e_bak;

			  if (p_ctr EQ bp)
			    p_ctr = p_ctr->e_bak;

			  if (p_bak EQ bp)
			    p_bak = p_bak->e_bak;

			  if (p_fwd EQ bp)
			    p_fwd = p_fwd->e_bak;

			  if (p_cur EQ bp)
			    p_cur = p_cur->e_bak;

			  e_del (e_rmv (bp));

			  disptag = TRUE;
			  runtag = FALSE;
			  scantag = FALSE;
			  continue;
			}

		      /* scan right */

		      ep = ep->e_fwd;

		    }		/* end while (scantag) */

		}		/* end if */
/* 
*/

	      /* scan left */

	      bp = bp->e_bak;

	    }			/* end while (runtag) */

	}			/* end for (grp) */

      /* handle accidentals */

      if (ac_code EQ N_SHARP)
	++tnote;		/* treat accidentals as sharps */
      else
	--tnote;		/* treat accidentals as flats */

    }				/* end while (cflag) */


/* 
*/

notnote:			/* jumped to if pix2mid() returned FAILURE (not on a note) */

  /* check for a bar marker delete operation */

  if (NOT disptag)
    {				/* if no notes were deleted, try for a bar */

      if (ctime < 0L)		/* time has to be good */
	return (FAILURE);

      /* search current time for a bar marker - delete any found */

      ep = ep_adj (p_cur, 1, ctime);

      while (ctime EQ ep->e_time)
	{

	  bp = ep->e_fwd;

	  if (EV_BAR EQ ep->e_type)
	    {

	      if (ep EQ p_bak)
		p_bak = p_bak->e_bak;

	      if (ep EQ p_ctr)
		p_ctr = p_ctr->e_bak;

	      if (ep EQ p_cur)
		p_cur = p_cur->e_bak;

	      if (ep EQ p_fwd)
		p_fwd = p_fwd->e_bak;

	      e_del (e_rmv (ep));
	      disptag = TRUE;
	    }

	  ep = bp;
	}
    }

  if (disptag)
    sc_refr (t_cur);

  return (disptag ? SUCCESS : FAILURE);
}
