/*
   =============================================================================
	scordsp.c -- MIDAS score display driver
	Version 136 -- 1988-12-08 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"
#include "glcdefs.h"
#include "glcfns.h"
#include "graphdef.h"
#include "lcdline.h"
#include "memory.h"
#include "secops.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "hwdefs.h"
#include "cmeta.h"
#include "macros.h"
#include "panel.h"
#include "smdefs.h"

#include "midas.h"
#include "score.h"
#include "scdsp.h"
#include "scfns.h"
#include "secdefs.h"
#include "neddefs.h"

/* 
*/

extern	unsigned	exp_c();

/* variables defined elsewhere */

extern	short	(*point)();

extern	unsigned *obj0, *obj2, *obj3, *obj8, *obj11, *obj13, *obj14, *obj15;

extern	unsigned	*consl;		/* constant slice pointer */
extern	unsigned	*saddr;		/* score VSDD RAM base pointer */

extern	unsigned	scrl;		/* score VSDD object scroll register image */

extern	short	ancmsw;			/* analog variable r/p control source */
extern	short	angroup;		/* analog variable group being shown */
extern	short	clkctl;			/* clock control */
extern	short	clkrun;			/* clock run switch */
extern	short	clksrc;			/* clock source */
extern	short	cxval;			/* cursor x */
extern	short	cyval;			/* cursor y */
extern	short	dubsw;			/* overdub / replace switch */
extern	short	insmode;		/* insert switch */
extern	short	lastam;			/* last assignment menu page */
extern	short	ndisp;			/* display number */
extern	short	pchsw;			/* punch-in enable switch */
extern	short	recsw;			/* record / play switch */
extern	short	sbase;			/* score VSDD RAM scroll offset */
extern	short	scmctl;			/* score submenu #2 state */
extern	short	sd;			/* score scroll direction */
extern	short	sdmctl;			/* score submenu #1 state */
extern	short	secop;			/* section operation */
extern	short	sgcsw;			/* score graphic / text cursor switch */
extern	short	sliders;		/* slider function */
extern	short	soffset;		/* score scroll offset */
extern	short	stccol;			/* text cursor column */
extern	short	stcrow;			/* text cursor row */
extern	short	submenu;		/* submenu cursor switch */
extern	short	velflag;		/* velocity display enable flag */

extern	char	bfs[];			/* display generation buffer */

extern	char	*nedlbl[];		/* note edit labels */

extern	short	kbobj[];		/* keyboard icon */

extern	short	grpmode[];		/* group mode - 0 = play, 1 = stdby, 2 = rec */
extern	short	grpstat[];		/* group status - 0 = off, non-0 = on */
extern	short	lastvel[];		/* last velocity sent to group */

extern	short	anrs[][16];		/* analog variable resolution */
extern	short	varmode[][16];		/* analog variable record mode */

extern	unsigned	slices[];	/* score display slices */

extern	struct	valent	valents[];	/* s/m value table */

/* 
*/

/* initialized stuff */

short	var2src[6] = {		/* analog variable map */

	SM_HTPW, SM_VTMW, SM_LPBR, SM_CTL1, SM_PED1, SM_KPRS
};

short	simled[3] = {		/* simulated LED colors for group modes */

	0x00A2,		/* play mode    = green */
	0x0062,		/* standby mode = yellow */
	0x0052		/* record mode  = red */
};

char	nsvtab[] = {		/* note value modifier (accidental) table */

	0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1,	/*  21 ..  32 -- A0 .. G#0 */
	0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1,	/*  33 ..  44 -- A1 .. G#1 */
	0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1,	/*  45 ..  56 -- A2 .. G#2 */
	0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1,	/*  57 ..  68 -- A3 .. G#3 */
	0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1,	/*  69 ..  80 -- A4 .. G#4 */
	0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1,	/*  81 ..  92 -- A5 .. G#5 */
	0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1,	/*  93 .. 104 -- A6 .. G#6 */
	0, 1, 0, 0				/* 105 .. 108 -- A7 .. C7  */
};

