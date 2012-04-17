/*
   =============================================================================
	asgvce.c -- MIDAS-VII -- assign voice / enter notes into score
	Version 12 -- 1988-10-03 -- D.N. Lynx Crowe
   =============================================================================
*/

#undef	DEBUGGER		/* define to enable debug trace */

#define	DEBUGIT		0

#include "debug.h"

#include "biosdefs.h"
#include "stddefs.h"
#include "graphdef.h"
#include "hwdefs.h"
#include "smdefs.h"
#include "sclock.h"
#include "score.h"
#include "slice.h"
#include "vsdd.h"

#include "midas.h"
#include "scfns.h"
#include "scdsp.h"

#if	DEBUGIT
extern short debugsw;
#endif

/* 
*/

extern short clksrc;		/* clock source */
extern short curasg;		/* current assignment table */
extern short legato;		/* "legato" mode flag */
extern short lstbgnc;		/* last note begin entry count */
extern short lstendc;		/* last note end entry count */
extern short lstflag;		/* last note list end flag */
extern short ndisp;		/* current display number */
extern short newflag;		/* new data entered while t_ctr == t_cur */
extern short nkdown;		/* number of keys down */
extern short recsw;		/* record/play switch */
extern short sd;		/* score display direction */
extern short se;		/* score execution direction */
extern short stepenb;		/* step enable */
extern short stepint;		/* note interval */
extern short stepwgt;		/* note weight */
extern short velflag;		/* velocity display enable flag */

extern short grpmode[];		/* group mode table */
extern short grpstat[];		/* group status table */
extern short ins2grp[];		/* instrument to group assigments */
extern short vce2grp[];		/* voice to group map */
extern short lastvce[];		/* last voice assigned in each group */
extern short lastvel[];		/* last velocity seen by each group */
extern short tuntab[];		/* current tuning table */
extern short vce2trg[];		/* voice to trigger map (-1 == NULL) */

extern short stepfrm[][17];	/* steps per frame table */

extern unsigned *obj8;		/* score display object pointer */

extern struct gdsel *gdstbc[];	/* group status table - center slice */

extern struct n_entry *lstbgns[NLSTENTS];	/* note begin pointers */
extern struct n_entry *lstends[NLSTENTS];	/* note end pointers */

extern struct nevent *nefree;	/* pointer to free note entries */
extern struct nevent *nelist;	/* pointer to note entry list */

extern struct nevent nevents[NNEVTS];	/* note event list */

/* 
*/

/*
   =============================================================================
	clrnl() -- clear note list
   =============================================================================
*/

clrnl ()
{
  register short i;

  DB_ENTR ("clrnl");

  for (i = 0; i < NNEVTS - 1; i++)
    {

      nevents[i].nxt = &nevents[i + 1];
      nevents[i].note = 0;
      nevents[i].group = 0;
    }

  nevents[NNEVTS - 1].nxt = (struct nevent *) 0L;

  nelist = (struct nevent *) 0L;
  nefree = &nevents[0];
  nkdown = 0;

  DB_EXIT ("clrnl");
}

/* 
*/

/*
   =============================================================================
	ne_end() -- enter a note end event
   =============================================================================
*/

