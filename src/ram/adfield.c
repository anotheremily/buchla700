/*
   =============================================================================
	adfield.c -- assignment display field processing and cursor motion
	Version 24 -- 1989-11-15 -- D.N. Lynx Crowe
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
#include "asgdsp.h"

extern	short	select(), whatbox(), nokey(), stdctp4();
extern	short	cxkstd(), cykstd(), stdmkey(), stddkey();

extern	short	et_null(), ef_null(), rd_null(), nd_null();
extern	short	et_atab(), ef_atab(), rd_atab(), nd_atab();
extern	short	et_aopt(), ef_aopt(), rd_aopt(), nd_aopt();
extern	short	et_avgr(), ef_avgr(), rd_avgr(), nd_avgr();
extern	short	et_accn(), ef_accn(), rd_accn(), nd_accn();
extern	short	et_agpt(), ef_agpt(), rd_agpt(), nd_agpt();
extern	short	et_agch(), ef_agch(), rd_agch(), nd_agch();
extern	short	et_ains(), ef_ains(), rd_ains(), nd_ains();
extern	short	et_adyn(), ef_adyn(), rd_adyn(), nd_adyn();
extern	short	et_atun(), ef_atun(), rd_atun(), nd_atun();
extern	short	et_aprg(), ef_aprg(), rd_aprg(), nd_aprg();
extern	short	et_aint(), ef_aint(), rd_aint(), nd_aint();
extern	short	et_arat(), ef_arat(), rd_arat(), nd_arat();
extern	short	et_adep(), ef_adep(), rd_adep(), nd_adep();

extern	short	vcputsv();

extern	char	*memset();

extern	short	adnamsw;
extern	short	asgmod;
extern	short	astat;
extern	short	cmtype;
extern	short	cmfirst;
extern	short	curasg;
extern	short	curhold;
extern	short	curmop;
extern	short	curslim;
extern	short	cxval;
extern	short	cyval;
extern	short	cxrate;
extern	short	cyrate;
extern	short	hcwval;
extern	short	hitbox;
extern	short	hitcx;
extern	short	hitcy;
extern	short	stccol;
extern	short	stcrow;
extern	short	thcwval;
extern	short	tvcwval;
extern	short	vcwval;
extern	short	vtccol;
extern	short	vtcrow;
extern	short	vtwcol;
extern	short	vtwrow;
extern	short	vtxval;
extern	short	vtyval;

extern	short	crate1[];

extern	unsigned *asgob;

extern	struct	asgent	asgtab[];

extern	struct	selbox	adboxes[];
extern	struct	selbox	*csbp;

extern	short	grp2prt[12][2];
extern	short	key2grp[];
extern	short	mctlnum[];
extern	short	vce2grp[];

/* forward references */

short	adcxupd(), adcyupd(), ad_xkey();

/* 
*/

struct	fet	ad_fet1[] = {		/* field definitions */

	{ 0,  9, 10, 0x0000, et_atab, ef_atab, rd_atab, nd_atab},

	{ 1,  1, 10, 0x0000, et_null, ef_null, rd_null, nd_null},

	{ 4, 10, 10, 0x0002, et_aopt, ef_aopt, rd_aopt, nd_aopt},

	{ 3,  9, 10, 0x0003, et_aprg, ef_aprg, rd_aprg, nd_aprg},

	{ 4, 15, 16, 0x0004, et_avgr, ef_avgr, rd_avgr, nd_avgr},
	{ 5, 15, 16, 0x0104, et_avgr, ef_avgr, rd_avgr, nd_avgr},
	{ 6, 15, 16, 0x0204, et_avgr, ef_avgr, rd_avgr, nd_avgr},
	{ 7, 15, 16, 0x0304, et_avgr, ef_avgr, rd_avgr, nd_avgr},
	{ 8, 15, 16, 0x0404, et_avgr, ef_avgr, rd_avgr, nd_avgr},
	{ 9, 15, 16, 0x0504, et_avgr, ef_avgr, rd_avgr, nd_avgr},
	{ 4, 21, 22, 0x0604, et_avgr, ef_avgr, rd_avgr, nd_avgr},
	{ 5, 21, 22, 0x0704, et_avgr, ef_avgr, rd_avgr, nd_avgr},
	{ 6, 21, 22, 0x0804, et_avgr, ef_avgr, rd_avgr, nd_avgr},
	{ 7, 21, 22, 0x0904, et_avgr, ef_avgr, rd_avgr, nd_avgr},
	{ 8, 21, 22, 0x0A04, et_avgr, ef_avgr, rd_avgr, nd_avgr},
	{ 9, 21, 22, 0x0B04, et_avgr, ef_avgr, rd_avgr, nd_avgr},

