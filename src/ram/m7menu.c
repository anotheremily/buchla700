/*
   =============================================================================
	m7menu.c -- MIDAS main menu functions
	Version 42 -- 1989-12-19 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "hwdefs.h"
#include "memory.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "fields.h"
#include "graphdef.h"
#include "curpak.h"

#include "midas.h"
#include "menu.h"
#include "libdsp.h"

#define	DENSITY		1		/* double density */

#define	M_COPY		8		/* "copy" text index in mtexts[] */
#define	M_FORM		10		/* "format" text index in mtexts[] */

#define	VER_ROW		2		/* version message row */
#define	VER_COL		51		/* version message column */

#if	DEBUGIT
extern	short	debugsw;

short	debugm7 = 1;
#endif

extern	char	m7verms[], bfs[], loadedf[][8];

extern	struct	selbox	*csbp, *curboxp;

extern	short	(*point)();

extern	short	nokey(), select(), cxkstd(), cykstd();
extern	short	cxgen(), cygen(), stdctp2();

extern	unsigned	exp_c();

extern	short	ndisp, runit, curslim, sgcsw, cxval, cyval, stcrow, stccol;
extern	short	crate1[], defect, dsp_ok;

extern	struct	octent	*mdoct;

extern	unsigned	*menuob, *obj0, vi_ctl;

static char	ml01[61], ml02[61], ml03[61], ml04[61], ml05[61];

short	mdselbx(), mdpoint();

/* 
*/

static char mlc01[] = {1, 0xBA,  58, 0xB1,  1, 0xBB,  -1};
static char mlc02[] = {1, 0xB6,  29, 0xB1,  1, 0xB7,  28, 0xB1,  1, 0xB4,  -1};
static char mlc03[] = {1, 0xB6,  29, 0xB1,  1, 0xB2,  28, 0xB1,  1, 0xB4,  -1};
static char mlc04[] = {1, 0xB6,  29, 0xB1,  1, 0xB5,  28, 0xB1,  1, 0xB4,  -1};
static char mlc05[] = {1, 0xB9,  58, 0xB1,  1, 0xB8,  -1};

static char	*mtexts[] = {

	/* 0 */		"MIDAS VII      for the Buchla 700      Version",
	/* 1 */		"Librarian",
	/* 2 */		"Patch Editor",
	/* 3 */		"Score editor",
	/* 4 */		"Sequence Editor",
	/* 5 */		"Instrument designer",
	/* 6 */		"Initialize system",
	/* 7 */		"Waveshape editor",
	/* 8 */		"Write program to disk",
	/* 9 */		"Tuning tables",
	/* 10 */	"Format disk",
	/* 11 */	"Assignment tables",
	/* 12 */	"Diagnostics",
	/* 13 */	"created by Buchla and Associates of Berkeley, California",
};

/* 
*/

static short mlintab[][4] = {

	{ 18,  26,  18, 322},

	{ 21,  26,  21,  42},
	{ 21,  54,  21,  84},
	{ 21,  96,  21, 126},
	{ 21, 138,  21, 168},
	{ 21, 180,  21, 210},
	{ 21, 222,  21, 252},
	{ 21, 264,  21, 294},
	{ 21, 306,  21, 322},

	{258,  54, 258,  84},
	{258,  96, 258, 126},
	{258, 138, 258, 168},
	{258, 180, 258, 210},
	{258, 222, 258, 252},
	{258, 264, 258, 294},

	{261,  54, 261,  84},
	{261,  96, 261, 126},
	{261, 138, 261, 168},
	{261, 180, 261, 210},
	{261, 222, 261, 252},
	{261, 264, 261, 294},

	{490,  26, 490,  42},
	{490,  54, 490,  84},
	{490,  96, 490, 126},
	{490, 138, 490, 168},
	{490, 180, 490, 210},
	{490, 222, 490, 252},
	{490, 264, 490, 294},
	{490, 306, 490, 322},

	{493,  26, 493, 322},

	{ -1,  -1,  -1,  -1}
};

/* 
*/

static short	mrowcol[][2] = {

	{ 2,  4},	/* 0 */
	{ 4,  4},	/* 1 */
	{ 4, 34},	/* 2 */
	{ 7,  4},	/* 3 */
	{ 7, 34},	/* 4 */
	{10,  4},	/* 5 */
	{10, 34},	/* 6 */
	{13,  4},	/* 7 */
	{13, 34},	/* 8 */
	{16,  4},	/* 9 */
	{16, 34},	/* 10 */
	{19,  4},	/* 11 */
	{19, 34},	/* 12 */
	{22,  4}	/* 13 */
};

