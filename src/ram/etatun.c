/*
   =============================================================================
	etatun.c -- MIDAS assignment editor - tuning table field handlers
	Version 1 -- 1987-12-10 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "asgdsp.h"

extern	unsigned	*asgob;

extern	short	stcrow, stccol;
extern	short	curtun;

extern	short	adbox[][8];

extern	char	dspbuf[];

/* 
*/

/*
   =============================================================================
	et_atun() -- load the edit buffer
   =============================================================================
*/

short
et_atun(n)
short n;
{
	sprintf(ebuf, "%01.1d", curtun);
	ebflag = TRUE;

	return(SUCCESS);
}

/*
   =============================================================================
	ef_atun() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_atun(n)
short n;
{
	register short tmpval;

	ebuf[1] = '\0';			/* terminate the string in ebuf */
	ebflag = FALSE;

	tmpval = ebuf[0] - '0';

	gettun(tmpval);

	modasg();
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_atun() -- (re)display the field
   =============================================================================
*/

short
rd_atun(nn)
short nn;
{
	register short n;

	n = nn & 0xFF;
	sprintf(dspbuf, "Tun %01.1d", curtun);

	vbank(0);
	vcputsv(asgob, 64, adbox[n][4], adbox[n][5],
		adbox[n][6], adbox[n][7], dspbuf, 14);

	return(SUCCESS);
}

/*
   =============================================================================
	nd_atun() -- handle new data entry
   =============================================================================
*/

short
nd_atun(nn, k)
short nn;
register short  k;
{
	register short n;

	n = nn & 0xFF;
	ebuf[0] = k + '0';
	ebuf[1] = '\0';

	dspbuf[0] = k + '0';
	dspbuf[1] = '\0';

	vbank(0);
	vcputsv(asgob, 64, AK_ENTRY, adbox[n][5],
		stcrow, stccol, dspbuf, 14);

	return(SUCCESS);
}

