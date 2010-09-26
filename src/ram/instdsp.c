/*
   =============================================================================
	instdsp.c -- instrument definition display driver and support functions
	Version 181 -- 1988-10-06 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	D_EXECKI	0		/* debug execins() */

#include "stddefs.h"
#include "memory.h"

#include "biosdefs.h"
#include "charset.h"
#include "fields.h"
#include "fpu.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "graphdef.h"
#include "smdefs.h"

#include "midas.h"
#include "instdsp.h"

#define	LSPCH		2		/* pitch source scale factor */
#define	MAXRAND		(0x00FFFFFFL)	/* largest random number */

/* things defined elsewhere */

extern	int	(*point)(), fpuint(), setipl();
extern	int	cnvc2p();

extern	unsigned	tofpu(), fromfpu(), exp_c();

extern	char	*dsimlt();

extern	long	rand24();

/* 
*/

extern	short	cxval, cyval, stcrow, stccol, initcfg;
extern	short	curslim, idimsw;

extern	unsigned	*obj0, *obj2;

extern	char	bfs[], *srctbl[], sfdsp[];
extern	char	vtlin1[], vtlin2[], vtlin3[];

extern	struct	sment	sments[];
extern	struct	valent	valents[];
extern	struct	sment	*vpsms[];

extern	short	s_inst[], vce2trg[], vce2grp[];

extern	short	curfunc;	/* current function number */
extern	short	curinst;	/* current instrument number */
extern	short	curpnt;		/* current point number (absolute) */
extern	short	curvce;		/* current voice number */
extern	short	idcfsw;		/* copy / fetch menu switch */
extern	short	idintmp;	/* temporary for instrument number */
extern	short	idnamsw;	/* typewriter switch */
extern	short	idsrcsw;	/* source menu switch */
extern	short	npts;		/* number of points in function */
extern	short	pecase;		/* point edit case variable */
extern	short	pntsv;		/* point selection state variable */
extern	short	subj;		/* edited point number (relative) */
extern	short	submenu;	/* submenu cursor switch */
extern	short	temax;		/* time upper limit */
extern	short	temin;		/* time lower limit */
extern	short	vlbtype;	/* type of message window display */
extern	short	wcflag;		/* ws/cf menu type */
extern	short	wcpage;		/* ws/cf menu page */

extern	short	instmod[12];	/* intrument data modified */

extern	unsigned *instob;	/* instrument object pointer */

extern	struct	octent *idoct;		/* instrument display octent pointer */
extern	struct	instpnt	*pntptr;	/* pointer to edited point */

extern	struct	instdef	idefs[NINST];	/* current instrument definitions */
extern	struct	instdef	vbufs[12];	/* voice instrument buffers */

extern	char	*funcndx[256][2];	/* function header pointers */
extern	char	*vlbptr[3];		/* variable label pointers */

extern	char	idbuf[65];		/* display conversion work area */

/* 
*/

/* forward references */

short	idpoint();