	{ 5, 35, 36, 0x0005, et_accn, ef_accn, rd_accn, nd_accn},
	{ 6, 35, 36, 0x0105, et_accn, ef_accn, rd_accn, nd_accn},
	{ 7, 35, 36, 0x0205, et_accn, ef_accn, rd_accn, nd_accn},
	{ 8, 35, 36, 0x0305, et_accn, ef_accn, rd_accn, nd_accn},

	{ 4, 46, 46, 0x0006, et_agpt, ef_agpt, rd_agpt, nd_agpt},
	{ 5, 46, 46, 0x0106, et_agpt, ef_agpt, rd_agpt, nd_agpt},
	{ 6, 46, 46, 0x0206, et_agpt, ef_agpt, rd_agpt, nd_agpt},
	{ 7, 46, 46, 0x0306, et_agpt, ef_agpt, rd_agpt, nd_agpt},
	{ 8, 46, 46, 0x0406, et_agpt, ef_agpt, rd_agpt, nd_agpt},
	{ 9, 46, 46, 0x0506, et_agpt, ef_agpt, rd_agpt, nd_agpt},
	{ 4, 59, 59, 0x0606, et_agpt, ef_agpt, rd_agpt, nd_agpt},
	{ 5, 59, 59, 0x0706, et_agpt, ef_agpt, rd_agpt, nd_agpt},
	{ 6, 59, 59, 0x0806, et_agpt, ef_agpt, rd_agpt, nd_agpt},
	{ 7, 59, 59, 0x0906, et_agpt, ef_agpt, rd_agpt, nd_agpt},
	{ 8, 59, 59, 0x0A06, et_agpt, ef_agpt, rd_agpt, nd_agpt},
	{ 9, 59, 59, 0x0B06, et_agpt, ef_agpt, rd_agpt, nd_agpt},

	{ 4, 48, 49, 0x0006, et_agch, ef_agch, rd_agch, nd_agch},
	{ 5, 48, 49, 0x0106, et_agch, ef_agch, rd_agch, nd_agch},
	{ 6, 48, 49, 0x0206, et_agch, ef_agch, rd_agch, nd_agch},
	{ 7, 48, 49, 0x0306, et_agch, ef_agch, rd_agch, nd_agch},
	{ 8, 48, 49, 0x0406, et_agch, ef_agch, rd_agch, nd_agch},
	{ 9, 48, 49, 0x0506, et_agch, ef_agch, rd_agch, nd_agch},
	{ 4, 61, 62, 0x0606, et_agch, ef_agch, rd_agch, nd_agch},
	{ 5, 61, 62, 0x0706, et_agch, ef_agch, rd_agch, nd_agch},
	{ 6, 61, 62, 0x0806, et_agch, ef_agch, rd_agch, nd_agch},
	{ 7, 61, 62, 0x0906, et_agch, ef_agch, rd_agch, nd_agch},
	{ 8, 61, 62, 0x0A06, et_agch, ef_agch, rd_agch, nd_agch},
	{ 9, 61, 62, 0x0B06, et_agch, ef_agch, rd_agch, nd_agch},

	{ 4, 41, 42, 0x0006, et_ains, ef_ains, rd_ains, nd_ains},
	{ 5, 41, 42, 0x0106, et_ains, ef_ains, rd_ains, nd_ains},
	{ 6, 41, 42, 0x0206, et_ains, ef_ains, rd_ains, nd_ains},
	{ 7, 41, 42, 0x0306, et_ains, ef_ains, rd_ains, nd_ains},
	{ 8, 41, 42, 0x0406, et_ains, ef_ains, rd_ains, nd_ains},
	{ 9, 41, 42, 0x0506, et_ains, ef_ains, rd_ains, nd_ains},
	{ 4, 54, 55, 0x0606, et_ains, ef_ains, rd_ains, nd_ains},
	{ 5, 54, 55, 0x0706, et_ains, ef_ains, rd_ains, nd_ains},
	{ 6, 54, 55, 0x0806, et_ains, ef_ains, rd_ains, nd_ains},
	{ 7, 54, 55, 0x0906, et_ains, ef_ains, rd_ains, nd_ains},
	{ 8, 54, 55, 0x0A06, et_ains, ef_ains, rd_ains, nd_ains},
	{ 9, 54, 55, 0x0B06, et_ains, ef_ains, rd_ains, nd_ains},

