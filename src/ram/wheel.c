/*
   =============================================================================
	wheel.c -- MIDAS-VII -- scroll wheel, trackball and mouse functions
	Version 47 -- 1989-12-19 -- D.N. Lynx Crowe

	M8 Mouse driver -- uses 3 byte Microsoft format  (default).
   =============================================================================
*/

#define	DEBUGMS		0
#define	DEBUGTF		0
#define	DEBUGTK		0

#include "stddefs.h"
#include "graphdef.h"
#include "biosdefs.h"
#include "uartio.h"
#include "vsdd.h"
#include "hwdefs.h"
#include "fields.h"
#include "macros.h"
#include "sclock.h"
#include "scwheel.h"
#include "timers.h"
#include "score.h"
#include "curpak.h"

#include "midas.h"

extern	unsigned short	setipl();

#define	M_FL_CT		1024			/* mouse flush count */

#define	M_ST_TM		2
#define	C_PER_S		500
#define	MAXTRIES	10

#define	MATIME		2000			/* mouse activity timeout -- ms*/

#define	CFR0		(BR_1200|NSB_1)		/* UART CFR0  1200 baud, 1 stop bit */
#define	CFR1		(P_NONE|NDB_7)		/* UART CFR1  7 data bits, no parity */

#define	M_NONE		0		/* No errors detected */
#define	M_FLUSH		1		/* Unable to flush mouse buffer */
#define	M_SYNC		2		/* Mouse out of sync */
#define	M_NORESP	3		/* No response from mouse */
#define	M_RESPNG	4		/* Bad response from mouse */

/* 
*/

#if	DEBUGMS
short	debugms;
#endif

#if	DEBUGTF
short	debugtf;

long	txficnt;
long	tyficnt;
#endif

#if	DEBUGTK
short	debugtk;
#endif

short	M_error;		/* mouse I/F error code */
short	M_state;		/* mouse state */
short	M_oldbs;		/* previous mouse button state */

char	M_strng[32];		/* mouse input string buffer */

short	msctrl;			/* mouse control flag -- mouse update */
short	msflag;			/* mouse control flag -- mouse movement */
short	msrtag;			/* mouse control flag -- mouse reset */
short	msxres;			/* mouse x residue */	
short	msyres;			/* mouse y residue */
short	msxmov;			/* mouse x movement */
short	msymov;			/* mouse y movement */
short	msxdiv;			/* mouse x divisor */
short	msydiv;			/* mouse y divisor */

short	tkboth;			/* both trackball axes went active */

short	txdiv;			/* text cursor X divider */
short	tydiv;			/* text cursor Y divider */

short	tkxdvr = TKXDVR;	/* text cursor X divisor */
short	tkydvr = TKYDVR;	/* text cursor Y divisor */

short	tkhdvr = TKCDVR;	/* text cursor horizontal movement threshold */
short	tkvdvr = TKCDVR;	/* text cursor vertical movement threshold */

/* 
*/

short	msxgdv[13] = {		/* mouse x cursor divisor table */

	1,	/* -1 -- Main menu */
	1,	/*  0 -- Librarian */
	1,	/*  1 -- Patch editor */
	1,	/*  2 -- Score editor */
	1,	/*  3 -- Sequence editor */
	1,	/*  4 -- Instrument editor */
	1,	/*  5 -- Initialize system */
	1,	/*  6 -- Waveshape editor */
	1,	/*  7 -- Write program to disk */
	1,	/*  8 -- Tuning editor */
	1,	/*  9 -- Format disk */
	1,	/* 10 -- Assignment editor */
	1	/* 11 -- Diagnostics */
};

short	msygdv[13] = {		/* mouse y cursor divisor table */

	2,	/* -1 -- Main menu */
	2,	/*  0 -- Librarian */
	2,	/*  1 -- Patch editor */
	2,	/*  2 -- Score editor */
	2,	/*  3 -- Sequence editor */
	2,	/*  4 -- Instrument editor */
	2,	/*  5 -- Initialize system */
	2,	/*  6 -- Waveshape editor */
	2,	/*  7 -- Write program to disk */
	2,	/*  8 -- Tuning editor */
	2,	/*  9 -- Format disk */
	2,	/* 10 -- Assignment editor */
	2	/* 11 -- Diagnostics */
};