short	idbox[][8] = {	/* display box parameters */

	{  1,   1,  84,  26, CFBX00, CBBX00,  0,  1},	/*  0: Freq 1 */
	{ 86,   1, 169,  26, CFBX01, CBBX01,  0, 11},	/*  1: Freq 2 */
	{171,   1, 254,  26, CFBX02, CBBX02,  0, 22},	/*  2: Freq 3 */
	{256,   1, 339,  26, CFBX03, CBBX03,  0, 33},	/*  3: Freq 4 */
	{341,   1, 424,  26, CFBX04, CBBX04,  0, 44},	/*  4: Filter / Resonance */
	{426,   1, 509,  26, CFBX05, CBBX05,  0, 54},	/*  5: Location */

	{  1,  29,  84,  54, CFBX06, CBBX06,  2,  1},	/*  6: Index 1 */
	{ 86,  29, 169,  54, CFBX07, CBBX07,  2, 11},	/*  7: Index 2 */
	{171,  29, 254,  54, CFBX08, CBBX08,  2, 22},	/*  8: Index 3 */
	{256,  29, 339,  54, CFBX09, CBBX09,  2, 33},	/*  9: Index 4 */
	{341,  29, 424,  54, CFBX10, CBBX10,  2, 44},	/* 10: Index 5 */
	{426,  29, 509,  54, CFBX11, CBBX11,  2, 54},	/* 11: Index 6 */

	{  1,  56, 509, 208, CFBX12, CBBX12,  4, 54},	/* 12: Level */

	{  1, 210, 110, 236, CFBX13, CBBX13, 15,  1},	/* 13: Source- Mlt */
	{112, 210, 142, 236, CFBX14, CBBX14, 15, 15},	/* 14: Pt */
	{144, 210, 206, 236, CFBX15, CBBX15, 15, 19},	/* 15: Time */
	{208, 210, 366, 236, CFBX16, CBBX16, 15, 27},	/* 16: Value */
	{368, 210, 509, 236, CFBX17, CBBX17, 15, 47},	/* 17: Action */

	{  1, 238, 131, 348, CFBX18, CBBX18, 17,  2},	/* 18: Configuration */
	{133, 238, 267, 251, CFBX19, CBBX19, 17, 17},	/* 19: Voice & Inst */
	{269, 238, 379, 306, CFBX20, CBBX20, 17, 35},	/* 20: Oscillators */
	{381, 238, 509, 348, CFBX21, CBBX21, 17, 48},	/* 21: Waveshape */

	{133, 308, 379, 348, CFBX22, CBBX22, 22, 17},	/* 22: (messages) */

	{133, 252, 267, 306, CFBX23, CBBX23, 18, 17}	/* 23: Name & comments */
};

/* 
*/

/* instrument function to FPU function table */

short	fnoff[NFINST] = {

	1,	/*  0: Freq 1 */
	3,	/*  1: Freq 2 */
	5,	/*  2: Freq 3 */
	7,	/*  3: Freq 4 */
	10,	/*  4: Filter / Resonance */
	4,	/*  5: Location */
	9,	/*  6: Index 1 */
	11,	/*  7: Index 2 */
	12,	/*  8: Index 3 */
	13,	/*  9: Index 4 */
	14,	/* 10: Index 5 */
	15,	/* 11: Index 6 */
	2	/* 12: Level */
};

short	inspal[16][3] = {	/* instrument display color palette */

	{0, 0, 0},	/*  0 */
	{3, 3, 3},	/*  1 */
	{2, 2, 2},	/*  2 */
	{3, 0, 0},	/*  3 */
	{0, 3, 2},	/*  4 */
	{0, 0, 1},	/*  5 */
	{0, 1, 1},	/*  6 */
	{1, 0, 1},	/*  7 */
	{3, 2, 0},	/*  8 */
	{1, 1, 2},	/*  9 */
	{2, 3, 0},	/* 10 */
	{2, 3, 3},	/* 11 */
	{3, 0, 2},	/* 12 */
	{0, 2, 3},	/* 13 */
	{0, 3, 0},	/* 14 */
	{3, 3, 0}	/* 15 */
};

/* 
*/

char	*idbxlbl[] = {	/* display box labels */

	"    Frq 1",		/*  0 */
	"    Frq 2",		/*  1 */
	"    Frq 3",		/*  2 */
	"    Frq 4",		/*  3 */
	"    Filtr",		/*  4 */
	"    Loctn",		/*  5 */
	"    Ind 1",		/*  6 */
	"    Ind 2",		/*  7 */
	"    Ind 3",		/*  8 */
	"    Ind 4",		/*  9 */
	"    Ind 5",		/* 10 */
	"    Ind 6",		/* 11 */
	"    Level",		/* 12 */
	"Source  Mult",		/* 13 */
	"Pt",			/* 14 */
	"Time  ",		/* 15 */
	"Value Source  Mult",	/* 16 */
	"Conditioned Acts",	/* 17 */
	"  Config  #",		/* 18 */
	"Voice    Inst",	/* 19 */
	"Oscillators",		/* 20 */
	"Wavshpe",		/* 21 */
	"",			/* 22 */
	""			/* 23 */
};

