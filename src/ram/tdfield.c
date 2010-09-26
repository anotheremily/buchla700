/*
   =============================================================================
	tdfield.c -- MIDAS-VII tuning display field processing and cursor motion
	Version 18 -- 1989-11-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "graphdef.h"
#include "charset.h"
#include "hwdefs.h"
#include "curpak.h"

#include "midas.h"
#include "tundsp.h"

extern	short	select(), whatbox(), nokey(), stdctp0();
extern	short	cxkstd(), cykstd(), stdmkey(), stddkey();

extern	short	et_null(), ef_null(), rd_null(), nd_null();
extern	short	et_ttab(), ef_ttab(), rd_ttab(), nd_ttab();
extern	short	et_tval(), ef_tval(), rd_tval(), nd_tval();
extern	short	et_tpch(), ef_tpch(), rd_tpch(), nd_tpch();

extern	char	*memset();

extern	short	cxval, cyval, cxrate, cyrate;
extern	short	stcrow, stccol, submenu, vtcrow, vtccol, vtwrow, vtwcol;
extern	short	vtxval, vtyval;

extern	short	crate1[];

extern	struct	selbox	*csbp;
extern	struct	selbox	tdboxes[];

extern	short	tunmod, tdnamsw;

/* forward references */

short	tdcxupd(), tdcyupd();

/* 
*/

struct	fet	td_fet1[] = {		/* field definitions */

	{ 0,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 1,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 2,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 3,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 4,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 5,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 6,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 7,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 8,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 9,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{10,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{11,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{12,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{13,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{14,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{15,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{16,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{17,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{18,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{19,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{20,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{21,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{22,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{23,  6, 10,  0, et_tpch, ef_tpch, rd_tpch, nd_tpch},

	{ 0, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 1, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 2, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 3, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 4, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 5, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 6, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 7, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 8, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 9, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{10, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{11, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{12, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{13, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{14, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{15, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{16, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{17, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{18, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{19, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{20, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{21, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{22, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{23, 16, 20,  1, et_tpch, ef_tpch, rd_tpch, nd_tpch},
/* 
*/
	{ 0, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 1, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 2, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 3, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 4, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 5, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 6, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 7, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 8, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 9, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{10, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{11, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{12, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{13, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{14, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{15, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{16, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{17, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{18, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{19, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{20, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{21, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{22, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{23, 26, 30,  2, et_tpch, ef_tpch, rd_tpch, nd_tpch},

	{ 0, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 1, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 2, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 3, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 4, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 5, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 6, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 7, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 8, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 9, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{10, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{11, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{12, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{13, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{14, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{15, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{16, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{17, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{18, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{19, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{20, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{21, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{22, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{23, 36, 40,  3, et_tpch, ef_tpch, rd_tpch, nd_tpch},
/* 
*/
	{ 0, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 1, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 2, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 3, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 4, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 5, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 6, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 7, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 8, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 9, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{10, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{11, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{12, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{13, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{14, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{15, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{16, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{17, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{18, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{19, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{20, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{21, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{22, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{23, 47, 51,  4, et_tpch, ef_tpch, rd_tpch, nd_tpch},

	{ 0, 58, 62,  5, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 1, 58, 62,  5, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 2, 58, 62,  5, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 3, 58, 62,  5, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 4, 58, 62,  5, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 5, 58, 62,  5, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 6, 58, 62,  5, et_tpch, ef_tpch, rd_tpch, nd_tpch},
	{ 7, 58, 62,  5, et_tpch, ef_tpch, rd_tpch, nd_tpch},

	{18, 58, 62,  6, et_tval, ef_tval, rd_tval, nd_tval},

	{24,  7, 38,  7, et_null, ef_null, rd_null, nd_null},
	{24, 61, 61,  6, et_ttab, ef_ttab, rd_ttab, nd_ttab},

	{ 0,  0,  0, 0x0000, FN_NULL, FN_NULL, FN_NULL, FN_NULL}
};

/* 
*/

short	tdbox[][8] = {		/* display box parameters */

	{  0,   0,  95, 335, TCFBX00, TCBBX00,  0,  1},	/* 0 */
	{ 96,   0, 175, 335, TCFBX01, TCBBX01,  0, 13},	/* 1 */
	{176,   0, 255, 335, TCFBX02, TCBBX02,  0, 23},	/* 2 */
	{256,   0, 335, 335, TCFBX03, TCBBX03,  0, 33},	/* 3 */
	{336,   0, 423, 335, TCFBX04, TCBBX04,  0, 43},	/* 4 */
	{424,   0, 511, 118, TCFBX05, TCBBX05,  0, 54},	/* 5 */
	{424, 119, 511, 349, TCFBX06, TCBBX06,  9, 54},	/* 6 */
	{  0, 336, 423, 349, TCFBX07, TCBBX07, 24,  1}	/* 7 */
};

char	*tdbxlb[] = {		/* display box labels */

	"",		/* 0 */
	"",		/* 1 */
	"",		/* 2 */
	"",		/* 3 */
	"",		/* 4 */
	"",		/* 5 */
	"",		/* 6 */
	"Name:"		/* 7 */
};

/* 
*/

struct curpak td_flds = {

	stdctp0,	/* curtype */
	nokey,		/* premove */
	nokey,		/* pstmove */
	cxkstd,		/* cx_key */
	cykstd,		/* cy_key */
	tdcxupd,	/* cx_upd */
	tdcyupd,	/* cy_upd */
	nokey,		/* xy_up */
	nokey,		/* xy_dn */
	nokey,		/* x_key */
	select,		/* e_key */
	stdmkey,	/* m_key */
	stddkey,	/* d_key */
	nokey,		/* not_fld */
	td_fet1,	/* curfet */
	tdboxes,	/* csbp */
	crate1,		/* cratex */
	crate1,		/* cratey */
	CT_TEXT,	/* cmtype */
	TDCURX,		/* cxval */
	TDCURY		/* cyval */
};

/* 
*/

/*
   =============================================================================
	tdcyupd() -- update cursor y location
   =============================================================================
*/

tdcyupd()
{
	register short pval, vh, vl;

	if (tdnamsw) {	/* see if we're in the menu area */

		vtcrow = YTOR(vtyval += cyrate);

		if (vtcrow > (vtwrow + 2))
			vtyval = RTOY(vtcrow = vtwrow + 2);
		else if (vtcrow < vtwrow)
			vtyval = RTOY(vtcrow = vtwrow);

	} else {		/* just moving the cursor */

		cyval += cyrate;

		if (cyval GT (CYMAX - 1))
			cyval = CYMAX - 1;
		else if (cyval LT 1)
			cyval = 1;
	}
}

/* 
*/

/*
   =============================================================================
	tdcxupd() -- update cursor x location
   =============================================================================
*/

tdcxupd()
{

	if (tdnamsw) {	/* see if we're in the menu area */

		vtccol = XTOC(vtxval += cxrate);

		if (vtccol > (vtwcol + 29))
			vtxval = CTOX(vtccol = vtwcol + 29);
		else if (vtccol < vtwcol)
			vtxval = CTOX(vtccol = vtwcol);

	} else {			/* just moving the cursor */

		cxval += cxrate;

		if (cxval GT (CXMAX- 1))
			cxval = CXMAX - 1;
		else if (cxval LT 1)
			cxval = 1;
	}
}

/* 
*/

/*
   =============================================================================
	tdfield() -- setup field routines for the tuning editor
   =============================================================================
*/

tdfield()
{
	tdnamsw = FALSE;
	submenu = FALSE;

	curset(&td_flds);
}
