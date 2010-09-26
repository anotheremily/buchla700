/*
   =============================================================================
	lcdlbls.c -- MIDAS-VII front panel support functions
	Version 48 -- 1989-12-19 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	ROMPOK		1		/* non-zero enables the ROMP switch */

#include "stddefs.h"
#include "biosdefs.h"
#include "graphdef.h"
#include "glcfns.h"
#include "glcdefs.h"
#include "lcdline.h"
#include "vsdd.h"
#include "vsddvars.h"
#include "hwdefs.h"
#include "memory.h"
#include "cmeta.h"
#include "fields.h"
#include "fpu.h"
#include "macros.h"
#include "panel.h"
#include "patch.h"
#include "smdefs.h"
#include "sclock.h"

#include "midas.h"
#include "instdsp.h"
#include "libdsp.h"
#include "score.h"
#include "scdsp.h"
#include "scfns.h"

#define	ARTIVAL(x)	(x << 5)
#define	SL2GAIN(x)	(sl2gain[(x >> 11) + 16])

#if	ROMPOK
#define	ROMPFN	trapper		/* ROMP switch table entry */
#else
#define	ROMPFN	nokey		/* ROMP switch table entry */
#endif

#define	PCHMAX	21920L		/* maximum pitch value */

/* 
*/
extern	char	gain2eq();

extern	short	nokey();
extern	short	setsr();

extern	short	(*d_key)();
extern	short	(*e_key)();
extern	short	(*m_key)();
extern	short	(*x_key)();
extern	short	(*olddkey)();
extern	short	(*oldekey)();
extern	short	(*oldmkey)();
extern	short	(*oldxkey)();
extern	short	(*point)();

extern	PFS	(*oldsw)[];
extern	PFS	(*swpt)[];

extern	PFS	aswtbl[14];

extern	short	asig;
extern	short	asmode;
extern	short	astat;
extern	short	aval;
extern	short	clkrun;
extern	short	ctrsw;
extern	short	curasg;
extern	short	gomode;
extern	short	ismode;
extern	short	lampio;
extern	short	lampsw;
extern	short	loadrow;
extern	short	loadsw;
extern	short	ltagged;
extern	short	ndisp;
extern	short	oldpk;
extern	short	oldsl;
extern	short	pkctrl;
extern	short	recsw;
extern	short	sliders;
extern	short	tagslot;
extern	short	tmpomlt;
extern	short	tmpoval;
extern	short	tuneval;

extern	unsigned short	seqdupd;

extern	short	BGeq[];
extern	short	BGother[];
extern	short	BGprmtr[];

extern	short	articen[];
extern	short	grpmode[];
extern	short	grpsel[];
extern	short	grpstat[];
extern	short	ins2grp[];
extern	short	lastart[];
extern	short	s_trns[];
extern	short	vce2grp[];
extern	short	vce2trg[];

extern	short	anrs[][16];

extern	long	iotime;
extern	long	lcdontm;
extern	long	lcdtime;

extern	struct	gdsel	*gdstbc[];

extern	struct	valent	valents[];

/* 
*/

/* forward references */

short	l_prmtr(), l_other(), l_voice(), l_init(), l_clock(), l_eq();
short	trapper(), setlamp(), rpctl(), quiet();
short	l_group(), l_dflt(), l_inst(), l_goto(), l_asgn(), l_adfl(), l_load();

/* initialized stuff */

/* BarType -- LCD slider bar graph type table -- 0 = bottom, 1 = centered */

short	BarType[14] = { 1, 0,   0, 0, 0,   1, 1, 1, 1,   0, 0, 0,   1, 0 };

short	artitab[] = {		/* articulation pot initialization values */

	ARTIVAL(500),	/* locn  */
	ARTIVAL(0),	/* level */
	ARTIVAL(0),	/* ind 1 */
	ARTIVAL(0),	/* ind 2 */
	ARTIVAL(0),	/* ind 3 */
	ARTIVAL(500), 	/* frq 1 */
	ARTIVAL(500), 	/* frq 2 */
	ARTIVAL(500),	/* frq 3 */
	ARTIVAL(500),	/* frq 4 */
	ARTIVAL(0),	/* ind 4 */
	ARTIVAL(0),	/* ind 5 */
	ARTIVAL(0),	/* ind 6 */
	ARTIVAL(500),	/* filtr */
	ARTIVAL(0)	/* reson */
};

/* arpar[] -- articulation parameter map */
/*	   sigtab[ 25  26  27  28  29  30  31  32  33  34  35  36  37  38] */ 

short	arpar[] = { 4,  2,  9, 11, 12,  1,  3,  5,  7, 13, 14, 15, 10,  6};

/* 
*/

short	sl2gain[32] = {		/* slider value to EQ gain conversion table */

	-12, -12, -12, -12, -12, -11, -10, -9 ,-8 ,-7, -6, -5, -4, -3, -2, -1,
	  0,   1,   2,   3,   4,   5,   6,  7,  8,  9, 10, 11, 12, 12, 12, 12
};

short	vmasks[16] = {		/* variable resolution masks */

	0x8000, 0xC000, 0xE000, 0xF000,
	0xF800, 0xFC00, 0xFE00, 0xFF00,
	0xFF80, 0xFFC0, 0xFFE0, 0xFFF0,
	0xFFF8, 0xFFFC, 0xFFFE, 0xFFFF
};

short	src2var[14] = {		/* source number to analog variable number map */

	-1, -1,  3, -1, -1, -1,  5, -1,  4, -1, -1,  0,  1,  2 };