char idhlbl[] =		/* main function time axis label */
	"       \324\302     \325\305     \326      \301      \302      \304      \310     \321\306    \323\302";


char	*osclbl[] = {	/* oscillator mode labels */
			/* NOTE: must match values in instdsp.h */

	"Int",		/* 0:  OC_INT - Interval */
	"Rat",		/* 1:  OC_RAT - Ratio */
	"Frq",		/* 2:  OC_FRQ - Frequency */
	"Pch"		/* 3:  OC_PCH - Pitch */
};

/* 
*/

long	rngdiv[] = {	/* divisors for ranges of 0..9 */

	MAXRAND,		/* 0..0 */
	MAXRAND / 2,		/* 0..1 */
	MAXRAND / 3,		/* 0..2 */
	MAXRAND / 4,		/* 0..3 */
	MAXRAND / 5,		/* 0..4 */
	MAXRAND / 6,		/* 0..5 */
	MAXRAND / 7,		/* 0..6 */
	MAXRAND / 8, 		/* 0..7 */
	MAXRAND / 9,		/* 0..8 */
	MAXRAND / 10		/* 0..9 */
};

/* 
*/

/*
   =============================================================================
	drawpt() -- draw a point as a cross
   =============================================================================
*/

drawpt(px, py, pc)
short px, py, pc;
{
	register short tmp;

	tmp = px - 1;

	if (tmp > 7)
		idpoint(tmp, py, pc);

	tmp = px + 1;

	if (tmp < 509)
		idpoint(tmp, py, pc);

	tmp = py - 1;

	if (tmp > 55)
		idpoint(px, tmp, pc);

	tmp = py + 1;

	if (tmp < 196)
		idpoint(px, tmp, pc);

	idpoint(px, py, pc);
}

/* 
*/

/*
   =============================================================================
	drawfn() -- draw a function
   =============================================================================
*/

drawfn(fn, how, pen, wn)
short fn, how, pen, wn;
{
	struct idfnhdr *fp;
	struct instdef *ip;
	register struct instpnt *pt1, *pt2;
	register short i, npt1, npt2, xp;
	register unsigned color;
	short np, ptx, pty, ptc;

	ip = &vbufs[curvce];
	fp = &ip->idhfnc[fn];
	point = idpoint;

	np = fp->idfpif;	/* number of points in the function */
	xp = subj;		/* edit point */

	npt1 = 0;		/* left endpoint of line */
	npt2 = 1;		/* right endpoint of line */

	pt1 = &ip->idhpnt[fp->idfpt1];		/* left endpoint of line */
	pt2 = pt1 + 1;				/* right endpoint of line */

	color = exp_c(pen);	/* replicate 4 bit 'pen' for 16 bit 'color' */

/* 
*/
	ptx = ttox(timeto(fn, npt1), wn);
	pty = vtoy((pt1->ipval >> 5), wn);

	ptc = how ? ID_SELD :
		((pt1->ipact OR pt1->ipvsrc) ?
			ID_ACTP : ID_CPNT);

	if (np EQ 1) {		/* single point ? */

		if (wn EQ 12)
			drawpt(ptx, pty, ptc);
		else
			idpoint(ptx, pty, ptc);
		
		return;
	}

	if (how AND (xp EQ 0))
		if (wn EQ 12)
			drawpt(ptx, pty, ID_SELD);
		else
			idpoint(ptx, pty, ID_SELD);

	for (i = 1; i < np; i++) {

		ptx = ttox(timeto(fn, npt2), wn);
		pty = vtoy((pt2->ipval >> 5), wn);

		ptc = (pt2->ipact OR pt2->ipvsrc) ?
			ID_ACTP : ID_CPNT;

		if (how AND (xp EQ (i - 1))) {


			if (wn EQ 12)
				drawpt(ptx, pty, ptc);
			else
				idpoint(ptx, pty, ptc);

		} else if (how AND (xp EQ i)) {

			ptc = ID_SELD;

			if (wn EQ 12)
				drawpt(ptx, pty, ptc);
			else
				idpoint(ptx, pty, ptc);
/* 
*/
		} else {

			lseg(ttox(timeto(fn, npt1), wn),
				vtoy((pt1->ipval >> 5), wn),
				ptx, pty, color);

			if (wn EQ 12) {

				drawpt(ttox(timeto(fn, npt1), wn),
					vtoy((pt1->ipval >> 5), wn),
					(pt1->ipact OR pt1->ipvsrc) ?
					 ID_ACTP : ID_CPNT);

				drawpt(ptx, pty, ptc);

			} else {

				idpoint(ttox(timeto(fn, npt1), wn),
					vtoy((pt1->ipval >> 5), wn),
					(pt1->ipact OR pt1->ipvsrc) ?
					 ID_ACTP : ID_CPNT);

				idpoint(ptx, pty, ptc);
			}
		}

		npt1++;
		npt2++;
		pt1++;
		pt2++;
	}

	return;
}

