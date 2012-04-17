/*
   =============================================================================
	idfield.c -- instrument display field processing and cursor motion
	Version 89 -- 1989-11-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "graphdef.h"
#include "charset.h"
#include "smdefs.h"
#include "curpak.h"

#include "midas.h"
#include "instdsp.h"

extern short select (), stdmkey (), stddkey (), nokey (), cxkstd (),
cykstd ();
extern short vtoy (), ttox (), timeto (), vcputsv (), dswin (), stdctp1 ();
extern short whatbox (), inspnt (), pntsel (), setseg (), showpt ();

extern short et_null (), ef_null (), rd_null (), nd_null ();
extern short et_ipnt (), ef_ipnt (), rd_ipnt (), nd_ipnt ();
extern short et_idin (), ef_idin (), rd_idin (), nd_idin ();
extern short et_ivce (), ef_ivce (), rd_ivce (), nd_ivce ();
extern short et_icnf (), ef_icnf (), rd_icnf (), nd_icnf ();
extern short et_ival (), ef_ival (), rd_ival (), nd_ival ();
extern short et_itim (), ef_itim (), rd_itim (), nd_itim ();
extern short et_iosc (), ef_iosc (), rd_iosc (), nd_iosc ();
extern short et_iact (), ef_iact (), rd_iact (), nd_iact ();
extern short et_iwsn (), ef_iwsn (), rd_iwsn (), nd_iwsn ();
extern short et_ires (), ef_ires (), rd_ires (), nd_ires ();

extern unsigned fromfpu ();

extern char *memset ();

/* 
*/

extern short curvce, curfunc, curpnt, hitbox, astat, idnamsw;
extern short cxval, cyval, cxrate, cyrate, curslim, cmtype, cmfirst;
extern short pecase, pntsv, npts, subj, idimsw, wcflag, imflag;
extern short idcfsw, idsrcsw, vtcrow, vtccol, stcrow, stccol;
extern short vtxval, vtyval;

extern unsigned temax, temin;

extern short *cratex, *cratey, crate0[], crate1[];

extern unsigned *instob;

extern char bfs[];
extern char *idbxlbl[];

extern short fnoff[];
extern short idbox[][8];

extern struct selbox *csbp;
extern struct selbox idboxes[];

extern struct sment sments[];
extern struct sment *vpsms[];

extern struct valent valents[];

extern struct instpnt *pntptr;

extern struct instdef vbufs[];

short idx_key (), idnfld (), idcxupd (), idcyupd (), ikyup (), ikydn ();

/* 
*/

short zonemax[9] = { 63, 119, 175, 231, 287, 343, 399, 455, 509 };

short zoneinc[9] = { 2, 2, 4, 9, 18, 36, 73, 146, 303 };

struct fet id_fet1[] = {

  {5, 62, 62, 12, et_ires, ef_ires, rd_ires, nd_ires},
  {16, 1, 12, 13, et_imlt, ef_imlt, rd_imlt, nd_imlt},
  {16, 15, 16, 14, et_ipnt, ef_ipnt, rd_ipnt, nd_ipnt},
  {16, 19, 24, 15, et_itim, ef_itim, rd_itim, nd_itim},
  {16, 27, 44, 16, et_ival, ef_ival, rd_ival, nd_ival},
  {16, 47, 62, 17, et_iact, ef_iact, rd_iact, nd_iact},
  {17, 12, 13, 18, et_icnf, ef_icnf, rd_icnf, nd_icnf},
  {17, 23, 24, 19, et_ivce, ef_ivce, rd_ivce, nd_ivce},
  {17, 31, 32, 19, et_idin, ef_idin, rd_idin, nd_idin},
  {17, 57, 58, 0x0015, et_iwsn, ef_iwsn, rd_iwsn, nd_iwsn},
  {17, 61, 62, 0x0115, et_iwsn, ef_iwsn, rd_iwsn, nd_iwsn},
  {18, 36, 46, 0x8014, et_iosc, ef_iosc, rd_iosc, nd_iosc},
  {18, 17, 32, 0, et_null, ef_null, rd_null, nd_null},
  {19, 36, 46, 0x8114, et_iosc, ef_iosc, rd_iosc, nd_iosc},
  {19, 17, 32, 0, et_null, ef_null, rd_null, nd_null},
  {20, 36, 46, 0x8214, et_iosc, ef_iosc, rd_iosc, nd_iosc},
  {20, 17, 32, 0, et_null, ef_null, rd_null, nd_null},
  {21, 36, 46, 0x8314, et_iosc, ef_iosc, rd_iosc, nd_iosc},
  {21, 17, 32, 0, et_null, ef_null, rd_null, nd_null},