char	*potlbls[] = {		/* LCD pot labels */

	/* Pot:  25    26    27    28    29    30    31    32    33    34    35    36    37    38 */

	/* 0 */	"Locn  Level Ind 1 Ind 2 Ind 3 Frq 1 Frq 2 Frq 3 Frq 4 Ind 4 Ind 5 Ind 6 Filtr Reson",
	/* 1 */	"Locn  Level Ind 1 Ind 2 Ind 3 Frq 1 Frq 2 Frq 3 Frq 4 Ind 4 Ind 5 Ind 6 Filtr Reson",
	/* 2 */	"      Aux   Depth Rate  Inten CV 1  CV 2  CV 3  CV 4                               ",
	/* 3 */	" 50    150   400    1k  2.5k    6k   15k   50    150   400    1k  2.5k    6k   15k ",
	/* 4 */	"Locn  Level Ind 1 Ind 2 Ind 3 Frq 1 Frq 2 Frq 3 Frq 4 Ind 4 Ind 5 Ind 6 Filtr Reson",
	/* 5 */	"Locn  Level Ind 1 Ind 2 Ind 3 Frq 1 Frq 2 Frq 3 Frq 4 Ind 4 Ind 5 Ind 6 Filtr Reson",
	/* 6 */	"Locn  Level Ind 1 Ind 2 Ind 3 Frq 1 Frq 2 Frq 3 Frq 4 Ind 4 Ind 5 Ind 6 Filtr Reson",
	/* 7 */	"Locn  Level Ind 1 Ind 2 Ind 3 Frq 1 Frq 2 Frq 3 Frq 4 Ind 4 Ind 5 Ind 6 Filtr Reson"
};

#if	ROMPOK
char	*swtlbls[] = {		/* LCD switch labels -- ROMP ENABLED */

	/* 0 */	"Quiet ROMP  Lamp  Clock  P/R  Go To Instr Asgmt Load              Other Voice Init ",
	/* 1 */	"                                                                        Voice Init ",
	/* 2 */	"Quiet ROMP  Lamp  Clock  P/R  Go To Instr Asgmt Load               EQ   Voice Init ",
	/* 3 */	"Quiet ROMP  Lamp  Clock  P/R  Go To Instr Asgmt Load              Prmtr Voice Init ",
	/* 4 */	"                                                                         +00  Deflt",
	/* 5 */	"Quiet ROMP  Lamp  Clock  P/R  Go To Instr Asgmt Load              Other Voice Deflt",
	/* 6 */	"Quiet ROMP  Lamp  Clock  P/R                                                       ",
	/* 7 */	"Quiet ROMP  Lamp                                Load                               "
};
#else
char	*swtlbls[] = {		/* LCD switch labels -- ROMP DISABLED */

	/* 0 */	"Quiet       Lamp  Clock  P/R  Go To Instr Asgmt Load              Other Voice Init ",
	/* 1 */	"                                                                        Voice Init ",
	/* 2 */	"Quiet       Lamp  Clock  P/R  Go To Instr Asgmt Load               EQ   Voice Init ",
	/* 3 */	"Quiet       Lamp  Clock  P/R  Go To Instr Asgmt Load              Prmtr Voice Init ",
	/* 4 */	"                                                                         +00  Deflt",
	/* 5 */	"Quiet       Lamp  Clock  P/R  Go To Instr Asgmt Load              Other Voice Deflt",
	/* 6 */	"Quiet       Lamp  Clock  P/R                                                       ",
	/* 7 */	"Quiet       Lamp                                Load                               "
};
#endif

char	*aslbls[] = {		/* Assignment select key labels */

	"Asgmt",	/* 0 */
	" +00 ",	/* 1 */
	" +20 ",	/* 2 */
	" +40 ",	/* 3 */
	" +60 ",	/* 4 */
	" +80 "		/* 5 */
};

/* 
*/

PFS	t_prmtr[] = {	/* switch assignments for LS_PRMTR */

	quiet,		/*  0: Quiet */
	ROMPFN,		/*  1: ROMP */

	setlamp,	/*  2: Lamp */
	l_clock,	/*  3: Clock */
	rpctl,		/*  4: R/P */

	l_goto,		/*  5: Go To */
	l_inst,		/*  6: Instr */
	l_asgn,		/*  7: Asgmt */
	l_load,		/*  8: Load */

	nokey,		/*  9: -unused- */
	nokey,		/* 10: -unused- */
	l_other,	/* 11: Other */

	l_prmtr,	/* 12: Prmtr */
	l_init		/* 13: Init */
};

PFS	t_voice[] = {	/* switch assignments for LS_VOICE */

	l_voice,	/*  0: toggle voice status */
	l_voice,	/*  1: toggle voice status */

	l_voice,	/*  2: toggle voice status */
	l_voice,	/*  3: toggle voice status */
	l_voice,	/*  4: toggle voice status */

	l_voice,	/*  5: toggle voice status */
	l_voice,	/*  6: toggle voice status */
	l_voice,	/*  7: toggle voice status */
	l_voice,	/*  8: toggle voice status */

	l_voice,	/*  9: toggle voice status */
	l_voice,	/* 10: toggle voice status */
	l_voice,	/* 11: toggle voice status */

	l_prmtr,	/* 12: Prmtr */
	l_init		/* 13: Init */
};

/* 
*/

PFS	t_other[] = {	/* switch assignments for LS_OTHER */

	quiet,		/*  0: Quiet */
	ROMPFN,		/*  1: ROMP */

	setlamp,	/*  2: Lamp */
	l_clock,	/*  3: Clock */
	rpctl,		/*  4: R/P */

	l_goto,		/*  5: Go To */
	l_inst,		/*  6: Instr */
	l_asgn,		/*  7: Asgmt */
	l_load,		/*  8: Load */

	nokey,		/*  9: -unused- */
	nokey,		/* 10: -unused- */
	l_eq,		/* 11: EQ */

	l_prmtr,	/* 12: Prmtr */
	l_init		/* 13: Init */
};

PFS	t_eq[] = {	/* switch assignments for LS_EQ */

	quiet,		/*  0: Quiet */
	ROMPFN,		/*  1: ROMP */

	setlamp,	/*  2: Lamp */
	l_clock,	/*  3: Clock */
	rpctl,		/*  4: R/P */

	l_goto,		/*  5: Go To */
	l_inst,		/*  6: Instr */
	l_asgn,		/*  7: Asgmt */
	l_load,		/*  8: Load */

	nokey,		/*  9: -unused- */
	nokey,		/* 10: -unused- */
	l_eq,		/* 11: Prmtr */

	l_prmtr,	/* 12: Voice */
	l_init		/* 13: Init */
};

/* 
*/