/* 
*/

/*
   =============================================================================
	addpch() -- add an offset to a pitch function value
   =============================================================================
*/

short
addpch(p1, p2)
short p1, p2;
{
	register long pl;

	pl = ((((long)p1 >> 5) - 500L) << LSPCH) + (long)p2;

	if (pl > PITCHMAX)
		pl = PITCHMAX;

	return((short)pl);
}

/* 
*/

/*
   =============================================================================
	irand() -- develop a random number in a given range
   =============================================================================
*/

short
irand(range)
short range;
{
	if (range LE 0)		/* limit things to 'reasonable' values */
		return(0);

	if (range > 9)		/* limit things to 'reasonable' values */
		range = 9;

	return((short)(rand24() / rngdiv[range]));
}

/* 
*/

/*
   =============================================================================
	xgetran() -- develop a random number
   =============================================================================
*/

short
xgetran(mlt)
short mlt;
{
	register short sltmp;

	sltmp = (short)rand24();

	if (mlt < 0)
		sltmp += (short)rand24();

	return(sltmp);
}

/* 
*/

/*
   =============================================================================
	dosync() -- update sync and configuration register
   =============================================================================
*/

dosync(vce)
short vce;
{
	register short sync;
	register struct instdef *ip;

	ip = &vbufs[vce];
	sync = ip->idhcfg << 8;

	if (ip->idhos1c & OC_SYN)
		sync |= 0x2000;

	if (ip->idhos2c & OC_SYN)
		sync |= 0x4000;

	if (ip->idhos3c & OC_SYN)
		sync |= 0x8000;

	*(io_fpu + vce + (long)FPU_OCFG) = sync;
}

/* 
*/

/*
   =============================================================================
	oscdsp() -- display an oscillator frequency specification
   =============================================================================
*/

oscdsp(row, val, n, lbl, frq)
short row, val, n, frq;
char *lbl;
{
		register short fh, fl;

		tsplot4(instob, 64, idbox[n][4], row, 34, lbl, 14);
		tsplot4(instob, 64, idbox[n][4], row, 36, osclbl[val & 3], 14);

		switch (val & 3) {

		case OC_INT:	/* interval */

			sprintf(idbuf, "%c%04d", (frq < 0 ? '-' : '+'),
				((frq < 0 ? -frq : frq) >> 1));

			break;

		case OC_RAT:	/* ratio */

			int2rat(frq >> 1);

			ebflag = FALSE;

			idbuf[0] = ebuf[0];
			idbuf[1] = ebuf[1];
			idbuf[2] = ebuf[2];
			idbuf[3] = '\0';

			break;

		case OC_FRQ:	/* frequency */

			fh = (frq >> 1) / 10;
			fl = (frq >> 1) - (fh * 10);

			sprintf(idbuf, "%02d.%d ", fh, fl);
			break;
/* 
*/
		case OC_PCH:

			cnvc2p(idbuf, (frq >> 1));
			idbuf[0] += '0';
			idbuf[1] += 'A';
			idbuf[2] = sfdsp[idbuf[2]];
			idbuf[3] += '0';
			idbuf[4] += '0';
			idbuf[5] = '\0';
			break;
		}

		tsplot4(instob, 64, idbox[n][4], row, 40, idbuf, 14);

		if (row NE 21)
			tsplot4(instob, 64, idbox[n][4], row, 46,
				((val & OC_SYN) ? "S" : "s"), 14);
}