struct	selbox	mdboxes[] = {	/* main menu box definitions */

	{ 22,  53, 257,  87,  0, mdselbx},	/* 0 */
	{262,  53, 489,  87,  1, mdselbx},	/* 1 */
	{ 22,  95, 257, 129,  2, mdselbx},	/* 2 */
	{262,  95, 489, 129,  3, mdselbx},	/* 3 */
	{ 22, 137, 257, 171,  4, mdselbx},	/* 4 */
	{262, 137, 489, 171,  5, mdselbx},	/* 5 */
	{ 22, 179, 257, 213,  6, mdselbx},	/* 6 */
	{262, 179, 489, 213,  7, mdselbx},	/* 7 */
	{ 22, 221, 257, 255,  8, mdselbx},	/* 8 */
	{262, 221, 489, 255,  9, mdselbx},	/* 9 */
	{ 22, 263, 257, 297, 10, mdselbx},	/* 10 */
	{262, 263, 489, 297, 11, mdselbx},	/* 11 */
	{  0,   0,   0,   0,  0, FN_NULL}	/* end of table */
};

/* 
*/

short	menupal[16][3] = {	/* menu display color palette */

	{0, 0, 0},	/* 0 */
	{2, 1, 0},	/* 1	MCURSOR		cursor color */
	{0, 0, 0},	/* 2	MBKGRND		background color */
	{3, 0, 2},	/* 3 */
	{2, 1, 0},	/* 4	MBUCHLA		"Created by ..." color */
	{2, 1, 0},	/* 5	MTITLE		title color */
	{0, 1, 0},	/* 6 */
	{2, 0, 0},	/* 7	MHILITE		hilite color */
	{2, 2, 2},	/* 8	MLFILES		file name color */
	{0, 0, 0},	/* 9 */
	{1, 1, 2},	/* 10	MLINES		outline color */
	{2, 3, 3},	/* 11	MBUTTON		button legend color */
	{3, 1, 1},	/* 12 */
	{3, 3, 0},	/* 13 */
	{0, 3, 3},	/* 14 */
	{0, 0, 3}	/* 15 */
};

short	blakpal[16][3] = {		/* black palette */

	{0, 0, 0},	/* 0 */
	{0, 0, 0},	/* 1 */
	{0, 0, 0},	/* 2 */
	{0, 0, 0},	/* 3 */
	{0, 0, 0},	/* 4 */
	{0, 0, 0},	/* 5 */
	{0, 0, 0},	/* 6 */
	{0, 0, 0},	/* 7 */
	{0, 0, 0},	/* 8 */
	{0, 0, 0},	/* 9 */
	{0, 0, 0},	/* 10 */
	{0, 0, 0},	/* 11 */
	{0, 0, 0},	/* 12 */
	{0, 0, 0},	/* 13 */
	{0, 0, 0},	/* 14 */
	{0, 0, 0}	/* 15 */
};

/* 
*/

short	mfrc[][2] = {	/* file name row and column  by load type LT_??? */

	{20,  6},	/* assignments */
	{11,  6},	/* lo orchestra */
	{11, 20},	/* hi orchestra */
	{ 8,  6},	/* score */
	{17,  6},	/* tunings */
	{14,  6},	/* waveshapes */
	{ 5, 36},	/* patches */
	{ 8, 36}	/* sequences */
};

/* 
*/

struct curpak m7_flds = {	/* main menu cursor package definitions */

	stdctp2,		/* curtype */
	nokey,			/* premove */
	nokey,			/* pstmove */
	cxkstd,			/* cx_key */
	cykstd,			/* cy_key */
	cxgen,			/* cx_upd */
	cygen,			/* cy_upd */
	nokey,			/* xy_up */
	nokey,			/* xy_dn */
	nokey,			/* x_key */
	select,			/* e_key */
	nokey,			/* m_key */
	nokey,			/* d_key */
	nokey,			/* not_fld */
	(struct fet *)NULL,	/* curfet */
	mdboxes,		/* csbp */
	crate1,			/* cratex */
	crate1,			/* cratey */
	CT_GRAF,		/* cmtype */
	MCURX,			/* cxval */
	MCURY			/* cyval */
};

/* 
*/

/*
   =============================================================================
	dswap() -- clear the screen for display switching
   =============================================================================
*/

dswap()
{
	register short i;

	vsndpal(blakpal);			/* blackout the palette */

	if (v_regs[5] & 0x0180)
		vbank(0);			/* select bank 0 */

	setipl(VID_DI);				/* disable the un-blanker */
	vi_ctl = 0;				/* reset un-blank control */

	for (i = 0; i < 16; i++)		/* blank the objects */
		v_odtab[i][0] |= V_BLA;

	VHinit();				/* init VSDD hardware */
	VSinit();				/* init VSDD software */
}

