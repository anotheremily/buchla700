/*
   =============================================================================
	etwvce.c -- waveshape editor - voice field handlers
	Version 8 -- 1987-12-11 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "wsdsp.h"

extern	short	advwcur();

extern	unsigned	*waveob;

extern	short	stcrow, stccol, curvce;

extern	short	wdbox[][8];

extern	char	dspbuf[];

/* 
*/

/*
   =============================================================================
	et_wvce() -- load the edit buffer
   =============================================================================
*/

short
et_wvce(n)
short n;
{
	sprintf(ebuf, "%02d", curvce + 1);
	ebflag = TRUE;

	return(SUCCESS);
}

/*
   =============================================================================
	ef_wvce() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_wvce(n)
short n;
{
	register short i, tmpval;

	ebuf[2] = '\0';			/* terminate the string in ebuf */
	ebflag = FALSE;
	tmpval = 0;

	for (i = 0; i < 2; i++)		/* convert from ASCII to binary */
		tmpval = (tmpval * 10) + (ebuf[i] - '0');

	if ((tmpval EQ 0) OR (tmpval GT 12))
		return(FAILURE);

	newvce(tmpval - 1);
	wwins();
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_wvce() -- (re)display the field
   =============================================================================
*/

short
rd_wvce(nn)
short nn;
{
	register short n;

	n = nn & 0xFF;
	sprintf(dspbuf, "%02d", curvce + 1);

	vbank(0);
	vcputsv(waveob, 64, wdbox[n][4], wdbox[n][5],
		wdbox[n][6], wdbox[n][7] + WVCE_OFF, dspbuf, 14);

	return(SUCCESS);
}

/*
   =============================================================================
	nd_wvce() -- handle new data entry
   =============================================================================
*/

short
nd_wvce(nn, k)
short nn;
register short  k;
{
	register short ec, n;

	n = nn & 0xFF;
	ec = stccol - cfetp->flcol;	/* setup edit buffer column */
	ebuf[ec] = k + '0';
	ebuf[2] = '\0';

	dspbuf[0] = k + '0';
	dspbuf[1] = '\0';

	vbank(0);
	vcputsv(waveob, 64, WS_ENTRY, wdbox[n][5],
		stcrow, stccol, dspbuf, 14);

	advwcur();
	return(SUCCESS);
}