/* 
*/

/*
   =============================================================================
	pltws() -- plot a small waveshape display
   =============================================================================
*/

pltws(ws, pen)
register unsigned ws[];
register short pen;
{
	register short i, x, y;

	for (i = 1; i < 254; i++) {

		x = 382 + (i >> 1);
		y = 348 - ((ws[i] ^ 0x8000) / 676);

		idpoint(x, y, pen);
	}
}

/* 
*/

/*
   =============================================================================
	dswin() -- display a window
   =============================================================================
*/

dswin(n)
register short n;
{
	short th, tl, vh, vl, vv, mltval, lboff;
	char *s1, mltstr[6], mltsgn;
	long tt, sc;
	register short cx, pnt, par;
	register struct instdef *ip;
	register struct idfnhdr *fp;
	register struct instpnt *pp;

	if (wcflag NE -1)
		return;

	if (idimsw AND (n NE 19) AND (n < 22))
		return;

	ip = &vbufs[curvce];

	fp = &ip->idhfnc[n LE 12 ? n : curfunc];

	curpnt = subj + fp->idfpt1;
	pp = &ip->idhpnt[curpnt];

	cx = idbox[n][5];
	cx |= cx << 4;
	cx |= cx << 8;

/* 
*/

	/* first, fill the box with the background color */

	if (v_regs[5] & 0x0180)
		vbank(0);

	vbfill4(instob, 128, idbox[n][0], idbox[n][1], idbox[n][2], idbox[n][3], cx);

	if (n < 12)		/* draw the function if it's a label window */
		drawfn(n, 0, ID_CLIN, n);

	/* put in the box label */

	if (n GT 12) {

		tsplot4(instob, 64, idbox[n][4], idbox[n][6], idbox[n][7],
			idbxlbl[n], 14);

	} else {

		tsplot4(instob, 64,
			((ip->idhfnc[n].idftmd & I_TM_KEY) ?  idbox[n][4] : ID_INST),
			idbox[n][6], idbox[n][7], idbxlbl[n], 14);
	}

/* 
*/
	switch (n) {	/* final text - overlays above stuff */

	case 12:	/* level */

		/* x labels */

		tsplot4(instob, 64, TGRID, 14, 0, idhlbl, 14);

		/* y labels */

		tsplot4(instob, 64, TGRID, 14, 0, "\300", 14);
		tsplot4(instob, 64, TGRID, 12, 0, "\302", 14);
		tsplot4(instob, 64, TGRID, 10, 0, "\304", 14);
		tsplot4(instob, 64, TGRID,  8, 0, "\306", 14);
		tsplot4(instob, 64, TGRID,  6, 0, "\310", 14);
		tsplot4(instob, 64, TGRID,  4, 0, "\312", 14);

		lseg( 7,  56,   7, 196, LGRID);		/* draw the grid */
		lseg( 7, 196, 509, 196, LGRID);

		drawfn(12, 0, ID_CLIN, 12);		/* draw the function */

		return;

	case 13:	/* source - multiplier */

		dsimlt(bfs, fp->idfsrc, fp->idfmlt);

		tsplot4(instob, 64, idbox[n][4],
			idbox[n][6]+1, idbox[n][7], bfs, 14);
		return;

	case 14:	/* point number */

		sprintf(bfs, "%02d", subj);

		tsplot4(instob, 64, idbox[n][4], idbox[n][6]+1, idbox[n][7], bfs, 14);

		return;
/* 
*/
	case 15:	/* time */

		sc = 1000L;
		tt = timeto(curfunc, subj);
		th = tt / sc;
		tl = tt - (th * sc);

		sprintf(bfs, "%02d.%03d", th, tl);
		tsplot4(instob, 64, idbox[n][4], idbox[n][6] + 1,
			idbox[n][7], bfs, 14);

		return;

	case 16:	/* value */

		vv = pp->ipval >> 5;
		vh = vv / 100;
		vl = vv - (vh * 100);

		sprintf(bfs, "%02d.%02d ", vh, vl);

		dsimlt(&bfs[6], pp->ipvsrc, pp->ipvmlt);

		tsplot4(instob, 64, idbox[n][4], idbox[n][6] + 1,
			idbox[n][7], bfs, 14);

		return;

/* 
*/
	case 17:	/* action */

		pnt = pp->ippar1;
		par = pp->ippar2;

		switch (pp->ipact) {

		case AC_NULL:

			s1 = "                ";
			break;

		case AC_SUST:

			sprintf(bfs, "Pause if key %c  ", SP_DNA);
			s1 = bfs;
			break;

		case AC_ENBL:

			sprintf(bfs, "Stop if key %c   ", SP_UPA);
			s1 = bfs;
			break;

		case AC_JUMP:

			sprintf(bfs, "GoTo %02d forever ", pnt);
			s1 = bfs;
			break;

/* 
*/
		case AC_LOOP:

			sprintf(bfs, "GoTo %02d %02d times", pnt, par);

			if (bfs[8] EQ '9')		/* random */
				bfs[8] = 'R';

			s1 = bfs;
			break;

		case AC_KYUP:

			sprintf(bfs, "GoTo %02d if key %c",
				pnt, SP_UPA);
			s1 = bfs;
			break;

		case AC_KYDN:

			sprintf(bfs, "GoTo %02d if key %c",
				pnt, SP_DNA);
			s1 = bfs;
			break;

		default:

			s1 = "????????????????";
			break;
		}

		tsplot4(instob, 64, idbox[n][4], idbox[n][6] + 1,
			idbox[n][7], s1, 14);

		return;

/* 
*/

	case 18:	/* configuration */

		showcfg(ip->idhcfg);
		return;

	case 19:	/* voice & instrument */

		sprintf(idbuf, "%02d", curvce + 1);
		tsplot4(instob, 64, idbox[n][4], 17, 23, idbuf, 14);

		sprintf(idbuf, "%02d", curinst);
		tsplot4(instob, 64, (instmod[curvce] ? ID_CHGD :idbox[n][4]),
			17, 31, idbuf, 14);

		return;

	case 20:	/* oscillators */

		oscdsp(18, ip->idhos1c, n, "1", ip->idhos1v);
		oscdsp(19, ip->idhos2c, n, "2", ip->idhos2v);
		oscdsp(20, ip->idhos3c, n, "3", ip->idhos3v);
		oscdsp(21, ip->idhos4c, n, "4", ip->idhos4v);

		return;

	case 21:	/* waveshapes */

		sprintf(idbuf, "A%02d", ip->idhwsa + 1);
		tsplot4(instob, 64, WSAFC, 17, 56, idbuf, 14);
		pltws(ip->idhwvaf, WSAFC);

		sprintf(idbuf, "B%02d", ip->idhwsb + 1);
		tsplot4(instob, 64, WSBFC, 17, 60, idbuf, 14);
		pltws(ip->idhwvbf, WSBFC);

		return;
/* 
*/
	case 22:	/* message window */

		tsplot4(instob, 64, idbox[n][4], 22, 17, vlbptr[0], 14);
		tsplot4(instob, 64, idbox[n][4], 23, 17, vlbptr[1], 14);
		tsplot4(instob, 64, idbox[n][4], 24, 17, vlbptr[2], 14);

		if (idsrcsw) {

			point = idpoint;

			lseg(196, 308, 196, 348, CBORD);
			lseg(260, 308, 260, 348, CBORD);
			lseg(324, 308, 324, 348, CBORD);
		}

		return;

	case 23:	/* Name and comments */

		tsplot4(instob, 64, idbox[n][4], 18, 17, ip->idhname, 14);
		tsplot4(instob, 64, idbox[n][4], 19, 17, ip->idhcom1, 14);
		tsplot4(instob, 64, idbox[n][4], 20, 17, ip->idhcom2, 14);
		tsplot4(instob, 64, idbox[n][4], 21, 17, ip->idhcom3, 14);

		return;
	}
}