short	scorpal[16][3] = {		/* score palette */

	{0, 0, 0},	/*  0 - black */
	{2, 3, 3},	/*  1 - white */
	{0, 0, 1},	/*  2 - dark blue #1 */
	{2, 0, 2},	/*  3 - dark violet */
	{0, 0, 3},	/*  4 - dark blue #2 */
	{3, 0, 0},	/*  5 - red */
	{3, 3, 0},	/*  6 - yellow */
	{1, 2, 0},	/*  7 - dark green */
	{0, 3, 3},	/*  8 - light blue */
	{3, 2, 0},	/*  9 - tan */
	{0, 3, 0},	/* 10 - light green */
	{3, 1, 2},	/* 11 - light violet */
	{0, 2, 3},	/* 12 - medium blue */
	{2, 0, 0},	/* 13 - dark red */
	{2, 2, 1},	/* 14 - ivory */
	{3, 3, 3}	/* 15 - bright white */
};

/* 
*/

char	*clklbl[] = {	/* clock control and scource labels */

	"Local",	/* 0 - CK_LOCAL */
	"MIDI ",	/* 1 - CK_MIDI  */
	"SMPTE",	/* 2 - CK_SMPTE */
	"PLS24",	/* 3 - CK_PLS24 */
	"PLS48",	/* 4 - CK_PLS48 */
	"PLS96",	/* 5 - CK_PLS96 */
	"Step "		/* 6 - CK_STEP  */
};

char	*sdisp[] = {	/* score display fixed text strings */

	/* 0 */		"\230\231\232\233     ",	/* Interpolate */

	/* 1 */		"\200\201\202\203 1    2    3    4    5    6    7    8    9    \254    \255    \256    ",
			/* Group / Instrument */

	/* 2 */		"\220\221\222\223  Pch/Hor  Mod/Vrt  Brth/LP  GPC/CV1  Pedal 1  Key Prs Group",
			/* Analog Source */

	/* 3 */		"\015\016\017 Play Recrd OvrDub PunchIn ",
			/* Rec Mode */

	/* 4 */		"\204\205\206\207",	/* Transpose */

	/* 5 */		"\210\211\212\213   /    /    /    /    /    /    /    /    /    /    /    / ",
			/* Dynamics / Location */
			
	/* 6 */		"\214\215\216\217",	/* Velocity */

	/* 7 */		"\224\225\226\227  0/+00.0  0/+00.0  0/+00.0  0/+00.0  0/+00.0  0/+00.0 # 00 ",
			/* Resolution / Value */

	/* 8 */		"                                                            "


};

/* 
*/

/*
   =============================================================================
	makeln(lnc, lec) -- create the constant slice at 'consl'.
	make the line object color 'lnc', and ledger line color 'lec', 
   =============================================================================
*/

makeln(lnc, lew)
register unsigned lnc, lew;
{
	register unsigned *lp, *cp;
	register short i;
	register unsigned lbg;

	lp = obj15;			/* setup line object pointer */
	cp = consl;			/* setup constant slice pointer */

	lew = exp_c(lew);		/* create ledger line constant */

	lnc &= 0x000F;			/* mask cursor line color */

/* 
*/	
	for (i = 0; i < 224; i++) {

		/* stuff data into line object */

		if ((i EQ  42) OR	/* top ledger line */
		    (i EQ  50) OR	/* top ledger line */
		    (i EQ  58) OR	/* top ledger line */
		    (i EQ  66) OR	/* top ledger line */
		    (i EQ 162) OR	/* bottom ledger line */
		    (i EQ 170) OR	/* bottom ledger line */
		    (i EQ 178) OR	/* bottom ledger line */
		    (i EQ 186)) {

			lbg = lew;
			*lp++ = lbg & 0xFFF0;
			*lp++ = lbg;
			*lp++ = lbg;
			*lp++ = lbg;

		} else if (i > 209) {	/* new data area */

			lbg = 0;
			*lp++ = lbg & 0xFFF0;
			*lp++ = lbg;
			*lp++ = lbg;
			*lp++ = lbg;

		} else {

			lbg = 0;
			*lp++ = lbg & 0xFFF0;
			*lp++ = lbg;
			*lp++ = (lbg & 0xFFF0) | lnc;
			*lp++ = lbg;
		}

		*cp++ = 0;		/* clear a line in the constant slice */
	}

/* 
*/

	consl[74]  = lew;		/* constant slice treble clef lines */
	consl[82]  = lew;
	consl[90]  = lew;
	consl[98]  = lew;
	consl[106] = lew;

	consl[122] = lew;		/* constant slice bass clef lines */
	consl[130] = lew;
	consl[138] = lew;
	consl[146] = lew;
	consl[154] = lew;

	consl[210] = exp_c(NDMC);	/* constant slice new data margin line */
}