	{ 4, 44, 44, 0x0006, et_adyn, ef_adyn, rd_adyn, nd_adyn},
	{ 5, 44, 44, 0x0106, et_adyn, ef_adyn, rd_adyn, nd_adyn},
	{ 6, 44, 44, 0x0206, et_adyn, ef_adyn, rd_adyn, nd_adyn},
	{ 7, 44, 44, 0x0306, et_adyn, ef_adyn, rd_adyn, nd_adyn},
	{ 8, 44, 44, 0x0406, et_adyn, ef_adyn, rd_adyn, nd_adyn},
	{ 9, 44, 44, 0x0506, et_adyn, ef_adyn, rd_adyn, nd_adyn},
	{ 4, 57, 57, 0x0606, et_adyn, ef_adyn, rd_adyn, nd_adyn},
	{ 5, 57, 57, 0x0706, et_adyn, ef_adyn, rd_adyn, nd_adyn},
	{ 6, 57, 57, 0x0806, et_adyn, ef_adyn, rd_adyn, nd_adyn},
	{ 7, 57, 57, 0x0906, et_adyn, ef_adyn, rd_adyn, nd_adyn},
	{ 8, 57, 57, 0x0A06, et_adyn, ef_adyn, rd_adyn, nd_adyn},
	{ 9, 57, 57, 0x0B06, et_adyn, ef_adyn, rd_adyn, nd_adyn},

	{ 5, 10, 10, 0x0009, et_atun, ef_atun, rd_atun, nd_atun},

	{ 7,  9, 10, 0x000A, et_aint, ef_aint, rd_aint, nd_aint},
	{ 8,  9, 10, 0x000A, et_arat, ef_arat, rd_arat, nd_arat},
	{ 9,  9, 10, 0x000A, et_adep, ef_adep, rd_adep, nd_adep},

	{ 0,  0,  0, 0x0000, FN_NULL, FN_NULL, FN_NULL, FN_NULL}
};

/* 
*/

short	adbox[][8] = {		/* display box parameters */

	{  1,   1,  94,  27, ACFBX00, ACBBX00,  0,  1},	/* 0 */
	{  1,  28,  94,  40, ACFBX01, ACBBX01,  2,  1},	/* 1 */
	{  1,  56,  94,  68, ACFBX02, ACBBX02,  4,  1},	/* 2 */
	{  1,  42,  94,  54, ACFBX03, ACBBX03,  3,  1},	/* 3 */
	{ 96,   1, 190, 138, ACFBX04, ACBBX04,  0, 13},	/* 4 */
	{192,   1, 302, 138, ACFBX05, ACBBX05,  0, 25},	/* 5 */
	{304,   1, 510, 138, ACFBX06, ACBBX06,  0, 39},	/* 6 */
	{  1, 140, 510, 348, ACFBX07, ACBBX07, 10,  1},	/* 7 */
	{  1,  70,  38,  82, ACFBX08, ACBBX08,  5,  1},	/* 8 */
	{ 40,  70,  94,  82, ACFBX09, ACBBX09,  5,  6},	/* 9 */
	{  1,  84,  94, 138, ACFBX10, ACBBX10,  6,  1}	/* 10 */
};

char	*adbxlb[] = {		/* display box labels */

	"Asgnmnt",			/* 0 */
	"Stor/Fetch",			/* 1 */
	"MIDI out",			/* 2 */
	"PrgChng",			/* 3 */
	"Assignment",			/* 4 */
	"Assignmnt of"	,		/* 5 */
	"Assignmnt of Instruments,",	/* 6 */
	"G Assignment of Keys to Groups for MIDI Port 1               G", /* 7 */
	"",				/* 8 */
	"Tun",				/* 9 */
	"PhaseShift"			/* 10 */
};

/* 
*/

struct curpak ad_flds = {

	stdctp4,	/* curtype */
	nokey,		/* premove */
	nokey,		/* pstmove */
	cxkstd,		/* cx_key */
	cykstd,		/* cy_key */
	adcxupd,	/* cx_upd */
	adcyupd,	/* cy_upd */
	nokey,		/* xy_up */
	nokey,		/* xy_dn */
	ad_xkey,	/* x_key */
	select,		/* e_key */
	stdmkey,	/* m_key */
	stddkey,	/* d_key */
	nokey,		/* not_fld */
	ad_fet1,	/* curfet */
	adboxes,	/* csbp */
	crate1,		/* cratex */
	crate1,		/* cratey */
	CT_GRAF,	/* cmtype */
	CTOX(0),	/* cxval */
	RTOY(9)		/* cyval */
};

