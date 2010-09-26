/*
   =============================================================================
	scfield.c -- score editor field setup
	Version 62 -- 1989-11-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "graphdef.h"
#include "hwdefs.h"
#include "panel.h"
#include "macros.h"
#include "score.h"
#include "scfns.h"
#include "curpak.h"

#include "midas.h"
#include "scdsp.h"

#define	PCENTER		256L

/* functions defined elsewhere */

extern	char	*memset();

extern	short	cxkstd(), cykstd(), select(), oktode(), stdctp3();
extern	short	enterit(), stddkey(), nokey(), stdmkey();

extern	short	et_strn(), ef_strn(), rd_strn(), nd_strn();
extern	short	et_snbt(), ef_snbt(), rd_snbt(), nd_snbt();

extern	short	et_ioas(), ef_ioas(), rd_ioas(), nd_ioas();
extern	short	et_tune(), ef_tune(), rd_tune(), nd_tune();
extern	short	et_tmpo(), ef_tmpo(), rd_tmpo(), nd_tmpo();
extern	short	et_intp(), ef_intp(), rd_intp(), nd_intp();

extern	short	et_inst(), ef_inst(), rd_inst(), nd_inst();
extern	short	et_trns(), ef_trns(), rd_trns(), nd_trns();
extern	short	et_dyn(), ef_dyn(), rd_dyn(), nd_dyn();
extern	short	et_loc(), ef_loc(), rd_loc(), nd_loc();
extern	short	et_vel(), ef_vel(), rd_vel(), nd_vel();

extern	short	et_res1(), ef_res1(), rd_res1(), nd_res1();
extern	short	et_aval(), ef_aval(), rd_aval(), nd_aval();
extern	short	et_agrp(), ef_agrp(), rd_agrp(), nd_agrp();

extern	short	et_scor(), ef_scor(), rd_scor(), nd_scor();

/* forward references */

short	et_null(), ef_null(), rd_null(), nd_null();
short	sdcxu(), sdcyu(), sdxkey(), sdmkey(), scntfld();

/* 
*/

/* variables defined elsewhere */

extern	short	ancmsw;
extern	short	angroup;
extern	short	astat;
extern	short	clkrun;
extern	short	cmfirst;
extern	short	cmtype;
extern	short	curhold;
extern	short	curscor;
extern	short	cxrate;
extern	short	cxval;
extern	short	cyrate;
extern	short	cyval;
extern	short	ebflag;
extern	short	hcwval;
extern	short	insmode;
extern	short	oldpk;
extern	short	oldsl;
extern	short	pkctrl;
extern	short	recsw;
extern	short	scmctl;
extern	short	sdmctl;
extern	short	sliders;
extern	short	stccol;
extern	short	stcrow;
extern	short	thcwval;
extern	short	tvcwval;
extern	short	vcwval;
extern	short	vtccol;
extern	short	vtcrow;
extern	short	vtxval;
extern	short	vtyval;

extern	long	ctime;
extern	long	t_ins;

extern	char	ebuf[MAXEBUF+1];

extern	short	crate1[];
extern	short	ehdlist[];
extern	short	grpmode[];
extern	short	grpstat[];
extern	short	grptmap[];
extern	short	sdmlim[][4];
extern	short	varmode[8][16];

extern	PFS	(*swpt)[];
extern	PFS	(*oldsw)[];

extern	struct	fet	*cfetp;
extern	struct	fet	*curfet;
extern	struct	fet	*infetp;

extern	struct	selbox	*csbp;

extern	struct	selbox	sdboxes[];

extern	struct	s_entry	*peg;

/* 
*/

struct	fet	sc_fet1[] = {

	{ 0,  2,  4, 0x0000, et_strn, ef_strn, rd_strn, nd_strn},
	{ 0, 11, 15, 0x0000, et_snbt, ef_snbt, rd_snbt, nd_snbt},

	{17, 11, 12, 0x0000, et_ioas, ef_ioas, rd_ioas, nd_ioas},
	{17, 19, 19, 0x0000, et_tune, ef_tune, rd_tune, nd_tune},
	{17, 27, 29, 0x0000, et_tmpo, ef_tmpo, rd_tmpo, nd_tmpo},
	{17, 35, 38, 0x0000, et_intp, ef_intp, rd_intp, nd_intp},