ne_end (trg, grp)
     short trg;
     register short grp;
{
  register short nn;
  register long fcend;
  register struct nevent *nx;
  register struct n_entry *ep;

  DB_ENTR ("ne_end");

  /* must be recording into a voice in record mode ... */

  if ((recsw == 0) || (grpmode[grp] != 2))
    {

      DB_EXIT ("ne_end");
      return;			/* ... or, we're done */
    }

  DB_CMNT ("ne_end - recording");

  /* make pointers point in the forward direction */

  if (sd == D_BAK)
    chgsdf ();

  if (se == D_BAK)
    chgsef ();
/* 
*/
  nn = trg & 0x007F;		/* note number */

  if (clksrc != CK_STEP)
    {				/* not in step mode */

      DB_CMNT ("ne_end - non-step");

      if (E_NULL != (ep = (struct n_entry *) e_alc (E_SIZE1)))
	{

	  DB_CMNT ("ne_end - enter note end");
	  ep->e_time = t_cur;
	  ep->e_type = EV_NEND | 0x80;
	  ep->e_note = nn;
	  ep->e_group = grp;
	  ep->e_vel = SM_SCALE (64);

	  p_cur = e_ins ((struct s_entry *) ep,
			 ep_adj (p_cur, 0, t_cur))->e_fwd;

	  if (t_cur == t_ctr)
	    newflag = TRUE;

	  se_disp (ep, D_FWD, gdstbc, 1);

	}
      else
	{

	  DB_CMNT ("ne_end - no space");
	}

      DB_EXIT ("ne_end");
      return;
/* 
*/
    }
  else if (nkdown >= 1)
    {				/* process note entry in step mode */

      DB_CMNT ("ne_end - log key up");

      if (0 EQ-- nkdown)
	{			/* if all keys are up ... */

	  if (stepenb)
	    {

	      /* advance by the note weight */

	      DB_CMNT ("ne_end - advance weight");
	      fcend = fc_val + stepfrm[3][stepint];
	      fc_val += stepfrm[stepwgt][stepint];
	      sc_trek (fc_val);
	      sc_trak (fc_val);
	      DB_CMNT ("ne_end - doing note ends");
	    }

	  while (nelist)
	    {

	      nn = nelist->note;	/* get note */
	      grp = nelist->group;	/* get group */

	      if (E_NULL != (ep = (struct n_entry *) e_alc (E_SIZE1)))
		{

		  DB_CMNT ("ne_end - enter note end");
		  ep->e_time = t_cur;
		  ep->e_type = EV_NEND | 0x80;
		  ep->e_note = nn;
		  ep->e_group = grp;
		  ep->e_vel = SM_SCALE (64);

		  p_cur = e_ins ((struct s_entry *) ep,
				 ep_adj (p_cur, 0, t_cur))->e_fwd;

		  se_disp (ep, D_FWD, gdstbc, 1);

		  if (lstendc < NLSTENTS)
		    lstends[lstendc++] = ep;

		}
	      else
		{

		  DB_CMNT ("ne_end - no space");
		}
/* 
*/
	      DB_CMNT ("ne_end - freeing nevent");
	      nx = nelist->nxt;	/* get next nelist ptr */
	      nelist->nxt = nefree;	/* free nelist entry */
	      nefree = nelist;	/* ... */
	      nelist = nx;	/* update nelist */
	    }

	  lstflag = TRUE;	/* indicate end of list */

	  DB_CMNT ("ne_end - note ends done");

	  if (stepenb)
	    {

	      if (fc_val < fcend)
		{		/* advance to the interval */

		  DB_CMNT ("ne_end - advance interval");
		  fc_val = fcend;
		  sc_trek (fc_val);
		  sc_trak (fc_val);
		}
	    }
	}

    }
  else
    {

      nkdown = 0;		/* no keys down */

      lstendc = 0;		/* no step entries to delete */
      lstbgnc = 0;
      lstflag = FALSE;
    }

  DB_EXIT ("ne_end");
}

/* 
*/

/*
   =============================================================================
	ne_bgn() -- enter a note begin event
   =============================================================================
*/

