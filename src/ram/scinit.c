/*
   =============================================================================
	scinit.c -- score list functions
	Version 33 -- 1988-07-28 -- D.N. Lynx Crowe

	long
	scinit()

		Initializes the score data structures.
		Returns the number of free storage units.

	struct s_entry *
	e_clr(e1)
	struct s_entry *e1;

		Clears the event pointed to by 'e1'.  Returns 'e1'.
		Preserves e_size field, all others set to zero.

	struct s_entry *
	e_ins(e1, e2)
	struct s_entry *e1, *e2;

		Inserts the event pointed to by 'e1' after the event
		pointed to by 'e2'.  Returns 'e1'.

	struct s_entry *
	e_rmv(e1)
	struct s_entry *e1;

		Removes the event pointed to by 'e1' from the list it's in.
		Returns 'e1'.

	struct s_entry *
	e_alc(w)
	int w;

		Allocates a new event entry.
		Returns the event entry address, or E_NULL
		if none can be allocated.

	short
	e_del(e1)
	struct s_entry *e1;

		Deallocates the event entry pointed to by 'e1'.
		Returns 0 if successful, 1 if not.

	long
	evleft()

		Returns total number of longs left for score storage.

	eh_ins(ne, et)
	struct s_entry *ne;
	short et;

		Inserts event 'ne' of type 'et' into score header list
		"hplist[curscor][et]" along the "up" chain.

	eh_rmv(ev, et)
	struct s_entry *ev;
	short et;

		Removes event 'ev' of type 'et' from score header list
		"hplist[curscor][et]" along the "up" chain.

	sc_clr(ns)
	short ns;

		Clears score 'ns'.

	short
	selscor(ns)
	short ns;

		Selects score 'ns' for use.

   =============================================================================
*/

#include "stddefs.h"
#include "hwdefs.h"
#include "memory.h"
#include "score.h"

/* 
*/

extern short sd;		/* Scroll direction */
extern short curasg;		/* Current assgnment table */
extern short tmpoval;		/* Current tempo */
extern short curtun;		/* Current tuning table */

extern BOOL se_chg;		/* Score free list changed flag */

extern long spcount;		/* Score storage pool free entry count */
extern long frags;		/* Score storage pool fragment count */

extern long se1_cnt;		/* Score free list E_SIZE1 entry count */
extern long se2_cnt;		/* Score free list E_SIZE2 entry count */
extern long se3_cnt;		/* Score free list E_SIZE3 entry count */

extern long *pspool;		/* Score storage pool pointer */

extern struct s_entry *size1;	/* Score E_SIZE1 entry free list pointer */
extern struct s_entry *size2;	/* Score E_SIZE2 entry free list pointer */
extern struct s_entry *size3;	/* Score E_SIZE3 entry free list pointer */

extern struct s_entry *scores[N_SCORES];	/* Score pointer table */
extern struct s_entry *seclist[N_SCORES][N_SECTS];	/* Section pointer table */
extern struct s_entry *hplist[N_SCORES][N_TYPES];	/* Score change list pointers */

extern struct s_time stimes[N_SCORES][N_SECTS];	/* section times */

extern char scname[N_SCORES][16];	/* Score names */

extern short curscor;		/* Current score number */
extern short cursect;		/* Current section number in score */

extern struct s_entry *scp;	/* Current score pointer */

extern long t_bak;		/* Time at p_bak */
extern long t_cur;		/* Time at p_cur */
extern long t_ctr;		/* Time at p_ctr */
extern long t_fwd;		/* Time at p_fwd */

extern struct s_entry *p_bak;	/* Pointer to entry at left edge of display */
extern struct s_entry *p_cur;	/* Pointer to entry at current execution time */
extern struct s_entry *p_ctr;	/* Pointer to entry at center of display */
extern struct s_entry *p_fwd;	/* Pointer to entry at right edge of display */

extern long spool[];		/* Score storage pool */

/* 
*/

/*
   =============================================================================
	evleft() -- returns total number of longs left for score storage.
   =============================================================================
*/

long
evleft ()
{
  return (spcount + (se1_cnt * E_SIZE1) + (se2_cnt * E_SIZE2) +
	  (se3_cnt * E_SIZE3));
}