/* 
*/

/*
   =============================================================================
	ds_vmod() -- display velocity mode
   =============================================================================
*/

ds_vmod()
{
	if (ndisp NE 2)
		return;

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputs(obj8, 5, 0, sdisp[6],
		velflag ? ((SD_CHNG << 4) | SDBG11) : SDW11ATR);
}

/*
   =============================================================================
	ds_vel() -- display velocities
   =============================================================================
*/

ds_vel()
{
	register short i;
	char buf[6];

	if (ndisp NE 2)
		return;

	if (v_regs[5] & 0x0180)
		vbank(0);

	ds_vmod();

	for (i = 0; i < 12; i++) {

		sprintf(buf, "%03d", lastvel[i] / 252);
		vputs(obj8, 5, (i * 5) + 6, buf, SDW11ATR);
	}
}

/* 
*/

/*
   =============================================================================
	dsvmode() -- display variable mode
   =============================================================================
*/

dsvmode(n)
short n;
{
	if (ndisp NE 2)
		return;

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputc(obj8, 6, 5 + (n * 9), '*', simled[varmode[n][abs(angroup) - 1]]);
}


/*
   =============================================================================
	ds_anmd() -- display mode for all variables
   =============================================================================
*/

ds_anmd()
{
	register short i;

	if (ndisp NE 2)
		return;

	for (i = 0; i < 6; i++)
		dsvmode(i);
}

/* 
*/

/*
   =============================================================================
	ds_angr() -- display analog variable group number
   =============================================================================
*/

ds_angr()
{
	char buf[4];

	if (ndisp NE 2)
		return;

	if (v_regs[5] & 0x0180)
		vbank(0);

	sprintf(buf, "# %2.2d", abs(angroup));
	vputs(obj8, 7, 59, buf, SDW12ATR);
}

/* 
*/

/*
   =============================================================================
	ds_anrs() -- display analog resolution
   =============================================================================
*/

ds_anrs()
{
	register short i;
	char buf[3];

	if (ndisp NE 2)
		return;

	if (v_regs[5] & 0x0180)
		vbank(0);

	buf[1] = '/';
	buf[2] = '\0';

	for (i = 0; i < 6; i++) {

		buf[0] = '0' + anrs[i][abs(angroup) - 1];
		vputs(obj8, 7, 6 + (i * 9), buf, SDW12ATR);
	}
}

/* 
*/

/*
   =============================================================================
	dsanval() -- display analog value
   =============================================================================
*/

dsanval(var)
short var;
{
	register short tmp, val, v1, v2;
	char buf[16];

	if (ndisp NE 2)
		return;

	if (v_regs[5] & 0x0180)
		vbank(0);

	val = valents[((abs(angroup) - 1) << 4) | var2src[var]].val >> 5;
	tmp = abs(val);	
	v1 = tmp / 100;
	v2 = (tmp - (v1 * 100)) / 10;
	sprintf(buf, "%c%02d.%1d", ((val < 0) ? '-' : '+'), v1, v2);
	vputs(obj8, 7, 8 + (var * 9), buf, SDW12ATR);
}