/* 
*/

extern	short	(*curmove)();
extern	short	(*cx_upd)();
extern	short	(*cy_upd)();
extern	short	(*xy_dn)();
extern	short	(*xy_up)();
extern	short	(*e_key)();
extern	short	(*m_key)();
extern	short	(*x_key)();

extern	short	asig;
extern	short	astat;
extern	short	aval;
extern	short	chtime;
extern	short	chwait;
extern	short	clkctl;
extern	short	clkrun;
extern	short	cmfirst;
extern	short	cmtype;
extern	short	ctrsw;
extern	short	curhold;
extern	short	cvtime;
extern	short	cvwait;
extern	short	cxrate;
extern	short	cxval;
extern	short	cyrate;
extern	short	cyval;
extern	short	msctrl;
extern	short	msflag;
extern	short	msxdiv;
extern	short	msydiv;
extern	short	nchwait;
extern	short	ncvwait;
extern	short	ndisp;
extern	short	ss_ptsw;
extern	short	ss_sqsw;
extern	short	stccol;
extern	short	stcrow;
extern	short	swback;
extern	short	swctrl;
extern	short	swdelta;
extern	short	swdir;
extern	short	swfiin;
extern	short	swflag;
extern	short	swlast;
extern	short	swndx;
extern	short	swstop;
extern	short	swthr;
extern	short	swtime;
extern	short	swwait;

/* 
*/

extern	short	tkback;
extern	short	tkctrl;
extern	short	tkrmin;
extern	short	tkthr;
extern	short	tktime;
extern	short	tkwait;
extern	short	trkball;
extern	short	txfiin;
extern	short	txflag;
extern	short	txlast;
extern	short	tyfiin;
extern	short	tyflag;
extern	short	tylast;
extern	short	txstop;
extern	short	tystop;
extern	short	vtccol;
extern	short	vtcrow;
extern	short	vtpcol;
extern	short	vtprow;
extern	short	xkstat;
extern	short	ykstat;

extern	long	swcount;
extern	long	swrate;
extern	long	swrmin;
extern	long	swtemp;

extern	short	*cratex;
extern	short	*cratey;

extern	short	sigtab[][2];
extern	short	swfifo[NSWFIFO];
extern	short	txfifo[NTKFIFO];
extern	short	tyfifo[NTKFIFO];

#include "swrtab.h"		/* long swrtab[128]; */

/* 
*/

/*
   =============================================================================
	MouseRT() -- reset the mouse activity timer
   =============================================================================
*/

MouseRT(t)
unsigned short t;
{
	unsigned short oldi;

	oldi = setipl(TIM_DI);		/* disable interrupts */

	timers[MUTIMER] = t;		/* set the mouse timer */

	setipl(oldi);			/* enable interrupts */

#if	DEBUGMS
	if (debugms > 2)
		printf("MouseRT(%d)\n", t);
#endif
}

/* 
*/

/*
   =============================================================================
	MouseRD() -- read a string from the mouse
   =============================================================================
*/

MouseRD(str, nc, nt)
char *str;
short nc, nt;
{
	long tc;

	tc = nt * (long)C_PER_S;

	while (nc > 0) {

		if (BIOS(B_RDAV, PRT_DEV)) {

			*str++ = (char)BIOS(B_GETC, PRT_DEV);
			*str = '\0';
			--nc;

		} else {

			if (tc-- LE 0)
				return(FAILURE);
		}
	}

	return(SUCCESS);
}

/*
   =============================================================================
	MouseWR() -- write a string to the mouse
   =============================================================================
*/

MouseWR(str)
char *str;
{
	register unsigned short c;

#if	DEBUGMS
	if (debugms > 1)
		printf("OUT \"%s\"\n", str);
#endif

	while (c = 0x00FF & *str++)		/* get a byte */
		BIOS(B_PUTC, PRT_DEV, c);	/* output it */
}

