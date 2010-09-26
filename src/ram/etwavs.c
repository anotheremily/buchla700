/*
   =============================================================================
	etwavs.c -- waveshape editor - waveshape number field handlers
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

extern	short	stccol, curwave, curvce, curwslt;

extern	short	wsnmod[12][2];

extern	short	wdbox[][8];

extern	char	dspbuf[];

/* 
*/

/*
   =============================================================================
	et_wavs() -- load the edit buffer
   =============================================================================
*/

short
et_wavs(n)
short n;
{
	sprintf(ebuf, "%02d", curwave + 1);
	ebflag = TRUE;

	return(SUCCESS);
}

/*
   =============================================================================
	ef_wavs() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_wavs(n)
short n;
{
	register short i, tmpval;

	ebuf[2] = '\0';			/* terminate the string in ebuf */
	ebflag = FALSE;

	tmpval = 0;

	for (i = 0; i < 2; i++)		/* convert from ASCII to binary */
		tmpval = (tmpval * 10) + (ebuf[i] - '0');

	if ((tmpval GT NUMWAVS) OR (tmpval EQ 0))
		return(FAILURE);

	wsnmod[curvce][curwslt] = TRUE;
	curwave = tmpval - 1;
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_wavs() -- (re)display the field
   =============================================================================
*/

short
rd_wavs(nn)
short nn;
{
	register short n;
	register unsigned cx;

	cx = exp_c(wsnmod[curvce][curwslt] ? WS_CHGC : wdbox[n][4]);
	n = nn & 0x00FF;

	sprintf(dspbuf, "%02d", curwave + 1);

	vbank(0);
	vcputsv(waveob, 64, cx, wdbox[n][5],
		wdbox[n][6], wdbox[n][7] + WAVE_OFF, dspbuf, 14);

	return(SUCCESS);
}

/*
   =============================================================================
	nd_wavs() -- handle new data entry
   =============================================================================
*/

short
nd_wavs(nn, k)
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
		wdbox[n][6], stccol, dspbuf, 14);

	advwcur();
	return(SUCCESS);
}