/*
   =============================================================================
	ds_anvl() -- display analog values
   =============================================================================
*/

ds_anvl()
{
	register short i;

	if (ndisp NE 2)
		return;

	for (i = 0; i < 6; i++)
		dsanval(i);
}

/* 
*/

/*
   =============================================================================
	uclk() -- underline clock label
   =============================================================================
*/

uclk()
{
	if (sliders NE LS_VOICE) {

		point = GLCplot;

		GLCcurs(G_ON);

		if (clkrun)
			lseg(CK_XL, CK_Y, CK_XR, CK_Y, 1);	/* clock on */
		else
			lseg(CK_XL, CK_Y, CK_XR, CK_Y, 0);	/* clock off */

		GLCcurs(G_OFF);
	}
}

/* 
*/

/*
   =============================================================================
	dclkmd() -- display clock mode
   =============================================================================
*/

dclkmd()
{
	uclk();

	if (ndisp NE 2)
		return;

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputs(obj8, 0, 29, "Clock",
		clkrun ? ((SD_CHNG << 4) | SDBG02) : SDW02ATR);
}

/* 
*/

/*
   =============================================================================
	dsclk() -- display clock status, control, and source
   =============================================================================
*/

dsclk()
{
	short	atr[26];

	uclk();

	if (ndisp NE 2)
		return;

	memsetw(atr, SDW02ATR, 26);

	if (clkrun)
		memsetw(atr, (SDBG02 | (SD_CHNG << 4)), 5);

	sprintf(bfs, "Clock Con %5.5s Src %5.5s ",
		clklbl[clkctl], clklbl[clksrc]);

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputsa(obj8, 0, 29, bfs, atr);
}

/* 
*/

/*
   =============================================================================
	dssect() -- display current score section
   =============================================================================
*/

dssect()
{
	if (ndisp NE 2)
		return;

	sprintf(bfs, "%02d", cursect + 1);

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputs(obj8, 0, 6, bfs, SDW01ATR);
}

/* 
*/

/*
   =============================================================================
	dslocn() -- display current score location
   =============================================================================
*/

dslocn()
{
	register long curfr, beat;
	register short frame;

	if (ndisp NE 2)
		return;

	curfr = t_ctr;
	beat  = curfr / 48;
	frame = curfr % 48;

	sprintf(bfs, "%05ld F %02d", beat, frame);

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputs(obj8, 0, 11, bfs, SDW00ATR);
}

/* 
*/

/*
   =============================================================================
	dnedmod() -- display note edit mode
   =============================================================================
*/

dnedmod()
{
	if (ndisp NE 2)
		return;

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputs(obj8, 0, 55, nedlbl[noteop],
		(notesel ? ((SD_CHNG << 4) | SDBG03) : SDW03ATR) |
		(noteop EQ NOP_NUL ? 0 : C_BLINK));
}

/*
   =============================================================================
	dsmem() -- display remaining memory 'units'  (long words)
   =============================================================================
*/

dsmem()
{
	if (ndisp NE 2)
		return;

	sprintf(bfs, "M %05ld", evleft());

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputs(obj8, 8, 57, bfs, SDW15ATR);
}

/* 
*/

/*
   =============================================================================
	dsgstat() -- display group status
   =============================================================================
*/

dsgstat(n)
short n;
{
	char c;

	if (ndisp NE 2)
		return;

	if (v_regs[5] & 0x0180)
		vbank(0);

	c = 1 + n + (grpstat[n] ? ((n > 8) ? 162 : '0') : 192);
	vputc(obj8, 2, 5 + (n * 5), c, (((n + 3) << 4) | 2));
}

/*
   =============================================================================
	dsgmode() -- display group mode
   =============================================================================
*/

dsgmode(n)
short n;
{
	if (ndisp NE 2)
		return;

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputc(obj8, 2, 6 + (n * 5), '*', simled[grpmode[n]]);
}

/* 
*/