/* 
*/

/*
   =============================================================================
	MouseFL() -- flush the mouse input buffer
   =============================================================================
*/

short
MouseFL(tc)
short tc;
{
	long flushed;

	flushed = 0L;				/* reset the flush counter */

	M_state = 0;				/* reset mouse state */
	msflag  = FALSE;			/* reset mouse movement flag */
	msctrl  = FALSE;			/* reset mouse update flag */

	while (BIOS(B_RDAV, PRT_DEV)) {		/* check for data */

		BIOS(B_GETC, PRT_DEV);		/* read a byte */
		++flushed;			/* update flush count */

		if (--tc LE 0) {		/* see if we've timed out */

#if	DEBUGMS
	if (debugms)
		printf("FLUSH %d %ld FAILURE\n", M_state, flushed);
#endif

			return(FAILURE);	/* FAILED */
		}
	}

#if	DEBUGMS
	if (debugms)
		printf("FLUSH %d %ld SUCCESS\n", M_state, flushed);
#endif

	return(SUCCESS);			/* SUCCEEDED */
}

/* 
*/

/*
   =============================================================================
	MouseWK() -- wake up the mouse
   =============================================================================
*/

short
MouseWK()
{
	short tries;

	M_error = M_NONE;		/* reset mouse error flag */
	M_oldbs = 0;			/* reset mouse button state */
	M_state = 0;			/* reset the mouse state machine */

	/* set the UART to 1200 baud, 7 data bits, No parity, 1 stop bit */

	XBIOS(X_SETPRT, PRT_DEV, L_NUL, -1, CFR0, CFR1);

	if (MouseFL(M_FL_CT)) {		/* flush the FIFO */

		M_error = M_FLUSH;	/* error -- can't flush */

#if	DEBUGMS
	if (debugms)
		printf("MouseWK() -- FAILURE\n");
#endif

		return(FAILURE);	/* return -- we failed */
	}

	for (tries = 0; tries < MAXTRIES; tries++) {

		MouseWR("t");			/* ask the mouse its mode */

		if (MouseRD(M_strng, 2, M_ST_TM)) {

			M_error = M_NORESP;

		} else if (strcmp("VO", M_strng)) {

			M_error = M_RESPNG;

			if (MouseFL(M_FL_CT))
				M_error = M_FLUSH;

		} else {

			M_error = M_NONE;
			MouseRT(MATIME);

#if	DEBUGMS
	if (debugms)
		printf("MouseWK() -- SUCCESS\n");
#endif
			return(SUCCESS);
		}
	}

#if	DEBUGMS
	if (debugms)
		printf("MouseWK() -- FAILURE\n");
#endif

	return(FAILURE);
}


/* 
*/

/*
   =============================================================================
	MouseEX() -- process a mouse report
   =============================================================================
*/