PFS	t_inst[] = {	/* switch assignments for LS_INST */

	l_group,	/*  0: toggle group status */
	l_group,	/*  1: toggle group status */

	l_group,	/*  2: toggle group status */
	l_group,	/*  3: toggle group status */
	l_group,	/*  4: toggle group status */

	l_group,	/*  5: toggle group status */
	l_group,	/*  6: toggle group status */
	l_group,	/*  7: toggle group status */
	l_group,	/*  8: toggle group status */

	l_group,	/*  9: toggle group status */
	l_group,	/* 10: toggle group status */
	l_group,	/* 11: toggle group status */

	l_inst,		/* 12: 01-20 / 21-40 */
	l_dflt		/* 13: Deflt */
};

PFS	t_libr[] = {	/* switch assignments for LS_LIBR */

	quiet,		/*  0: Quiet */
	ROMPFN,		/*  1: ROMP */

	setlamp,	/*  2: Lamp */
	l_clock,	/*  3: Clock */
	rpctl,		/*  4: R/P */

	nokey,		/*  5: -unused- */
	nokey,		/*  6: -unused- */
	nokey,		/*  7: -unused- */
	nokey,		/*  8: -unused- */

	nokey,		/*  9: -unused- */
	nokey,		/* 10: -unused- */
	nokey,		/* 11: -unused- */

	nokey,		/* 12: -unused- */
	nokey		/* 13: -unused- */
};

/* 
*/

PFS	t_ngrp[] = {	/* switch assignments for LS_NGRP */

	quiet,		/*  0: Quiet */
	ROMPFN,		/*  1: ROMP */

	setlamp,	/*  2: Lamp */
	l_clock,	/*  3: Clock */
	rpctl,		/*  4: R/P */

	nokey,		/*  5: -unused- */
	nokey,		/*  6: -unused- */
	nokey,		/*  7: -unused- */
	nokey,		/*  8: -unused- */

	nokey,		/*  9: -unused- */
	nokey,		/* 10: -unused- */
	nokey,		/* 11: -unused- */

	nokey,		/* 12: -unused- */
	nokey		/* 13: -unused- */
};

PFS	t_load[] = {	/* switch assignments for LS_LOAD */

	quiet,		/*  0: Quiet */
	ROMPFN,		/*  1: ROMP */

	setlamp,	/*  2: Lamp */
	nokey,		/*  3: -unused- */
	nokey,		/*  4: -unused- */

	nokey,		/*  5: -unused- */
	nokey,		/*  6: -unused- */
	nokey,		/*  7: -unused- */
	l_load,		/*  8: Load */

	nokey,		/*  9: -unused- */
	nokey,		/* 10: -unused- */
	nokey,		/* 11: -unused- */

	nokey,		/* 12: -unused- */
	nokey		/* 13: -unused- */
};

/* 
*/

/*
   =============================================================================
	quiet() -- quiet the instrument
   =============================================================================
*/

quiet()
{
	register short vce;

	for (vce = 0; vce < 12; vce++)		/* for each voice ... */
		sendval(vce, 2, 0);		/* ... set level = 0 */

	memsetw(seqflag, 0, 16);		/* stop the sequences */
	memsetw(seqtime, 0, 16);		/* ... */

	memsetw(trstate, 0, 16);		/* reset the triggers */

	seqdupd = 0xFFFF;			/* request display update */

	stmproc(NULL_DEF);			/* trigger the blank definer */
}

/*
   =============================================================================
	rpctl() -- toggle record/play switch
   =============================================================================
*/

rpctl(stat)
short stat;
{
	if (NOT stat)
		return;

	recsw = NOT recsw;
	dsrpmod();
}

/* 
*/

/*
   =============================================================================
	trapper() -- trap to ROMP
   =============================================================================
*/

trapper(stat)
short stat;
{
	if (stat)
		xtrap15();
}

/*
   =============================================================================
	l_clock() -- process 'Clock' switch
   =============================================================================
*/

l_clock(stat)
short stat;
{
	if (NOT stat)
		return;

	clkset(NOT clkrun);
	dclkmd();
}

/*
   =============================================================================
	ulamp() -- underline lamp if forced on
   =============================================================================
*/

ulamp()
{
	if ((sliders EQ LS_VOICE) OR (sliders EQ LS_INST))
		return;

	point = GLCplot;	/* setup to draw line */
	GLCcurs(G_ON);

	if (lampsw)
		lseg (LAMP_XL, LAMP_Y, LAMP_XR, LAMP_Y, 1);
	else
		lseg (LAMP_XL, LAMP_Y, LAMP_XR, LAMP_Y, 0);

	GLCcurs(G_OFF);
}

/* 
*/

/*
   =============================================================================
	preio() -- turn off LCD backlight before disk I/O
   =============================================================================
*/

preio()
{
	short oldipl;

	oldipl = setipl(TIM_DI);

	lampio  = lampsw;
	iotime  = lcdtime;
	lampsw  = FALSE;
	io_leds = 0x1F;

	setipl(oldipl);
}

/*
   =============================================================================
	postio() -- restore LCD backlight state after disk I/O
   =============================================================================
*/

postio()
{
	short oldipl;

	oldipl = setipl(TIM_DI);

	lampsw = lampio;

	if (iotime OR lampsw) {

		lcdtime = lcdontm;
		io_leds = 0x9F;
	}

	setipl(oldipl);
}

/* 
*/

/*
   =============================================================================
	setlamp() -- toggle LCD backlight mode
   =============================================================================
*/

setlamp(stat)
{
	register short oldi;

	if (NOT stat)		/* only act on key closures */
		return;

	oldi = setipl(TIM_DI);	/* disable interrupts to keep out the timer */

	lcdtime = lcdontm;	/* set the LCD backlight timer */

	if (lampsw) {

		lampsw = FALSE;		/* enable LCD timeouts */
		io_leds = 0x1F;		/* turn off the LCD backlight */

	} else {

		lampsw = TRUE;		/* disable LCD timeouts */
		io_leds = 0x9F;		/* turn on the LCD backlight */
	}

	setipl(oldi);		/* turn interrupts back on */

	ulamp();		/* update underline on lamp key label */
}

/*
   =============================================================================
	lcd_on() -- turn on the LCD backlight
   =============================================================================
*/

lcd_on()
{
	register short oldi;

	oldi = setipl(TIM_DI);	/* disable interrupts to keep out the timer */

	lcdtime = lcdontm;	/* set the LCD backlight timer */
	io_leds = 0x9F;		/* turn on the LCD backlight */

	setipl(oldi);		/* turn interrupts back on */
}

