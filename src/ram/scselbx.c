/*
   =============================================================================
	scselbx.c -- MIDAS-VII -- score editor box selection functions
	Version 87 -- 1989-11-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "hwdefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddvars.h"
#include "glcfns.h"
#include "glcdefs.h"
#include "graphdef.h"
#include "lcdline.h"
#include "panel.h"
#include "score.h"
#include "scfns.h"
#include "sclock.h"
#include "slice.h"
#include "secops.h"
#include "secdefs.h"
#include "macros.h"

#include "midas.h"
#include "asgdsp.h"
#include "scdsp.h"

/* functions defined elsewhere */

extern	short	enterit(), advscur(), oktode(), nokey();

/* 
*/

/* variables defined elsewhere */

extern	short	(*itxput)();
extern	short	(*point)();

extern	short	ancmsw;
extern	short	angroup;
extern	short	asmode;
extern	short	clkrun;
extern	short	clkctl;
extern	short	clksrc;
extern	short	ctrsw;
extern	short	cxval;
extern	short	cyval;
extern	short	dubsw;
extern	short	gomode;
extern	short	grptran;
extern	short	gtmsel;
extern	short	hitbox;
extern	short	hitcx;
extern	short	hitcy;
extern	short	insmode;
extern	short	ismode;
extern	short	lastam;
extern	short	midiclk;
extern	short	noteop;
extern	short	notesel;
extern	short	oldpk;
extern	short	oldsl;
extern	short	pchsw;
extern	short	pkctrl;
extern	short	pulsclk;
extern	short	recsw;
extern	short	scmctl;
extern	short	sdmcol;
extern	short	sdmctl;
extern	short	sdmrow;
extern	short	secop;
extern	short	sgoflag;
extern	short	sliders;
extern	short	stccol;
extern	short	stcrow;
extern	short	stepclk;
extern	short	stepenb;
extern	short	submenu;
extern	short	velflag;
extern	short	vtccol;
extern	short	vtcrow;

/* 
*/

extern	short	grptmap[];

extern	short	sctctab[][64];

extern	struct	asgent	asgtab[];

extern	struct	selbox	*csbp;
extern	struct	selbox	*curboxp;

extern	struct	s_time	stimes[N_SCORES][N_SECTS];	/* section times */

extern	unsigned	*obj8, *obj11;

extern	PFS	(*swpt)[];
extern	PFS	(*oldsw)[];
extern	PFS	t_ngrp[];

extern	char	bfs[];
extern	char	tunname[][32];

extern	struct	gdsel	*gdstbc[];

/* forward references */

short	sdboxfn(), bspscur(), svtdsp(), svtstop();

/* 
*/

char	*sdmenus[][3] = {	/* score display menus */

	/* 0 - typewriter */

	{" ABCDEFGHIJKLMNOPQRSTUVWXYZ  |                                  ",
	 " abcdefghijklmnopqrstuvwxyz -*-                                 ",
	 "  0123456789+-/*().,:;!?&<>  |                                  "},

	/* 1 - note edit */

	{" Begin Acc   Move Note   Escape                                 ",
	 " Begin Nat   Move Begin                                         ",
	 " End Note    Move End                                           "},

	/* 2 - clock source */

	{" Local   PLS24   Step                                           ",
	 " MIDI    PLS48                                                  ",
	 " SMPTE   PLS96                                                  "},

	/* 3 - clock control */

	{" Local                                                          ",
	 " MIDI                                                           ",
	 " SMPTE                                                          "},

	/* 4 - Section menu */

	{" GoTo   Move   SMPTE                                            ",
	 " Begin  Copy   Del Gr                                           ",
	 " End    Merge  Del Ev                                           "}
};

char	sdmln3[] =
	 "        ReGrp  Remove Trn S                                     ";

char	sdmln4[] =
	 "Escape                    D                                     ";

/* 
*/

short	sdmht[][3] = {		/* score section menu highlight table */

	/* row,	lcol,	rcol */

	{ 6,  	0,	 0},		/*  0 - SOP_NUL */
	{ 3,  	1,	 4},		/*  1 - SOP_GO  */
	{ 4,  	1,	 5},		/*  2 - SOP_BGN */
	{ 5,  	1,	 3},		/*  3 - SOP_END */
	{ 3,  	8,	11},		/*  4 - SOP_MOV */
	{ 4,  	8,	11},		/*  5 - SOP_CPY */
	{ 5,  	8,	12},		/*  6 - SOP_MRG */
	{ 3, 	15,	19},		/*  7 - SOP_STC */
	{ 4, 	15,	23},		/*  8 - SOP_DGR */
	{ 5, 	15,	23},		/*  9 - SOP_DEV */
	{ 6, 	15,	20},		/* 10 - SOP_RMV */
	{ 6,	8,	12}		/* 11 - SOP_GRP */
};

short	sdmlim[][4] = {		/* score display menu cursor limits */

	/* top,	left,	bottom,	right */

	{ 19,  	1,	 21,	 30 },		/* 0 - typewriter */
	{ 19,	1,	 21,	 30 },		/* 1 - note edit */
	{ 19, 	1,	 21,	 20 },		/* 2 - clock source */
	{ 19, 	1,	 21,	 19 },		/* 3 - clock control */
	{ 19, 	1,	 23,	 62 }		/* 4 - section menu */
};

/* 
*/

char	*nedlbl[] = {		/* note edit function labels */

	"Note Edit",	/* 0 - NOP_NUL */
	"Begin Acc",	/* 1 - NOP_ACC */
	"Begin Nat",	/* 2 - NOP_NAT */
	"End Note ",	/* 3 - NOP_END */
	"Move Note",	/* 4 - NOP_MVN */
	"Move Beg ",	/* 5 - NOP_MVB */
	"Move End "	/* 6 - NOP_MVE */
};