MouseEX(str)
char *str;
{
	unsigned short oldi, msc, mst;

#if	DEBUGMS
	if (debugms > 2)
		printf("REPORT %02.2X %02.2X %02.2X\n",
			str[0] & 0x00FF, str[1] & 0x00FF, str[2] & 0x00FF);
#endif

	M_error = M_NONE;			/* reset error code */

	if ((str[0] & 0x0040) NE 0x0040) {

		if (MouseFL(M_FL_CT))
			M_error = M_FLUSH;
		else
			M_error = M_SYNC;

		M_state = 0;
		MouseRT(MATIME);

#if	DEBUGMS
	if (debugms)
		printf("SYNC\n");
#endif

		return;
	}

	mst     = str[0] & 0x0030;		/* extract mouse buttons */
	msc     = M_oldbs ^ mst;		/* see what changed */
	M_oldbs = mst;				/* update button state */

	if (msc) {			/* check for button status change */

		if (msc & 0x0010) {		/* m key ?  (right mouse button) */

			astat = (mst & 0x0010) >> 4;
			(*m_key)();
		}

		if (msc & 0x0020) {		/* e key ?  (left mouse button) */

			astat = (mst & 0x0020) >> 5;
			(*e_key)();
		}
	}

	if ((ss_ptsw NE 0) OR (ss_sqsw NE 0)) {

		cxrate = 0;
		cyrate = 0;
		return;
	}

/* 
*/

	msxmov  =  str[1] | ((str[0] & 0x0003) << 6);

	if (msxmov & 0x0080)
		msxmov |= 0xFF80;

	msxmov += msxres;

	msymov =  str[2] + ((str[0] & 0x000C) << 4);

	if (msymov & 0x0080)
		msymov |= 0xFF80;

	msymov += msyres;

	msxdiv = msxgdv[ndisp + 1];
	msydiv = msygdv[ndisp + 1];

	/* calculate rates */

	if (msxdiv > 1)
		cxrate = msxmov / msxdiv;
	else
		cxrate = msxmov;

	if (msydiv > 1)
		cyrate = msymov / msydiv;
	else
		cyrate = msymov;

	/* calculate residues */

	if (msxdiv > 1)
		msxres = msxmov % msxdiv;
	else
		msxres = 0;

	if (msydiv > 1)
		msyres = msymov % msydiv;
	else
		msyres = 0;

/* 
*/

	if ((cxrate EQ 0) AND (cyrate EQ 0))	/* we're done if nothing changed */
		return;

	msctrl  = TRUE;				/* setup movement switches */
	cmfirst = FALSE;
	trkball = FALSE;

	cvtime  = 0;				/* clear delays */
	ncvwait = 0;
	chtime  = 0;
	nchwait = 0;
	curhold = 0;

	if (NOT msflag)			/* if mouse just started moving ... */
		(*xy_dn)();		/* ... process key down */

	oldi = setipl(TIM_DI);		/* set the mouse movement timer */
	timers[MSTIMER] = MSTOVAL;
	setipl(oldi);

	msflag = TRUE;			/* indicate the mouse has started moving */
}

/* 
*/

/*
   =============================================================================
	MouseIN() -- process a byte from the mouse
   =============================================================================
*/

MouseIN(c)
short c;
{
	c &= 0x00FF;		/* mask off extraneous bits from mouse input */

#if	DEBUGMS
	if (debugms > 2)
		printf("IN %d %02.2X\n", M_state, c);
#endif

	if ((M_state GE 0) AND (M_state < 3)) {

		M_strng[M_state] = c;

		if (M_state EQ 2) {

			MouseEX(M_strng);
			M_state = 0;

		} else {

			++M_state;
		}
	}
}

/* 
*/

#if	DEBUGTF
/*
   =============================================================================
	tfdump() -- dump a trackball FIFO
   =============================================================================
*/

tfdump(msg, fifo, ndx, fin, cnt)
char *msg;
register short *fifo, ndx, fin;
long cnt;
{
	register short nol = 0, i = fin, j = fin - 1;

	printf("Dump of %s FIFO\n", msg);
	printf("  ndx=%d  fin=%d  cnt=%ld\n", ndx, fin, cnt);

	if (j < 0)
		j = NTKFIFO - 1;

	do {

		if (nol EQ 0)
			printf("    ");

		if (i EQ ndx)
			printf("<%2d  ", fifo[i]);
		else if (i EQ j)
			printf(" %2d> ", fifo[i]);
		else
			printf(" %2d  ", fifo[i]);

		if (++nol GE 15) {

			printf("\n");
			nol = 0;
		}

		if (++i GE NTKFIFO)
			i = 0; 

	} while (i NE fin);

	if (nol)
		printf("\n");

	printf("\n");
}
#endif

/* 
*/

/*
   =============================================================================
	wheel() -- process inputs from the scroll wheel
   =============================================================================
*/

