/*
   =============================================================================
	ldfield.c -- librarian display field processing and cursor motion
	Version 47 -- 1989-11-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "graphdef.h"
#include "charset.h"
#include "panel.h"
#include "hwdefs.h"
#include "fpu.h"
#include "curpak.h"

#include "midas.h"
#include "instdsp.h"
#include "libdsp.h"
#include "score.h"

extern short select (), whatbox (), nokey (), cxkstd (), cykstd (),
stdmkey ();
extern short et_null (), ef_null (), rd_null (), nd_null (), lmwclr ();
extern short vcputsv (), ftkind (), _clsvol (), lcancel (), stdctp0 ();
extern short ldline (), lin2slt (), dslslot (), putcat (), showcat (),
showsiz ();

extern unsigned exp_c ();

extern char *memset (), *slotnam ();

/* 
*/

#if	DEBUGIT
extern short debugsw;
#endif

extern short astat;
extern short catin;
extern short cmfirst;
extern short cxrate;
extern short cxval;
extern short cyrate;
extern short cyval;
extern short ldelsw;
extern short lderrsw;
extern short ldkind;
extern short ldpass;
extern short ldrow;
extern short ldslot;
extern short lmwtype;
extern short loadrow;
extern short loadsw;
extern short lorchl;
extern short lrasw;
extern short lselsw;
extern short ltagged;
extern short ndisp;
extern short oldltag;
extern short oldpk;
extern short oldsl;
extern short pkctrl;
extern short sliders;
extern short stcrow;
extern short stccol;
extern short submenu;
extern short tagslot;
extern short thcwval;
extern short tvcwval;

extern unsigned *librob;

extern char bfs[];
extern char ldfile[];
extern char ldcmnt[];

extern short crate1[];
extern short ldmap[];

extern
PFS (*swpt)[];
     extern PFS (*oldsw)[];

     extern struct selbox *csbp;
     extern struct selbox ldboxes[];

/* forward reference */

     short ld_prmv (), ldxkey (), ldmkey (), ldkey (), ldcxupd (), ldcyupd ();

/* 
*/

     struct fet ld_fet1[] = {

       {21, 10, 17, 0x0000, et_null, ef_null, rd_null, nd_null},	/* name */
       {21, 27, 63, 0x0000, et_null, ef_null, rd_null, nd_null},	/* comment */

       {0, 0, 0, 0x0000, FN_NULL, FN_NULL, FN_NULL, FN_NULL}
     };

short ldbox[][8] = {		/* display box parameters */

  {1, 1, 510, 13, LCFBX00, LCBBX00, 0, 1},	/*  0 - index area label */
  {1, 14, 510, 292, LCFBX01, LCBBX01, 1, 1},	/*  1 - index area */
  {1, 294, 78, 307, LCFBX02, LCBBX02, 21, 1},	/*  2 - file name label */
  {80, 294, 143, 307, LCFBX03, LCBBX03, 21, 10},	/*  3 - file name */
  {145, 294, 214, 307, LCFBX04, LCBBX04, 21, 19},	/*  4 - comment label */
  {216, 294, 510, 307, LCFBX05, LCBBX05, 21, 27},	/*  5 - comment */
  {1, 309, 70, 321, LCFBX06, LCBBX06, 22, 1},	/*  6 - fetch */
  {1, 322, 70, 335, LCFBX07, LCBBX07, 23, 1},	/*  7 - replace / append */
  {1, 336, 70, 348, LCFBX08, LCBBX08, 24, 1},	/*  8 - lo orch / hi orch */
  {72, 309, 255, 348, LCFBX09, LCBBX09, 22, 10},	/*  9 - store */
  {257, 309, 510, 348, LCFBX10, LCBBX10, 22, 33}	/* 10 - message window */
};

char *ldbxlb0[] = {		/* display box labels */

  "RecrdType L FileName Comment                               Mem",	/*  0 */
  "",				/*  1 */
  "FileName",			/*  2 */
  "",				/*  3 */
  "Comment",			/*  4 */
  "",				/*  5 */
  "Index",			/*  6 */
  "",				/*  7 */
  "",				/*  8 */
  "",				/*  9 */
  ""				/* 10 */
};

/* 
*/

struct curpak ld_flds = {

