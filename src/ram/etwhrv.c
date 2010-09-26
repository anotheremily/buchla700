/*
   =============================================================================
	etwhrv.c -- waveshape editor - harmonic value field handlers
	Version 12 -- 1988-09-09 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"
#include "charset.h"

#include "midas.h"
#include "instdsp.h"
#include "wsdsp.h"

extern	short	advwcur(), wdswin();

extern	unsigned	*waveob;

extern	short	stcrow, stccol, curwhrv, curvce, curwslt, curwhrm;

extern	short	wdbox[][8];

extern	char	dspbuf[];

extern	struct	instdef	vbufs[];

extern	short	wsnmod[12][2];

extern	short	vmtab[NUMHARM];

/* 
*/

/*
   =============================================================================
	et_whrv() -- load the edit buffer
   =============================================================================
*/

short
et_whrv(n)
short n;
{
	register short harv;
	register char hvsgn;

	if (curwhrv LT 0) {

		hvsgn = '-';
		harv = -curwhrv;

	} else {

		hvsgn = '+';
		harv = curwhrv;
	}

	sprintf(ebuf, "%c%03d", hvsgn, harv);
	ebflag = TRUE;

	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_whrv() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_whrv(n)
short n;
{
	register short *hv;
	register short i, tmpval;

	ebuf[4] = '\0';			/* terminate the string in ebuf */
	ebflag = FALSE;

	tmpval = 0;

	for (i = 1; i < 4; i++)		/* convert from ASCII to binary */
		tmpval = (tmpval * 10) + (ebuf[i] - '0');

	if (tmpval GT 100)
		return(FAILURE);

	if (ebuf[0] EQ '-')
		curwhrv = -tmpval;
	else
		curwhrv = tmpval;

	hv = curwslt ? vbufs[curvce].idhwvbh : vbufs[curvce].idhwvah;
	hv[curwhrm] = curwhrv;
	vmtab[curwhrm] = curwhrv;
	adj(curwhrm);
	wscalc();
	whupd();
	wsnmod[curvce][curwslt] = TRUE;
	wdswin(0);
	wdswin(2);
	wdswin(4);
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_whrv() -- (re)display the field
   =============================================================================
*/

short
rd_whrv(nn)
short nn;
{
	register short harv, n;
	register char hvsgn;

	n = nn & 0x00FF;

	if (curwhrv LT 0) {

		hvsgn = '-';
		harv = -curwhrv;

	} else {

		hvsgn = '+';
		harv = curwhrv;
	}

	sprintf(dspbuf, "%c%03d", hvsgn, harv);

	vbank(0);			/* display the value */
	vcputsv(waveob, 64, wdbox[n][4], wdbox[n][5],
		wdbox[n][6] + 1, wdbox[n][7] + WHRV_OFF, dspbuf, 14);

	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_whrv() -- handle new data entry
   =============================================================================
*/

short
nd_whrv(nn, k)
short nn;
register short  k;
{
	register short ec, n;

	n  = nn & 0x00FF;
	ec = stccol - cfetp->flcol;	/* setup edit buffer column */

	if (ec EQ 0) {

		if (k EQ 8) {

			ebuf[0] = dspbuf[0] = '-';
			ebuf[4] = dspbuf[1] = '\0';

		} else if (k EQ 9) {

			ebuf[0] = dspbuf[0] = '+';
			ebuf[4] = dspbuf[1] = '\0';

		} else {

			return(FAILURE);
		}

	} else {

		ebuf[ec] = dspbuf[0] = k + '0';
		ebuf[4]  = dspbuf[1] = '\0';
	}

	vbank(0);
	vcputsv(waveob, 64, WS_ENTRY, wdbox[n][5],
		stcrow, stccol, dspbuf, 14);

	advwcur();
	return(SUCCESS);
}