  {0, 0, 0, 0x0000, FN_NULL, FN_NULL, FN_NULL, FN_NULL}
};

char *srctbl[] = {		/* source label table */

  "       ",			/*  0 */
  "Random ",			/*  1 */
  "GPC/CV1",			/*  2 */
  "???????",			/*  3 */
  "???????",			/*  4 */
  "Pitch  ",			/*  5 */
  "Key Prs",			/*  6 */
  "Key Vel",			/*  7 */
  "Pedal 1",			/*  8 */
  "???????",			/*  9 */
  "Freq   ",			/* 10 */
  "PchW/HT",			/* 11 */
  "ModW/VT",			/* 12 */
  "Brth/LP",			/* 13 */
};

/* 
*/

struct curpak id_flds = {

  stdctp1,			/* curtype */
  nokey,			/* premove */
  nokey,			/* pstmove */
  cxkstd,			/* cx_key */
  cykstd,			/* cy_key */
  idcxupd,			/* cx_upd */
  idcyupd,			/* cy_upd */
  ikyup,			/* xy_up */
  ikydn,			/* xy_dn */
  idx_key,			/* x_key */
  select,			/* e_key */
  stdmkey,			/* m_key */
  stddkey,			/* d_key */
  idnfld,			/* not_fld */
  id_fet1,			/* curfet */
  idboxes,			/* csbp */
  crate1,			/* cratex */
  crate1,			/* cratey */
  CT_GRAF,			/* cmtype */
  ICURX,			/* cxval */
  ICURY				/* cyval */
};

/* 
*/

/*
   =============================================================================
	ikydn() -- process instrument cursor key down
   =============================================================================
*/

ikydn ()
{
  imflag = FALSE;
}

/*
   =============================================================================
	ikyup() -- process instrument cursor key up
   =============================================================================
*/

ikyup ()
{
  if (imflag)
    modinst ();
}

/* 
*/

/*
   =============================================================================
	idcyupd() -- update cursor y location from value
   =============================================================================
*/

