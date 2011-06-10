/*
   =============================================================================
	sctrak.c -- advance the score to a given frame
	Version 28 -- 1988-09-23 -- D.N. Lynx Crowe
   =============================================================================
*/

#undef	DEBUGGER		/* define to enable debug trace */

#undef	DEBUG_TR		/* define for sc_trak() debug */

#undef	CHECKPTR		/* define to check pointers */

#include "debug.h"

#include "hwdefs.h"
#include "stddefs.h"
#include "score.h"
#include "scfns.h"
#include "memory.h"
#include "slice.h"

#ifdef	DEBUG_TR
extern short debugsw;
#endif

extern short ancmsw;
extern short dubsw;
extern short ndisp;
extern short recsw;
extern short sbase;
extern short sd;
extern short se;
extern short soffset;

extern short grpmode[];
extern short grpstat[];

extern short varmode[][16];

extern struct gdsel *gdfsep;

extern struct gdsel *gdstbc[];
extern struct gdsel *gdstbn[];
extern struct gdsel *gdstbp[];

/* 
*/

/*
   =============================================================================
	sc_trak(tval) -- position score display such that t_ctr EQ 'tval'
	Assumes that the pointers are correct.
   =============================================================================
*/

int
sc_trak (tval)
     register long tval;
{
  register struct s_entry *rpb, *rpc, *rpf;
  register long rtb, rtc, rtf;
  register short mod48 = 48;
  struct gdsel *gdsp;

  DB_ENTR ("sc_trak");

  if (ndisp NE 2)
    {

      DB_EXIT ("sc_trak - ndisp NE 2");
      return (FAILURE);
    }

#ifdef	CHECKPTR
  Pcheck (p_fwd, "p_fwd - sc_trak - entry");
  Pcheck (p_ctr, "p_ctr - sc_trak - entry");
  Pcheck (p_bak, "p_bak - sc_trak - entry");
#endif

  if ((p_fwd EQ E_NULL) OR (p_ctr EQ E_NULL) OR (p_bak EQ E_NULL))
    {

      DB_EXIT ("sc_trak - NULL ptr");
      return (FAILURE);
    }
/* 
*/
  if (v_regs[5] & 0x0180)	/* setup for VSDD bank 0 */
    vbank (0);

  rpf = p_fwd;			/* forward event pointer */
  rtf = t_fwd;			/* forward event time */

  rpc = p_ctr;			/* current event pointer */
  rtc = t_ctr;			/* current event time */

  rpb = p_bak;			/* backward event pointer */
  rtb = t_bak;			/* backward event time */

#ifdef	DEBUG_TR
  if (debugsw)
    {

      printf
	("\n## sc_trak(%10ld) ENTRY - fc_val=%10ld  sd = %s  sbase=%d  soffset=%d\n",
	 tval, fc_val, sd ? "BAK" : "FWD", sbase, soffset);

      printf ("   p_bak:   %08lX  p_ctr:   %08lX  p_fwd:   %08lX\n",
	      p_bak, p_ctr, p_fwd);

      printf ("   t_bak: %10ld  t_ctr: %10ld  t_fwd: %10ld\n",
	      t_bak, t_ctr, t_fwd);

      printf ("   T.bak: %10ld  T.ctr: %10ld  T.fwd: %10ld\n",
	      p_bak->e_time, p_ctr->e_time, p_fwd->e_time);
    }
#endif

/* 
*/
  DB_CMNT ("sc_trak - update loop");

  if (sd EQ D_FWD)
    {				/* scroll forward */

      DB_CMNT ("sc_trak - forward");

      while (rtc++ LT tval)
	{			/* advance to tval */

	  sreset ();		/* reset highlighting */

	  ++rtb;		/* update target time at p_bak */
	  ++rtf;		/* update target time at p_fwd */

	  if (rpb->e_type NE EV_FINI)
	    {			/* up to end of score */

	      while (rpb->e_time EQ rtb)
		{		/* check event time */

		  se_disp (rpb, D_FWD, gdstbp, 0);	/* display event */
		  rpb = rpb->e_fwd;	/* point at next event */

		  if (rpb->e_type EQ EV_FINI)	/* stop at end of score */
		    break;
		}
	    }

	  if (0 EQ (rtb % mod48))
	    {			/* handle beat markers */

	      if ((struct gdsel *) NULL NE (gdsp = gdfsep))
		{

		  gdfsep = gdsp->next;

		  gdsp->next = gdstbp[12];
		  gdsp->note = 0x1111;
		  gdsp->code = 1;

		  gdstbp[12] = gdsp;
		}
	    }
/* 
*/
	  if (rpc->e_type NE EV_FINI)
	    {			/* up to end of score */

	      while (rpc->e_time EQ rtc)
		{		/* check event time */

		  se_disp (rpc, D_FWD, gdstbc, 1);	/* display event */
		  rpc = rpc->e_fwd;	/* point at next event */

		  if (rpc->e_type EQ EV_FINI)	/* stop at end of score */
		    break;
		}
	    }

	  if (rpf->e_type NE EV_FINI)
	    {			/* up to end of score */

	      while (rpf->e_time EQ rtf)
		{		/* check event time */

		  se_disp (rpf, D_FWD, gdstbn, 0);	/* display event */
		  rpf = rpf->e_fwd;	/* next event pointer */

		  if (rpf->e_type EQ EV_FINI)	/* stop at end of score */
		    break;
		}
	    }

	  if (0 EQ (rtf % mod48))
	    {			/* handle beat markers */

	      if ((struct gdsel *) NULL NE (gdsp = gdfsep))
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

/* 
*/
    }
  else
    {				/* scroll backward */

      DB_CMNT ("sc_trak - backward");

      while (rtc-- GT tval)
	{			/* advance to tval */

	  sreset ();		/* reset highlighting */

	  --rtb;		/* update target time at p_bak */
	  --rtf;		/* update target time at p_fwd */

	  if (rpb->e_type NE EV_SCORE)
	    {			/* up to start of score */

	      while (rpb->e_time EQ rtb)
		{		/* check event time */

		  se_disp (rpb, D_BAK, gdstbp, 0);
		  rpb = rpb->e_bak;	/* point at next event */

		  if (rpb->e_type EQ EV_SCORE)	/* stop at end of score */
		    break;
		}
	    }

	  if (0 EQ (rtb % mod48))
	    {			/* handle beat markers */

	      if ((struct gdsel *) NULL NE (gdsp = gdfsep))
		{

		  gdfsep = gdsp->next;

		  gdsp->next = gdstbp[12];
		  gdsp->note = 0x1111;
		  gdsp->code = 1;

		  gdstbp[12] = gdsp;
		}
	    }
/* 
*/
	  if (rpc->e_type NE EV_SCORE)
	    {			/* up to start of score */

	      while (rpc->e_time EQ rtc)
		{		/* check event time */

		  se_disp (rpc, D_BAK, gdstbc, 1);	/* display event */
		  rpc = rpc->e_bak;	/* point at next event */

		  if (rpc->e_type EQ EV_SCORE)	/* stop at end of score */
		    break;
		}
	    }

	  if (rpf->e_type NE EV_SCORE)
	    {			/* up to end of score */

	      while (rpf->e_time EQ rtf)
		{		/* check event time */

		  se_disp (rpf, D_BAK, gdstbn, 0);	/* display event */
		  rpf = rpf->e_bak;	/* next event pointer */

		  if (rpf->e_type EQ EV_SCORE)	/* stop at end of score */
		    break;
		}
	    }

	  if (0 EQ (rtf % mod48))
	    {			/* handle beat markers */

	      if ((struct gdsel *) NULL NE (gdsp = gdfsep))
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
/* 
*/
  p_fwd = rpf;			/* update p_fwd */
  t_fwd = tval + TO_FWD;	/* update t_fwd */

  p_ctr = rpc;			/* update p_ctr */
  t_ctr = tval;			/* update t_ctr */

  p_bak = rpb;			/* update p_bak */
  t_bak = tval - TO_BAK;	/* update t_bak */

  DB_CMNT ("sc_trak - dslocn");
  dslocn ();			/* display current location */

#ifdef	DEBUG_TR
  if (debugsw)
    {

      printf
	("\n## sc_trak(%10ld) EXIT - fc_val=%10ld  sbase=%d  soffset=%d\n",
	 tval, fc_val, sbase, soffset);

      printf ("   p_bak:   %08lX,  p_ctr:   %08lX,  p_fwd:   %08lX\n",
	      p_bak, p_ctr, p_fwd);

      printf ("   t_bak: %10ld,  t_ctr: %10ld,  t_fwd: %10ld\n",
	      t_bak, t_ctr, t_fwd);

      printf ("   T.bak: %10ld  T.ctr: %10ld  T.fwd: %10ld\n",
	      p_bak->e_time, p_ctr->e_time, p_fwd->e_time);
    }
#endif

  DB_EXIT ("sc_trak");
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	sc_trek(tval) -- follow score chain such that t_cur EQ 'tval'
   =============================================================================
*/

int
sc_trek (tval)
     register long tval;
{
  register struct s_entry *rp, *ep;
  register long tc, rt;
  register short et, grp;

  DB_ENTR ("sc_trek");

#ifdef	CHECKPTR
  Pcheck (p_cur, "p_cur - sc_trek - entry");
#endif

  if (p_cur EQ E_NULL)
    {

      DB_EXIT ("sc_trek - NULL ptr");
      return (FAILURE);
    }

  rp = p_cur;			/* current event pointer */
  rt = t_cur;			/* current event time */

  DB_CMNT ("sc_trek - chasing p_cur");

#ifdef DEBUGGER
  if (se EQ D_FWD)
    DB_CMNT ("sc_trek - forward");
  else
    DB_CMNT ("sc_trek - backward");
#endif

/* 
*/
  while (rt NE tval)
    {				/* track tval */

      if (se EQ D_FWD)
	++rt;
      else
	--rt;

      if (rp->e_type NE EV_FINI)
	{

	  while (rp->e_time EQ rt)
	    {			/* process events a rt */

	      /* "erase head" logic */

	      if (recsw AND (se EQ D_FWD))
		{

		  et = 0x007F & rp->e_type;

		  if ((NOT dubsw) AND ((et EQ EV_NBEG) OR (et EQ EV_NEND)))
		    {

		      grp = rp->e_data2;

		      if (grpstat[grp] AND (2 EQ grpmode[grp]))
			{

			  DB_CMNT ("sc_trek - erasing note");

			  /* erasing a note */

			  ep = rp->e_fwd;

			  if (rp EQ p_bak)
			    p_bak = ep;

			  if (rp EQ p_cur)
			    p_cur = ep;

			  if (rp EQ p_ctr)
			    p_ctr = ep;

			  if (rp EQ p_fwd)
			    p_fwd = ep;

			  e_del (e_rmv (rp));
			  rp = ep;
			  goto nextev;
			}
/* 
*/
		    }
		  else if (et EQ EV_ANVL)
		    {

		      grp = 0x000F & rp->e_data1;

		      if (grpstat[grp] AND (2 EQ (ancmsw ?
						  varmode[0x000F &
							  (rp->
							   e_data1 >> 4)][grp]
						  : grpmode[grp])))
			{

			  DB_CMNT ("sc_trek - erasing var");

			  /* erasing an analog variable */

			  ep = rp->e_fwd;

			  if (rp EQ p_bak)
			    p_bak = ep;

			  if (rp EQ p_cur)
			    p_cur = ep;

			  if (rp EQ p_ctr)
			    p_ctr = ep;

			  if (rp EQ p_fwd)
			    p_fwd = ep;

			  e_del (e_rmv (rp));
			  rp = ep;
			  goto nextev;
			}
		    }
		}

	      if (rp->e_time EQ rt)
		{		/* check event time */

		  se_exec (rp, se);	/* execute event */

		  if (se EQ D_FWD)
		    rp = rp->e_fwd;	/* point at next event */
		  else
		    rp = rp->e_bak;	/* point at next event */
		}

	    nextev:
	      if (((se EQ D_FWD) AND (rp->e_type EQ EV_FINI)) OR
		  ((se EQ D_BAK) AND (rp->e_type EQ EV_SCORE)))
		break;
	    }
	}
    }
/* 
*/
  p_cur = rp;			/* update p_cur */
  t_cur = tval;			/* update t_cur */

  DB_EXIT ("sc_trek");
  return (SUCCESS);
}
