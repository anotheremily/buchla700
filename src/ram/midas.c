/*
   =============================================================================
	midas.c -- MIDAS-VII main function
	Version 26 -- 1989-07-19 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "biosdefs.h"
#include "vsdd.h"
#include "hwdefs.h"
#include "wordq.h"
#include "sclock.h"

#include "graphdef.h"
#include "midas.h"

#define	LCD_TIME	((long)(800 * 240))	/* LCD backlight 'on' time */

#if	DEBUGIT
extern	short	debugsw;

short	debugmm = 1;
#endif

extern	short	fpuint(), clk_ped(), pch_ped();

extern	short	(*foot1)(), (*foot2)(), (*pulse1)(), (*pulse2)();

extern	char	*end, *etext, *edata;

extern	short	clkrun, clksrc, initcfg, ndisp, verbose;

extern	long	lcdontm, lcdtime;

extern	short	fp_resv[];

extern	char	*script0[];

extern	char	VerDate[];

extern	struct wordq	ptefifo;	/* trigger fifo */

char	m7verms[32];	/* version message for the main menu */

short	clkdiv;		/* clock divider */

/* 
*/

/*
   =============================================================================
	dopls1() -- process pulse 1 inputs
   =============================================================================
*/

dopls1()
{
	putwq(&ptefifo, 0x1180);	/* pulse 1 trigger -> STM fifo */

	if (NOT clkrun)				/* check for clock enable */
		return;				/* done if not enabled */

	if (clksrc EQ CK_PLS24) {		/* Pulse 24 mode ? */

		fc_val += 2L;			/* 2 clocks per pulse */

		if (fc_val > 0x00FFFFFFL)	/* limit clock at maximum */
			fc_val = 0x00FFFFFFL;

	} else if (clksrc EQ CK_PLS48) {	/* Pulse 48 mode ? */

		++fc_val;			/* 1 clock per pulse */

		if (fc_val > 0x00FFFFFFL)	/* limit clock at maximum */
			fc_val = 0x00FFFFFFL;

	} else if (clksrc EQ CK_PLS96) {	/* Pulse 96 mode ? */

		if (clkdiv++) {			/* 2 pulses per clock */

			clkdiv = 0;		/* reset divider */
			++fc_val;		/* update frame clock */

			if (fc_val > 0x00FFFFFFL)	/* limit clock at maximum */
				fc_val = 0x00FFFFFFL;
		}
	}
}

/*
   =============================================================================
	dopls2() -- process pulse 2 inputs
   =============================================================================
*/

dopls2()
{
	putwq(&ptefifo, 0x1181);	/* pulse 2 trigger -> STM fifo */
}

/* 
*/

/*
   =============================================================================
	mpcupd() -- MIDI program change display update
   =============================================================================
*/

mpcupd()
{
	switch (ndisp) {

	case 0:		/* Librarian */

		break;

	case 1:		/* Patch Editor */

		break;

	case 2:		/* Score Editor */

		sdwins();		/* fill in the windows */
		break;

	case 3:		/* Sequence Editor */

		break;

	case 4:		/* Instrument Editor */

		setinst();		/* bring variables up to date */
		allwins();		/* fill in the windows */
		break;

	case 5:		/* Initialize System */

		break;

	case 6:		/* Waveshape Editor */

		newws();		/* bring variables up to date */
		wwins();		/* fill in the windows */
		break;

	case 7:		/* Write Program to Disk */

		break;

	case 8:		/* Tuning Editor */

		twins();		/* fill in the windows */
		break;

	case 9:		/* Format Disk */

		break;

	case 10:	/* Assignment Editor */

		awins();		/* fill in the windows */
		break;
	}
}

/* 
*/

/*
   =============================================================================
	MIDAS 700 main function
   =============================================================================
*/

main()
{
	setipl(FPU_DI);			/* disable FPU interrupts */

/* +++++++++++++++++++++++ FPU INTERRUPTS DISABLED ++++++++++++++++++++++++++ */

	BIOS(B_SETV, 26, fpuint);	/* set level 2 int. vector for FPU */

	initcfg    = 0;			/* set initial configuration  (in MS bits) */
	fp_resv[0] = (-1000) << 5;	/* initial output amplitude = 0.0 */

	fpuclr();		/* quiet the FPU */

	tsetup();		/* setup the timer and timer interrupts */
	setsio();		/* setup the serial I/O port interrupts */

	foot1 = clk_ped;	/* setup clock on/off pedal processing */
	foot2 = pch_ped;	/* setup punch in/out pedal processing */

	pulse1 = dopls1;	/* setup pulse input 1 processing */
	pulse2 = dopls2;	/* setup pulse input 2 processing */

	lcdontm = LCD_TIME;	/* set the LCD backlight 'on' time */
	lcd_on();		/* turn on the LCD backlight */

	strcpy(m7verms, &VerDate[2]);	/* setup the version message */

	im700();		/* initialize everything */
	settune();		/* ... including fine tuning */

	ndisp = -1;		/* say nothing has been selected yet */
	verbose = FALSE;	/* setup to run the script quietly */
	rscript(script0);	/* run the initial script */
	sc_goto(0L);		/* position the score */

	MouseWK();		/* wake up the mouse if it's there */

	goto startup;		/* go put up the main menu */

/* 
*/

newdisp:

	msl();		/* run the new display */

startup:

#if	DEBUGIT
	if (debugsw AND debugmm)
		printf("main():  switching to MAIN MENU\N");
#endif

	m7menu();	/* put up the main menu */
	msl();		/* run the main menu */

#if	DEBUGIT
	if (debugsw AND debugmm)
		printf("main():  switching to %d\n", ndisp);
#endif

	switch (ndisp) {	/* setup for a new display */

	case 0:		/* =================== librarian ==================== */

		ldfield();	/* setup the librarian field handlers */
		libdsp();	/* setup the librarian display */
		break;

	case 1:		/* =================== patch editor ================= */

		ptfield();	/* setup the patch field handlers */
		ptdisp();	/* setup the patch display */
		break;

	case 2:		/* =================== score editor ================= */

		scfield();		/* initialize score field handlers */
		sdsetup();		/* setup the score display */
		break;

	case 3:		/* ================ sequence editor ================= */

		sqfield();		/* initialize sequence field handlers */
		sqdisp();		/* setup the sequence display */
		break;
/* 
*/
	case 4:		/* =============== instrument editor ================ */

		idfield();		/* setup instrument field handlers */
		instdsp();		/* setup the instrument display */
		break;

	case 6:		/* ================ waveshape editor ================ */

		wdfield();		/* setup waveshape field handlers */
		wsdsp();		/* setup the waveshape display */
		break;

	case 8:		/* ================ tuning editor ================ */

		tdfield();		/* setup tuning editor field handlers */
		tundsp();		/* setup the tuning display */
		break;

	case 10:	/* ================ assignment editor =============== */

		adfield();		/* setup assignment field handlers */
		asgdsp();		/* setup the assignment display */
		break;

	case 11:	/* ================ diagnostics ===================== */

		scopeon();		/* setup the diagnostics */
		break;

	default:

#if	DEBUGIT
	if (debugsw AND debugmm)
		printf("main():  UNKNOWN display (%d)\n", ndisp);
#endif

		ndisp = -1;
		goto startup;
	}

#if	DEBUGIT
	if (debugsw AND debugmm)
		printf("main():  display switch complete to %d\n", ndisp);
#endif

	goto newdisp;
}