wheel()
{
	register short i, oldi;

	if (astat) {	/* if it's touched ... */

		if (NOT swflag) {

			/* GOING DOWN */

			swflag  = TRUE;
			swctrl  = FALSE;
			swfiin  = 0;
			swlast  = aval;
			swstop  = swwait;
			swcount = 0;

			for (i = 0; i < NSWFIFO; i++)
				swfifo[i] = aval;
/* 
*/
		} else {		/* finger moved */

			if (0 EQ swstop) {

				swdelta = swlast - aval;

				if (swdelta GE swthr) {

					if ((clkctl EQ CK_STEP) OR
					     ((clkctl NE CK_STEP) AND
					      (NOT clkrun))) {

						/* FINGER MOVED LEFT */

						swtemp  = swdelta / swthr;
						oldi    = setipl(TIM_DI);
						fc_val += swtemp;

						if (fc_val GE 0x00FFFFFFL)
							fc_val = 0x00FFFFFFL;

						setipl(oldi);
						swlast = aval;
					}

				} else if ((swdelta = abs(swdelta)) GE swthr) {

					if ((clkctl EQ CK_STEP) OR
					     ((clkctl NE CK_STEP) AND
					      (NOT clkrun))) {

						/* FINGER MOVED RIGHT */

						swtemp = swdelta / swthr;
						oldi = setipl(TIM_DI);
						fc_val -= swtemp;

						if (fc_val LT 0L)
							fc_val = 0L;

						setipl(oldi);
						swlast = aval;
					}
				}

			} else {

				swlast = aval;
				--swstop;
			}
		}
/* 
*/
	} else {	/* FINGER UP */

		swlast = aval;
		swflag = FALSE;
		swctrl = FALSE;

		if ((clkctl EQ CK_STEP) OR
		    ((clkctl NE CK_STEP) AND
		     (NOT clkrun))) {

			swndx = swfiin - swback;

			if (swndx < 0)
				swndx += NSWFIFO;

			swrate = swfifo[swndx] - swlast;

			if (swrate > swrmin) {

				swdir  = D_FWD;		/* SCROLL FORWARD */
				swrate = swrtab[swrate - swrmin];
				swctrl = TRUE;

			} else if ((swrate = abs(swrate)) > swrmin) {

				swdir  = D_BAK;		/* SCROLL BACKWARD */
				swrate = swrtab[swrate - swrmin];
				swctrl = TRUE;

			} else {

				swrate = 0L;		/* STOP SCROLLING */
			}
		}
	}
}

/* 
*/

/*
   =============================================================================
	txyup() -- process trackball -- both axes inactive
   =============================================================================
*/

txyup()
{
	register short txndx, tyndx, txrate, tyrate;

#if	DEBUGTF
	if (debugtf)
		printf("txyup():  both inactive\n");
#endif

	tkboth = FALSE;				/* both axes inactive now */

	txdiv  = 0;				/* reset text cursor dividers */
	tydiv  = 0;

	if ((txndx = txfiin - tkback) < 0)	/* get fifo index */
		txndx += NTKFIFO;		/* adjust index */

	txrate = txfifo[txndx] - txlast;	/* get movement */

#if	DEBUGTF
	if (debugtf)
		tfdump("X", txfifo, txndx, txfiin, txficnt);
#endif

	if (txrate GE tkrmin)
		cxrate = -cratex[txrate - tkrmin];	/* tracking left */
	else if ((txrate = abs(txrate)) GE tkrmin)
		cxrate =  cratex[txrate - tkrmin];	/* tracking right */
	else
		cxrate = 0;				/* X inactive */

/* 
*/
	if ((tyndx = tyfiin - tkback) < 0)	/* get fifo index */
		tyndx += NTKFIFO;		/* adjust index */

	tyrate = tyfifo[tyndx] - tylast;	/* get movement */

#if	DEBUGTF
	if (debugtf)
		tfdump("Y", tyfifo, tyndx, tyfiin, tyficnt);
#endif

	if (tyrate GE tkrmin)
		cyrate =  cratey[tyrate - tkrmin];	/* tracking down */
	else if ((tyrate = abs(tyrate)) GE tkrmin)
		cyrate = -cratey[tyrate - tkrmin];	/* tracking up */
	else
		cyrate = 0;				/* Y inactive */

	if ((cxrate EQ 0) AND (cyrate EQ 0)) {

		tkctrl = FALSE;		/* STOP -- both rates are zero */
		(*xy_up)();

	} else {

		tkctrl = TRUE;		/* ROLL -- some rate is non-zero  */
	}

#if	DEBUGTK
	if (debugtk)
		printf("txyup():  %s  rmin=%d  txr=%d cxr=%d  tyr=%d cyr=%d\n",
			tkctrl ? "ROLL" : "STOP", tkrmin, txrate, cxrate, tyrate, cyrate);
#endif

}

