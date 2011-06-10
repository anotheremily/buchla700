/*
   =============================================================================
	nedacc.c -- MIDAS-VII note edit functions
	Version 11 -- 1988-08-16 -- D.N. Lynx Crowe

	This file contains the note edit operations:

		NOP_ACC		Begin Acc	ned_acc()
		NOP_NAT		Begin Nat	ned_nat()
		NOP_END		End Note	ned_end()
		NOP_MVN		Move Note	ned_mvn()
		NOP_MVB		Move Begin	ned_mvb()
		NOP_MVE		Move End	ned_mve()

	and some related support functions:

		accnote()
		donote()
		nedesub()
   =============================================================================
*/

#undef	DEBUGGER		/* define to enable debug trace */
#define	DEBUGIT		0

#include "stddefs.h"
#include "debug.h"
#include "graphdef.h"
#include "hwdefs.h"
#include "score.h"
#include "scfns.h"
#include "secops.h"
#include "neddefs.h"
#include "vsdd.h"

#include "midas.h"
#include "scdsp.h"

#if	DEBUGIT
extern short debugsw;
#endif

/* 
*/

extern short pix2mid ();

extern struct n_entry *fcnote ();

extern short cflag;		/* accidental flag */
extern short cnote;		/* note value at cursor */
extern short cyval;		/* cursor y value */
extern short cxval;		/* cursor x value */
extern short recsw;		/* record / play switch */
extern short scmctl;		/* area 1 menu status */

extern short grpmode[];		/* group record / play status */
extern short grpstat[];		/* group enable status */

extern long ctime;		/* time at cursor */

/* 
*/

/*
   =============================================================================
	accnote() -- return accidental note number or -1 for errors
   =============================================================================
*/

short
accnote ()
{
  register short rc;

  DB_ENTR ("accnote");

  rc = -1;

  if ((ac_code EQ N_SHARP) AND cflag)
    {

      DB_CMNT ("accnote - N_SHARP");
      rc = cnote + 1;

    }
  else if ((ac_code EQ N_FLAT) AND cflag)
    {

      DB_CMNT ("accnote - N_FLAT");
      rc = cnote - 1;

    }
  else
    {

      DB_CMNT ("accnote - no accidental possible");
    }

#if	DEBUGIT
  if (debugsw)
    printf ("accnote():  cnote = %d, ac_code = %d, cflag = %d, rc = %d\n",
	    cnote, ac_code, cflag, rc);
#endif
  DB_EXIT ("accnote");
  return (rc);
}

/* 
*/

/*
   =============================================================================
	ned_acc() -- enter accidental note
   =============================================================================
*/

short
ned_acc (grp)
     short grp;
{
  register short nn;
  register struct n_entry *ep;

  DB_ENTR ("ned_acc");

  if (-1 EQ (nn = accnote ()))
    {

      DB_EXIT ("ned_acc - accnote() failed");
      return (FAILURE);
    }

  if (E_NULL NE (ep = (struct n_entry *) e_alc (E_SIZE1)))
    {

#if	DEBUGIT
      if (debugsw)
	printf ("ned_acc():  NBEG g=%d, n=%d, t=%ld, ep=$%08lX\n",
		grp, nn, ctime, ep);
#endif
      DB_CMNT ("ned_acc - entering note begin");
      ep->e_time = ctime;
      ep->e_type = EV_NBEG;
      ep->e_note = nn;
      ep->e_group = grp;
      ep->e_vel = SM_SCALE (64);
      e_ins ((struct s_entry *) ep, ep_adj (p_cur, 0, ctime));
      noteop = NOP_END;		/* setup for end of note */
      DB_EXIT ("ned_acc - note begin entered");
      return (SUCCESS);
    }

  noteop = NOP_NUL;		/* clear pending operation code */
  DB_EXIT ("ned_acc - no space for note begin");
  return (FAILURE);
}

/* 
*/

/*
   =============================================================================
	ned_nat() -- enter natural note
   =============================================================================
*/