  stdctp0,			/* curtype */
  ld_prmv,			/* premove */
  nokey,			/* pstmove */
  cxkstd,			/* cx_key */
  cykstd,			/* cy_key */
  ldcxupd,			/* cx_upd */
  ldcyupd,			/* cy_upd */
  nokey,			/* xy_up */
  nokey,			/* xy_dn */
  ldxkey,			/* x_key */
  select,			/* e_key */
  ldmkey,			/* m_key */
  ldkey,			/* d_key */
  nokey,			/* not_fld */
  ld_fet1,			/* curfet */
  ldboxes,			/* csbp */
  crate1,			/* cratex */
  crate1,			/* cratey */
  CT_TEXT,			/* cmtype */
  LCURX,			/* cxval */
  LCURY				/* cyval */
};

/* 
*/

/*
   =============================================================================
	ldxkey() -- handle the 'X' (delete) key
   =============================================================================
*/

short
ldxkey ()
{
  register char *fn;
  register short slot;
  char buf[4];

  if (! astat)		/* only on key closure */
    return (FAILURE);

  clrerms ();
  stcrow = cyval / 14;
  stccol = cxval >> 3;

  if (lselsw && lrasw)
    {

      if (0 != (slot = ldline (cyval)))
	{

	  slot -= 1;
	  ldmap[slot] = -1;
	  dpy_scr (ldbox[1][4], slot);
	  return (SUCCESS);

	}
      else
	return (FAILURE);
    }

  if (lcancel (2))
    return (SUCCESS);

  if (! ldelsw)
    {				/* deletion not yet selected ? */

      if (0 != (ldrow = ldline (cyval)))
	{			/* index area ? */

	  if (stccol == 11)
	    {			/* load tag ? */

	      if (-1 == (slot = lin2slt (stcrow)))
		return (FAILURE);

	      if (ltagged)
		{

		  if (slot != tagslot)
		    {

		      filecat[tagslot].fcp0 = oldltag;
		      ltagged = FALSE;
		      showcat ();
		      return (FAILURE);
		    }

		  putcat ();
		  ltagged = FALSE;
		  showcat ();

		}
	      else
		{

		  oldltag = filecat[slot].fcp0;
		  filecat[slot].fcp0 = ' ';
		  ltagged = TRUE;
		  tagslot = slot;

		  buf[0] = oldltag;
		  buf[1] = '\0';

		  if (v_regs[5] & 0x0180)
		    vbank (0);

		  vcputsv (librob, 64, exp_c (LD_DELC), ldbox[1][5],
			   stcrow, 11, buf, 14);
		}

	      return (SUCCESS);
	    }

	  if ((stccol < 13) || (stccol > 20))	/* name ? */
	    return (FAILURE);

	  if (-1 != (ldslot = lin2slt (ldrow)))
	    {

	      /* select file to delete */

	      dslslot (ldslot, exp_c (LD_DELC), ldrow);
	      ldelsw = TRUE;
	      return (SUCCESS);
	    }
/* 
*/
	}
      else
	{			/* not in the index area */

	  if (21 == stcrow)
	    {			/* filename or comment ? */

	      if ((stccol >= 10) && (stccol <= 17))
		{

		  /* clear filename field */

		  memset (ldfile, ' ', 8);
		  ldswin (3);
		  return (SUCCESS);

		}
	      else if (stccol >= 27)
		{

		  /* clear comment field */

		  memset (ldcmnt, ' ', 37);
		  ldswin (5);
		  return (SUCCESS);
		}
	    }

	  return (FAILURE);
	}

/* 
*/

    }
  else
    {				/* file selected for deletion */

      /* delete the file if cursor hasn't moved */

      if (ldrow == ldline (cyval))
	{

	  filecat[ldslot].fcsize[0] = 0;
	  fn = slotnam (ldslot, ftkind (ldslot));

	  unlink (fn);

	  ldelsw = FALSE;
	  putcat ();

	  _clsvol ();
	  showcat ();
	  showsiz ();

	  return (SUCCESS);
	}

      ldelsw = FALSE;
      showcat ();
      return (SUCCESS);
    }
}

/* 
*/

/*
   =============================================================================
	ld_prmv() -- librarian cursor pre-move function
   =============================================================================
*/