/* 
*/

/*
   =============================================================================
	scinit() -- initializes the score data structures.
	Returns the number of free storage units.
   =============================================================================
*/

long
scinit ()
{
  register long i, *cp;

  se1_cnt = 0L;			/* fragments */
  size1 = E_NULL;
  se2_cnt = 0L;
  size2 = E_NULL;
  se3_cnt = 0L;
  size3 = E_NULL;
  se_chg = TRUE;
  frags = 0L;

  spcount = (long) MAX_SE;	/* storage pool */
  pspool = spool;
  cp = spool;

  for (i = spcount; i-- > 0;)
    *cp++ = 0L;

  for (i = 0; i < N_SCORES; i++)	/* score names */
    memcpy (scname[i], "{ empty score  }", 16);

  memsetw (scores, 0, 2 * N_SCORES);	/* score list */
  memsetw (hplist, 0, 2 * N_SCORES * N_TYPES);	/* header lists */
  memsetw (seclist, 0, 2 * N_SCORES * N_SECTS);	/* section lists */
  memset (stimes, 0, 12 * N_SCORES * N_SECTS);	/* section times */

  t_cur = t_ctr = 0L;		/* times */
  t_bak = t_cur - TO_BAK;
  t_fwd = t_cur + TO_FWD;

  p_bak = E_NULL;		/* pointers */
  p_cur = E_NULL;
  p_ctr = E_NULL;
  p_fwd = E_NULL;
  scp = E_NULL;

  curscor = 0;			/* current score */
  cursect = 0;			/* current section */

  return (evleft ());
}

/* 
*/

/*
   =============================================================================
	e_clr(e1) -- clear the event pointed to by 'e1'.  Returns 'e1'.
	Preserves e_size field, all others set to zero.
   =============================================================================
*/

struct s_entry *
e_clr (e1)
     register struct s_entry *e1;
{
  switch (e1->e_size)
    {

    case E_SIZE3:

      e1->e_lft = E_NULL;
      e1->e_rgt = E_NULL;

    case E_SIZE2:

      e1->e_up = E_NULL;
      e1->e_dn = E_NULL;

    case E_SIZE1:

      e1->e_fwd = E_NULL;
      e1->e_bak = E_NULL;

      e1->e_time = 0L;
      e1->e_type = EV_NULL;
      e1->e_data1 = 0;
      e1->e_data2 = 0;
    }

  return (e1);
}

/* 
*/

/*
   =============================================================================
	e_ins(e1, e2) -- inserts the event pointed to by 'e1' after the event
	pointed to by 'e2'.  Returns 'e1'.
   =============================================================================
*/

struct s_entry *
e_ins (e1, e2)
     register struct s_entry *e1, *e2;
{
  register struct s_entry *t1;

  t1 = e2->e_fwd;
  e1->e_fwd = t1;
  e1->e_bak = e2;
  e2->e_fwd = e1;
  t1->e_bak = e1;
  return (e1);
}

/*
   =============================================================================
	e_rmv(e1) -- removes the event pointed to by 'e1' from the list it's in.
	Returns 'e1'.
   =============================================================================
*/

struct s_entry *
e_rmv (e1)
     register struct s_entry *e1;
{
  register struct s_entry *t1, *t2;

  t1 = e1->e_bak;
  t2 = e1->e_fwd;
  t1->e_fwd = t2;
  t2->e_bak = t1;
  e1->e_fwd = E_NULL;
  e1->e_bak = E_NULL;
  return (e1);
}

/* 
*/

/*
   =============================================================================
	e_alc() -- allocates a new event entry.
	Returns the event entry address, or E_NULL if none can be allocated.
   =============================================================================
*/