/* 
*/

/*
   =============================================================================
	loadkey() -- process data keys in load mode
   =============================================================================
*/

loadkey(key)
short key;
{
	if (NOT astat)
		return;

	if (loadsw) {				/* if it's load time */

		if (key < 7) {			/* ... and the key is valid */

			loadem(key + 'A');	/* ... load the files */

			loadsw  = FALSE;
			ltagged = FALSE;
			d_key = olddkey;
			x_key = oldxkey;
			e_key = oldekey;
			m_key = oldmkey;
			pkctrl  = oldpk;
			sliders = oldsl;
			swpt    = oldsw;
			lcdlbls();
		}

		return;
	}
}

/* 
*/

/*
   =============================================================================
	pcancel() -- cancel panel modes
   =============================================================================
*/

pcancel(pm)
short pm;
{
	point = GLCplot;		/* setup to plot on LCD */
	GLCcurs(G_ON);

	if ((pm NE 0) AND asmode) {

		/* cancel assignment mode */

		asmode = 0;	
		pkctrl = oldpk;
		swpt   = oldsw;
		lseg(ASGN_XL, ASGN_Y, ASGN_XR, ASGN_Y, 0);
	}

	if ((pm NE 1) AND (gomode NE GO_NULL)) {

		/* cancel Go To mode */

		gomode = GO_NULL;
		pkctrl = oldpk;
		lseg(GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 0);
	}

	if ((pm NE 2) AND (ismode NE IS_NULL)) {

		/* cancel instrument select mode */

		ismode  = IS_NULL;
		pkctrl  = oldpk;
		sliders = oldsl;
		swpt    = oldsw;
		lcdlbls();
	}

	GLCcurs(G_OFF);
	setleds();
}

/* 
*/

/*
   =============================================================================
	l_load() -- process "Load" key
   =============================================================================
*/

l_load(stat)
short stat;
{
	if (NOT stat)
		return;

	pcancel(3);			/* cancel panel selections */
	lcancel(3);			/* cancel librarian selections */

	point = GLCplot;		/* setup to plot on LCD */
	GLCcurs(G_ON);

	if (loadsw) {		/* if load is selected, deselect */

		d_key   = olddkey;
		e_key   = oldekey;
		m_key   = oldmkey;
		x_key   = oldxkey;

		pkctrl  = oldpk;
		sliders = oldsl;
		swpt    = oldsw;

		loadsw  = FALSE;

		lcdlbls();
/* 
*/
	} else {		/* setup for load mode */

		clkset(0);	/* stop the clock */

		if (ltagged) {	/* cancel tag mode */

			dslslot(tagslot, exp_c(LCFBX01), loadrow);
			ltagged = FALSE;
		}

		if ((pkctrl EQ PK_PFRM) OR (pkctrl EQ PK_NOTE))
			oldpk = pkctrl;

		if (sliders NE LS_LOAD)
			oldsl = sliders;

		pkctrl  = PK_LOAD;
		sliders = LS_LOAD;
		oldsw   = swpt;
		swpt    = t_load;

		olddkey = d_key;
		oldxkey = x_key;
		oldekey = e_key;
		oldmkey = m_key;
		d_key   = loadkey;
		x_key   = l_load;
		e_key   = l_load;
		m_key   = l_load;

		loadsw = TRUE;

		lcdlbls();
		lseg(LOAD_XL, LOAD_Y, LOAD_XR, LOAD_Y, 1);
	}

	GLCcurs(G_OFF);
	setleds();
}

/* 
*/

/*
   =============================================================================
	lcdlbls() -- put up the labels on the LCD display
   =============================================================================
*/

lcdlbls()
{
	register short i, barval;
	char buf[8];
	char dsp[86];

	point = GLCplot;		/* set up to plot on the LCD */

	GLCinit();			/* clear the LCD display */
	GLCcurs(G_ON);

	/* label the switches */

	switch (sliders) {

	case LS_PRMTR:		/* parameter articulation */
	case LS_OTHER:		/* other stuff */
	case LS_EQ:		/* equalization */

		GLCtext(0, 1, swtlbls[sliders]);

		switch (gomode) {

		case GO_SECT:

			GLCtext(0, 31, "Sectn");
			lseg(GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 1);
			break;

		case GO_SCOR:

			GLCtext(0, 31, "Score");
			lseg(GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 1);
			break;

		default:

			GLCtext(0, 31, "Go To");
			lseg(GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 0);
			break;
		}

		break;
/* 
*/
	case LS_VOICE:		/* voice selection */

		dsp[0] = '\0';

		for (i = 0; i < 12; i++) {

			sprintf(buf, "%c%2d %c ",
				(articen[i] ? '\176' : ' '), i + 1,
				(articen[i] ? '\177' : ' '));

			strcat(dsp, buf);
		}

		strcat(dsp, "Voice Init ");

		GLCtext(0, 1, dsp);
		break;

	case LS_INST:		/* instrument selection */

		dsp[0] = '\0';

		for (i = 0; i < 12; i++) {

			sprintf(buf, "%c%2d %c ",
				(grpsel[i] ? '\176' : ' '), i + 1,
				(grpsel[i] ? '\177' : ' '));

			strcat(dsp, buf);
		}

		if (ismode EQ IS_HORC)
			strcat(dsp, " +20 ");	/* hi orc */
		else
			strcat(dsp, " +00 ");	/* lo orc */

		strcat(dsp, " Deflt");

		GLCtext(0, 1, dsp);
		break;

	case LS_LIBR:
	case LS_NGRP:
	case LS_LOAD:

		GLCtext(0, 1, swtlbls[sliders]);
		break;
	}

/* 
*/
	GLCtext(7, 1, potlbls[sliders]);	/* label the pots */

	for (i = 0; i < 13; i++)
		lseg(39 + (i * 36), 0, 39 + (i * 36), 63, 1);

	switch (sliders) {			/* draw the slider bars */

	case LS_PRMTR:	/* parameter articulation */
	case LS_VOICE:	/* voice articulation enable */
	case LS_INST:	/* instrument select */
	case LS_LIBR:	/* library select */
	case LS_NGRP:	/* group select */
	case LS_LOAD:	/* load select */

		for (i = 0; i < 14; i++) {

			if (BarType[i]) {

				barval = (((BGprmtr[i] >> 5) - 500) << 6) / 252;
				BarCset(i, barval);

			} else {

				barval = BGprmtr[i] / 252;
				BarBset(i, barval);
			}
		}

		break;

	case LS_OTHER:
	
		for (i = 0; i < 14; i++)
			BarBset(i, (BGother[i] / 252));

		break;

	case LS_EQ:

		for (i = 0; i < 14; i++)
			BarCset(i, BGeq[i]);

		break;

	default:

		break;
	}

	GLCcurs(G_OFF);
	dsrpmod();
	dclkmd();
	ulamp();
}