/* 
*/

/*
   =============================================================================
	allwins() -- display all windows
   =============================================================================
*/

allwins()
{
	register short i;

	for (i = 0; i < 24; i++)
		dswin(i);
}

/*
   =============================================================================
	idpoint() -- plot a point for the lseg function
   =============================================================================
*/

idpoint(x, y, pen)
short x, y, pen;
{
	if (v_regs[5] & 0x0180)
		vbank(0);

	vputp(idoct, x, y, pen);
}

/* 
*/

/*
   =============================================================================
	idbord() -- draw the border for the instrument display
   =============================================================================
*/

idbord()
{
	point = idpoint;

	lseg(  0,   0, 510,   0, CBORD);	/* outer border */
	lseg(510,   0, 510, 349, CBORD);
	lseg(510, 349,   0, 349, CBORD);
	lseg(  0, 349,   0,   0, CBORD);

	lseg(  0,  27, 510,  27, CBORD);	/* label widows - H lines */
	lseg(  0,  55, 510,  55, CBORD);

	lseg( 85,   1,  85,  54, CBORD);	/* label windows - V lines */
	lseg(170,   1, 170,  54, CBORD);
	lseg(255,   1, 255,  54, CBORD);
	lseg(340,   1, 340,  54, CBORD);
	lseg(425,   1, 425,  54, CBORD);

	lseg(  0, 209, 510, 209, CBORD);	/* S/M, Time, Val - H lines */
	lseg(  0, 237, 510, 237, CBORD);

	lseg(111, 210, 111, 236, CBORD);	/* S/M, Time, Val - V lines */
	lseg(143, 210, 143, 236, CBORD);
	lseg(207, 210, 207, 236, CBORD);
	lseg(367, 210, 367, 236, CBORD);

	lseg(132, 238, 132, 349, CBORD);	/* Voice, Osc, Vars */
	lseg(380, 238, 380, 349, CBORD);
	lseg(132, 307, 380, 307, CBORD);
	lseg(268, 238, 268, 306, CBORD);
}

