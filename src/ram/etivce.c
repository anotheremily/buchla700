/*
   =============================================================================
	etivce.c -- instrument editor - voice number field handlers
	Version 12 -- 1987-12-09 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "instdsp.h"

#define	VCE_OFF		6

extern	short	advicur(), newvce();

extern	unsigned	*instob;

extern	short	stccol, curvce;

extern	short	idbox[][8];

extern	char	dspbuf[];

/* 
*/

/*
   =============================================================================
	et_ivce() -- load the edit buffer
   =============================================================================
*/

short
et_ivce(n)
short n;
{
	sprintf(ebuf, "%02d", curvce + 1);
	ebflag = TRUE;

	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_ivce() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_ivce(n)
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
	allwins();
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_ivce() -- (re)display the field
   =============================================================================
*/

short
rd_ivce(n)
short n;
{
	/* convert to ASCII */

	sprintf(dspbuf, "%02d", curvce + 1);

	vbank(0);			/* display the value */

	vcputsv(instob, 64, idbox[n][4], idbox[n][5],
		idbox[n][6], idbox[n][7] + VCE_OFF, dspbuf, 14);

	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_ivce() -- handle new data entry
   =============================================================================
*/

short
nd_ivce(n, k)
short n;
register short  k;
{
	register short ec;

	ec = stccol - cfetp->flcol;	/* setup edit buffer column */
	ebuf[ec] = k + '0';
	ebuf[2] = '\0';

	dspbuf[0] = k + '0';
	dspbuf[1] = '\0';

	vbank(0);

	vcputsv(instob, 64, ID_ENTRY, idbox[n][5],
		idbox[n][6], stccol, dspbuf, 14);

	advicur();

	return(SUCCESS);
}