short
ned_nat (grp)
     short grp;
{
  register struct n_entry *ep;

  DB_ENTR ("ned_nat");

  if (E_NULL NE (ep = (struct n_entry *) e_alc (E_SIZE1)))
    {

#if	DEBUGIT
      if (debugsw)
	printf ("ned_nat():  NBEG g=%d, n=%d, t=%ld, ep=$%08lX\n",
		grp, cnote, ctime, ep);
#endif
      DB_CMNT ("ned_nat - entering note begin");
      ep->e_time = ctime;
      ep->e_type = EV_NBEG;
      ep->e_note = cnote;
      ep->e_group = grp;
      ep->e_vel = SM_SCALE (64);
      e_ins ((struct s_entry *) ep, ep_adj (p_cur, 0, ctime));
      noteop = NOP_END;		/* set up for end of note */
      DB_EXIT ("ned_nat - note begin entered");
      return (SUCCESS);
    }

  noteop = NOP_NUL;		/* clear pending operation code */
  DB_EXIT ("ned_nat - no space for note begin");
  return (FAILURE);
}

/* 
*/

/*
   =============================================================================
	nedesub() -- enter note end subroutine

	Returns:

		-1	note end not entered - out of space
		 0	note end entered OK
		 1	note end not entered - no matching begin
   =============================================================================
*/

short
nedesub (grp, note)
     register short grp, note;
{
  register struct s_entry *ep, *np;
  register short et;

  DB_ENTR ("nedesub");

  ep = ep_adj (p_cur, 0, ctime);

#if	DEBUGIT
  if (debugsw)
    printf
      ("nedesub(%d, %d):  ctime=%ld, ep=$%08lX, t_cur=%ld, p_cur=$%08lX\n",
       grp, note, ctime, ep, t_cur, p_cur);
#endif

/* 
*/
  DB_CMNT ("nedesub - left scan");

  FOREVER
  {				/* left scan */

    et = 0x007F & ep->e_type;

    switch (et)
      {				/* dispatch off of event type */

      case EV_NEND:		/* note end */

	if ((ep->e_data1 EQ note) AND (ep->e_data2 EQ grp))
	  {

	    DB_EXIT ("nedesub - note end hit");
	    return (1);
	  }
	else
	  break;

      case EV_SCORE:		/* score begin */

	DB_EXIT ("nedesub - score begin hit");
	return (1);

      case EV_NBEG:		/* note begin */

	if ((ep->e_data1 EQ note) AND (ep->e_data2 EQ grp))
	  {			/* group and note match */

	    ep = ep->e_fwd;	/* setup for scan */
	    DB_CMNT ("nedesub - NBEG nit - right scan");
/* 
*/
	    FOREVER
	    {			/* right scan */

	      et = 0x007F & ep->e_type;

	      switch (et)
		{

		case EV_NEND:

		  if ((ep->e_data1 EQ note) AND (ep->e_data2 EQ grp))
		    {

		      DB_EXIT ("nedesub - note end hit");
		      return (1);

		    }
		  else
		    break;
/* 
*/
		case EV_NBEG:

		  if ((ep->e_data1 EQ note) AND (ep->e_data2 EQ grp))
		    {

		case EV_FINI:

		      DB_CMNT ("nedesub - note begin / fini hit");

		      if (E_NULL NE (np = (struct n_entry *) e_alc (E_SIZE1)))
			{

#if	DEBUGIT
			  if (debugsw)
			    printf
			      ("nedesub():  note end entered - $%08lX t=%ld g=%d n=%d\n",
			       np, ctime, grp, note);
#endif
			  DB_CMNT ("nedesub - entering note end");
			  np->e_time = ctime;
			  np->e_type = EV_NEND;
			  np->e_note = note;
			  np->e_group = grp;
			  np->e_vel = SM_SCALE (64);
			  e_ins ((struct s_entry *) np,
				 ep_adj (p_cur, 0, ctime));
			  DB_EXIT ("nedesub - note end entered");
			  return (0);

			}
		      else
			{

			  DB_EXIT ("nedesub - no space for note end");
			  return (-1);
			}

		    }		/* end if */

		}		/* end switch */

	      ep = ep->e_fwd;	/* point at next event */

	    }			/* end FOREVER - right scan */

	  }			/* end if */

      }				/* end switch */

    ep = ep->e_bak;		/* back up to previous event */

  }				/* end FOREVER - left scan */
}

/* 
*/

/*
   =============================================================================
	ned_end() -- enter note end
   =============================================================================
*/