/*
   =============================================================================
	dsgmods() -- display mode and status for all groups
   =============================================================================
*/

dsgmods()
{
	register short i;

	if (ndisp NE 2)
		return;

	for (i = 0; i < 12; i++) {

		dsgstat(i);
		dsgmode(i);
	}
}

/*
   =============================================================================
	dsgmodz() -- display modes for all groups and variables
   =============================================================================
*/

dsgmodz()
{
	register short i;

	if (ndisp NE 2)
		return;

	for (i = 0; i < 12; i++)	/* group modes */
		dsgmode(i);

	for (i = 0; i < 6; i++)		/* variable modes */
		dsvmode(i);
}

/* 
*/

/*
   =============================================================================
	dsrpmod() -- display record / play modes
   =============================================================================
*/

dsrpmod()
{
	short	atr[25];

	switch (sliders) {

	case LS_PRMTR:
	case LS_OTHER:
	case LS_EQ:
	case LS_NGRP:

		point = GLCplot;

		GLCcurs(G_ON);

		if (recsw) {

			lseg(RP_RL, RP_Y, RP_RR, RP_Y, 1);	/* record */
			lseg(RP_PL, RP_Y, RP_PR, RP_Y, 0);

		} else {

			lseg(RP_RL, RP_Y, RP_RR, RP_Y, 0);	/* play */
			lseg(RP_PL, RP_Y, RP_PR, RP_Y, 1);
		}

		GLCcurs(G_OFF);
	}
/* 
*/
	if (ndisp NE 2)
		return;

	memsetw(atr, SDW14ATR, 25);

	if (recsw)	/* record */
		memsetw(&atr[5], (C_BLINK | SDBG14 | (SD_CHNG << 4)), 5);
	else		/* play */
		memsetw(&atr[0], (SDBG14 | (SD_CHNG << 4)), 4);

	if (dubsw)	/* overdub */
		memsetw(&atr[11], (SDBG14 | (SD_CHNG << 4)), 6);

	if (pchsw)	/* punch in enable */
		memsetw(&atr[18], (SDBG14 | (SD_CHNG << 4)), 7);

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputsa(obj8, 8, 31, "Play Recrd OvrDub PunchIn", atr);
}

/* 
*/

/*
   =============================================================================
	dsimode() -- display insert mode
   =============================================================================
*/

dsimode()
{
	short	atr[7];

	if (ndisp NE 2)
		return;

	if (insmode)
		memsetw(atr, (SDBG01 | (SD_CHNG << 4)), 7);
	else
		memsetw(atr, SDW01ATR, 7);

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputsa(obj8, 0, 22, "Insert ", atr);
}

/* 
*/

/*
   =============================================================================
	sdwin() -- fill in a score display window
   =============================================================================
*/