/* 
*/

struct	selbox	sdboxes[] = {

	{  0,   0, 175,  13,       0, sdboxfn},	/*  0 - Sec, Beat, Frame */
	{176,   0, 231,  13,       1, sdboxfn},	/*  1 - Insert */
	{232,   0, 439,  13,       2, sdboxfn},	/*  2 - Clock */
	{440,   0, 511,  13,       3, sdboxfn},	/*  3 - Note Edit */

	{  0, 238, 111, 251,       4, sdboxfn},	/*  4 - Assignment */
	{112, 238, 167, 251,       5, sdboxfn},	/*  5 - Tune */
	{168, 238, 247, 251,       6, enterit},	/*  6 - Tempo */
	{248, 238, 319, 251,       7, enterit},	/*  7 - Interpolate */
	{320, 238, 398, 251,       8, sdboxfn},	/*  8 - Stop/Next */
	{400, 238, 455, 251,       9, sdboxfn},	/*  9 - In/Out */
	{456, 238, 511, 251,      10, sdboxfn},	/* 10 - Output */

	{  0, 252, 511, 307,      11, sdboxfn},	/* 11 - Grp, Trns, Dyn, Vel */

	{  0, 308, 511, 335,      12, sdboxfn},	/* 12 - Ansrc, Anval */

	{  0, 336, 215, 349,      13, sdboxfn},	/* 13 - Score Title */
	{216, 336, 455, 349,      14, sdboxfn},	/* 14 - Rec Mode */
	{456, 336, 511, 349,      15, sdboxfn},	/* 15 - Memory */

	{  0,  14, 511, 237,      16, sdboxfn},	/* 16 - Note Display */

	{  0,   0,   0,   0,       0, FN_NULL}	/* end of table */
};

/* 
*/

/*
   =============================================================================
	insect() -- return section number pointed to in section menu, or -1
   =============================================================================
*/

short
insect()
{
	register short col;

	col = vtccol - 24;

	if (col < 0)
		return(-1);

	sgoflag = col & 3;

	if ((vtcrow < 19) OR (vtcrow > 20))
		return(-1);

	return((col >> 2) + (10 * (vtcrow - 19)));
}

/*
   =============================================================================
	svtdsp() -- display data for the virtual typewriter
   =============================================================================
*/

svtdsp(obj, fg, bg, row, col, buf)
unsigned *obj;
unsigned fg, bg;
short row, col;
char *buf;
{
	if (v_regs[5] & 0x0180)
		vbank(0);

	vputs(obj, row - 16, col, buf, SDW13ATR);
}

/* 
*/

/*
   =============================================================================
	svtstop() -- end vitrual typewriter data entry
   =============================================================================
*/

svtstop()
{
	sdmenu(-1);
}

/*
   =============================================================================
	bspscur() -- backspace the score display text cursor
   =============================================================================
*/

bspscur()
{
	register short newcol;

	if (infield(stcrow, stccol, curfet))
		cfetp = infetp;
	else
		return;

	newcol = stccol - 1;

	if (newcol GE cfetp->flcol)
		stcpos(stcrow, newcol);

	cxval = CTOX(stccol);
	cyval = RTOY(stcrow);
}

/* 
*/

/*
   =============================================================================
	dsects() -- display list of section numbers
	Highlight active sections and indicate SMPTE time-coded sections.
   =============================================================================
*/

dsects()
{
	register short atr, row, col, i, j;
	char buf[8];
	char cl, cr, csl, csr;
	short begun;

	register struct s_entry *sp;

	if (v_regs[5] & 0x0180)
		vbank(0);

	row = 3;

	for (i = 0; i < N_SECTS; i += 10) {

		col = 24;

		for (j = 0; j < 10; j++) {

			if (stimes[curscor][i + j].sflags) {	/* SMPTE time */

				csl = '[';
				csr = ']';

			} else {		/* no SMPTE time */

				csl = '{';
				csr = '}';
			}

			cl = ' ';
			cr = ' ';

			if (E_NULL NE seclist[curscor][i + j]) {

				begun = TRUE;
				atr = (SDBGMM | (SD_CHNG << 4));
				cl  = csl;

			} else {

				begun = FALSE;
				atr = SDMENUBG;
			}

			sp = hplist[curscor][EH_SEND];

			while (sp) {

				if (sp->e_data1 EQ (i + j)) {

					cr = begun ? csr : '}';
					break;
				}

				sp = sp->e_up;
			}

			sprintf(buf, "%c%02d%c", cl, 1 + (i + j), cr);

			vputs(obj8, row, col, buf, atr);
			col += 4;
		}

		++row;
	}
}

/* 
*/

/*
   =============================================================================
	dsgtmn() -- display group map source group number
   =============================================================================
*/

dsgtmn(n, f)
short n, f;
{
	char buf[4];
	short atr, col;

	sprintf(buf, "%02d", n + 1);

	col = (3 * n) + 28;
	atr = f ? (SDBGMM | (SD_CHNG <<4)) : SDMENUBG;

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputs(obj8, 6, col, buf, atr);
}

/*
   =============================================================================
	dsgtme() -- display group map destination group number
   =============================================================================
*/

dsgtme(n)
short n;
{
	char buf[4];
	short col;

	if (grptmap[n] EQ -1)
		strcpy(buf, "  ");
	else
		sprintf(buf, "%02d", grptmap[n] + 1);

	col = (3 * n) + 28;

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputs(obj8, 7, col, buf, SDMENUBG);
}

/* 
*/

/*
   =============================================================================
	dsgtmap() -- display group map and transposition value
   =============================================================================
*/