	{18,  7,  8,      0, et_inst, ef_inst, rd_inst, nd_inst},
	{18, 12, 13,      1, et_inst, ef_inst, rd_inst, nd_inst},
	{18, 17, 18,      2, et_inst, ef_inst, rd_inst, nd_inst},
	{18, 22, 23,      3, et_inst, ef_inst, rd_inst, nd_inst},
	{18, 27, 28,      4, et_inst, ef_inst, rd_inst, nd_inst},
	{18, 32, 33,      5, et_inst, ef_inst, rd_inst, nd_inst},
	{18, 37, 38,      6, et_inst, ef_inst, rd_inst, nd_inst},
	{18, 42, 43,      7, et_inst, ef_inst, rd_inst, nd_inst},
	{18, 47, 48,      8, et_inst, ef_inst, rd_inst, nd_inst},
	{18, 52, 53,      9, et_inst, ef_inst, rd_inst, nd_inst},
	{18, 57, 58,     10, et_inst, ef_inst, rd_inst, nd_inst},
	{18, 62, 63,     11, et_inst, ef_inst, rd_inst, nd_inst},

	{19,  5,  8,      0, et_trns, ef_trns, rd_trns, nd_trns},
	{19, 10, 13,      1, et_trns, ef_trns, rd_trns, nd_trns},
	{19, 15, 18,      2, et_trns, ef_trns, rd_trns, nd_trns},
	{19, 20, 23,      3, et_trns, ef_trns, rd_trns, nd_trns},
	{19, 25, 28,      4, et_trns, ef_trns, rd_trns, nd_trns},
	{19, 30, 33,      5, et_trns, ef_trns, rd_trns, nd_trns},
	{19, 35, 38,      6, et_trns, ef_trns, rd_trns, nd_trns},
	{19, 40, 43,      7, et_trns, ef_trns, rd_trns, nd_trns},
	{19, 45, 48,      8, et_trns, ef_trns, rd_trns, nd_trns},
	{19, 50, 53,      9, et_trns, ef_trns, rd_trns, nd_trns},
	{19, 55, 58,     10, et_trns, ef_trns, rd_trns, nd_trns},
	{19, 60, 63,     11, et_trns, ef_trns, rd_trns, nd_trns},

/* 
*/

	{20,  6,  6,      0, et_dyn,  ef_dyn,  rd_dyn,  nd_dyn},
	{20,  8,  8,      0, et_loc,  ef_loc,  rd_loc,  nd_loc},

	{20, 11, 11,      1, et_dyn,  ef_dyn,  rd_dyn,  nd_dyn},
	{20, 13, 13,      1, et_loc,  ef_loc,  rd_loc,  nd_loc},

	{20, 16, 16,      2, et_dyn,  ef_dyn,  rd_dyn,  nd_dyn},
	{20, 18, 18,      2, et_loc,  ef_loc,  rd_loc,  nd_loc},

	{20, 21, 21,      3, et_dyn,  ef_dyn,  rd_dyn,  nd_dyn},
	{20, 23, 23,      3, et_loc,  ef_loc,  rd_loc,  nd_loc},

	{20, 26, 26,      4, et_dyn,  ef_dyn,  rd_dyn,  nd_dyn},
	{20, 28, 28,      4, et_loc,  ef_loc,  rd_loc,  nd_loc},

	{20, 31, 31,      5, et_dyn,  ef_dyn,  rd_dyn,  nd_dyn},
	{20, 33, 33,      5, et_loc,  ef_loc,  rd_loc,  nd_loc},

	{20, 36, 36,      6, et_dyn,  ef_dyn,  rd_dyn,  nd_dyn},
	{20, 38, 38,      6, et_loc,  ef_loc,  rd_loc,  nd_loc},

	{20, 41, 41,      7, et_dyn,  ef_dyn,  rd_dyn,  nd_dyn},
	{20, 43, 43,      7, et_loc,  ef_loc,  rd_loc,  nd_loc},

	{20, 46, 46,      8, et_dyn,  ef_dyn,  rd_dyn,  nd_dyn},
	{20, 48, 48,      8, et_loc,  ef_loc,  rd_loc,  nd_loc},

	{20, 51, 51,      9, et_dyn,  ef_dyn,  rd_dyn,  nd_dyn},
	{20, 53, 53,      9, et_loc,  ef_loc,  rd_loc,  nd_loc},