/* 
*/

/*
   =============================================================================
	selasg() -- select assignment table
   =============================================================================
*/

selasg(n)
short n;
{
	register struct s_entry *ep;

	getasg(curasg = n);	/* set the assignment */

	if (recsw) {		/* if we're recording ... */

		if (E_NULL NE (ep = findev(p_cur, t_cur, EV_ASGN, -1, -1))) {

			ep->e_data1 = n;	/* update old event */

		} else if (E_NULL NE (ep = e_alc(E_SIZE2))) {

			ep->e_type  = EV_ASGN;	/* create new event */
			ep->e_data1 = n;
			ep->e_time  = t_cur;
			p_cur = e_ins(ep, ep_adj(p_cur, 0, t_cur))->e_fwd;
			eh_ins(ep, EH_ASGN);

			if (ndisp EQ 2) {	/* if the score is up ... */

				ctrsw = TRUE;
				se_disp(ep, D_FWD, gdstbc, 1);
				scupd();
			}
		}
	}

	asmode = 0;			/* drop out of assignment mode */
	pkctrl = oldpk;			/* restore previous mode */
	swpt   = oldsw;

	point = GLCplot;		/* update the LCD */
	GLCcurs(G_ON);
	lseg(ASGN_XL, ASGN_Y, ASGN_XR, ASGN_Y, 0);
	GLCtext(0, 1, swtlbls[sliders]);
	GLCcurs(G_OFF);

	mpcupd();			/* update the video display ... */
	setleds();			/* ... and the LEDs */
}

/* 
*/

/*
   =============================================================================
	l_adfl() -- select default assignment table
   =============================================================================
*/

l_adfl(stat)
short stat;
{
	if (NOT stat)
		return;

	selasg(0);
}

/* 
*/

/*
   =============================================================================
	l_asgn() -- select assignment
   =============================================================================
*/

l_asgn(stat)
short stat;
{
	if (NOT stat)			/* only do this on key closure */
		return;

	point = GLCplot;		/* set up to plot on the LCD */
	GLCcurs(G_ON);

	if (ismode NE IS_NULL) {	/* cancel instrument select mode */

		ismode  = IS_NULL;
		pkctrl  = oldpk;
		sliders = oldsl;
		swpt    = oldsw;
		lcdlbls();
	}

	if (gomode NE GO_NULL) {	/* cancel Go To mode */

		gomode = GO_NULL;
		pkctrl = oldpk;
		lseg(GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 0);
	}
/* 
*/
	switch (asmode) {

	case 0:			/* initial actuation */

		if ((pkctrl EQ PK_PFRM) OR (pkctrl EQ PK_NOTE))
			oldpk = pkctrl;

		asmode = (curasg ? ((curasg - 1) / 20) : 0) + 1;
		pkctrl = PK_ASGN;
		memcpy(aswtbl, swpt, sizeof t_prmtr);
		oldsw = swpt;
		swpt  = aswtbl;
		aswtbl[13] = l_adfl;
		lseg(ASGN_XL, ASGN_Y, ASGN_XR, ASGN_Y, 1);
		GLCtext(0, 79, "Deflt");
		break;

	case 1:			/* +00 -- 01..20 to +20 */
	case 2:			/* +20 -- 21..40 to +40 */
	case 3:			/* +40 -- 41..60 to +60 */
	case 4:			/* +60 -- 61..80 to +80 */

		++asmode;
		break;

	case 5:			/* +80 -- 81..99 to +00 */

		asmode = 1;
		break;

	}

	GLCtext(0, 43, aslbls[asmode]);
	GLCcurs(G_OFF);
	setleds();
}

/* 
*/

/*
   =============================================================================
	l_inst() -- set select instrument mode
   =============================================================================
*/

l_inst(stat)
short stat;
{
	if (NOT stat)
		return;

	point = GLCplot;		/* set up to plot on the LCD */
	GLCcurs(G_ON);

	if (asmode) {			/* cancel assignment mode */

		asmode = 0;	
		pkctrl = oldpk;
		swpt   = oldsw;
		lseg(ASGN_XL, ASGN_Y, ASGN_XR, ASGN_Y, 0);
	}

	if (gomode NE GO_NULL) {	/* cancel Go To mode */

		gomode = GO_NULL;
		pkctrl = oldpk;
		lseg(GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 0);
	}

	GLCcurs(G_OFF);
/* 
*/
	switch (ismode) {

	case IS_NULL:		/* select +00 -- low orchestra */

		if ((pkctrl EQ PK_PFRM) OR (pkctrl EQ PK_NOTE))
			oldpk = pkctrl;

		if (sliders NE LS_INST)
			oldsl = sliders;

		oldsw   = swpt;
		swpt    = t_inst;
		pkctrl  = PK_INST;
		ismode  = IS_LORC;
		sliders = LS_INST;
		lcdlbls();
		break;

	case IS_LORC:		/* select +20 -- high orchestra */

		ismode  = IS_HORC;
		GLCcurs(G_ON);
		GLCtext(0, 73, " +20 ");
		GLCcurs(G_OFF);
		break;

	case IS_HORC:		/* deselect */
	default:

		ismode  = IS_NULL;
		pkctrl  = oldpk;
		sliders = oldsl;
		swpt    = oldsw;
		lcdlbls();
		break;
	}

	setleds();
}

/* 
*/

/*
   =============================================================================
	selins() -- select an instrument
   =============================================================================
*/