/* 
*/

/*
   =============================================================================
	mlbld() -- build a menu line
   =============================================================================
*/

mlbld(where, from)
char *where, *from;
{
	register short i, j;
	register char c;

	while (-1 NE (i = *from++)) {

		c = *from++;

		for (j = 0; j < i; j++)
			*where++ = c;
	}

	*where = '\0';
}

/*
   =============================================================================
	mlpoint() -- draw a point for the MIDAS 700 main menu
   =============================================================================
*/

mlpoint(x, y, pen)
short x, y, pen;
{
	if (v_regs[5] & 0x0180)
		vbank(0);

	vputp(mdoct, x, y, pen);
}

/* 
*/

/*
   =============================================================================
	mdselbx() -- process a hit in a window for the MIDAS 700 main menu
   =============================================================================
*/

short
mdselbx(n)
short n;
{
	register unsigned mlcv, mbcv, mbak;
	register short rc;

#if	DEBUGIT
	if (debugsw AND debugm7)
		printf("mdselbx(%d): ENTRY  old ndisp = %d\n", n, ndisp);
#endif

	mlcv = exp_c(MLINES);
	mbcv = exp_c(MBUTTON);
	mbak = exp_c(MBKGRND);

	rc   = SUCCESS;

	if (ndisp EQ 7) {		/* "Write program to disk" selected */

		if (n EQ 7) {		/* verified, time to do it */

			if (dcopy()) {

				sprintf(bfs, "  FAILED -- Error %d", defect);

				vcputsv(menuob, 64, MHILITE, mbak,
					mrowcol[M_COPY][0] + 1,
					mrowcol[M_COPY][1],
					bfs, 14);

				rc = FAILURE;
			}

			ndisp = -1;

		} else {		/* not verified, cancel it */

			ndisp = n;	/* display number = box number */
			runit = FALSE;	/* cue msl() to setup the new display */
			dsp_ok = FALSE;	/* disallow display updates */
		}

		vcputsv(menuob, 64, mbcv, mbak,
			mrowcol[M_COPY][0], mrowcol[M_COPY][1],
			mtexts[M_COPY], 14);

/* 
*/

	} else if (ndisp EQ 9) {	/* "Format disk" selected */

		if (n EQ 9) {		/* verified, time to do it */

			if (dformat(DENSITY)) {

				sprintf(bfs, "  FAILED -- Error %d", defect);

				vcputsv(menuob, 64, MHILITE, mbak,
					mrowcol[M_FORM][0] + 1,
					mrowcol[M_FORM][1],
					bfs, 14);

				rc = FAILURE;
			}

			ndisp = -1;

		} else {		/* not verified, cancel it */

			ndisp  = n;	/* display number = box number */
			runit  = FALSE;	/* cue msl() to setup the new display */
			dsp_ok = FALSE;	/* disallow display updates */
		}

		vcputsv(menuob, 64, mbcv, mbak,
			mrowcol[M_FORM][0], mrowcol[M_FORM][1],
			mtexts[M_FORM], 14);

/* 
*/
	} else {	/* select a new menu item */

		ndisp = n;		/* display number = box number */

		if (n EQ 7) {		/* select "Write program to disk" */

			vcputsv(menuob, 64, MHILITE, mbak,
				mrowcol[M_COPY][0], mrowcol[M_COPY][1],
				mtexts[M_COPY], 14);
			
		} else if (n EQ 9) {	/* select "Format disk" */

			vcputsv(menuob, 64, MHILITE, mbak,
				mrowcol[M_FORM][0], mrowcol[M_FORM][1],
				mtexts[M_FORM], 14);
			
		} else {

			runit  = FALSE;	/* cue msl() to setup the new display */
			dsp_ok = FALSE;	/* disallow display updates */
		}
	}

#if	DEBUGIT
	if (debugsw AND debugm7)
		printf("mdselbx(%d): EXIT  ndisp = %d  rc = %d  runit = %d\n",
			n, ndisp, rc, runit);
#endif

	return(rc);
}

/* 
*/

/*
   =============================================================================
	drawm() -- draw the MIDAS 700 main menu
   =============================================================================
*/

