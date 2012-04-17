/*
   =============================================================================
	scgoto.c -- position display at a given frame time
	Version 48 -- 1988-09-23 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUG_GO	0	/* define non-zero for sc_goto() debug */
#define	CHECKPTR	0	/* define non-zero to check pointers */

#include "hwdefs.h"
#include "stddefs.h"
#include "graphdef.h"
#include "score.h"
#include "scfns.h"
#include "memory.h"
#include "vsdd.h"
#include "slice.h"
#include "midas.h"

/* variables defined elsewhere */

#if	DEBUG_GO
extern short verbose, testing;
#endif

extern short recsw;
extern short ndisp;
extern short sd;
extern short swctrl;
extern short swflag;

extern struct gdsel *gdfsep;

extern struct gdsel *gdstbc[];
extern struct gdsel *gdstbn[];

/* 
*/

/*
   =============================================================================
	sc_goto(tval) -- position score display at time 'tval'
   =============================================================================
*/

int
sc_goto (tval)
     register long tval;
{
  register struct gdsel *gdsp;
  register struct s_entry *rp;
  register long tf, rt;
  register short mod48 = 48;

#if	CHECKPTR
  Pcheck (p_fwd, "p_fwd - sc_goto entry");
  Pcheck (p_ctr, "p_ctr - sc_goto entry");
  Pcheck (p_bak, "p_bak - sc_goto entry");
  Pcheck (p_cur, "p_cur - sc_goto entry");
#endif

  /* quick check of pointers so we don't crash */

  if ((p_fwd == E_NULL) || (p_cur == E_NULL) ||
      (p_bak == E_NULL) || (p_ctr == E_NULL))
    return (FAILURE);

  if (v_regs[5] & 0x0180)	/* setup for VSDD bank 0 */
    vbank (0);

  sd = D_FWD;			/* set display and scroll direction */
  swctrl = FALSE;		/* stop scroll wheel */
  swflag = FALSE;		/* ... */

  recsw = FALSE;		/* force play mode on goto */
  dsrpmod ();			/* update video and LCD displays */

  if (ndisp == 2)
    sreset ();			/* reset highlighting if score is up */

  quiet ();			/* quiet the instrument */
  clrnl ();			/* clear note entry lists */
  clrsctl ();			/* clear slice control data */

  t_bak = tval - TO_BAK;	/* set target time at p_bak */
  t_fwd = t_bak;		/* set target time at p_fwd */
  t_ctr = tval;			/* set target time at p_ctr */
  t_cur = tval;			/* set target time at p_cur */

  p_bak = frfind (t_bak, 0);	/* position p_bak at t_bak */
  p_cur = frfind (t_cur, 1);	/* position p_cur at t_cur */
  p_fwd = frfind (t_fwd, 1);	/* position p_fwd at t_fwd */

/* 
*/
  /* reset the display pointers to the target time */

  if ((t_fwd <= 0) && (p_fwd->e_type == EV_SCORE))
    p_fwd = p_fwd->e_fwd;	/* skip score header */

  rp = p_fwd;			/* current forward event pointer */
  rt = t_fwd;			/* current forward event time */
  tf = tval + TO_FWD;		/* target event time */

#if	DEBUG_GO
  if (verbose)
    {

      printf ("## sc_goto(%8ld) ENTRY - tf: %8ld\n", tval, tf);

      printf ("  t_bak: %8ld  t_ctr: %8ld  t_fwd: %8ld  t_cur: %8ld\n",
	      t_bak, t_ctr, t_fwd, t_cur);

      printf ("  p_bak: %08lX  p_ctr: %08lX  p_fwd: %08lX  p_cur: %08lX\n",
	      p_bak, p_ctr, p_fwd, p_cur);
    }
#endif

/* 
*/

  while (rt++ < tf)
    {				/* advance p_fwd chain to tf */

      if (rp->e_type != EV_FINI)
	{			/* don't pass end of score */

	  while (rp->e_time <= rt)
	    {			/* check event time */

	      if (ndisp == 2)	/* display event */
		se_disp (rp, D_FWD, gdstbn, 0);

	      rp = rp->e_fwd;	/* point at next event */

	      if (rp->e_type == EV_FINI)	/* done if at end */
		break;
	    }
	}

      if (ndisp == 2)
	{

	  if (0 == (rt % mod48))
	    {			/* handle beat markers */

	      if ((struct gdsel *) NULL != (gdsp = gdfsep))
		{

		  gdfsep = gdsp->next;

		  gdsp->next = gdstbn[12];
		  gdsp->note = 0x1111;
		  gdsp->code = 1;

		  gdstbn[12] = gdsp;
		}
	    }

	  sc_adv ();		/* scroll the display */
	}
    }

  p_fwd = rp;			/* update p_fwd for next event */
  t_fwd = tf;			/* update t_fwd */

/* 
*/
  /* execute & display things at current time to start things out right */

  if (ndisp == 2)		/* if score is up ... */
    dssect ();			/* display section */

  rp = p_cur;			/* current event pointer */
  rt = t_cur;			/* current event time */

  if ((rt <= 0) && (rp->e_type == EV_SCORE))	/* skip score header */
    rp = rp->e_fwd;

  if (rp->e_type != EV_FINI)
    {				/* if we aren't at end of score */

      while (rp->e_time == rt)
	{			/* do events at current time */

	  se_exec (rp, D_FWD);	/* execute event */

	  if (ndisp == 2)
	    se_disp (rp, D_FWD, gdstbc, 1);	/* update center slice */

	  rp = rp->e_fwd;	/* point at next event */

	  if (rp->e_type == EV_FINI)	/* done if at end */
	    break;
	}
    }

  p_cur = rp;			/* update p_cur */
  p_ctr = rp;			/* update p_ctr */

  if (ndisp == 2)
    {				/* if score is up ... */

      scupd ();			/* update event display */
      sdwins ();		/* refresh windows */
    }

/* 
*/

#if	DEBUG_GO
  if (verbose)
    {

      printf ("## sc_goto(%8ld) EXIT\n");

      printf ("  t_bak: %8ld  t_ctr: %8ld  t_fwd: %8ld  t_cur: %8ld\n",
	      t_bak, t_ctr, t_fwd, t_cur);

      printf ("  p_bak: %08lx  p_ctr: %08lx  p_fwd: %08lx  p_cur: %08lx\n",
	      p_bak, p_ctr, p_fwd, p_cur);
    }
#endif

#if	CHECKPTR
  Pcheck (p_fwd, "p_fwd - sc_goto exiting");
  Pcheck (p_ctr, "p_ctr - sc_goto exiting");
  Pcheck (p_bak, "p_bak - sc_goto exiting");
  Pcheck (p_cur, "p_cur - sc_goto exiting");
#endif

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	sc_refr() -- refresh the display to a particular time
   =============================================================================
*/

short
sc_refr (t)
     long t;
{
  short oldrs, rc;

  oldrs = recsw;		/* save recsw */
  rc = sc_goto (t);		/* refresh the display via a goto */
  recsw = oldrs;		/* restore recsw */
  dsrpmod ();			/* update display of recsw */
  return (rc);			/* return status from sc_goto */
}