selins(ival)
register short ival;
{
	register short n;
	register struct s_entry *ep;

	for (n = 0; n < 12; n++) {

		if (FALSE EQ grpsel[n])
			continue;

		ins2grp[n] = ival | (ins2grp[n] & 0xFF00);
		setv2gi(n);

		if (recsw AND grpstat[n] AND (2 EQ grpmode[n])) {

			if (E_NULL NE (ep = findev(p_cur, t_cur, EV_INST, n, -1))) {

				ep->e_data2 = ival;

			} else if (E_NULL NE (ep = e_alc(E_SIZE2))) {

				ep->e_type  = EV_INST;
				ep->e_data1 = n;
				ep->e_data2 = ival;
				ep->e_time  = t_cur;
				p_cur = e_ins(ep, ep_adj(p_cur, 0, t_cur))->e_fwd;
				eh_ins(ep, EH_INST);

				if (ndisp EQ 2) {

					ctrsw = TRUE;
					se_disp(ep, D_FWD, gdstbc, 1);
					scupd();
				}
			}
		}
	}

	setinst();

/* 
*/
	ismode  = IS_NULL;
	pkctrl  = oldpk;
	sliders = oldsl;
	swpt    = oldsw;
	lcdlbls();
	setleds();
	mpcupd();
}

/* 
*/

/*
   =============================================================================
	l_dflt() -- select default isntrument
   =============================================================================
*/

l_dflt(stat)
short stat;
{
	if (NOT stat)
		return;

	selins(0);
}

/* 
*/

/*
   =============================================================================
	l_goto() -- go to (section / score)
   =============================================================================
*/

l_goto(stat)
short stat;
{
	if (NOT stat)			/* only do this on key closure */
		return;

	point = GLCplot;		/* set up to plot on the LCD */
	GLCcurs(G_ON);

	if (asmode) {			/* cancel assignment mode */

		asmode = 0;	
		pkctrl = oldpk;
		swpt = oldsw;
		lseg(ASGN_XL, ASGN_Y, ASGN_XR, ASGN_Y, 0);
	}

	if (ismode NE IS_NULL) {	/* cancel instrument select mode */

		ismode  = IS_NULL;
		sliders = oldsl;
		pkctrl  = oldpk;
		swpt    = oldsw;
		lcdlbls();
	}

	recsw = FALSE;			/* force play mode */
	dsrpmod();
/* 
*/
	switch (gomode) {		/* select Go To mode */

	case GO_NULL:

		if ((pkctrl EQ PK_PFRM) OR (pkctrl EQ PK_NOTE))
			oldpk = pkctrl;

		pkctrl = PK_GOTO;
		gomode = GO_SECT;
		GLCtext(0, 31, "Sectn");
		lseg(GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 1);
		break;

	case GO_SECT:

		gomode = GO_SCOR;
		GLCtext(0, 31, "Score");
		lseg(GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 1);
		break;

	case GO_SCOR:
	default:

		pkctrl = oldpk;
		gomode = GO_NULL;
		GLCtext(0, 31, "Go To");
		lseg(GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 0);
		break;
	}

	GLCcurs(G_OFF);
	setleds();
}

/* 
*/

/*
   =============================================================================
	newpps() -- send out new polyphonic pressure source value
   =============================================================================
*/

newpps(trg, grp, src, ival)
register short trg;
short grp, src;
register short ival;
{
	register struct sment *smf, *smp;
	register unsigned *fpu;
	register long ltmp;
	register short val;
	register short vce;
	short gs, oldi;

	gs = (grp << 4) | src;		/* calculate group/source index */
	fpu = io_fpu + FPU_OFNC;	/* point at FPU base */
	valents[gs].val = ival;		/* update value in table */
	smp = smf = (struct sment *)&valents[gs];

/* 
*/
	while (smf NE (smp = smp->nxt)) {	/* for each function ... */

		vce = (smp->vp >> 4) & 0x0F;	/* extract voice number */

		if (vce2trg[vce] EQ trg) {	/* for each key that matches ... */

			switch (smp->vp & 0x000F) {

			case 1:		/* frq 1 */
			case 3:		/* frq 2 */
			case 5:		/* frq 3 */
			case 7:		/* frq 4 */

				val = ival >> 3;
				break;

			case 10:	/* filtr */

				ltmp = ((long)ival >> 1) + ((long)ival >> 2);

				if (ltmp > (long)VALMAX)
					ltmp = (long)VALMAX;
				else if (ltmp < (long)VALMIN)
					ltmp = (long)VALMIN;

				val = (short)ltmp;
				break;

			default:

				val = ival;
			}

			oldi = setipl(FPU_DI);

/* ++++++++++++++++++++++++++++ FPU interrupts disabled +++++++++++++++++++++ */

			*(fpu + (smp->vp << 4) + FPU_TCV1) = val;

			setipl(oldi);

/* ++++++++++++++++++++++++++++ Interrupts restored +++++++++++++++++++++++++ */

		}
	}
}

/* 
*/

/*
   =============================================================================
	newsv() -- send out new source value if it's changed
   =============================================================================
*/

short
newsv(grp, src, ival)
short grp, src;
register short ival;
{
	register struct sment *smf, *smp;
	register unsigned *fpu;
	register long ltmp;
	register short val;
	register short gs;
	register short oldi;
	short vmask, var;

	gs  = (grp << 4) | src;			/* calculate grp/src index */

	if (-1 NE (var = src2var[src])) {	/* see if its a variable */

		vmask = vmasks[anrs[var][grp]];	/* setup resolution mask */

		/* see if we have a change in the selected significant bits */

		if (((val = valents[gs].val) & vmask) EQ (ival & vmask)) {

			/* make sure we always see a change to zero */

			if (NOT ((ival EQ 0) AND val))	/* if not becoming zero */
				return(FALSE);	/* ... it didn't change enough */
		}
	}

	fpu = io_fpu + FPU_OFNC;		/* point to FPU */

	valents[gs].val = ival;			/* update value in table */

	smf = (struct sment *)&valents[gs];	/* point at value table */
	smp = smf;

/* 
*/
	while (smf NE (smp = smp->nxt)) {	/* update functions */

		switch (smp->vp & 0x000F) {

		case 1:		/* frq 1 */
		case 3:		/* frq 2 */
		case 5:		/* frq 3 */
		case 7:		/* frq 4 */

			val = ival >> 3;
			break;

		case 10:	/* filtr */

			ltmp = ((long)ival >> 1) + ((long)ival >> 2);

			if (ltmp > (long)VALMAX)
				ltmp = (long)VALMAX;
			else if (ltmp < (long)VALMIN)
				ltmp = (long)VALMIN;

			val = (short)ltmp;
			break;

		default:

			val = ival;
		}

		oldi = setipl(FPU_DI);

/* ++++++++++++++++++++++++++++ FPU interrupts disabled +++++++++++++++++++++ */

		*(fpu + (smp->vp << 4) + FPU_TCV1) = val;

		setipl(oldi);

/* ++++++++++++++++++++++++++++ Interrupts restored +++++++++++++++++++++++++ */

	}

	return(TRUE);
}