sdwin(n)
short n;
{
	short atr[64], atrib, i;

	if (ndisp NE 2)			/* only if score is selected */
		return;

	if (v_regs[5] & 0x0180)		/* select bank 0 */
		vbank(0);

	switch (n) {			/* dispatch off of window number */

	case 0:		/* score location */

		vputs(obj8, 0,  0,
			ac_code EQ N_SHARP ? "\250 " : "\251 ",
			SDW00ATR);

		vputs(obj8, 0,  2, "Sec    ",		SDW01ATR);
		vputs(obj8, 0,  9, "B       F    ",	SDW00ATR);

		dssect();		/* display current section */
		dslocn();		/* display score location */
		return;

	case 1:		/* insert mode */

		dsimode();		/* display score insert mode */
		return;

	case 2:		/* clock */

		dsclk();		/* display clock status */
		return;

	case 3:		/* note edit */

		dnedmod();
		return;
/* 
*/
	case 4:		/* assignment */

		vputs(obj8, 1,  0, "Assignment    ", SDW04ATR);
		rd_ioas(0);		/* display assignment table number */
		return;

	case 5:		/* tuning */

		vputs(obj8, 1, 14, "Tune   ", SDW05ATR);
		rd_tune(0);		/* display tuning table number */
		return;

	case 6:		/* tempo */

		vputs(obj8, 1, 21, "Tempo     ", SDW06ATR);
		rd_tmpo(0);		/* display tempo */
		return;

	case 7:		/* interpolate */

		vputs(obj8, 1, 31, "Int     ", SDW07ATR);
		rd_intp(0);
		return;

	case 8:		/* stop/next */

		vputs(obj8, 1, 40, "Stop/Next ", SDW08ATR);
		return;
/* 
*/

	case 9:		/* punch in/out */

		vputs(obj8, 1, 50, "In/Out ", SDW09ATR);
		return;

	case 10:	/* output */

		vputs(obj8, 1, 57, "Output ", SDW10ATR);
		return;

	case 11:	/* group/instr, transposition, dyn/loc, velocity */

		vputs(obj8, 2,  0, sdisp[1], SDW11ATR);
		dsgmods();		/* display mode and status for all groups */
		ds_inst();		/* display instruments */

		vputs(obj8, 3,  4, sdisp[8], SDW11ATR);
		vputs(obj8, 3,  0, sdisp[4], SDW11ATR);
		ds_trns();		/* display transpositions */

		vputs(obj8, 4,  4, sdisp[8], SDW11ATR);	/* dyn/loc */
		vputs(obj8, 4,  0, sdisp[5], SDW11ATR);
		ds_dyn();		/* display dynamics */
		ds_loc();		/* display locations */

		vputs(obj8, 5,  4, sdisp[8], SDW11ATR);	/* velocity */
		vputs(obj8, 5,  0, sdisp[6], SDW11ATR);
		ds_vel();		/* display velocities */

		return;
/* 
*/
	case 12:	/* analog variables */

		vputs(obj8, 6,  0, sdisp[2], SDW12ATR);
		vputs(obj8, 7,  0, sdisp[7], SDW12ATR);

		if (angroup < 0)
			atrib = SDW12ATR;
		else
			atrib = (SD_CHNG << 4) | SDBG12;

		for (i = 0; i < 4; i++)
			vputa(obj8, 7, i, atrib);

		if (ancmsw)
			atrib = (SD_CHNG << 4) | SDBG12;
		else
			atrib = SDW12ATR;

		for (i = 0; i < 4; i++)
			vputa(obj8, 6, i, atrib);

		ds_anmd();
		ds_angr();
		ds_anrs();
		ds_anvl();

		return;

	case 13:	/* score */

		sprintf(bfs, "Score  %02.2d %16.16s ",
			curscor + 1, scname[curscor]);

		vputs(obj8, 8,  0, bfs, SDW13ATR);
		return;

	case 14:	/* record/play, overdub, punchin */

		vputs(obj8, 8, 27, sdisp[3], SDW14ATR);		/* Rec Mode */
		dsrpmod();		/* display rec/play modes */
		return;

	case 15:	/* memory */

		dsmem();		/* display available events */
		return;
	}
}

/* 
*/

/*
   =============================================================================
	sdwins() -- display all score windows
   =============================================================================
*/

sdwins()
{
	register short i;

	if (ndisp NE 2)			/* only if score display is selected */
		return;

	for (i = 0; i < 17; i++)	/* refresh each of the windows */
		sdwin(i);
}

/* 
*/

/*
   =============================================================================
	sdsetup() -- setup the score display
   =============================================================================
*/