/* 
*/

/*
   =============================================================================
	txydn() -- process trackball -- both axes active
   =============================================================================
*/

txydn()
{
	register short i;

#if	DEBUGTK
	if (debugtk)
		printf("txydn():  both active  txlast=%d  tylast=%d\n",
			txlast, tylast);
#endif

	tkboth = TRUE;			/* both down now */
	(*xy_dn)();

	tkctrl  = FALSE;		/* stop rolling */
	cxrate  = 0;
	cyrate  = 0;

	txfiin  = 0;			/* preset the FIFOs */
	tyfiin  = 0;

	for (i = 0; i < NTKFIFO; i++) {

		txfifo[i] = txlast;
		tyfifo[i] = tylast;
	}

#if	DEBUGTF
	txficnt = 0;
	tyficnt = 0;
#endif
}

/* 
*/

/*
   =============================================================================
	txstd() -- process inputs from the trackball X axis
   =============================================================================
*/

txstd()
{
	register short i, oldcx, oldi, txdelta, txcdvr;

	trkball = TRUE;				/* set trackball mode */
	cmfirst = FALSE;
	chtime  = tktime;
	nchwait = tktime;
	chwait  = tktime;


	/* get rate divisor */

	txcdvr  = (cmtype EQ CT_GRAF) ? tkthr : tkhdvr;

	if (astat) {				/* is axis active ? */

		if (NOT txflag) {

			/* GOING ACTIVE */

			txflag  = TRUE;
			txstop  = tkwait;
			txlast  = aval;

			if (tyflag AND (NOT tkboth))
				txydn();	/* both active now */

/* 
*/
		} else {			/* finger moved ? */

			if (txstop LE 0) {	/* debounced ? */

				txdelta = txlast - aval;
				oldcx   = cxval;

				if (txdelta GE txcdvr) {

					/* FINGER MOVED LEFT */

					cxrate  = -(txdelta / txcdvr);

					(*cx_upd)();	/* update cxval */

					if (oldcx NE cxval)	/* new cxval ? */
						(*curmove)();	/* move cursor */

					txlast = aval;
					cxrate = 0;

				} else if ((txdelta = abs(txdelta)) GE txcdvr) {

					/* FINGER MOVED RIGHT */

					cxrate = txdelta / txcdvr;

					(*cx_upd)();	/* update cxval */

					if (oldcx NE cxval)	/* new cxval ? */
						(*curmove)();	/* move cursor */

					txlast = aval;
					cxrate = 0;
				}

			} else {		/* debounce data */

				txlast = aval;
				--txstop;
			}
		}
/* 
*/
	} else {				/* AXIS GOING INACTIVE */

		txlast = aval;			/* get current value */
		txflag = FALSE;			/* X axis inactive */

		if (NOT tyflag)			/* check other axis */
			txyup();		/* both inactive now */
	}
}

/* 
*/

/*
   =============================================================================
	tystd() -- process inputs from the trackball Y axis
   =============================================================================
*/