ld_prmv ()
{
  if (lderrsw)			/* clear any error messages in the window */
    clrerms ();

  if (ltagged)
    {				/* clear any non-stored load tag */

      filecat[tagslot].fcp0 = oldltag;
      ltagged = FALSE;
      showcat ();
    }
}

/* 
*/

/*
   =============================================================================
	ldcyupd() -- update cursor y location
   =============================================================================
*/

ldcyupd ()
{
  if (lmwtype == 1)
    {

      vtcyupd ();		/* update virtual typewriter cursor y */

    }
  else
    {

      cyval += cyrate;

      if (cyval > (CYMAX - 1))
	cyval = CYMAX - 1;
      else if (cyval < 1)
	cyval = 1;
    }
}

/* 
*/

/*
   =============================================================================
	ldcxupd() -- update cursor x location
   =============================================================================
*/

ldcxupd ()
{
  if (lmwtype == 1)
    {

      vtcxupd ();		/* update virtual typewriter cursor x */

    }
  else
    {

      cxval += cxrate;

      if (cxval > (CXMAX - 1))
	cxval = CXMAX - 1;
      else if (cxval < 1)
	cxval = 1;
    }
}

/* 
*/

/*
   =============================================================================
	ldmkey() -- librarian menu key processor
   =============================================================================
*/

ldmkey ()
{
  if (astat)
    {

      ltagged = FALSE;

      if ((sliders == LS_LIBR) || (pkctrl == PK_LIBR))
	{

	  sliders = oldsl;
	  swpt = oldsw;
	  pkctrl = oldpk;
	  lcdlbls ();
	  setleds ();
	}
    }

  stdmkey ();
}

/* 
*/

/*
   =============================================================================
	loadem() -- load multiple files
   =============================================================================
*/

loadem (key)
     short key;
{
  register short c, i, rc, slot;

  rc = FALSE;

  if (getcat (0))		/* get the file catalog */
    return;

  for (ldslot = 0; ldslot < 20; ldslot++)
    {				/* check each slot */

      if (ocslot (ldslot) &&
	  ((c = filecat[ldslot].fcp0) & 0x007F) == (key & 0x007F))
	{

	  ldkind = ftkind (ldslot);

	  if ((ldkind == FT_ORC) || (ldkind == FT_ORL) || (ldkind == FT_ORH))
	    lorchl = (c & 0x0080) ? 1 : 0;

	  if (ldkind == FT_SCR)
	    for (i = 0; i < N_SCORES; i++)
	      ldmap[i] = i;

	  if (getit ())
	    return;

	  rc = TRUE;
	}
    }

  if (rc && (ndisp == - 1))
    m7menu ();
}

/* 
*/

/*
   =============================================================================
	ldkey() -- process data key entry
   =============================================================================
*/

ldkey (k)
     short k;
{
  register short c, col, row, slot;
  char buf[4];

  if (! astat)		/* only on key closure */
    return (FAILURE);

  if (! catin)		/* catalog must be valid */
    return;

  row = cyval / 14;		/* determine cursor position */
  col = cxval >> 3;

  if (col != 11)		/* must be column 11 */
    return;

  if (-1 == (slot = lin2slt (row)))	/* ... and a valid slot */
    return;

  if (! ltagged)
    {

      oldltag = filecat[slot].fcp0;	/* save old tag */
      lcancel (3);		/* cancel other selections */
    }

/* 
*/
  /* process the key */

  if (k < 7)
    filecat[slot].fcp0 = (c = k + 'A');
  else if (k == 7)
    return;
  else if (k == 8)
    filecat[slot].fcp0 = (c = filecat[slot].fcp0 & 0x007F);
  else				/* k == 9 */
    filecat[slot].fcp0 = (c = filecat[slot].fcp0 | 0x0080);

  loadrow = row;
  tagslot = slot;
  ltagged = TRUE;

  dslslot (slot, exp_c (LD_SELC), row);
  return;
}

/* 
*/

/*
   =============================================================================
	ldfield() -- setup field routines for the librarian
   =============================================================================
*/

ldfield ()
{
  lmwtype = 0;
  ltagged = FALSE;
  submenu = FALSE;

  curset (&ld_flds);
}