dsgtmap()
{
	char buf[6];
	register short i;

	if (v_regs[5] & 0x0180)
		vbank(0);

	sprintf(buf, "%c%02d", grptran < 0 ? '-' : '+', abs(grptran));

	vputs(obj8, 7, 22, buf, SDMENUBG);

	for (i = 0; i < 12; i++) {

		dsgtmn(i, gtmsel EQ i);
		dsgtme(i);
	}
}

/*
   =============================================================================
	ingroup() -- return group number selected
   =============================================================================
*/

short
ingroup()
{
	register short col;

	col = vtccol - 28;

	if (col < 0)
		return(-1);

	if (2 EQ (col % 3))
		return(-1);

	return(col / 3);
}

/* 
*/

/*
   =============================================================================
	sdmtxt() -- output text to the score area submenu
   =============================================================================
*/

sdmtxt(row, col, txt, tag)
short row, col;
char *txt;
short tag;
{
	if ((v_regs[5] & 0x0180) NE 0x0100)
		vbank(1);

	vcputsv(obj11, 64, SD_TEXT, SDBG16, row, col, txt, 14);
}

/* 
*/

/*
   =============================================================================
	showam() -- show menu of assignments
   =============================================================================
*/

showam(asg)
register short asg;
{
	register short col, na, row;

	if (asg EQ 1)
		na = 48;
	else
		na = NASGS;

	for (col = 2; col < 60; col += 15) {

		if (asg GE na)
			return;

		(*itxput)(0, col, "No Assignment", 0);

		for (row = 1; row < 15; row++) {

			if (asg GE na)
				return;

			sprintf(bfs, "%02d %-10.10s", asg, asgtab[asg].a_name);
			(*itxput)(row, col, bfs, 1);
			++asg;
		}
	}
}

/* 
*/

/*
   =============================================================================
	showtm() -- show menu of tunings
   =============================================================================
*/

showtm()
{
	register short row, tun;

	tun = 0;

	(*itxput)(0, 0, "N Name", 0);

	for (row = 1; row < 11; row++) {

		sprintf(bfs, "%d %-32.32s", tun, tunname[tun]);
		(*itxput)(row, 0, bfs, 1);
		++tun;
	}
}

/* 
*/

/*
   =============================================================================
	showsm() -- show menu of scores
   =============================================================================
*/

showsm()
{
	register short col, row, scr;

	scr = 1;

	for (col = 1; col < 46; col+= 22) {

		if (scr > N_SCORES)
			return;

		(*itxput)(0, col, "No Score", 0);

		for (row = 1; row < 15; row++) {

			if (scr > N_SCORES)
				return;

			sprintf(bfs, "%02d %-16.16s", scr, scname[scr - 1]);
			(*itxput)(row, col, bfs, 1);
			++scr;
		}
	}
}

/* 
*/

/*
   =============================================================================
	scmenu() -- display a submenu in area 1  (score area)
   =============================================================================
*/

scmenu(n)
register short n;
{
	register short i;
	register struct octent *op;

	if (n GE 0) {		/* put up a submenu */

		itxput = sdmtxt;	/* setup output function */

		vbank(0);

		v_odtab[0][0] |= V_BLA;		/* turn off line objcect */
		objclr(0);

		v_odtab[1][0] |= V_BLA;		/* turn off score object */
		objclr(1);

		v_odtab[2][0] |= V_BLA;		/* turn off keyboard object */
		objclr(2);

		vbank(1);			/* clear the window */
		vbfill4(obj11, 128, 0, 0, 511, 223, exp_c(SDBG16));

		vbank(0);			/* turn on window object */
		SetPri(11, 4);

		switch (n) {			/* fill the window */

		case 0:		/* instruments */

			showim();		/* show the instruments */
			break;

		case 1:		/* assignments */

			showam(lastam);		/* show the assignments */
			stcpos(17, 11);		/* position the cursor */

			cxval = CTOX(11);
			cyval = RTOY(17);

			if (infield(stcrow, stccol, curfet))
				cfetp = infetp;

			break;

		case 2:		/* tunings */

			showtm();		/* show the tunings */
			stcpos(17, 19);		/* position the cursor */

			cxval = CTOX(19);
			cyval = RTOY(17);

			if (infield(stcrow, stccol, curfet))
				cfetp = infetp;

			break;

		case 3:		/* scores */

			showsm();		/* show the scores */
			stcpos(24, 7);		/* position the cursor */

			cxval = CTOX(7);
			cyval = RTOY(24);

			if (infield(stcrow, stccol, curfet))
				cfetp = infetp;

			break;

		default:	/* eh ? */

			break;
		}

	} else {		/* take down the submenu */

		vbank(0);

		v_odtab[4][0] |= V_BLA;		/* turn off window object */
		objclr(4);

		op = &v_obtab[13];		/* turn on keyboard object */
		objon(2, op->objy, op->ysize);
		v_odtab[2][0] &= ~V_BLA;

		op = &v_obtab[14];		/* turn on score object */
		objon(1, op->objy, op->ysize);
		v_odtab[1][0] &= ~V_BLA;

		op = &v_obtab[15];		/* turn on line object */
		objon(0, op->objy, op->ysize);
		v_odtab[0][0] &= ~V_BLA;
	}

	scmctl = n;		/* set new menu type */
}
/* 
*/

/*
   =============================================================================
	sdmenu() -- display a submenu in area 2  (text areas)
   =============================================================================
*/