tystd()
{
	register short i, oldcy, oldi, tydelta, tycdvr;

	trkball = TRUE;				/* set trackball mode */
	cmfirst = FALSE;
	cvtime  = tktime;
	ncvwait = tktime;
	cvwait  = tktime;

	/* get rate divisor */

	tycdvr  = (cmtype EQ CT_GRAF) ? tkthr : tkvdvr;

	if (astat) {				/* if axis active ? */

		if (NOT tyflag) {

			/* GOING ACTIVE */

			tyflag  = TRUE;
			tystop  = tkwait;
			tylast  = aval;

			if (txflag AND (NOT tkboth))
				txydn();	/* both active now */
/* 
*/
		} else {			/* finger moved ? */

			if (tystop LE 0) {	/* debounced ? */

				tydelta = tylast - aval;
				oldcy   = cyval;

				if (tydelta GE tycdvr) {

					/* FINGER MOVED DOWN */

					cyrate  = tydelta / tycdvr;

					(*cy_upd)();	/* update cyval */

					if (oldcy NE cyval)	/* new cyval ? */
						(*curmove)();	/* move cursor */

					tylast = aval;
					cyrate = 0;

				} else if ((tydelta = abs(tydelta)) GE tycdvr) {

					/* FINGER MOVED UP */

					cyrate = -(tydelta / tycdvr);

					(*cy_upd)();	/* udpate cyval */

					if (oldcy NE cyval)	/* new cyval ? */
						(*curmove)();	/* move cursor */

					tylast = aval;
					cyrate = 0;
				}

			} else {		/* debounce data */

				tylast = aval;
				--tystop;
			}
		}
/* 
*/
	} else {				/* AXIS GOING INACTIVE */

		tylast = aval;			/* get current value */
		tyflag = FALSE;			/* Y axis inactive */

		if (NOT txflag)			/* check other axis */
			txyup();		/* both inactive now */
	}
}

/* 
*/

/*
   =============================================================================
	curproc() -- process cursor trackball and scroll wheel updates
   =============================================================================
*/

