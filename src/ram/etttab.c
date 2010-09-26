/*
   =============================================================================
	etttab.c -- MIDAS tuning editor - tuning table number field handlers
	Version 3 -- 1988-08-22 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "tundsp.h"

extern	unsigned	*tunob;

extern	short	stcrow, stccol;
extern	short	curtun, tunmod;

extern	short	tdbox[][8];

extern	char	dspbuf[];

/* 
*/

/*
   =============================================================================
	et_ttab() -- load the edit buffer
   =============================================================================
*/

short
et_ttab(n)
short n;
{
	sprintf(ebuf, "%d", curtun);
	ebflag = TRUE;

	return(SUCCESS);
}

/*
   =============================================================================
	ef_ttab() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_ttab(n)
short n;
{
	register short i, tmpval;

	ebuf[1] = '\0';			/* terminate the string in ebuf */
	ebflag = FALSE;

	curtun = ebuf[0] - '0';
	modtun();
	settc(22, 54);
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_ttab() -- (re)display the field
   =============================================================================
*/

short
rd_ttab(nn)
short nn;
{
	register short n;

	n = nn & 0xFF;
	sprintf(dspbuf, "%d", curtun);

	vbank(0);
	vcputsv(tunob, 64, (tunmod ? TDCHGD : tdbox[n][4]), tdbox[n][5],
		24, 61, dspbuf, 14);

	return(SUCCESS);
}

/*
   =============================================================================
	nd_ttab() -- handle new data entry
   =============================================================================
*/

short
nd_ttab(nn, k)
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
	vcputsv(tunob, 64, TDENTRY, tdbox[n][5], stcrow, stccol, dspbuf, 14);

	return(SUCCESS);
}