sdmenu(n)
register short n;
{
	register short i, wasup;

	wasup  = sdmctl;	/* save previous menu type */
	sdmctl = n;		/* set new menu type */

	if (n GE 0) {		/* put up a menu */

		submenu = TRUE;		/* indicate there's a menu up */

		sdmrow = stcrow;	/* save underline cursor position */
		sdmcol = stccol;	/* ... */

		memsetw(&sctctab[3][0], exp_c(SDBGMM), 64);	/* recolor */
		memsetw(&sctctab[4][0], exp_c(SDBGMM), 64);
		memsetw(&sctctab[5][0], exp_c(SDBGMM), 64);

		if (n EQ 4) {		/* extra 2 lines for section menu */

			memsetw(&sctctab[6][0], exp_c(SDBGMM), 64);
			memsetw(&sctctab[7][0], exp_c(SDBGMM), 64);
		}

		stcclr();		/* re-color the cursor background */
		stcpos(sdmrow, sdmcol);	/* turn underline cursor back on */

		if (infield(stcrow, stccol, curfet))
			cfetp = infetp;

		if (v_regs[5] & 0x0180)
			vbank(0);

		for (i = 0; i < 3; i++)	/* top 3 lines of menu */
			vputs(obj8, 3 + i, 0, sdmenus[n][i], SDMENUBG);
/* 
*/
		if (n EQ 4) {		/* extra 2 lines for section menu */

			vputs(obj8, 6, 0, sdmln3, SDMENUBG);
			vputs(obj8, 7, 0, sdmln4, SDMENUBG);

			secop = SOP_NUL;	/* clear section operation code */

			grptran = 0;		/* reset transpose value */
			gtmsel  = -1;		/* de-select source group */

			for (i = 0; i < 12; i++)	/* reset the map */
				grptmap[i] = i;

			dsects();	/* display list of sections */
			dsgtmap();	/* display group map and transpose */

			point = GLCplot;	/* setup to plot on LCD */
			GLCcurs(G_ON);

			if (ismode NE IS_NULL) {	/* cancel inst. mode */

				ismode  = IS_NULL;
				pkctrl  = oldpk;
				sliders = oldsl;
				swpt    = oldsw;
				lcdlbls();
			}

			if (gomode NE GO_NULL) {	/* cancel goto mode */

				gomode = GO_NULL;
				pkctrl = oldpk;
				lseg(GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 0);
			}

			if (asmode) {			/* cancel assign mode */

				asmode = 0;
				pkctrl = oldpk;
				swpt   = oldsw;
				lseg(ASGN_XL, ASGN_Y, ASGN_XR, ASGN_Y, 0);
			}

			if ((pkctrl EQ PK_PFRM) OR (pkctrl EQ PK_NOTE))
				oldpk = pkctrl;

			if (sliders NE LS_LIBR)
				oldsl = sliders;

			oldsw   = swpt;		/* put panel in group mode */
			swpt    = t_ngrp;
			pkctrl  = PK_NGRP;
			sliders = LS_NGRP;
	
			lcdlbls();
			setleds();
		}

		SetPri(TTCURS, TTCPRI);		     /* turn on menu cursor */
		ttcpos(sdmlim[n][0], sdmlim[n][1]);  /* position menu cursor */
/* 
*/
	} else {		/* take down the menu */

		submenu = FALSE;	/* indicate menu has gone bye bye */

		if (v_regs[5] & 0x0180)
			vbank(0);

		objclr(TTCPRI);		/* turn off the menu cursor */
		stcset();		/* restore underline cursor */
		stcclr();		/* ... */
		sdwin(11);		/* redisplay window 11 data */

		if (wasup EQ 4) {

			sdwin(12);		/* redisplay window 12 data */

			stcpos(sdmrow, sdmcol);	/* display underline cursor */

			cxval = CTOX(sdmcol);
			cyval = RTOY(sdmrow);

			if (infield(stcrow, stccol, curfet))
				cfetp = infetp;

			pkctrl  = oldpk;	/* restore panel state */
			sliders = oldsl;
			swpt    = oldsw;

			lcdlbls();		/* refresh LCD */
			setleds();		/* refresh LEDs */

		} else {

			stcpos(sdmrow, sdmcol);	/* display underline cursor */

			cxval = CTOX(sdmcol);
			cyval = RTOY(sdmrow);

			if (infield(stcrow, stccol, curfet))
				cfetp = infetp;
		}
	}
}

/* 
*/

/*
   =============================================================================
	hilitnt() -- setup for a note operation
   =============================================================================
*/

hilitnt(nop)
short nop;
{
	noteop  = nop;		/* set note operation */
	notesel = FALSE;	/* indicate no note selected */
	sdmenu(-1);		/* take down the menu */
	dnedmod();		/* update the note edit window */
}

/* 
*/

/*
   =============================================================================
	hilitop() -- highlight a section operation and set secop
   =============================================================================
*/

hilitop(n)
register short n;
{
	register short row, lc, rc, col;

	if (clkrun)	/* don't do anything if clock is running */
		return;

	if (v_regs[5] & 0x0180)
		vbank(0);

	if (secop NE SOP_NUL) {

		row = sdmht[secop][0];
		lc  = sdmht[secop][1];
		rc  = sdmht[secop][2];

		for (col = lc; col LE rc; col++)
			vputa(obj8, row, col, SDMENUBG);
	}

	secop = n;

	row = sdmht[n][0];
	lc  = sdmht[n][1];
	rc  = sdmht[n][2];

	for (col = lc; col LE rc; col++)
		vputa(obj8, row, col, (SDBGMM | (SD_ENTR << 4)));
}

/* 
*/

/*
   =============================================================================
	dosecop() -- do the current section operation
   =============================================================================
*/