/* 
*/

/*
   =============================================================================
	idvlblc() -- clear the message window
   =============================================================================
*/

idvlblc()
{
	vlbtype = 0;	/* nothing in the message window */

	vlbptr[0] = vlbptr[1] = vlbptr[2] = "                              ";
}

/*
   =============================================================================
	idvlbld() -- put the labels in the message window
   =============================================================================
*/

idvlbld()
{
	vlbtype = 1;	/* variable labels */

/*                  "123456789012345678901234567890"  */

	vlbptr[0] = "PchW/HT Pch/Frq Random  GPC/V1";
	vlbptr[1] = "ModW/VT Key Vel Pedal 1       ";
	vlbptr[2] = "Brth/LP Key Prs               ";
}

/* 
*/

/*
   =============================================================================
	idvtyp() -- display the virtual typewriter in the message window
   =============================================================================
*/

idvtyp()
{
	vlbtype = 2;	/* virtual typewriter */

	vlbptr[0] = vtlin1;
	vlbptr[1] = vtlin2;
	vlbptr[2] = vtlin3;
}

/*
   =============================================================================
	idcpfch() -- display the copy / fetch menu in the message window
   =============================================================================
*/

idcpfch()
{
	vlbtype = 3;	/* copy / fetch menu */

/*		    "123456789012345678901234567890"	*/

	vlbptr[0] = "Fetch from library     Escape ";
	vlbptr[1] = "Copy to library               ";
	vlbptr[2] = "                              ";
}