	{20, 56, 56,     10, et_dyn,  ef_dyn,  rd_dyn,  nd_dyn},
	{20, 58, 58,     10, et_loc,  ef_loc,  rd_loc,  nd_loc},

	{20, 61, 61,     11, et_dyn,  ef_dyn,  rd_dyn,  nd_dyn},
	{20, 63, 63,     11, et_loc,  ef_loc,  rd_loc,  nd_loc},

/* 
*/

	{21,  6,  8,      0, et_vel,  ef_vel,  rd_vel,  nd_vel},
	{21, 11, 13,      1, et_vel,  ef_vel,  rd_vel,  nd_vel},
	{21, 16, 18,      2, et_vel,  ef_vel,  rd_vel,  nd_vel},
	{21, 21, 23,      3, et_vel,  ef_vel,  rd_vel,  nd_vel},
	{21, 26, 28,      4, et_vel,  ef_vel,  rd_vel,  nd_vel},
	{21, 31, 33,      5, et_vel,  ef_vel,  rd_vel,  nd_vel},
	{21, 36, 38,      6, et_vel,  ef_vel,  rd_vel,  nd_vel},
	{21, 41, 43,      7, et_vel,  ef_vel,  rd_vel,  nd_vel},
	{21, 46, 48,      8, et_vel,  ef_vel,  rd_vel,  nd_vel},
	{21, 51, 53,      9, et_vel,  ef_vel,  rd_vel,  nd_vel},
	{21, 56, 58,     10, et_vel,  ef_vel,  rd_vel,  nd_vel},
	{21, 61, 63,     11, et_vel,  ef_vel,  rd_vel,  nd_vel},


	{23,  6,  6,      0, et_res1, ef_res1, rd_res1, nd_res1},
	{23,  8, 12,      0, et_aval, ef_aval, rd_aval, nd_aval},

	{23, 15, 15,      1, et_res1, ef_res1, rd_res1, nd_res1},
	{23, 17, 21,      1, et_aval, ef_aval, rd_aval, nd_aval},

	{23, 24, 24,      2, et_res1, ef_res1, rd_res1, nd_res1},
	{23, 26, 30,      2, et_aval, ef_aval, rd_aval, nd_aval},

	{23, 33, 33,      3, et_res1, ef_res1, rd_res1, nd_res1},
	{23, 35, 39,      3, et_aval, ef_aval, rd_aval, nd_aval},

	{23, 42, 42,      4, et_res1, ef_res1, rd_res1, nd_res1},
	{23, 44, 48,      4, et_aval, ef_aval, rd_aval, nd_aval},

	{23, 51, 51,      5, et_res1, ef_res1, rd_res1, nd_res1},
	{23, 53, 57,      5, et_aval, ef_aval, rd_aval, nd_aval},

	{23, 61, 62,      0, et_agrp, ef_agrp, rd_agrp, nd_agrp},

	{24,  7,  8, 0x0000, et_scor, ef_scor, rd_scor, nd_scor},
	{24, 10, 25, 0x0000, et_null, ef_null, rd_null, nd_null},


	{ 0,  0,  0, 0x0000, FN_NULL, FN_NULL, FN_NULL, FN_NULL}
};

/* 
*/

struct curpak sc_flds = {

	stdctp3,	/* curtype */
	nokey,		/* premove */
	nokey,		/* pstmove */
	cxkstd,		/* cx_key */
	cykstd,		/* cy_key */
	sdcxu,		/* cx_upd */
	sdcyu,		/* cy_upd */
	nokey,		/* xy_up */
	nokey,		/* xy_dn */
	sdxkey,		/* x_key */
	select,		/* e_key */
	sdmkey,		/* m_key */
	stddkey,	/* d_key */
	scntfld,	/* not_fld */
	sc_fet1,	/* curfet */
	sdboxes,	/* csbp */
	crate1,		/* cratex */
	crate1,		/* cratey */
	CT_GRAF,	/* cmtype */
	C1X,		/* cxval */
	C1Y		/* cyval */
};

/* 
*/

/*
   =============================================================================
	et_null() .. nd_null() -- null data entry key processors
   =============================================================================
*/

short		/* null et_... */
et_null()
{
	return(SUCCESS);
}