/* 
*/

/*
   =============================================================================
	setsv() -- unconditionally send out new source value
   =============================================================================
*/

setsv(grp, src, ival)
short grp, src;
register short ival;
{
	register struct sment *smf, *smp;
	register unsigned *fpu;
	register long ltmp;
	register short val;
	register short gs;
	register short oldi;

	gs  = (grp << 4) | src;			/* calculate grp/src index */
	fpu = io_fpu + FPU_OFNC;		/* point to FPU */

	valents[gs].val = ival;			/* update value in table */

	smf = (struct sment *)&valents[gs];	/* point at value table */
	smp = smf;

/* 
*/
	while (smf NE (smp = smp->nxt)) {	/* update functions */

		switch (smp->vp & 0x000F) {

		case 1:		/* frq 1 */
		case 3:		/* frq 2 */
		case 5:		/* frq 3 */
		case 7:		/* frq 4 */

			val = ival >> 3;
			break;

		case 10:	/* filtr */

			ltmp = ((long)ival >> 1) + ((long)ival >> 2);

			if (ltmp > (long)VALMAX)
				ltmp = (long)VALMAX;
			else if (ltmp < (long)VALMIN)
				ltmp = (long)VALMIN;

			val = (short)ltmp;
			break;

		default:

			val = ival;
		}

		oldi = setipl(FPU_DI);

/* ++++++++++++++++++++++++++++ FPU interrupts disabled +++++++++++++++++++++ */

		*(fpu + (smp->vp << 4) + FPU_TCV1) = val;

		setipl(oldi);

/* ++++++++++++++++++++++++++++ Interrupts restored +++++++++++++++++++++++++ */

	}
}

/* 
*/

/*
   =============================================================================
	l_prmtr() -- set parameter articulation status
   =============================================================================
*/

l_prmtr(stat, sig)
short stat, sig;
{
	if (NOT stat)
		return;

	switch (sliders) {

	case LS_PRMTR:
	case LS_OTHER:
	case LS_EQ:

		sliders = LS_VOICE;
		swpt    = t_voice;
		break;

	case LS_VOICE:
	default:

		sliders = LS_PRMTR;
		swpt    = t_prmtr;
		break;
	}

	lcdlbls();
	setleds();
}

/* 
*/

/*
   =============================================================================
	l_voice() -- set voice selection status
   =============================================================================
*/

l_voice(stat, vce)
short stat, vce;
{
	register short ap, j, par;
	register unsigned *fpu;

	char buf[8];

	if (NOT stat)
		return;

	articen[vce] = NOT articen[vce];	/* toggle voice enable */

	sprintf(buf, "%c%2d %c", (articen[vce] ? '\176' : ' '), vce + 1,
		(articen[vce] ? '\177' : ' '));

	GLCcurs(G_ON);
	GLCtext(0, (1 + (6 * vce)), buf);
	GLCcurs(G_OFF);

	if (articen[vce]) {	/* turn articulation on if voice is enabled */

		for (ap = 0; ap < 14; ap++) {

			par = arpar[ap];
			sendart(vce, par, lastart[par]);
		}

	} else {	/* ... or turn it off if voice is disabled */

		fpu = io_fpu + FPU_OFNC + (vce << 8);

		for (j = 1; j < 16; j++)
			*(fpu + (j << 4) + FPU_TSF2) = 0;
	}
}

/* 
*/

/*
   =============================================================================
	l_group() -- set group selection status
   =============================================================================
*/

l_group(stat, grp)
short stat, grp;
{
	char buf[8];

	if (NOT stat)
		return;

	grpsel[grp] = NOT grpsel[grp];	/* toggle voice enable */

	sprintf(buf, "%c%2d %c", (grpsel[grp] ? '\176' : ' '), grp + 1,
		(grpsel[grp] ? '\177' : ' '));

	GLCcurs(G_ON);
	GLCtext(0, (1 + (6 * grp)), buf);
	GLCcurs(G_OFF);
}

/*
   =============================================================================
	l_other -- set aux function status
   =============================================================================
*/

l_other(stat, sig)
short stat, sig;
{
	if (NOT stat)
		return;

	sliders = LS_OTHER;
	swpt    = t_other;

	lcdlbls();
}

/* 
*/

/*
   =============================================================================
	l_eq -- set eq mode
   =============================================================================
*/

l_eq(stat, sig)
short stat, sig;
{
	if (NOT stat)
		return;

	if (sliders EQ LS_EQ) {

		sliders = LS_PRMTR;
		swpt    = t_prmtr;

	} else {

		sliders = LS_EQ;
		swpt    = t_eq;
	}

	lcdlbls();
}

/*
   =============================================================================
	artclr() -- clear the articulation pot values
   =============================================================================
*/

artclr()
{
	register short i;

	for (i = 0; i < 14; i++)
		lastart[arpar[i]] = artitab[i];
}

/* 
*/

/*
   =============================================================================
	l_init() -- initialize articulation values  (reset the sliders)
   =============================================================================
*/