/* 
*/

/*
   =============================================================================
	instdsp() -- put up the instrument display
   =============================================================================
*/

instdsp()
{
	register struct instdef *ip;
	register struct idfnhdr *fp;

	instob = &v_score[0];		/* setup object pointer */
	idoct  = &v_obtab[INSTOBJ];	/* setup object control table pointer */
	obj0   = &v_curs0[0];		/* setup cursor object pointer */
	obj2   = &v_tcur[0];		/* setup typewriter cursor pointer */

	idnamsw = FALSE;	/* typewriter not up */
	idsrcsw = FALSE;	/* source menu not up */
	idcfsw  = FALSE;	/* copy / fetch menu not up */
	idimsw  = FALSE;	/* instrument menu not up */
	submenu = FALSE;	/* submenu cursor not enabled */
	wcflag  = -1;		/* no menu page up */
	wcpage	= 0;		/* initial ws/cf page */

	setinst();		/* setup editing variables */

	dswap();		/* initialize display */

	vbank(0);		/* clear the display */
	memsetw(instob, 0, 32767);
	memsetw(instob+32767L, 0, 12033);

	SetObj(INSTOBJ, 0, 0, instob, 512, 350, 0, 0, INSTFL, -1);
	SetObj(0, 0, 1, obj0, 16, 16, ICURX, ICURY, OBFL_00, -1);
	SetObj(TTCURS, 0, 1, obj2, 16, 16,     0,     0,  TTCCFL, -1);

	arcurs(ID_NCUR);	/* setup arrow cursor object */
	itcini(ID_TCUR);	/* setup text cursor object */
	ttcini(ID_TCUR);	/* setup typewriter cursor object */

	vbank(0);
	idbord();		/* draw the border */
	idvlblc();		/* no initial label */
	allwins();		/* fill in the windows */

	SetPri(INSTOBJ, INSTPRI);
	SetPri(0, GCPRI);	/* display the graphic cursor */

	setgc(ICURX, ICURY);

	vsndpal(inspal);	/* set the palette */
}

/* 
*/

/*
   =============================================================================
	reshowi() -- redisplay the instrument
   =============================================================================
*/

reshowi()
{
	idfield();		/* fix up field table, etc. */

	wcflag = -1;		/* no menu page up */
	wcpage = 0;		/* first page of waveshapes */

	dswap();		/* initialize display */

	vbank(0);		/* clear the display */
	memsetw(instob, 0, 32767);
	memsetw(instob+32767L, 0, 12033);

	SetObj(INSTOBJ, 0, 0, instob, 512, 350, 0, 0, INSTFL, -1);
	SetObj(0, 0, 1, obj0, 16, 16, ICURX, ICURY, OBFL_00, -1);
	SetObj(TTCURS, 0, 1, obj2, 16, 16,     0,     0,  TTCCFL, -1);

	arcurs(ID_NCUR);	/* setup arrow cursor object */
	itcini(ID_TCUR);	/* setup text cursor object */
	ttcini(ID_TCUR);	/* setup typewriter cursor object */

	vbank(0);
	idbord();		/* draw the border */
	idvlblc();		/* no initial label */
	allwins();		/* fill in the windows */

	SetPri(INSTOBJ, INSTPRI);
	SetPri(0, GCPRI);	/* enable the cursor */

	if (cyval > curslim)
		settc(stcrow, stccol);
	else
		setgc(cxval, cyval);

	vsndpal(inspal);	/* set the palette */
}