idcyupd ()
{
  register short pval, vh, vl;

  if (wcflag != - 1)		/* ws/cf menu page can't be up */
    return;

  if (idimsw)			/* no y update if instrument menu is up */
    return;

  if (idsrcsw || idcfsw)
    {				/* see if we're in the menu area */

      vtcrow = YTOR (vtyval += cyrate);

      if (vtcrow > 24)
	vtyval = RTOY (vtcrow = 24);
      else if (vtcrow < 22)
	vtyval = RTOY (vtcrow = 22);

    }
  else if (idnamsw)
    {				/* see if we're in the typewriter */

      vtcyupd ();

    }
  else if (pntsv)
    {				/* see if we're moving a point */

      pval = (pntptr->ipval >> 5) - (cyrate * 7);

      if (pval > 1000)		/* limit at +10.00 */
	pval = 1000;
      else if (pval < 0)	/* limit at +00.00 */
	pval = 0;

      cyval = vtoy (pval, 12);	/* new cursor location */
      pntptr->ipval = pval << 5;	/* update function value */

      vh = pval / 100;		/* display the new value */
      vl = pval - (vh * 100);

      sprintf (bfs, "%02d.%02d", vh, vl);

      vbank (0);
      vcputsv (instob, 64, idbox[16][4], idbox[16][5],
	       idbox[16][6] + 1, idbox[16][7], bfs, 14);

      imflag = TRUE;
/* 
*/
    }
  else
    {				/* just moving the cursor */

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
	idcxupd() -- update cursor x location from time
   =============================================================================
*/

idcxupd ()
{
  register unsigned th, tl;
  register unsigned fptime, stime;
  short zone;

  if (idimsw)
    {				/* see if instrument menu is up */

      cxval += cxrate;

      if (cxval > CTOX (32))
	cxval = CTOX (32);
      else if (cxval < CTOX (26))
	cxval = CTOX (26);

    }
  if (idsrcsw || idcfsw)
    {				/* see if we're in a submenu */

      vtccol = XTOC (vtxval += cxrate);

      if (vtccol > 46)
	vtxval = CTOX (vtccol = 46);
      else if (vtccol < 17)
	vtxval = CTOX (vtccol = 17);

    }
  else if (idnamsw)
    {				/* see if we're in the typewriter */

      vtcxupd ();
/* 
*/
    }
  else if (pntsv)
    {				/* see if we're moving a point */

      for (zone = 0; zone <= 8; zone++)	/* find display zone */
	if (cxval <= zonemax[zone])
	  break;

      switch (pecase)
	{

	case 0:		/* single point */
	case 1:		/* last point */

	  if (cxrate >= 0)
	    fptime = addfpu (pntptr->iptim, cxrate * zoneinc[zone]);
	  else
	    fptime = subfpu (pntptr->iptim, -cxrate * zoneinc[zone]);

	  stime = segtime (subj, fptime);

	  if (stime >= temax)
	    {

	      setseg (subj, temax - 1);
	      break;

	    }
	  else if (stime == 0)
	    {

	      setseg (subj, 1);
	      break;
	    }

	  setseg (subj, stime);
	  break;

/* 
*/
	case 2:		/* interior point */

	  if (cxrate >= 0)
	    fptime = addfpu (pntptr->iptim, cxrate * zoneinc[zone]);
	  else
	    fptime = subfpu (pntptr->iptim, -cxrate * zoneinc[zone]);

	  stime = temin + fromfpu (fptime);

	  if (stime >= temax)
	    break;

	  setseg (subj, segtime (subj, fptime));
	  setseg (subj + 1, temax);
	  break;
	}

      th = timeto (curfunc, subj);	/* display the time */
      tl = th - ((th / 1000) * 1000);
      th /= 1000;

      sprintf (bfs, "%02d.%03d", th, tl);

      if (v_regs[5] & 0x0180)
	vbank (0);

      vcputsv (instob, 64, idbox[15][4], idbox[15][5],
	       idbox[15][6] + 1, idbox[15][7], bfs, 14);

      imflag = TRUE;
      cxval = ttox (timeto (curfunc, subj), 12);

    }
  else
    {				/* just moving the cursor */

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
	idnfld() -- process not-in-field key entry
   =============================================================================
*/

short
idnfld (k)
     short k;
{
  register short endpnt, basepnt, t, fn;
  register struct idfnhdr *fp;
  register struct instdef *ip;

  ip = &vbufs[curvce];
  fp = &ip->idhfnc[curfunc];

  if (astat)
    {

      if (whatbox ())
	{

	  if (hitbox == 12)
	    {			/* edit window */

	      if (k == 8)
		{		/* - = toggle I_TM_KEY */

		  t = (fp->idftmd ^= I_TM_KEY);

		  if (v_regs[5] & 0x0180)
		    vbank (0);

		  tsplot4 (instob, 64,
			   ((t & I_TM_KEY) ?
			    idbox[curfunc][4] : ID_INST),
			   4, 54, idbxlbl[curfunc], 14);

		  modinst ();
		  return (SUCCESS);
/* 
*/
		}
	      else if (k == 9)
		{		/* + = new point */

		  basepnt = fp->idfpt1;
		  endpnt = basepnt + fp->idfpif - 1;

		  if (fp->idfpif == 99)
		    return (FAILURE);

		  if (FALSE == inspnt (ip, curfunc, endpnt))
		    {

		      return (FAILURE);

		    }
		  else
		    {

		      subj = fp->idfpif - 1;
		      pntsel ();
		      pntsv = 1;
		      memset (pntptr, 0, sizeof (struct instpnt));
		      pntptr->ipval = (pntptr - 1)->ipval;
		      setseg (subj, timeto (curfunc, subj - 1) + 1);
		      edfunc (curfunc);
		      showpt (1);
		      cxval = ttox (timeto (curfunc, subj), 12);
		      cyval = vtoy ((pntptr->ipval >> 5), 12);
		      arcurs (ID_SELD);
		      gcurpos (cxval, cyval);
		      modinst ();
		      return (SUCCESS);
		    }

		}
/* 
*/
	    }
	  else if (hitbox < 12)
	    {			/* label window */

	      fn = (hitbox == curfunc) ? 12 : hitbox;

	      if (k == 8)
		{		/* + = toggle I_TM_KEY */

		  if (v_regs[5] & 0x0180)
		    vbank (0);

		  tsplot4 (instob, 64,
			   (((ip->idhfnc[fn].idftmd ^= I_TM_KEY) & I_TM_KEY)
			    ? idbox[fn][4] : ID_INST),
			   idbox[hitbox][6], idbox[hitbox][7],
			   idbxlbl[fn], 14);

		  modinst ();
		}
	    }
	}

      return (FAILURE);
    }
}

/* 
*/

/*
   =============================================================================
	idx_key() -- process the 'X' (delete) key
   =============================================================================
*/

idx_key ()
{
  register struct sment *smp;
  register struct valent *vep;
  register short i;
  register struct instdef *ip;

  if (astat)
    {

      if (idsrcsw || idcfsw || idnamsw || (wcflag != - 1))
	return;

      if (stcrow == 16)
	{

	  if ((stccol >= 1) && (stccol <= 7))
	    {

	      /* general source */

	      vep = &valents[0];
	      smp = vpsms[(curvce << 4) + fnoff[curfunc]];

	      if (smp->sm != SM_NONE)
		{		/* check for new general S/M */

		  (smp->prv)->nxt = smp->nxt;	/* unlink from old S/M chain */
		  (smp->nxt)->prv = smp->prv;

		  smp->prv = (struct sment *) vep;	/* link into new S/M chain */
		  smp->nxt = vep->nxt;
		  (vep->nxt)->prv = smp;
		  vep->nxt = smp;

		  smp->sm = SM_NONE;	/* update sment S/M entry */
		}

	      vbufs[curvce].idhfnc[curfunc].idfsrc = SM_NONE;
	      dswin (13);
	      modinst ();
/* 
*/
	    }
	  else if ((stccol == 15) || (stccol == 16))
	    {

	      /* current point and points to the right */

	      delpnts ();

	    }
	  else if ((stccol >= 33) && (stccol <= 39))
	    {

	      /* point source */

	      pntptr->ipvsrc = SM_NONE;
	      dswin (16);
	      modinst ();
	    }

	}
      else if ((stcrow == 17) && ((stccol == 31) || (stccol == 32)))
	{

	  /* entire instrument */

	  initi (&vbufs[curvce]);
	  execins (curvce, 0, 0);
	  setinst ();
	  allwins ();
	  modinst ();
	}
    }
}

/* 
*/

/*
   =============================================================================
	idfield() -- setup field routines for the score display
   =============================================================================
*/

idfield ()
{
  idimsw = FALSE;
  curslim = 210;

  curset (&id_flds);
}