dosecop()
{
	register short sect;
	register struct s_entry *ep;

	if (clkrun OR (-1 EQ (sect = insect()))) {

		secop = SOP_NUL;
		return;
	}

	if (insmode) {

		icancel();
		dsimode();
	}

	switch (secop) {

	case SOP_GO:	/* GoTo */

		if (sgoflag EQ 3) {	/* GoTo End Section */

			ep = hplist[curscor][EH_SEND];

			while (ep) {

				if (ep->e_data1 EQ sect) {

					sc_goto(fc_val = ep->e_time);
					break;
				}

				ep = ep->e_up;
			}

		} else {		/* GoTo Begin Section */

			if (E_NULL NE (ep = seclist[curscor][sect]))
				sc_goto(fc_val = ep->e_time);
		}

		break;
/* 
*/
	case SOP_BGN:	/* Begin */

		if (NOT recsw)		/* only in record mode */
			break;

		if (v_regs[5] & 0x0180)
			vbank(0);

		if (E_NULL NE (ep = seclist[curscor][sect])) {

			eh_rmv(ep, EH_SBGN);
			e_rmv(ep);
			ep->e_time = t_cur;
			p_cur = e_ins(ep, ep_adj(p_cur, 1, t_cur)->e_bak)->e_fwd;
			eh_ins(ep, EH_SBGN);
			se_exec(ep, D_FWD);

		} else  if (E_NULL NE (ep = e_alc(E_SIZE2))) {

			ep->e_time  = t_cur;
			ep->e_type  = EV_SBGN;
			ep->e_data1 = sect;
			p_cur = e_ins(ep, ep_adj(p_cur, 1, t_cur)->e_bak)->e_fwd;
			eh_ins(ep, EH_SBGN);
			seclist[curscor][sect] = ep;
			se_exec(ep, D_FWD);
		}

		sc_refr(fc_val);	/* refresh the screen */
		break;
/* 
*/
	case SOP_END:	/* End */

		if (NOT recsw)		/* only in record mode */
			break;

		if (v_regs[5] & 0x0180)
			vbank(0);

		if (E_NULL NE (ep = ehfind(EH_SEND, -1L, sect, -1))) {

			eh_rmv(ep, EH_SEND);
			e_rmv(ep);
			ep->e_time = t_cur;
			p_cur = e_ins(ep, ep_adj(p_cur, 0, t_cur))->e_fwd;
			eh_ins(ep, EH_SEND);
			se_exec(ep, D_FWD);

		} else  if (E_NULL NE (ep = e_alc(E_SIZE2))) {

			ep->e_time  = t_cur;
			ep->e_type  = EV_SEND;
			ep->e_data1 = sect;
			p_cur = e_ins(ep, ep_adj(p_cur, 0, t_cur))->e_fwd;
			eh_ins(ep, EH_SEND);
			se_exec(ep, D_FWD);
		}

		sc_refr(fc_val);	/* refresh the screen */
		break;
/* 
*/
	case SOP_MOV:	/* Move */

		if (NOT recsw)		/* only in record mode */
			break;

		if (sec_mov(sect))	/* move the section */
			break;

		sc_refr(fc_val);	/* refresh screen if successful */
		break;

	case SOP_CPY:	/* Copy */

		if (NOT recsw)		/* only in record mode */
			break;

		if (sec_cpy(sect))	/* copy the section */
			break;

		sc_refr(fc_val);	/* refresh screen if successful */
		break;

	case SOP_MRG:	/* Merge */

		if (NOT recsw)		/* only in record mode */
			break;

		if (sec_mrg(sect))	/* merge the section */
			break;

		sc_refr(fc_val);	/* refresh screen if successful */
		break;

	case SOP_GRP:	/* ReGrp */

		if (NOT recsw)		/* only in record mode */
			break;

		if (sec_grp(sect))	/* regroup the section */
			break;

		sc_refr(fc_val);	/* refresh screen if successful */
		break;

/* 
*/
	case SOP_STC:	/* SMPTE */

		if (NOT recsw)		/* only in record mode */
			break;

		break;

	case SOP_DGR:	/* Delete Gr */

		if (NOT recsw)		/* only in record mode */
			break;

		if (sec_dgr(sect))	/* delete note events */
			break;

		sc_refr(fc_val);	/* refresh display if successful */
		break;

	case SOP_DEV:	/* Delete Ev */

		if (NOT recsw)		/* only in record mode */
			break;

		if (sec_dev(sect))	/* delete non-note events */
			break;

		sc_refr(fc_val);	/* refresh display if successful */
		break;

	case SOP_RMV:	/* Remove */

		if (NOT recsw)		/* only in record mode */
			break;

		if (sec_rmv(sect))	/* remove section */
			break;

		sc_refr(fc_val);	/* refresh display if successful */
		break;
	}

	secop = SOP_NUL;
}

/* 
*/

/*
   =============================================================================
	clkset() -- set the clock state
   =============================================================================
*/

clkset(st)
register short st;
{
	clkrun = st;

	switch (clksrc) {

	case CK_LOCAL:				/* Local */
	case CK_SMPTE:				/* SMPTE */

		midiclk = FALSE;
		pulsclk = FALSE;

		if (st)
			fc_sw   = 1;
		else
			fc_sw   = 0;

		return;

	case CK_MIDI:				/* MIDI */

		fc_sw   = 0;
		pulsclk = FALSE;

		if (st)
			midiclk = TRUE;
		else
			midiclk = FALSE;

		return;
/* 
*/
	case CK_PLS24:				/* PLS24 */
	case CK_PLS48:				/* PLS48 */
	case CK_PLS96:				/* PLS96 */

		fc_sw   = 0;
		midiclk = FALSE;

		if (st)
			pulsclk = TRUE;
		else
			pulsclk = FALSE;

		return;

	case CK_STEP:				/* Step */

		fc_sw   = 0;
		midiclk = FALSE;
		pulsclk = FALSE;

		return;
	}
}