/* 
*/

/*
   =============================================================================
	adcyupd() -- update cursor y location
   =============================================================================
*/

adcyupd()
{
	if (adnamsw) {

		vtcrow = YTOR(vtyval += cyrate);

		if (vtcrow > (vtwrow + 2))
			vtyval = RTOY(vtcrow = vtwrow + 2);
		else if (vtcrow < vtwrow)
			vtyval = RTOY(vtcrow = vtwrow);

	} else {

		cyval += cyrate;

		if (cyval GT (CYMAX - 1))
			cyval = CYMAX - 1;
		else if (cyval LT 1)
			cyval = 1;
	}
}

/*
   =============================================================================
	adcxupd() -- update cursor x location
   =============================================================================
*/

adcxupd()
{
	if (adnamsw) {

		vtccol = XTOC(vtxval += cxrate);

		if (vtccol > (vtwcol + 29))
			vtxval = CTOX(vtccol = vtwcol + 29);
		else if (vtccol < vtwcol)
			vtxval = CTOX(vtccol = vtwcol);

	} else {

		cxval += cxrate;

		if (cxval GT (CXMAX - 1))
			cxval = CXMAX - 1;
		else if (cxval LT 1)
			cxval = 1;
	}
}

/* 
*/

/*
   =============================================================================
	ad_xkey() -- process the X key
   =============================================================================
*/

short
ad_xkey(nn)
short nn;
{
	register short row, col, grp, i;

	if (astat) {

		if (whatbox()) {

			row = hitcy / 14;
			col = hitcx >> 3;

			switch (hitbox) {

			case 0:		/* name */

				if (row EQ 1) {

					memset(asgtab[curasg].a_name, ' ', 16);
					asgmod = TRUE;
					adswin(0);
					return(SUCCESS);

				} else
					return(FAILURE);
			case 2:		/* MIDI output port */

				if ((col GE 1) AND (col LE 10)) {

					curmop = 0;
					adswin(hitbox);
					modasg();
					return(SUCCESS);
				}

				break;
/* 
*/
			case 4:		/* voice to group */

				if (row LT 4)
					break;

				if ((col GE 13) AND (col LE 16)) {

					grp = row - 4;
					vce2grp[grp] = -1;
					adswin(hitbox);
					modasg();
					return(SUCCESS);

				} else if ((col GE 19) AND (col LE 22)) {

					grp = row + 2;
					vce2grp[grp] = -1;
					adswin(hitbox);
					modasg();
					return(SUCCESS);
				}

				break;

			case 5:		/* controller number */

				if ((row LT 5) OR (row GT 8))
					break;

				if ((col EQ 35) OR (col EQ 36)) {

					mctlnum[row - 5] = -1;
					adswin(hitbox);
					modasg();
					return(SUCCESS);
				}

				break;
/* 
*/
			case 6:		/* port or channel */

				if (row LT 4)
					break;

				if (col EQ 46 ) {

					/* port */

					grp = row - 4;
					grp2prt[grp][0] = 0;

					for (i = 0; i < 88; i++)
						key2grp[i] &= ~(0x0001 << grp);

					adswin(hitbox);
					drawk2g(grp);
					modasg();
					return(SUCCESS);

				} else if ((col EQ 48) OR (col EQ 49)) {

					/* channel */

					grp = row - 4;
					grp2prt[grp][1] = -1;
					adswin(hitbox);
					modasg();
					return(SUCCESS);
/* 
*/
				} else if (col EQ 59) {

					/* port */

					grp = row + 2;
					grp2prt[grp][0] = 0;

					for (i = 0; i < 88; i++)
						key2grp[i] &= ~(0x0001 << grp);

					adswin(hitbox);
					drawk2g(grp);
					modasg();
					return(SUCCESS);

				} else if ((col EQ 61) OR (col EQ 62)) {

					/* channel */

					grp = row + 2;
					grp2prt[grp][1] = -1;
					adswin(hitbox);
					modasg();
					return(SUCCESS);
				}

				break;
			}
		}
	}

	return(FAILURE);
}

/* 
*/

/*
   =============================================================================
	adfield() -- setup field routines for the assignment editor
   =============================================================================
*/

adfield()
{
	curslim = 140;

	curset(&ad_flds);
}