struct s_entry *
e_alc (w)
     short w;
{
  register struct s_entry *ev, *ex;

  se_chg = TRUE;

  switch (w)
    {

    case E_SIZE1:

      if (spcount < E_SIZE1)
	{			/* try raw free pool first */

	  if (se1_cnt == 0)
	    {			/* try for a deleted entry */

	      if (se3_cnt)
		{		/* try to split a size3 entry */

		  ex = size3;
		  size3 = ex->e_fwd;
		  ++frags;
		  --se3_cnt;
		  ex->e_size = E_SIZE1;
		  ex->e_fwd = E_NULL;
		  return (ex);

		}
	      else
		{

		  if (se2_cnt)
		    {		/* try a size2 split */

		      ex = size2;
		      size2 = size2->e_fwd;
		      ex->e_size = E_SIZE1;
		      ex->e_fwd = E_NULL;
		      ++frags;
		      --se2_cnt;
		      return (ex);

		    }
		  else
		    {

		      return (E_NULL);	/* no space */
		    }
		}
/* 
*/
	    }
	  else
	    {			/* deleted entry available */

	      ex = size1;
	      size1 = size1->e_fwd;
	      ex->e_fwd = E_NULL;
	      --se1_cnt;
	      return (ex);
	    }

	}
      else
	{			/* raw storage available */

	  ex = (struct s_entry *) pspool;
	  pspool += E_SIZE1;
	  spcount -= E_SIZE1;
	  ex->e_size = E_SIZE1;
	  return (ex);
	}

/* 
*/

    case E_SIZE2:

      if (spcount < E_SIZE2)
	{			/* try for raw storage */

	  if (se2_cnt == 0)
	    {			/* try for a deleted entry */

	      if (se3_cnt)
		{		/* try to split a size3 entry */

		  ex = size3;
		  size3 = size3->e_fwd;
		  ex->e_size = E_SIZE2;
		  ex->e_fwd = E_NULL;
		  --se3_cnt;
		  ++frags;
		  return (ex);

		}
	      else
		{

		  return (E_NULL);	/* no space */
		}

	    }
	  else
	    {			/* deleted entry available */

	      ex = size2;
	      size2 = size2->e_fwd;
	      ex->e_fwd = E_NULL;
	      --se2_cnt;
	      return (ex);
	    }

	}
      else
	{			/* raw storage available */

	  ex = (struct s_entry *) pspool;
	  pspool += E_SIZE2;
	  spcount -= E_SIZE2;
	  ex->e_size = E_SIZE2;
	  return (ex);

	}

/* 
*/

    case E_SIZE3:

      if (spcount < E_SIZE3)
	{			/* try for raw storage */

	  if (se3_cnt == 0)
	    {			/* try for a deleted entry */

	      return (E_NULL);	/* no space left */

	    }
	  else
	    {			/* deleted entry available */

	      ex = size3;
	      size3 = size3->e_fwd;
	      ex->e_fwd = E_NULL;
	      --se3_cnt;
	      return (ex);
	    }

	}
      else
	{			/* raw storage available */

	  ex = (struct s_entry *) pspool;
	  pspool += E_SIZE3;
	  spcount -= E_SIZE3;
	  ex->e_size = E_SIZE3;
	  return (ex);
	}

    default:

      return (E_NULL);		/* invalid request */
    }

  return (E_NULL);		/* something went 'worng' ... */
}

/* 
*/

/*
   =============================================================================
	e_del(e1) -- deallocates the event entry pointed to by 'e1'.
	Returns 0 if successful, 1 if not.
   =============================================================================
*/

short
e_del (e1)
     register struct s_entry *e1;
{
  e_clr (e1);

  switch (e1->e_size)
    {

    case E_SIZE1:

      e1->e_fwd = size1;
      size1 = e1;
      ++se1_cnt;
      break;

    case E_SIZE2:

      e1->e_fwd = size2;
      size2 = e1;
      ++se2_cnt;
      break;

    case E_SIZE3:

      e1->e_fwd = size3;
      size3 = e1;
      ++se3_cnt;
      break;

    default:
      return (1);
    }

  se_chg = TRUE;
  return (0);
}

/* 
*/

/*
   =============================================================================
	eh_ins(ne, et) -- insert event 'ne' of type 'et' into score header list
	"hplist[curscor][et]" along the "up" chain.
   =============================================================================
*/