/* 
*/

/*
   =============================================================================
	sdboxfn() -- process score display box hits
   =============================================================================
*/

short
sdboxfn(n)
short n;
{
	register short	row, col, grp, what, i;
	register struct s_entry *ep;
	short atr, modewas;

	row = hitcy / 14;
	col = hitcx >> 3;

	switch (hitbox) {

	case 0:		/* signature, section, beat, frame */

		if (col EQ 0) {				/* signature */

			if (clkrun)		/* clock can't be running */
				return(FAILURE);

			if (ac_code EQ N_SHARP)
				ac_code = N_FLAT;
			else
				ac_code = N_SHARP;

			sc_refr(fc_val);
			return(SUCCESS);

		} else if ((col GE 2) AND (col LE 4)) {	/* section menu */

			if (clkrun)		/* clock can't be running */
				return(FAILURE);

			if (sdmctl EQ -1) {	/* menu not up yet */

				sdmenu(4);	/* put up the menu */
				return(SUCCESS);
/* 
*/
			} else {		/* menu up */

				if (vtcrow EQ 19) {

					if ((vtccol GE 1) AND
					    (vtccol LE 4)) {	      /* GoTo */

						hilitop(SOP_GO);
						return(SUCCESS);

					} else if ((vtccol GE 8) AND
						   (vtccol LE 11)) {  /* Move */

						hilitop(SOP_MOV);
						return(SUCCESS);

					} else if ((vtccol GE 15) AND
						   (vtccol LE 19)) {  /* SMPTE */

						hilitop(SOP_STC);
						return(SUCCESS);

					} else if ((vtccol GE 24) AND
						   (vtccol LE 62)) {  /* number */

						if (-1 NE insect()) {

							sdmenu(-1);
							dosecop();
						}

						return(SUCCESS);
					}
/* 
*/
				} else if (vtcrow EQ 20) {

					if ((vtccol GE 1) AND
					    (vtccol LE 5)) {	      /* Begin */

						hilitop(SOP_BGN);
						return(SUCCESS);

					} else if ((vtccol GE 8) AND
						   (vtccol LE 11)) {  /* Copy */

						hilitop(SOP_CPY);
						return(SUCCESS);

					} else if ((vtccol GE 15) AND
						   (vtccol LE 20)) {  /* Del Gr */

						hilitop(SOP_DGR);
						return(SUCCESS);

					} else if ((vtccol GE 24) AND
						   (vtccol LE 62)) {  /* number */

						if (-1 NE insect()) {

							sdmenu(-1);
							dosecop();
						}

						return(SUCCESS);
					}
/* 
*/
				} else if (vtcrow EQ 21) {

					if ((vtccol GE 1) AND
					    (vtccol LE 3)) {	      /* End */

						hilitop(SOP_END);
						return(SUCCESS);

					} else if ((vtccol GE 8) AND
						   (vtccol LE 12)) {  /* Merge */

						hilitop(SOP_MRG);
						return(SUCCESS);

					} else if ((vtccol GE 15) AND
						   (vtccol LE 20)) {  /* Del Ev */

						hilitop(SOP_DEV);
						return(SUCCESS);
					}

/* 
*/
				} else if (vtcrow EQ 22) {

					if ((vtccol GE 8) AND
						   (vtccol LE 12)) {  /* ReGrp */

						hilitop(SOP_GRP);
						return(SUCCESS);

					} else if ((vtccol GE 15) AND
					    (vtccol LE 20)) {         /* Remove */

						hilitop(SOP_RMV);
						return(SUCCESS);

					} else if ((vtccol GE 28) AND
						   (vtccol LE 62)) {  /* number */

						if (gtmsel GE 0)
							dsgtmn(gtmsel, FALSE);

						if (-1 NE (gtmsel = ingroup()))
							dsgtmn(gtmsel, TRUE);

						return(SUCCESS);
					}

				} else if (vtcrow EQ 23) {

					if ((vtccol GE 1) AND
					    (vtccol LE 6)) {	      /* Escape */

						secop = SOP_NUL;
						sdmenu(-1);
						return(SUCCESS);

					} else if ((vtccol GE 22) AND
						   (vtccol LE 24)) {	/* Transpose */

						return(enterit());
					}
				}
			}

			return(FAILURE);

		} else
			return(enterit());

/* 
*/
	case 1:		/* insert */

		modewas = insmode;		/* save old insert state */

		if (insmode)			/* toggle ... */
			icancel();		/* ... insert on -> off */
		else
			istart();		/* ... insert of -> on */

		if (modewas NE insmode)		/* only refresh if changed */
			sc_refr(fc_val);

		dsimode();			/* update insert mode display */
		return(SUCCESS);

	case 2:		/* clock control, source */

		if (col LE 33) {				/* clock */

			clkset(NOT clkrun);
			dsclk();
			return(SUCCESS);

/* 
*/
		} else if ((col GE 35) AND (col LE 43)) {	/* control */

			if (sdmctl EQ -1) {	/* menu not yet up */

				sdmenu(3);
				return(SUCCESS);

			} else {		/* menu up */

				if (vtcrow EQ 19) {

					if ((vtccol GE 1) AND
					    (vtccol LE 5)) {

						clkctl = CK_LOCAL;
						sdmenu(-1);
						sdwin(2);
						return(SUCCESS);
					}

				} else if (vtcrow EQ 20) {

					if ((vtccol GE 1) AND
					    (vtccol LE 5)) {

						clkctl = CK_MIDI;
						sdmenu(-1);
						sdwin(2);
						return(SUCCESS);
					}

				} else if (vtcrow EQ 21) {

					if ((vtccol GE 1) AND
					    (vtccol LE 5)) {

						clkctl = CK_SMPTE;
						sdmenu(-1);
						sdwin(2);
						return(SUCCESS);
					}
				}
			}

			return(FAILURE);
/* 
*/
		} else if (col GE 45) {				/* source */

			if (sdmctl EQ -1) {	/* menu not yet up */

				sdmenu(2);
				setleds();
				return(SUCCESS);

			} else {		/* menu up */

				if (vtcrow EQ 19) {

					if ((vtccol GE 1) AND
					    (vtccol LE 5)) {

						clksrc = CK_LOCAL;
						pkctrl = PK_PFRM;
						setleds();
						sdmenu(-1);
						sdwin(2);
						return(SUCCESS);

					} else if ((vtccol GE 9) AND
						   (vtccol LE 13)) {

						clksrc = CK_PLS24;
						pkctrl = PK_PFRM;
						setleds();
						sdmenu(-1);
						sdwin(2);
						return(SUCCESS);

					} else if ((vtccol GE 17) AND
						   (vtccol LE 20)) {

						clksrc 	= CK_STEP;
						pkctrl 	= PK_NOTE;
						stepenb = TRUE;
						setleds();
						sdmenu(-1);
						sdwin(2);
						return(SUCCESS);
					}

/* 
*/
				} else if (vtcrow EQ 20) {

					if ((vtccol GE 1) AND
					    (vtccol LE 5)) {

						clksrc = CK_MIDI;
						pkctrl = PK_PFRM;
						setleds();
						sdmenu(-1);
						sdwin(2);
						return(SUCCESS);

					} else if ((vtccol GE 9) AND
						   (vtccol LE 13)) {

						clksrc = CK_PLS48;
						pkctrl = PK_PFRM;
						setleds();
						sdmenu(-1);
						sdwin(2);
						return(SUCCESS);
					}
/* 
*/
				} else if (vtcrow EQ 21) {

					if ((vtccol GE 1) AND
					    (vtccol LE 5)) {

						clksrc = CK_SMPTE;
						pkctrl = PK_PFRM;
						setleds();
						sdmenu(-1);
						sdwin(2);
						return(SUCCESS);

					} else if ((vtccol GE 9) AND
						   (vtccol LE 13)) {

						clksrc = CK_PLS96;
						pkctrl = PK_PFRM;
						setleds();
						sdmenu(-1);
						sdwin(2);
						return(SUCCESS);
					}
				}
			}
		}

		return(FAILURE);

/* 
*/
	case 3:		/* note edit */

		if (clkrun)		/* clock can't be running */
			return(FAILURE);

		if (scmctl NE -1)
			scmenu(-1);

		if (sdmctl EQ -1) {	/* menu not up */

			sdmenu(1);
			noteop = NOP_NUL;
			return(SUCCESS);

		} else {		/* menu up - select operation */

			if (vtcrow EQ 19) {

				if ((vtccol GE 1) AND
				    (vtccol LE 9)) {		/* Begin Acc */

					hilitnt(NOP_ACC);
					return(SUCCESS);

				} else if ((vtccol GE 13) AND
					   (vtccol LE 21)) {	/* Move Note */

					hilitnt(NOP_MVN);
					return(SUCCESS);

				} else if ((vtccol GE 25) AND
					   (vtccol LE 30)) {	/* Escape */

					hilitnt(NOP_NUL);
					return(SUCCESS);

				}

				return(FAILURE);
/* 
*/
			} else if (vtcrow EQ 20) {

				if ((vtccol GE 1) AND
				    (vtccol LE 9)) {		/* Begin Nat */

					hilitnt(NOP_NAT);
					return(SUCCESS);

				} else if ((vtccol GE 13) AND
					   (vtccol LE 22)) {	/* Move Begin */

					hilitnt(NOP_MVB);
					return(SUCCESS);
				}

				return(FAILURE);

			} else if (vtcrow EQ 21) {

				if ((vtccol GE 1) AND
				    (vtccol LE 8)) {		/* End Note */

					hilitnt(NOP_END);
					return(SUCCESS);

				} else if ((vtccol GE 13) AND
					   (vtccol LE 20)) {	/* Move End */

					hilitnt(NOP_MVE);
					return(SUCCESS);
				}

				return(FAILURE);

			} else
				return(FAILURE);
		}

		return(FAILURE);

/* 
*/
	case 4:		/* assignments */

		if (col LE 9) {

			if (scmctl NE 1) {	/* not up yet -- show page 1 */

				lastam = 0;
				scmenu(1);

			} else {		/* up - switch pages */

				if (lastam EQ 0) {	/* show page 2 */

					lastam = 60;
					scmenu(1);

				} else  {		/* take down menu */

					lastam = 0;
					scmenu(-1);
				}
			}

			return(SUCCESS);

		} else {

			return(enterit());	/* data entry */
		}

	case 5:		/* tunings */

		if ((col GE 14) AND (col LE 17)) {

			if (scmctl NE 2)
				scmenu(2);	/* put up menu */
			else
				scmenu(-1);	/* take down menu */

			return(SUCCESS);

		} else {

			return(enterit());	/* data entry */
		}
/* 
*/
	case 8:		/* stop/next */

		if ((col GE 40) AND (col LE 43)) {	/* stop */

			if (recsw) {

				if (v_regs[5] & 0x0180)
					vbank(0);

				if (E_NULL NE (ep = findev(p_cur, t_cur, EV_STOP, -1, -1))) {

					se_exec(ep, D_FWD);

				} else if (E_NULL NE (ep = e_alc(E_SIZE1))) {

					ep->e_time = t_cur;
					ep->e_type = EV_STOP;
					p_cur= e_ins(ep, ep_adj(p_cur, 0, t_cur))->e_fwd;
					se_exec(ep, D_FWD);
					ctrsw = TRUE;
					se_disp(ep, D_FWD, gdstbc, 1);
					scupd();
				}
			}

		} else if ((col GE 45) AND (col LE 48)) {	/* next */

			if (recsw) {

				if (v_regs[5] & 0x0180)
					vbank(0);

				if (E_NULL NE (ep = findev(p_cur, t_cur, EV_NEXT, -1, -1))) {

					se_exec(ep, D_FWD);

				} else if (E_NULL NE (ep = e_alc(E_SIZE1))) {

					ep->e_time = t_cur;
					ep->e_type = EV_NEXT;
					p_cur= e_ins(ep, ep_adj(p_cur, 0, t_cur))->e_fwd;
					se_exec(ep, D_FWD);
					ctrsw = TRUE;
					se_disp(ep, D_FWD, gdstbc, 1);
					scupd();
				}
			}

		}

		return(SUCCESS);

/* 
*/
	case 9:		/* punch in/out */

		if ((col GE 50) AND (col LE 51)) {	/* punch in */

			if (recsw) {

				if (v_regs[5] & 0x0180)
					vbank(0);

				if (E_NULL NE (ep = findev(p_cur, t_cur, EV_STOP, 1, -1))) {

					se_exec(ep, D_FWD);

				} else if (E_NULL NE (ep = e_alc(E_SIZE1))) {

					ep->e_time = t_cur;
					ep->e_type = EV_PNCH;
					ep->e_data1 = 1;
					p_cur= e_ins(ep, ep_adj(p_cur, 0, t_cur))->e_fwd;
					se_exec(ep, D_FWD);
					ctrsw = TRUE;
					se_disp(ep, D_FWD, gdstbc, 1);
					scupd();
				}
			}

		} else if ((col GE 53) AND (col LE 55)) {	/* punch out */

			if (recsw) {

				if (v_regs[5] & 0x0180)
					vbank(0);

				if (E_NULL NE (ep = findev(p_cur, t_cur, EV_PNCH, 0, -1))) {

					se_exec(ep, D_FWD);

				} else if (E_NULL NE (ep = e_alc(E_SIZE1))) {

					ep->e_time = t_cur;
					ep->e_type = EV_PNCH;
					ep->e_data1 = 0;
					p_cur= e_ins(ep, ep_adj(p_cur, 0, t_cur))->e_fwd;
					se_exec(ep, D_FWD);
					ctrsw = TRUE;
					se_disp(ep, D_FWD, gdstbc, 1);
					scupd();
				}
			}

		}

		return(SUCCESS);
/* 
*/
	case 10:	/* Output */

		return(SUCCESS);

	case 11:	/* group/ins, trans, dyn/loc, velocity */

		if ((row EQ 18) AND (col LE 3)) {		/* inst. menu */

			if (scmctl NE 0)
				scmenu(0);	/* put up the menu */
			else
				scmenu(-1);	/* take down the menu */

			return(SUCCESS);

		} else if ((row EQ 21) AND (col LE 3)) {	/* velocity flag */

			velflag = NOT velflag;
			ds_vmod();
			return(SUCCESS);

		} else
			return(enterit());			/* inst. number */

/* 
*/
	case 12:	/* analog source, value */

		if ((row EQ 23) AND (col LE 4)) {	/* display enable */

			angroup = -angroup;

			if (angroup < 0)
				atr = SDW12ATR;
			else
				atr = (SD_CHNG << 4) | SDBG12;

			if (v_regs[5] & 0x0180)
				vbank(0);

			for (i = 0; i < 4; i++)
				vputa(obj8, 7, i, atr);

			return(SUCCESS);

		} else if ((row EQ 22) AND (col LE 4)) {	/* r/p source */

			ancmsw = NOT ancmsw;

			if (ancmsw)
				atr = (SD_CHNG << 4) | SDBG12;
			else
				atr = SDW12ATR;

			if (v_regs[5] & 0x0180)
				vbank(0);

			for (i = 0; i < 4; i++)
				vputa(obj8, 6, i, atr);

			return(SUCCESS);

		} else {

			return(enterit());
		}
/* 
*/
	case 13:	/* score number and title */

		if (col LE 4) {					/* score menu */

			if (scmctl NE 3)
				scmenu(3);	/* put up menu */
			else
				scmenu(-1);	/* take down menu */

			return(SUCCESS);

		} else if ((col GE 10) AND (col LE 25)) {	/* score name */

			if (sdmctl NE 0) {

				sdmenu(0);

				vtsetup(obj8, svtdsp, 10, scname[curscor], 19, 1,
					advscur, bspscur, nokey, nokey, svtstop,
					SDW13DEA, SDBG13);

			} else {

				vtyper();
			}

			return(SUCCESS);

		} else
			return(enterit());
/* 
*/
	case 14:	/* rec mode */

		if ((col GE 31) AND (col LE 34)) {		/* Play */

			recsw = FALSE;
			dsrpmod();
			return(SUCCESS);

		} else if ((col GE 36) AND (col LE 40)) {	/* Recrd */

			recsw = TRUE;
			dsrpmod();
			return(SUCCESS);

		} else if ((col GE 42) AND (col LE 47)) {	/* OvrDub */

			dubsw = NOT dubsw;
			dsrpmod();
			return(SUCCESS);

		} else if ((col GE 49) AND (col LE 55)) {	/* PunchIn */

			pchsw = NOT pchsw;
			dsrpmod();
			return(SUCCESS);

		}

		return(FAILURE);

	case 16:	/* note display */

		if (NOP_NUL NE noteop)
			donote();

		return(SUCCESS);
	}

	return(FAILURE);
}