sdsetup()
{
	dswap();		/* clear the video display */

	sd     	= D_FWD;	/* display direction = forward */
	noteop 	= NOP_NUL;	/* no pending note edit operation */
	notesel = FALSE;	/* no note selected */
	secop  	= SOP_NUL;	/* no pending section edit operation */
	scmctl  = -1;		/* no area 1 menu up */
	sdmctl 	= -1;		/* no area 2 menu up */
	submenu = FALSE;	/* submenu cursor not enabled */
	lastam  =  0;		/* reset assignment menu page */

	clrsctl();		/* clear out the slices */

	/* setup object pointers */

	obj0  = v_curs0;		/*  0 - cursor (arrow) */
	obj2  = v_tcur;			/*  2 - cursor (typewriter) */
	obj3  = v_cur;			/*  3 - cursor (underline) */
	obj8  = v_ct0;			/*  8 - character text */
	obj11 = v_win0;			/* 11 - window */
	obj13 = v_kbobj;		/* 13 - keyboard */
	obj14 = v_score;		/* 14 - score */
	obj15 = v_lnobj;		/* 15 - line */

	/* ---------------- initialize object table -------------------- */
	/*     obj,  typ, bnk,  base, xpix, ypix,  x0,  y0,   flags, pri */

	SetObj(  0,    0,   1,  obj0,   16,   16, C1X, C1Y, OBFL_00, -1);
	SetObj(TTCURS, 0,   1,  obj2,   16,   16,   0,   0, TTCCFL,  -1);
	SetObj(  3,    0,   1,  obj3,  512,   18,   0,   0, OBFL_01, -1);
	SetObj(  8,    1,   0,  obj8,  512,  108,   0,   0, OBFL_08, -1);
	SetObj( 11,    0,   1, obj11,  512,  224,   0,  14, OBFL_11, -1);
	SetObj( 13,    0,   1, obj13,   16,  224,   0,  14, OBFL_13, -1);
	SetObj( 14,    0,   0, obj14,  512,  224,   0,  14, OBFL_14, -1);
	SetObj( 15,    0,   1, obj15,   16,  224, 248,  14, OBFL_15, -1);

	if (v_regs[5] & 0x0180)			/* select bank 0 */
		vbank(0);

	memsetw(obj14, 0, 32767);		/* clear score object */
	memsetw(obj14+32767L, 0, 24577);

	vbank(1);				/* select bank 1 */

	stcset();				/* setup text cursor colors */
	stcclr();				/* clear text cursors */
	arcurs(SDCURSR);			/* setup arrow cursor */
	ttcini(SDCURSR);			/* setup typewriter cursor */

/* 
*/
	if ((v_regs[5] & 0x0180) NE 0x0100)
		vbank(1);			/* select bank 1 */

	memcpyw(obj13, kbobj, 896);		/* setup 'keyboard' object */
	makeln(LN_VRT, LN_HOR);			/* setup 'lines' object */
	sdwins();				/* fill in the windows */

	/* display some objects */

	SetPri( 3, 12);		/* Underline cursors */
	objclr(12);
	objon(12,  12, 2);	/* ... row  0 */
	objon(12, 250, 2);	/* ... row 17 */
	objon(12, 264, 2);	/* ... row 18 */
	objon(12, 278, 2);	/* ... row 19 */
	objon(12, 292, 2);	/* ... row 20 */
	objon(12, 306, 2);	/* ... row 21 */
	objon(12, 320, 2);	/* ... row 22 */
	objon(12, 334, 2);	/* ... row 23 */
	objon(12, 348, 2);	/* ... row 24 */

	SetPri( 8,  7);		/* Character Text */
	objclr(7);
	objon(7,   0, 12);	/* ... row  0 */
	objon(7, 238, 12);	/* ... row 17 */
	objon(7, 252, 12);	/* ... row 18 */
	objon(7, 266, 12);	/* ... row 19 */
	objon(7, 280, 12);	/* ... row 20 */
	objon(7, 294, 12);	/* ... row 21 */
	objon(7, 308, 12);	/* ... row 22 */
	objon(7, 322, 12);	/* ... row 23 */
	objon(7, 336, 12);	/* ... row 24 */
	
	SetPri(13,  2);		/* Keyboard object */
	SetPri(14,  1);		/* Score object */
	SetPri(15,  0);		/* Line object */

	vsndpal(scorpal);	/* set the palette */

	SetPri( 0, 15);		/* Turn on arrow cursor */
	setgc(C1X, C1Y);

	sc_refr(t_cur);		/* position the score on the display */
}