short		/* null ef_... */
ef_null()
{
	return(SUCCESS);
}

short		/* null rd_... */
rd_null()
{
	return(SUCCESS);
}

short		/* null nd_... */
nd_null()
{
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	istart() -- start insert mode
   =============================================================================
*/

istart()
{
	register struct s_entry *ep, *pp, *tp;

	if (insmode)				/* can't already be inserting */
		return;

	ep    = ep_adj(p_cur, 1, t_cur);	/* locate tail */

	if (EV_SCORE EQ (0x007F & ep->e_type))	/* skip begin score event */
		ep = ep->e_fwd;

	if (EV_FINI EQ (0x007F & ep->e_type))	/* null if at end of score */
		return;
		
	tp = scores[curscor]->e_bak;		/* locate end of score */
	pp = ep->e_bak;				/* locate end of head */

	ep->e_bak = E_NULL;			/* unhook tail from head */
	pp->e_fwd = tp;				/* unhook head from tail */
	(tp->e_bak)->e_fwd = E_NULL;		/* unhook tail from end */

	tp->e_bak = pp;				/* connect end to head */

	t_ins = t_cur;				/* record tail time */
	peg   = ep;				/* hang tail on peg */

	p_cur = ep_adj(pp, 0, t_cur);		/* adjust p_cur */
	p_ctr = p_cur;				/* adjust p_ctr */
	p_bak = ep_adj(p_cur, 0, t_bak);	/* adjust p_bak */
	p_fwd = ep_adj(p_cur, 0, t_fwd);	/* adjust p_fwd */

	insmode = TRUE;
}

/* 
*/

/*
   =============================================================================
	icancel() -- cancel insert mode
   =============================================================================
*/

icancel()
{
	register struct s_entry *ep, *tp, *rp;
	struct s_entry *pp;
	long endtime;

	if (NOT insmode)
		return;

	tp = scores[curscor]->e_bak;		/* locate end of score */
	pp = tp->e_bak;				/* locate end of head */

	endtime = pp->e_time;			/* find last time in score */

	if (t_cur > endtime)			/* use max of t_cur, endtime */
		endtime = t_cur;

	rp = peg;				/* point at start of tail */

	while (E_NULL NE rp) {			/* scan each event in the tail */

		ep = rp;			/* update pointers */
		rp = ep->e_fwd;

		ep->e_time = (ep->e_time - t_ins) + endtime;	/* adjust time */
	}

	ep->e_fwd = tp;				/* connect tail to end */
	tp->e_bak = ep;				/* connect end to tail */

	pp->e_fwd  = peg;			/* connect head to tail */
	peg->e_bak = pp;			/* connect tail to head */

	p_cur = ep_adj(pp, 0, t_cur);		/* adjust p_cur */
	p_ctr = p_cur;				/* adjust p_ctr */
	p_bak = ep_adj(p_cur, 0, t_bak);	/* adjust p_bak */
	p_fwd = ep_adj(p_cur, 0, t_fwd);	/* adjust p_fwd */

	insmode = FALSE;
}

/* 
*/

/*
   =============================================================================
	sdcxu() -- score cursor x update processing
   =============================================================================
*/

sdcxu()
{
	if (sdmctl EQ 0) {		/* typewriter up */

		vtcxupd();

	} else if (sdmctl > 0) {	/* menu up */

		vtccol = XTOC(vtxval += cxrate);

		if (vtccol > sdmlim[sdmctl][3])
			vtxval = CTOX(vtccol = sdmlim[sdmctl][3]);
		else if (vtccol < sdmlim[sdmctl][1])
			vtxval = CTOX(vtccol = sdmlim[sdmctl][1]);

	} else {		/* no menu showing */

		cxval += cxrate;

		if (cxval > CXMAX)
			cxval = CXMAX;
		else if (cxval < 0)
			cxval = 0;
	}

	return;
}

/* 
*/

/*
   =============================================================================
	sdcyu() -- score cursor y update processing
   =============================================================================
*/

sdcyu()
{
	if (sdmctl EQ 0) {

		vtcyupd();

	} else if (sdmctl > 0) {	/* menu up */

		vtcrow = YTOR(vtyval += cyrate);

		if (vtcrow < sdmlim[sdmctl][0])
			vtyval = RTOY(vtcrow = sdmlim[sdmctl][0]);
		else if (vtcrow > sdmlim[sdmctl][2])
			vtyval = RTOY(vtcrow = sdmlim[sdmctl][2]);

	} else {		/* no menu showing */

		cyval += cyrate;

		if (cyval > CYMAX)
			cyval = CYMAX;
		else if (cyval < 0)
			cyval = 0;

	}

	return;
}

/* 
*/

/*
   =============================================================================
	scntfld() -- not-in-field data entry key processing
   =============================================================================
*/

short
scntfld(k)
short k;
{
	register short grp, fnc, var;

	if (NOT astat)
		return(FAILURE);

	if (stcrow EQ 18) {		/* group status or mode */

		if (stccol LT 5)
			return(FAILURE);

		grp = (stccol - 5) / 5;		/* group */
		fnc = (stccol - 5) % 5;		/* status or mode */

		if (fnc EQ 0) {			/* status */

			if (k EQ 8) {		/* '-' */

				grpstat[grp] = FALSE;
				dsgstat(grp);
				return(SUCCESS);

			} else if (k EQ 9) {	/* '+' */

				grpstat[grp] = TRUE;
				dsgstat(grp);
				return(SUCCESS);

			} else
				return(FAILURE);
/* 
*/
		} else if (fnc EQ 1) {		/* mode */

			if (k EQ 8) {		/* '-' */

				grpmode[grp] = 0;
				dsgmode(grp);
				return(SUCCESS);

			} else if (k EQ 9) {	/* '+' */

				if (++grpmode[grp] GT 1)
					grpmode[grp] = 2;

				dsgmode(grp);
				return(SUCCESS);

			} else
				return(FAILURE);

		} else
			return(FAILURE);
/* 
*/
	} else if (stcrow EQ 22) {	/* analog variables */

		if (stccol < 5)
			return(FAILURE);

		var = (stccol - 5) / 9;		/* variable */
		fnc = (stccol - 5) % 9;		/* field column */
		grp = abs(angroup) - 1;		/* group */

		if (fnc EQ 0) {			/* mode column */

			if (k EQ 8) {		/* '-' */

				varmode[var][grp] = 0;
				dsvmode(var);
				return(SUCCESS);

			} else if (k EQ 9) {	/* '+' */

				if (++varmode[var][grp] GT 1)
					varmode[var][grp] = 2;

				dsvmode(var);
				return(SUCCESS);
			}
		}
	}

	return(FAILURE);
}

/* 
*/

/*
   =============================================================================
	cp2time() -- check cursor position and convert it to time

	Sets 'ctime' if the cursor x argument, 'cx',  is within the score.
   =============================================================================
*/

short
cp2time(cx)
short cx;
{
	register long ct;

	ct = t_cur + ((long)cx - PCENTER);

	if (ct < 0L)			/* if out of score time range */
		return(FALSE);		/* ... return FALSE */

	ctime = ct;			/* set ctime */
	return(TRUE);			/* ... and return TRUE */
}

/* 
*/

/*
   =============================================================================
	delevts() -- delete record enabled non-note events at current time
   =============================================================================
*/

short
delevts(etp, thetime)
register short etp;
register long thetime;
{
	register struct s_entry *ep, *np;
	register short et;
	register short rc;
	register short dsw;

	rc = FALSE;				/* intialize return code */

	ep = ep_adj(p_cur, 1, thetime);		/* start at left end of chain */

/* 
*/
	while (ep->e_time EQ thetime) {		/* for each event at ctime ... */

		np = ep->e_fwd;			/* get next event pointer */
		et = 0x007F & ep->e_type;	/* get type */
		dsw = oktode(ep);		/* get deletion status */

		if (dsw AND	/* delete event if it's deletable ... */
		    ((etp EQ -1) OR (et EQ etp))) {  /* ... and the right kind */

			rc = TRUE;

			if (-1 NE ehdlist[et])	/* if this is a header event ... */
				eh_rmv(ep, ehdlist[et]);  /* remove from hdr list */

			/* fix up any effected pointers */

			if (p_bak EQ ep)
				p_bak = np;

			if (p_cur EQ ep)
				p_cur = np;

			if (p_ctr EQ ep)
				p_ctr = np;

			if (p_fwd EQ ep)
				p_fwd = np;

			e_del(e_rmv(ep));	/* delete the event */
		}

		ep = np;
	}

	if (rc)				/* if anything was deleted ... */
		sc_refr(t_cur);		/* ... update the display */

	return(rc);
}

/* 
*/

/*
   =============================================================================
	deladat() -- delete analog varaible data
   =============================================================================
*/

short
deladat()
{
	register struct s_entry *ep, *rp;
	register short grp, rc, var, vc, vn;

	rc = FALSE;

	if (stccol < 6)				/* pointing at var ? */
		return(rc);

	vn = (stccol - 6) / 9;			/* variable number */
	vc = (stccol - 6) % 9;			/* variable column */

	ep = ep_adj(p_cur, 1, t_cur);		/* start at left end of chain */

	if (vc EQ 0) {				/* resolution */

		while (t_cur EQ ep->e_time) {

			rp = ep->e_fwd;

			if (EV_ANRS EQ (0x007F & ep->e_type)) {

				grp = 0x000F & ep->e_data1;
				var = 0x000F & (ep->e_data1 >> 4);

				if ((var EQ vn) AND grpstat[grp] AND
				    (2 EQ (ancmsw ? varmode[var][grp] :
				     grpmode[grp]))) {

					/* fix pointers */

					if (p_bak EQ ep)
						p_bak = rp;

					if (p_cur EQ ep)
						p_cur = rp;

					if (p_ctr EQ ep)
						p_ctr = rp;

					if (p_fwd EQ ep)
						p_fwd = rp;

					/* delete event */

					eh_rmv(ep, EH_ANRS);
					e_del(e_rmv(ep));
					rc = TRUE;
				}
			}

			ep = rp;		/* next event */
		}
/* 
*/
	} else if ((vc GE 2) AND (vc LE 6)) {	/* value */

		while (t_cur EQ ep->e_time) {

			rp = ep->e_fwd;

			if (EV_ANVL EQ (0x007F & ep->e_type)) {

				grp = 0x000F & ep->e_data1;
				var = 0x000F & (ep->e_data1 >> 4);

				if ((var EQ vn) AND grpstat[grp] AND
				    (2 EQ (ancmsw ? varmode[var][grp] :
				     grpmode[grp]))) {

					/* fix pointers */

					if (p_bak EQ ep)
						p_bak = rp;

					if (p_cur EQ ep)
						p_cur = rp;

					if (p_ctr EQ ep)
						p_ctr = rp;

					if (p_fwd EQ ep)
						p_fwd = rp;

					e_del(e_rmv(ep));	/* delete */
					rc = TRUE;
				}
			}

			ep = rp;	/* next event */
		}
	}

	if (rc) {			/* refresh the screen if it changed */

		sc_refr(fc_val);
		ds_anrs();
		ds_anvl();
	}

	return(rc);
}

/* 
*/

/*
   =============================================================================
	sdxkey() -- score delete key processing
   =============================================================================
*/

sdxkey()
{
	register short col, grp, sect;
	register struct s_entry *ep, *rp;

	if (NOT astat)			/* only do when key goes down */
		return;

	if (clkrun)			/* ... and not while clock is running */
		return;

	if (scmctl NE -1)		/* ... and area 1 menu is down */
		return;
/* 
*/
	if (sdmctl EQ 4) {		/* section menu is up */

		if (((vtcrow EQ 19) OR (vtcrow EQ 20)) AND
		    (vtccol GE 24)) {

			if (NOT recsw)
				return;

			/* set up to delete a section marker */

			sect = ((vtccol - 24) >> 2) + ((vtcrow EQ 20) ? 10 : 0);
			col  = (vtccol - 24) & 3;

			switch (col) {

			case 0:		/* delete begin section */


				if (E_NULL NE (ep = seclist[curscor][sect])) {

					/* fix pointers */

					rp = ep->e_fwd;

					if (p_bak EQ ep)
						p_bak = rp;

					if (p_cur EQ ep)
						p_cur = rp;

					if (p_ctr EQ ep)
						p_ctr = rp;

					if (p_fwd EQ ep)
						p_fwd = rp;

					eh_rmv(ep, EH_SBGN);	/* delete */
					e_del(e_rmv(ep));
					seclist[curscor][sect] = E_NULL;
					sdmenu(-1);		/* take down menu */
					sc_refr(fc_val);	/* refresh screen */
				}

				break;
/* 
*/
			case 1:		/* section number */
			case 2:
				break;

			case 3:		/* delete end section */

				if (E_NULL NE
				   (ep = ehfind(EV_SEND, -1L, sect, -1))) {

					/* fix pointers */

					rp = ep->e_fwd;

					if (p_bak EQ ep)
						p_bak = rp;

					if (p_cur EQ ep)
						p_cur = rp;

					if (p_ctr EQ ep)
						p_ctr = rp;

					if (p_fwd EQ ep)
						p_fwd = rp;

					eh_rmv(ep, EH_SEND);	/* delete */
					e_del(e_rmv(ep));
					sdmenu(-1);		/* take down menu */
					sc_refr(fc_val);	/* refresh screen */
				}

				break;
			}

			return;

/* 
*/
		} else if ((vtcrow EQ 23) AND
			   (vtccol GE 28) AND (vtccol LE 62)) {

			/* group map entry */

			col = vtccol - 28;

			if (2 EQ (col % 3))
				return;

			grp = col / 3;
			grptmap[grp] = -1;
			dsgtme(grp);
			return;
		}
	}

	if (NOT recsw)	/* do only if recording */
		return;

	if ((cyval GE 14) AND (cyval LE 223)) {			/* notes */

		delnote();		/* delete a note */

	} else if ((cyval GE 224) AND (cyval LE 237)) {		/* new data */

		if (cp2time(cxval))
			delevts(-1, ctime);	/* delete all events at cursor */

/* 
*/

	} else if (stcrow EQ 17) {

		if ((stccol GE 11) AND (stccol LE 12))
			delevts(EV_ASGN, t_cur);		/* assignment */
		else if (stccol EQ 19)
			delevts(EV_TUNE, t_cur);		/* tuning */
		else if ((stccol GE 27) AND (stccol LE 29))
			delevts(EV_TMPO, t_cur);		/* tempo */
		else if ((stccol GE 36) AND (stccol LE 38))
			delevts(EV_INTP, t_cur);		/* interpolation */
		else if ((stccol GE 40) AND (stccol LE 43))
			delevts(EV_STOP, t_cur);		/* stop */
		else if ((stccol GE 45) AND (stccol LE 48))
			delevts(EV_NEXT, t_cur);		/* next */
		else if ((stccol GE 50) AND (stccol LE 55))
			delevts(EV_PNCH, t_cur);		/* punch in/out */

	} else if ((stcrow EQ 18) AND
		   (0 EQ ((stccol - 5) % 5))) {			/* group status */

		delevts(EV_GRP, t_cur);

	} else if ((stcrow EQ 18) AND
		    ((2 EQ ((stccol - 5) % 5)) OR
		     (3 EQ ((stccol - 5) % 5)))) {		/* instrument */

		delevts(EV_INST, t_cur);

	} else if (stcrow EQ 19) {				/* transposition */

		delevts(EV_TRNS, t_cur);

	} else if ((stcrow EQ 20) AND (stccol GE 6) AND
		   (0 EQ ((stccol - 6) % 5))) {			/* dynamics */

		delevts(EV_DYN, t_cur);

	} else if ((stcrow EQ 20) AND (stccol GE 6) AND
		   (2 EQ ((stccol - 6) % 5))) {			/* location */

		delevts(EV_LOCN, t_cur);

	} else if (stcrow EQ 23) {				/* analog */

		deladat();
/* 
*/
	} else if (stcrow EQ 24) {				/* score */

		if (stccol LE 4) {

			/* clear score */

			if (insmode)		/* cancel insert mode */
				icancel();

			sc_clr(curscor);	/* clear score itself */
			selscor(curscor);	/* refresh the display */

		} else if ((stccol GE 10) AND (stccol LE 25)) {

			/* clear score name */

			memset(scname[curscor], ' ', 16);
			dswin(13);
		}
	}
}

/* 
*/

/*
   =============================================================================
	sdmkey() -- score display menu key processor
   =============================================================================
*/

sdmkey()
{
	if (astat) {

		if (insmode)
			icancel();
	}

	stdmkey();
}

/* 
*/

/*
   =============================================================================
	scfield() -- setup field routines for the score display
   =============================================================================
*/

scfield()
{
	curset(&sc_flds);
}