eh_ins (ne, et)
     register struct s_entry *ne;
     short et;
{
  register struct s_entry *hp, *ep;

  hp = hplist[curscor][et];	/* get list pointer */

  if (hp == E_NULL)
    {				/* if list was empty */

      hplist[curscor][et] = ne;	/* start the list */
      ne->e_up = E_NULL;
      ne->e_dn = E_NULL;
      return;
    }

  if (hp->e_time > ne->e_time)
    {				/* if first entry was later */

      hp->e_dn = ne;		/* add to start of list */
      ne->e_up = hp;
      ne->e_dn = E_NULL;
      hplist[curscor][et] = ne;
      return;
    }

  while (E_NULL != (ep = hp->e_up))
    {				/* search forward */

      if (ep->e_time > ne->e_time)
	{			/* if we find a later event */

	  ne->e_up = ep;	/* insert into list */
	  ne->e_dn = hp;
	  hp->e_up = ne;
	  ep->e_dn = ne;
	  return;
	}

      hp = ep;
    }

  hp->e_up = ne;		/* add to end of list */
  ne->e_up = E_NULL;
  ne->e_dn = hp;
  return;
}

/* 
*/

/*
   =============================================================================
	eh_rmv(ev, et) -- remove event 'ev' of type 'et' from score header list
	"hplist[curscor][et]" along the "up" chain.
   =============================================================================
*/

eh_rmv (ev, et)
     register struct s_entry *ev;
     short et;
{
  if (hplist[curscor][et] == ev)	/* update hplist */
    hplist[curscor][et] = ev->e_up;

  if (ev->e_up != E_NULL)	/* update entry above */
    (ev->e_up)->e_dn = ev->e_dn;

  if (ev->e_dn != E_NULL)	/* update entry below */
    (ev->e_dn)->e_up = ev->e_up;

  ev->e_up = ev->e_dn = E_NULL;	/* update entry itself */
}

/* 
*/

/*
   =============================================================================
	sc_clr(ns) -- clear score 'ns'.
   =============================================================================
*/

sc_clr (ns)
     register short ns;
{
  register struct s_entry *dsp, *nsp;
  register short i;

  if (E_NULL != (nsp = scores[ns]))
    {

      while (nsp != (dsp = nsp->e_fwd))
	{

	  e_del (e_rmv (nsp));	/* delete this one */
	  nsp = dsp;		/* point at the next one */

	}

      e_del (nsp);		/* delete final entry */
    }

  for (i = 0; i < N_TYPES; i++)	/* clear header list */
    hplist[ns][i] = E_NULL;

  for (i = 0; i < N_SECTS; i++)
    {				/* clear section lists */

      seclist[ns][i] = E_NULL;
      memset (stimes[ns], 0, 12);
    }

  memcpy (scname[ns], "{ empty score  }", 16);

  scores[ns] = E_NULL;
}

/* 
*/

/*
   =============================================================================
	selscor(ns) -- select score 'ns' for use
   =============================================================================
*/

short
selscor (ns)
     register short ns;
{
  register short oldscor, grp;
  register struct s_entry *nsp, *ep;

  clkset (0);			/* stop the clock */
  dsclk ();
  fc_val = 0L;			/* reset to start of score */
  sd = D_FWD;
  oldscor = curscor;		/* setup for new score */
  curscor = ns;

  if (E_NULL == (nsp = scores[ns]))
    {				/* initialize score if needed */

      if (E_NULL == (nsp = e_alc (E_SIZE1)))
	{			/* score header */

	  curscor = oldscor;
	  return (FAILURE);
	}

      scores[ns] = nsp;
      nsp->e_fwd = nsp;
      nsp->e_bak = nsp;
      nsp->e_type = EV_SCORE;
      nsp->e_data1 = ns;
      nsp->e_time = 0L;

      if (E_NULL == (ep = e_alc (E_SIZE1)))
	{			/* score end */

	  sc_clr (ns);
	  curscor = oldscor;
	  return (FAILURE);
	}

      ep->e_type = EV_FINI;
      ep->e_data1 = ns;
      e_ins (ep, nsp);
      ep->e_time = 0x7FFFFFFFL;
      nsp = scores[ns];
      memcpy (&scname[ns][0], "%%% Untitled %%%", 16);
    }

  scp = nsp;
  p_bak = nsp;
  p_cur = nsp;
  p_ctr = nsp;
  p_fwd = nsp;
  cursect = 0;
  sc_goto (0L);
  return (SUCCESS);
}