short
ned_end (grp)
     short grp;
{
  register short rc;

  DB_ENTR ("ned_end");

#if	DEBUGIT
  if (debugsw)
    printf ("ned_end():  trying grp = %d, note = %d\n", grp, cnote);
#endif
  DB_CMNT ("ned_end - trying natural");

  if (-1 EQ (rc = nedesub (grp, cnote)))
    {				/* try for a natural */

      noteop = NOP_NUL;		/* clear pending operation code */
      DB_EXIT ("ned_end - nedesub returned -1  (no space)");
      return (FAILURE);

    }
  else if (0 EQ rc)
    {

      noteop = NOP_NUL;		/* clear pending operation code */
      DB_EXIT ("ned_end - natural note end entered");
      return (SUCCESS);

    }
  else if (1 NE rc)
    {

#if	DEBUGIT
      if (debugsw)
	printf ("ned_end():  nedesub(%d, %d) returned %d\n",
		grp, cnote + 1, rc);
#endif

      noteop = NOP_NUL;		/* clear pending operation code */
      DB_EXIT ("ned_end - nedesub returned non-zero  (unknown error)");
      return (FAILURE);
    }
/* 
*/
#if	DEBUGIT
  if (debugsw)
    printf ("ned_end():  trying grp = %d, note = %d\n", grp, cnote + 1);
#endif

  DB_CMNT ("ned_end - trying accidental");

  if (-1 EQ (rc = nedesub (grp, cnote + 1)))
    {				/* try for an accidental */

      noteop = NOP_NUL;		/* clear pending operation code */
      DB_EXIT ("ned_end - nedesub returned -1  (no space)");
      return (FAILURE);

    }
  else if (1 EQ rc)
    {

      noteop = NOP_NUL;		/* clear pending operation code */
      DB_EXIT ("ned_end - nedesub returned 1  (no match)");
      return (FAILURE);

    }
  else if (0 NE rc)
    {

#if	DEBUGIT
      if (debugsw)
	printf ("ned_end():  nedesub(%d, %d) returned %d\n",
		grp, cnote + 1, rc);
#endif

      noteop = NOP_NUL;		/* clear pending operation code */
      DB_EXIT ("ned_end - nedesub returned non-zero  (unknown error)");
      return (FAILURE);
    }

  noteop = NOP_NUL;		/* clear pending operation code */
  DB_EXIT ("ned_end - accidental note end entered");
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ned_mvn() -- move entire note
   =============================================================================
*/

short
ned_mvn (grp)
     short grp;
{
  register struct n_entry *bp, *ep;
  register short note;

  DB_ENTR ("ned_mvn");

  if (notesel)
    {				/* note selected -- now do the dirty work */

      bp = p_nbeg;		/* point at note begin event */
      ep = p_nend;		/* point at note end event */

      /* clip out the note begin event */

      DB_CMNT ("ned_mvn - clipping out begin");

      if (p_bak EQ bp)
	p_bak = bp->e_fwd;

      if (p_ctr EQ bp)
	p_ctr = bp->e_fwd;

      if (p_cur EQ bp)
	p_cur = bp->e_fwd;

      if (p_fwd EQ bp)
	p_fwd = bp->e_fwd;

      e_rmv ((struct s_entry *) bp);
/* 
*/
      /* clip out the note end event */

      DB_CMNT ("ned_mvn - clipping out end");

      if (p_bak EQ ep)
	p_bak = ep->e_fwd;

      if (p_ctr EQ ep)
	p_ctr = ep->e_fwd;

      if (p_cur EQ ep)
	p_cur = ep->e_fwd;

      if (p_fwd EQ ep)
	p_fwd = ep->e_fwd;

      e_rmv ((struct s_entry *) ep);

      bp->e_time = ctime;	/* correct begin time */
      ep->e_time = ctime + t_note;	/* correct end time */

      /* re-insert the note */

      DB_CMNT ("ned_mvn - re-inserting note");

      e_ins ((struct s_entry *) bp, ep_adj (p_cur, 0, ctime));
      e_ins ((struct s_entry *) ep, ep_adj (p_cur, 0, ctime + t_note));

      notesel = FALSE;		/* note not selected */
      noteop = NOP_NUL;		/* clear pending operation code */
      DB_EXIT ("ned_mvn - note moved");
      return (SUCCESS);

    }
  else
    {

      if (E_NULL NE fcnote (grp, cnote))
	{

	  notesel = TRUE;	/* note selected */
	  DB_EXIT ("ned_mvn - natural selected");
	  return (FAILURE);

	}
      else if (-1 NE (note = accnote ()))
	{

	  if (E_NULL NE (bp = fcnote (grp, note)))
	    {

	      notesel = TRUE;	/* note selected */
	      DB_EXIT ("ned_mvn - accidental selected");
	      return (FAILURE);
	    }
	}

      notesel = FALSE;		/* note not selected */
      noteop = NOP_NUL;		/* clear pending operation code */
      DB_EXIT ("ned_mvn - uanble to find note");
      return (FAILURE);
    }
}

/* 
*/

/*
   =============================================================================
	ned_mvb() -- move beginning of note
   =============================================================================
*/

short
ned_mvb (grp)
     short grp;
{
  register struct n_entry *bp, *ep;
  register short note;

  DB_ENTR ("ned_mvb");

  if (notesel)
    {				/* note selected -- now do the dirty work */

      bp = p_nbeg;		/* point at note begin event */
      ep = p_nend;		/* point at note end event */

      if (ctime GE ep->e_time)
	{			/* check move point */

	  noteop = NOP_NUL;	/* clear pending operation */
	  notesel = FALSE;	/* note not selected */
	  DB_EXIT ("ned_mvb - move point after end");
	  return (FAILURE);
	}

      /* clip out the note begin event */

      DB_CMNT ("ned_mvb - clipping out begin");

      if (p_bak EQ bp)
	p_bak = bp->e_fwd;

      if (p_ctr EQ bp)
	p_ctr = bp->e_fwd;

      if (p_cur EQ bp)
	p_cur = bp->e_fwd;

      if (p_fwd EQ bp)
	p_fwd = bp->e_fwd;

      e_rmv ((struct s_entry *) bp);

      bp->e_time = ctime;	/* correct begin time */

      /* re-insert begin event */

      e_ins ((struct s_entry *) bp, ep_adj (p_cur, 0, ctime));

      noteop = NOP_NUL;		/* clear pending operation */
      notesel = FALSE;		/* note not selected */
      DB_EXIT ("ned_mvb - begin moved");
      return (SUCCESS);
/* 
*/
    }
  else
    {

      if (E_NULL NE fcnote (grp, cnote))
	{			/* natural ? */

	  notesel = TRUE;	/* note selected */
	  DB_EXIT ("ned_mvb - natural selected");
	  return (FAILURE);

	}
      else if (-1 NE (note = accnote ()))
	{			/* accidental ? */

	  if (E_NULL NE (bp = fcnote (grp, note)))
	    {

	      notesel = TRUE;	/* note selected */
	      DB_EXIT ("ned_mvb - accidental selected");
	      return (FAILURE);
	    }
	}

      notesel = FALSE;		/* note not selected */
      noteop = NOP_NUL;		/* clear pending operation code */
      DB_EXIT ("ned_mvb - unable to find note");
      return (FAILURE);
    }
}

/* 
*/

/*
   =============================================================================
	ned_mve() -- move end of note
   =============================================================================
*/

short
ned_mve (grp)
     short grp;
{
  register struct n_entry *bp, *ep;
  register short note;

  DB_ENTR ("ned_mve");

  if (notesel)
    {				/* note selected -- now do the dirty work */

      bp = p_nbeg;		/* point at note begin event */
      ep = p_nend;		/* point at note end event */

      if (ctime LE bp->e_time)
	{			/* check move point */

	  noteop = NOP_NUL;	/* clear pending operation */
	  notesel = FALSE;	/* note not selected */
	  DB_EXIT ("ned_mve - move point before begin");
	  return (FAILURE);
	}

      /* clip out the note end event */

      DB_CMNT ("ned_mve - clipping out end");

      if (p_bak EQ ep)
	p_bak = ep->e_fwd;

      if (p_ctr EQ ep)
	p_ctr = ep->e_fwd;

      if (p_cur EQ ep)
	p_cur = ep->e_fwd;

      if (p_fwd EQ ep)
	p_fwd = ep->e_fwd;

      e_rmv ((struct s_entry *) ep);

      ep->e_time = ctime;	/* correct end time */

      /* re-insert end event */

      e_ins ((struct s_entry *) ep, ep_adj (p_cur, 0, ctime));

      noteop = NOP_NUL;		/* clear pending operation */
      notesel = FALSE;		/* note not selected */
      DB_EXIT ("ned_mve - end moved");
      return (SUCCESS);

    }
  else
    {

      if (E_NULL NE fcnote (grp, cnote))
	{			/* natural ? */

	  notesel = TRUE;	/* note selected */
	  DB_EXIT ("ned_mve - natural selected");
	  return (FAILURE);

	}
      else if (-1 NE (note = accnote ()))
	{

	  if (E_NULL NE (bp = fcnote (grp, note)))
	    {			/* accidental ? */

	      notesel = TRUE;	/* note selected */
	      DB_EXIT ("ned_mve - accidental selected");
	      return (FAILURE);
	    }
	}

      notesel = FALSE;		/* note not selected */
      noteop = NOP_NUL;		/* clear pending operation code */
      DB_EXIT ("ned_mve - unable to find note");
      return (FAILURE);
    }
}

/* 
*/

/*
   =============================================================================
	donote() -- do a note operation
   =============================================================================
*/

donote ()
{
  register short grp, i, gs;

  DB_ENTR ("donote");

  if (scmctl NE - 1)
    {				/* area 1 menu must be down */

      DB_EXIT ("donote - scmctl NE -1");
      return;
    }

  if (NOT recsw)
    {				/* must be in record mode */

      DB_EXIT ("donote - not in record mode");
      return;
    }

  /* determine which group we want to work with */

  gs = 0;
  grp = -1;

  for (i = 0; i < 12; i++)
    {				/* scan the groups */

      if ((grpmode[i] EQ 2) AND grpstat[i])
	{

	  grp = i;		/* log the group */
	  ++gs;			/* count enabled groups */
#if	DEBUGIT
	  if (debugsw)
	    printf ("donote():  gs = %d, grp = %d\n", gs, grp);
#endif
	}
    }

#if	DEBUGIT
  if (debugsw)
    printf ("donote():  final gs = %d, grp = %d\n", gs, grp);
#endif

/* 
*/
  if (gs NE 1)
    {				/* must have a single group enabled */

      DB_EXIT ("donote - no single group enabled");
      return;
    }

  if (pix2mid (cxval, cyval))
    {				/* must point at a note position */

#if	DEBUGIT
      if (debugsw)
	printf ("donote():  no note at cxval = %d, cyval = %d\n",
		cxval, cyval);
#endif
      DB_EXIT ("donote - not at a note position");
      return;
    }
/* 
*/
#if	DEBUGIT
  if (debugsw)
    printf
      ("donote():  at (cxval=%d, cyval=%d):  cnote=%d, cflag=%d, ctime=%ld\n",
       cxval, cyval, cnote, cflag, ctime);
#endif

  DB_CMNT ("donote - dispatching");

  switch (noteop)
    {				/* dispatch off of note operation code */

    case NOP_ACC:		/* Begin Acc */

      if (ned_acc (grp))
	break;

      sc_refr (fc_val);		/* refresh screen if successful */
      break;

    case NOP_NAT:		/* Begin Nat */

      if (ned_nat (grp))
	break;

      sc_refr (fc_val);		/* refresh screen if successful */
      break;

    case NOP_END:		/* End Note */

      if (ned_end (grp))
	break;

      sc_refr (fc_val);		/* refresh screen if successful */
      break;
/* 
*/
    case NOP_MVN:		/* Move Note */

      if (ned_mvn (grp))
	break;

      sc_refr (fc_val);		/* refresh screen if successful */
      break;

    case NOP_MVB:		/* Move Begin */

      if (ned_mvb (grp))
	break;

      sc_refr (fc_val);		/* refresh screen if successful */
      break;

    case NOP_MVE:		/* Move End */

      if (ned_mve (grp))
	break;

      sc_refr (fc_val);		/* refresh screen if successful */
      break;

    default:

      DB_CMNT ("donote - bad operation");
      noteop = NOP_NUL;		/* clear pending operation code */
    }

  dnedmod ();			/* update note edit window */
  DB_EXIT ("donote");
  return;
}
