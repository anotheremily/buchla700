/*
   =============================================================================
	sqfield.c -- MIDAS-VII sequence display field functions
	Version 9 -- 1989-11-16 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "fields.h"
#include "curpak.h"
#include "patch.h"
#include "macros.h"

#include "midas.h"
#include "ptdisp.h"

#if	DEBUGIT
extern short debugsw;

short debugqf = 1;
#endif

extern short (*xy_up) (), (*xy_dn) ();
extern short (*premove) (), (*pstmove) ();
extern short (*curtype) ();

extern short select (), nokey (), stdctp5 ();
extern short cxkstd (), cykstd (), stdmkey (), stddkey ();
extern short stopsm (), smxupd (), sqyupd ();
extern short postcm ();

extern short astat;
extern short cmfirst;
extern short cmtype;
extern short curhold;
extern short curslin;
extern short cvtime;
extern short cvwait;
extern short ncvwait;
extern short sqdeflg;
extern short stccol;
extern short ss_ptsw;
extern short ss_sqsw;
extern short syrate;
extern short tvcwval;
extern short vcwval;

extern short crate1[];

extern struct selbox *csbp;
extern struct selbox sqboxes[];

extern struct seqent seqbuf;
extern struct seqent seqtab[];

extern char sqdebuf[];

short sqxkey ();		/* forward reference */
short sqekey ();		/* forward reference */
short sqmkey ();		/* forward reference */

/* 
*/

struct curpak sq_flds = {

  stdctp5,			/* curtype */
  nokey,			/* premove */
  postcm,			/* pstmove */
  cxkstd,			/* cx_key */
  cykstd,			/* cy_key */
  smxupd,			/* cx_upd */
  sqyupd,			/* cy_upd */
  stopsm,			/* xy_up */
  nokey,			/* xy_dn */
  sqxkey,			/* x_key */
  sqekey,			/* e_key */
  sqmkey,			/* m_key */
  sqdkey,			/* d_key */
  nokey,			/* not_fld */
  (struct fet *) NULL,		/* curfet */
  sqboxes,			/* csbp */
  crate1,			/* cratex */
  crate1,			/* cratey */
  CT_SMTH,			/* cmtype */
  CTOX (2),			/* cxval */
  RTOY (DATAROW)		/* cyval */
};

/* 
*/

/*
   =============================================================================
	sqmkey() -- M key processing for the sequence display
   =============================================================================
*/

sqmkey ()
{
  register short nc;

  if (astat)
    {

      if (stccol EQ 48)
	{

	  if (ss_ptsw EQ 0)
	    {

	      (*xy_dn) ();	/* handle KEY_DOWN functions */

	      (*premove) ();	/* handle PRE-MOVE functions */

	      nc = (*curtype) ();	/* get new CURSOR TYPE wanted */

	      cvtime = syrate;
	      ncvwait = curhold;

	      ss_sqsw = -1;
	      cmtype = nc;
	      cvwait = 1;
	    }

	}
      else
	stdmkey ();

    }
  else
    {

      if (stccol EQ 48)
	{

	  ss_sqsw = 0;
	  cvwait = 1;
	  ncvwait = cvtime;
	  cmfirst = TRUE;

	  if (ss_ptsw EQ 0)
	    (*xy_up) ();

	  (*pstmove) ();	/* handle POST-MOVE functions */

	}
      else
	stdmkey ();
    }
}

/* 
*/

/*
   =============================================================================
	sqekey() -- E key processing for the sequence display
   =============================================================================
*/

sqekey ()
{
  register short nc;

  if (astat)
    {

      if (stccol EQ 48)
	{

	  if (ss_ptsw EQ 0)
	    {

	      (*xy_dn) ();	/* handle KEY_DOWN functions */

	      (*premove) ();	/* handle PRE-MOVE functions */

	      nc = (*curtype) ();	/* get new CURSOR TYPE wanted */

	      cvtime = syrate;
	      ncvwait = curhold;

	      ss_sqsw = 1;
	      cmtype = nc;
	      cvwait = 1;
	    }

	}
      else
	select ();

    }
  else
    {

      if (stccol EQ 48)
	{

	  ss_sqsw = 0;
	  cvwait = 1;
	  ncvwait = cvtime;
	  cmfirst = TRUE;

	  if (ss_ptsw EQ 0)
	    (*xy_up) ();

	  (*pstmove) ();	/* handle POST-MOVE functions */

	}
      else
	select ();
    }
}

/* 
*/

/*
   =============================================================================
	sqxkey() -- delete key handler
   =============================================================================
*/

sqxkey ()
{
  if (NOT astat)
    return;

  if (inrange (stccol, 2, 4))
    {

      memsetw (&seqbuf, 0, NSEQW);
      memsetw (&seqtab[curslin], 0, NSEQW);
      dsqlin (sqdebuf, curslin);
      sqdeflg = TRUE;
      dcursq ();

    }
  else if (inrange (stccol, 12, 22))
    {

      seqtab[curslin].seqact1 = 0;
      seqtab[curslin].seqdat1 = 0;
      memcpyw (&seqbuf, &seqtab[curslin], NSEQW);
      dsqlin (sqdebuf, curslin);
      sqdeflg = TRUE;
      dcursq ();

    }
  else if (inrange (stccol, 24, 34))
    {

      seqtab[curslin].seqact2 = 0;
      seqtab[curslin].seqdat2 = 0;
      memcpyw (&seqbuf, &seqtab[curslin], NSEQW);
      dsqlin (sqdebuf, curslin);
      sqdeflg = TRUE;
      dcursq ();

    }
  else if (inrange (stccol, 36, 46))
    {

      seqtab[curslin].seqact3 = 0;
      seqtab[curslin].seqdat3 = 0;
      memcpyw (&seqbuf, &seqtab[curslin], NSEQW);
      dsqlin (sqdebuf, curslin);
      sqdeflg = TRUE;
      dcursq ();
    }
}

/* 
*/

/*
   =============================================================================
	sqfield() -- setup field routines for the sequence editor
   =============================================================================
*/

sqfield ()
{

#if	DEBUGIT
  if (debugsw AND debugqf)
    printf ("sqfield(): ENTRY\n");
#endif

  curset (&sq_flds);

#if	DEBUGIT
  if (debugsw AND debugqf)
    printf ("sqfield(): EXIT\n");
#endif

}