drawm()
{
	register short *mlp;
	register short i;
	register unsigned mlcv, mbcv, mbak;
	char buf[16];

	mlcv = exp_c(MLINES);
	mbcv = exp_c(MBUTTON);
	mbak = exp_c(MBKGRND);

	if (ml01[0] EQ 0) {

		mlbld(ml01, mlc01);
		mlbld(ml02, mlc02);
		mlbld(ml03, mlc03);
		mlbld(ml04, mlc04);
		mlbld(ml05, mlc05);
	}

	if (v_regs[5] & 0x0180)
		vbank(0);

	vbfill4(menuob, 128, 0, 0, 511, 349, mbak);

	vcputsv(menuob, 64, mlcv, mbak,  1,  2,   ml01, 14);
	vcputsv(menuob, 64, mlcv, mbak,  2,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak,  2, 61, "\260", 14);

	vcputsv(menuob, 64, mlcv, mbak,  3,  2,   ml02, 14);
	vcputsv(menuob, 64, mlcv, mbak,  4,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak,  4, 32, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak,  4, 61, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak,  5,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak,  5, 32, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak,  5, 61, "\260", 14);

	vcputsv(menuob, 64, mlcv, mbak,  6,  2,   ml03, 14);
	vcputsv(menuob, 64, mlcv, mbak,  7,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak,  7, 32, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak,  7, 61, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak,  8,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak,  8, 32, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak,  8, 61, "\260", 14);
/* 
*/
	vcputsv(menuob, 64, mlcv, mbak,  9,  2,   ml03, 14);
	vcputsv(menuob, 64, mlcv, mbak, 10,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 10, 32, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 10, 61, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 11,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 11, 32, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 11, 61, "\260", 14);

	vcputsv(menuob, 64, mlcv, mbak, 12,  2,   ml03, 14);
	vcputsv(menuob, 64, mlcv, mbak, 13,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 13, 32, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 13, 61, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 14,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 14, 32, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 14, 61, "\260", 14);

	vcputsv(menuob, 64, mlcv, mbak, 15,  2,   ml03, 14);
	vcputsv(menuob, 64, mlcv, mbak, 16,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 16, 32, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 16, 61, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 17,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 17, 32, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 17, 61, "\260", 14);

	vcputsv(menuob, 64, mlcv, mbak, 18,  2,   ml03, 14);
	vcputsv(menuob, 64, mlcv, mbak, 19,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 19, 32, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 19, 61, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 20,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 20, 32, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 20, 61, "\260", 14);

	vcputsv(menuob, 64, mlcv, mbak, 21,  2,   ml04, 14);
	vcputsv(menuob, 64, mlcv, mbak, 22,  2, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 22, 61, "\260", 14);
	vcputsv(menuob, 64, mlcv, mbak, 23,  2,   ml05, 14);
/* 
*/
	mlp = &mlintab[0][0];
	point = mlpoint;

	while (-1  NE mlp[0]) {

		lseg(mlp[0], mlp[1], mlp[2], mlp[3], mlcv);
		mlp += 4;
	}

	vcputsv(menuob, 64, MTITLE, mbak, mrowcol[0][0], mrowcol[0][1],
		mtexts[0], 14);

	vcputsv(menuob, 64, MTITLE, mbak, VER_ROW, VER_COL, m7verms, 14);

	for (i = 1; i < 13; i++)
		vcputsv(menuob, 64, mbcv, mbak, mrowcol[i][0], mrowcol[i][1],
			mtexts[i], 14);

	vcputsv(menuob, 64, MBUCHLA, mbak, mrowcol[13][0], mrowcol[13][1],
		mtexts[13], 14);

	for (i = 0; i < NLTYPES; i++) {

		memcpy(buf, loadedf[i], 8);
		buf[8] = '\0';

		vcputsv(menuob, 64, MLFILES, mbak, mfrc[i][0], mfrc[i][1],
			buf, 14);
	}
}

/* 
*/

/*
   =============================================================================
	m7menu() -- setup the MIDAS 700 main menu display
   =============================================================================
*/

m7menu()
{
	menuob = &v_score[0];
	obj0   = &v_curs0[0];
	mdoct  = &v_obtab[MENUOBJ];

	curslim = 350;

	curset(&m7_flds);

	ndisp = -1;
	dswap();

	vbank(0);
	memsetw(menuob, 0, 32767);
	memsetw(menuob+32767L, 0, 12033);

	SetObj(MENUOBJ, 0, 0, menuob, 512, 350, 0, 0, MENUFL, -1);
	SetObj(0, 0, 1, obj0, 16, 16, MCURX, MCURY, OBFL_00, -1);

	arcurs(MCURSOR);

	drawm();

	vsndpal(menupal);

	SetPri(MENUOBJ, MENUPRI);
	SetPri(0, GCPRI);

	setgc(MCURX, MCURY);
}