curproc()
{
	register short i, cxprev, cyprev;
	short oldcx, oldcy;
	register unsigned short oldi;

	/* SET CURRENT WAIT COUNTS FROM TIMERS */

	chwait = timers[TXTIMER];
	cvwait = timers[TYTIMER];

	if (trkball) {		/* are we tracking ? */

		if (txflag AND (chwait LE 0)) {

			/* SAMPLE THE TRACKBALL X AXIS */

			txfifo[txfiin] = sigtab[55][0];

#if	DEBUGTF
	++txficnt;
#endif

			if (++txfiin GE NTKFIFO)
				txfiin = 0;
		}

		if (tyflag AND (cvwait LE 0)) {

			/* SAMPLE THE TRACKBALL Y AXIS */

			tyfifo[tyfiin] = sigtab[56][0];

#if	DEBUGTF
	++tyficnt;
#endif

			if (++tyfiin GE NTKFIFO)
				tyfiin = 0;
		}
	}
/* 
*/
	/* PROCESS MOUSE INPUTS */

	if (BIOS(B_RDAV, PRT_DEV)) {

		MouseIN((short)BIOS(B_GETC, PRT_DEV));

		MouseRT(MATIME);	/* reset mouse activity timer */
	}

	/* PROCESS THE CURSORS */

	if (cvwait LE 0) {		/* has the vertical timer run out ? */

		if (ss_ptsw)			/* see if the patches need scrolled */
			smy_up(ss_ptsw);	/* scroll the patch display */

		if (ss_sqsw)			/* see if the sequences need scrolled */
			sqy_up(ss_sqsw);	/* scroll the sequence display */
	}

	if (msctrl OR					/* is the mouse moving ? */
		(cvwait LE 0) OR (chwait LE 0)) {	/* has X or Y timer runout ? */

		if (msctrl OR tkctrl OR (xkstat AND ykstat)) {	/* cursor moving ? */

			oldcx   = cxrate;	/* save old cxrate, cyrate */
			oldcy   = cyrate;
			cxprev  = cxval;	/* save old cxval, cyval */
			cyprev  = cyval;
			vtprow  = vtcrow;	/* save old vtrow, vtcol */
			vtpcol  = vtccol;

			if (NOT msctrl)		/* if it's not the mouse ... */
				cmfix();	/* ... adjust the rates */

/* 
*/
			if ((cxrate NE 0) AND (msctrl OR (chwait LE 0)))	/* UPDATE X VALUES */
				(*cx_upd)();

			if ((cyrate NE 0) AND (msctrl OR (cvwait LE 0)))	/* UPDATE Y VALUES */
				(*cy_upd)();

			cxrate = oldcx;
			cyrate = oldcy;

			/* see if cursor should be moved */

			if ( (cxprev NE cxval)  OR (cyprev NE cyval) OR
			     (vtprow NE vtcrow) OR (vtpcol NE vtccol) ) {

				(*curmove)();	/* MOVE CURSOR */

				if (ebflag) {	/* FIXUP EDIT BUFFER */

					if ((struct fet *)NULL NE curfet) {

						if (infield(stcrow, stccol, curfet)) {

							if ((cfetp NE NULL) AND
							    (cfetp NE infetp)) {

								(*cfetp->redisp)(cfetp->ftags);
								ebflag = FALSE;
							}

						} else {	/* isn't in a field */

							if (cfetp NE NULL)
								(*cfetp->redisp)(cfetp->ftags);

							ebflag = FALSE;
						}

					} else {	/* no fet, no field */

						ebflag = FALSE;
					}
				}
			}
		}
	}
/* 
*/
	/* CHECK THE MOUSE MOTION TIMER */

	if (msflag AND (0 EQ timers[MSTIMER])) {

		(*xy_up)();		/* indicate key up */

		msflag = FALSE;		/* indicate mouse stopped */
	}

	msctrl = FALSE;		/* indicate mouse movement processed */


	/* CHECK THE MOUSE ACTIVITY TIMER */

	if (0 EQ timers[MUTIMER]) {

		if (M_state)
			if (MouseFL(M_FL_CT))
				M_error = M_FLUSH;

		M_state = 0;
		MouseRT(MATIME);

#if	DEBUGMS
	if (debugms > 1)
		printf("MRESET\n");
#endif
	}

/* 
*/
	/* UPDATE THE CURSOR TIMERS */

	if (cvwait LE 0) {	/* reset vertical timer */

		cvwait  = ncvwait;
		ncvwait = cvtime;

		oldi = setipl(TIM_DI);
		timers[TYTIMER] = cvwait;
		setipl(oldi);
	}


	if (chwait LE 0) {	/* reset horizontal timer */

		chwait  = nchwait;
		nchwait = chtime;

		oldi = setipl(TIM_DI);
		timers[TXTIMER] = chwait;
		setipl(oldi);
	}
/* 
*/
	/* PROCESS THE SCROLL WHEEL */

	if (0 EQ timers[SWTIMER]) {		/* is it time ? */

		if (swflag) {			/* SAMPLE THE SCROLL WHEEL */

			swfifo[swfiin] = sigtab[59][0];

			if (++swfiin GE NSWFIFO)
				swfiin = 0;
		}

		if (swctrl) {			/* SCROLL ... */

			swtemp   = swcount + swrate;
			swcount  = swtemp & 0x0000FFFFL;
			swtemp >>= 16;

			if (swdir EQ D_FWD) {	/* ... FORWARD */

				oldi = setipl(TIM_DI);
				fc_val += swtemp;

				if (fc_val GE 0x00FFFFFFL) {

					swctrl = FALSE;
					fc_val = 0x00FFFFFFL;
				}

				setipl(oldi);

			} else {		/* ... BACKWARD */

				oldi = setipl(TIM_DI);
				fc_val -= swtemp;

				if (fc_val < 0) {

					swctrl = FALSE;
					fc_val = 0L;
				}

				setipl(oldi);
			}
		}
/* 
*/
		if (swflag OR swctrl) {		/* RESET THE SCROLL TIMER */

			oldi = setipl(TIM_DI);
			timers[SWTIMER] = swtime;
			setipl(oldi);
		}
	}
}

/* 
*/

/*
   =============================================================================
	tkinit() -- initialize trackball variables
   =============================================================================
*/

tkinit()
{
	trkball = FALSE;		/* stop the trackball */
	txflag  = FALSE;		/* ... */
	tyflag  = FALSE;		/* ... */
	tkctrl  = FALSE;		/* ... */
	tkboth  = FALSE;		/* ... */

	memsetw(txfifo, 0, NTKFIFO);	/* clear trackball X fifo */
	txfiin = 0;			/* ... */

	memsetw(tyfifo, 0, NTKFIFO);	/* clear trackball Y fifo */
	tyfiin = 0;			/* ... */
}