ne_bgn (grp, key, vel)
     register short grp, key, vel;
{
  register struct n_entry *ep;
  register struct nevent *np;

  DB_ENTR ("ne_bgn");

  /* must be recording into a group in record mode ... */

  if ((recsw != 0) && (grpmode[grp] == 2))
    {

      DB_CMNT ("ne_bgn - recording");

      /* make pointers point in the forward direction */

      if (sd == D_BAK)
	chgsdf ();

      if (se == D_BAK)
	chgsef ();

      if (lstflag)
	{			/* cancel old list of notes */

	  lstbgnc = 0;
	  lstendc = 0;
	  lstflag = FALSE;
	}

      if (clksrc == CK_STEP)
	{			/* step mode */

	  DB_CMNT ("ne_bgn - step");
	  ++nkdown;		/* count keys down */

	  if (nefree)
	    {			/* log a key closure */

	      np = nefree;
	      nefree = np->nxt;
	      np->note = key;
	      np->group = grp;
	      np->nxt = nelist;
	      nelist = np;
	      DB_CMNT ("ne_bgn - key logged");

	    }
	  else
	    {

	      DB_CMNT ("ne_bgn - nefree empty");
	    }
	}
/* 
*/
      if (E_NULL != (ep = (struct n_entry *) e_alc (E_SIZE1)))
	{

	  DB_CMNT ("ne_bgn - enter note begin");
	  ep->e_time = t_cur;
	  ep->e_type = EV_NBEG | 0x80;
	  ep->e_note = key;
	  ep->e_group = grp;
	  ep->e_vel = (clksrc == CK_STEP) ? SM_SCALE (64) : vel;

	  p_cur = e_ins ((struct s_entry *) ep,
			 ep_adj (p_cur, 0, t_cur))->e_fwd;

	  se_disp (ep, D_FWD, gdstbc, 1);

	  if (t_cur == t_ctr)
	    newflag = TRUE;

	  if ((clksrc == CK_STEP) && (lstbgnc < NLSTENTS))
	    lstbgns[lstbgnc++] = ep;

	}
      else
	{

	  DB_CMNT ("ne_bgn - no space");
	}
    }

  DB_EXIT ("ne_bgn");
}

/* 
*/

/*
   =============================================================================
	showvel() -- display velocity for a group
   =============================================================================
*/

showvel (g, v)
     short g, v;
{
  char buf[6];

  if (v_regs[5] & 0x0180)
    vbank (0);

  lastvel[g] = v;

  sprintf (buf, "%03d", (v / 252));

  vputs (obj8, 5, (g * 5) + 6, buf, SDW11ATR);
}

/* 
*/

/*
   =============================================================================
	asgvce() -- assign a voice
   =============================================================================
*/

asgvce (grp, port, chan, key, vel)
     short grp, port, chan, key, vel;
{
  register short i;
  register short tv;
  register short vp;
  register short trg;
  register short aflag;
  register struct nevent *np;
  register struct n_entry *ep;

  DB_ENTR ("asgvce");

  trg = (port << 11) + (chan << 7) + key;	/* trigger number */
  vp = lastvce[grp] + 1;	/* voice to start with */
  vp = (vp > 11) ? 0 : vp;	/* ... (adjust into range) */

/* 
*/

  DB_CMNT ("asgvce - search unassgined");

  for (i = 12; i--;)
    {				/* search for unassigned voice */

      if ((vce2trg[vp] == - 1) && (vce2grp[vp] == (grp + 1)))
	{

	  if ((ndisp == 2) && velflag && (! recsw))
	    showvel (grp, vel);

	  lastvce[grp] = vp;
	  execkey (trg, tuntab[key], vp, 0);
	  DB_EXIT ("asgvce - free voice");
	  return;
	}

      if (++vp > 11)
	vp = 0;
    }

  DB_CMNT ("asgvce - seach non-held");

  for (i = 12; i--;)
    {				/* search for non-held voice */

      tv = vce2trg[vp];

      if (tv == - 1)
	aflag = TRUE;		/* OK - unassigned */
      else if (0 == (tv & (MKEYHELD << 8)))
	aflag = TRUE;		/* OK - not held */
      else
	aflag = FALSE;		/* NO - held */

      if (aflag && (vce2grp[vp] == (grp + 1)))
	{

	  if ((ins2grp[grp] & GTAG1) && (tv != trg) && (tv != - 1))
	    legato = 1;

	  if ((ndisp == 2) && velflag && (! recsw))
	    showvel (grp, vel);

	  lastvce[grp] = vp;
	  execkey (trg, tuntab[key], vp, 0);
	  DB_EXIT ("asgvce - stolen voice");
	  return;
	}

      if (++vp > 11)
	vp = 0;
    }

  DB_EXIT ("asgvce - no voice");
}