l_init(stat, sig)
short stat, sig;
{
	register unsigned *fpu;
	register short barval, i, j;
	char buf[16];

	if (NOT stat)
		return;

	fpu = io_fpu + FPU_OFNC;

	switch (sliders) {

	case LS_PRMTR:
	case LS_VOICE:
	case LS_INST:
	case LS_LIBR:
	case LS_NGRP:
	case LS_LOAD:

		/* clear all but spares in voices 1..12 */

		for (i = 0; i < 12; i++)
			for (j = 1; j < 16; j++)
				*(fpu + (i << 8) + (j << 4) + FPU_TSF2) = 0;

		artclr();

		for (i = 0; i < 14; i++) {

			BGprmtr[i] = artitab[i];

			if (BarType[i]) {

				barval = (((artitab[i] >> 5) - 500) << 6) / 252;
				BarCset(i, barval);

			} else {

				barval = artitab[i] / 252;
				BarBset(i, barval);
			}
		}

		break;

	case LS_OTHER:

		/* clear spares in voices 2..12 */

		for (i = 1; i < 12; i++)
			*(fpu + (i << 8) + FPU_TSF2) = 0;

		for (i = 0; i < 14; i++)
			BarBset(i, BGother[i] = 0);

		break;

	case LS_EQ:

		for (i = 0; i < 14; i++) {

			sendeq(i, gain2eq(0));
			BarCset(i, BGeq[i] = 0);
		}

		break;
	}
}

/* 
*/

/*
   =============================================================================
	sendart() -- send out an articulation value
   =============================================================================
*/

sendart(vce, par, ival)
short vce, par, ival;
{
	register unsigned *fpu;
	register short val, oldi, mult;
	short nop;

	lastart[par] = ival;

	fpu = io_fpu + FPU_OFNC + (vce << 8) + (par << 4);
	mult = 0x7FFF;

	switch (par) {

	case 1:		/* freq 1 */
	case 3:		/* freq 2 */
	case 5:		/* freq 3 */
	case 7:		/* freq 4 */

		val = addpch(ival, 0);
		break;

	case 10:	/* filter */

		val = ((ival >> 5) - 500) << 5;
		break;

/* 
*/

	case 2:		/* level */

		val = ival;
		mult = 0xFFFF;
		break;

	case 4:		/* location */

		val = ((ival >> 5) - 500) << 5;
		mult = 0xFFFF;
		break;

	default:

		val = ival;
	}

	oldi = setipl(FPU_DI);

/* ++++++++++++++++++++++++++++ FPU interrupts disabled +++++++++++++++++++++ */

	*(fpu + FPU_TCV2) = val;
	++nop;	++nop;	++nop;
	*(fpu + FPU_TSF2) = mult;

	setipl(oldi);

/* ++++++++++++++++++++++++++++ Interrupts restored +++++++++++++++++++++++++ */

}

/* 
*/

/*
   =============================================================================
	doslide() -- update a changed slider value
   =============================================================================
*/

doslide()
{
	register short arp, bar, barval, i, slval;

	slval = ART_VAL(aval);	/* scaled articulation value */
	bar   = asig - 25;	/* bar number */

	switch (sliders) {

	case LS_PRMTR:		/* parameter articulation */
	case LS_VOICE:
	case LS_INST:
	case LS_LIBR:
	case LS_NGRP:
	case LS_LOAD:

		BGprmtr[bar] = slval;
		arp = arpar[bar];

		for (i = 0; i < 12; i++)
			if (articen[i])
				sendart(i, arp, slval);

		if (BarType[bar]) {		/* center-zero bar */

			barval = (((slval >> 5) - 500) << 6) / 252;
			BarCadj(bar, barval);

		} else {			/* bottom-zero bar */

			barval = slval / 252;
			BarBadj(bar, barval);
		}

		return;
/* 
*/
	case LS_OTHER:		/* aux, dome, cv control */

		switch (asig) {

		case 26:	/* aux mod */
			sendart(9, 0, slval);
			break;

		case 27:	/* depth */
			sendart(3, 0, slval);
			break;

		case 28:	/* rate */
			sendart(2, 0, slval);
			break;

		case 29:	/* intensity */
			sendart(1, 0, slval);
			break;

		case 30:	/* cv1 */
			sendart(11, 0, slval);
			break;

		case 31:	/* cv2 */
			sendart(10, 0, slval);
			break;

		case 32:	/* cv3 */
			sendart(6, 0, slval);
			break;

		case 33:	/* cv4 */
			sendart(8, 0, slval);
			break;

		default:
			slval = 0;	/* force unused pots to stay zero */
			break;
		}

		BGother[bar] = slval;
		barval = slval / 252;
		BarBadj(bar, barval);

		return;

/* 
*/
	case LS_EQ:		/* eq control */

		barval = (((slval >> 5) - 500) << 6) / 252;
		BGeq[bar] = barval;
		sendeq(bar, gain2eq(SL2GAIN(slval)));
		BarCadj(bar, barval);

		return;

	default:
		return;
	}
}

/* 
*/

/*
   =============================================================================
	settmpo() -- set the tempo
   =============================================================================
*/

settmpo(val)
short val;
{
	register short timeval;

	tmpoval  = val;
	timeval  = ( ( (tmpomlt + 50) * tmpoval) / 100);
	timeval  = (short)( (192000L / timeval) - 1);
	TIME_T2H = timeval >> 8;
	TIME_T2L = timeval & 0x00FF;
}

/* 
*/

/*
   =============================================================================
	settune() -- send the fine tuning from tuneval to the FPU
   =============================================================================
*/

settune()
{
	register short oldi, i, grp;
	register unsigned *fpu;
	register long trval, trmax;
	short nop;

	trmax = PCHMAX;

	oldi = setsr(0x2200);

/* +++++++++++++++++++++++ FPU interrupts disabled ++++++++++++++++++++++++++ */

	for (i = 0; i < 12; i++) {

		if (-1 NE (grp = vce2grp[i])) {

			trval = (long)tuneval + ((long)s_trns[grp - 1] << 1);

			if (trval > trmax)
				trval = trmax;

		} else {

			trval = (long)tuneval;
		}

		fpu = io_fpu + FPU_OFNC + (i << 8) + 0x0010;	/* freq 1 */

		*(fpu + FPU_TCV3) = (unsigned)(0x0000FFFFL & trval);
		++nop;	++nop;	++nop;
		*(fpu + FPU_TSF3) = 0x7FFF;
	}

	setsr(oldi);

/* ++++++++++++++++++++++++++++ Interrupts restored +++++++++++++++++++++++++ */

}
