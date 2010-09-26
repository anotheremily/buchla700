/*
   =============================================================================
	etagch.c -- assignment editor - group to channel number field handlers
	Version 6 -- 1987-12-10 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "asgdsp.h"

extern	char	*numblk();

extern	unsigned	*asgob;

extern	short	stcrow, stccol;

extern	short	adbox[][8];

extern	char	dspbuf[];

extern	short	grp2prt[12][2];	/* group to port and channel table */
				/* port [0] = 0..4, channel [1] = -1, 1..16 */

/* 
*/

/*
   =============================================================================
	et_agch() -- load the edit buffer
   =============================================================================
*/

short
et_agch(n)
short n;
{
	register short grp;

	char buf[4];

	grp = 0x00FF & (n >> 8);

	numblk(ebuf, grp2prt[grp][1]);
	ebflag = TRUE;

	return(SUCCESS);
}

/*
   =============================================================================
	ef_agch() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_agch(n)
short n;
{
	register short tmpval, grp, i;

	grp = 0x00FF & (n >> 8);

	ebuf[2] = '\0';			/* terminate the string in ebuf */
	ebflag = FALSE;
	tmpval = 0;

	if ((ebuf[0] EQ ' ') AND (ebuf[1] EQ ' ')) {

		tmpval = -1;

	} else {

		for (i = 0; i < 2; i++) {

			if (ebuf[i] EQ ' ')
				ebuf[i] = '0';

			tmpval = (tmpval * 10) + (ebuf[i] - '0');
		}

		if ((tmpval EQ 0) OR (tmpval GT 16))
			return(FAILURE);
	}

	grp2prt[grp][1] = tmpval;

	modasg();
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_agch() -- (re)display the field
   =============================================================================
*/

short
rd_agch(nn)
short nn;
{
	register short n, grp;
	char buf[4];

	n = 0x00FF & nn;
	grp = 0x00FF & (nn >> 8);

	vbank(0);
	vcputsv(asgob, 64, adbox[n][4], adbox[n][5],
		cfetp->frow, cfetp->flcol, numblk(buf, grp2prt[grp][1]), 14);

	return(SUCCESS);
}

/*
   =============================================================================
	nd_agch() -- handle new data entry
   =============================================================================
*/

short
nd_agch(nn, k)
short nn;
register short  k;
{
	register short n;

	n = nn & 0xFF;
	ebuf[stccol - cfetp->flcol] = k + '0';
	ebuf[2] = '\0';

	dspbuf[0] = k + '0';
	dspbuf[1] = '\0';

	vbank(0);
	vcputsv(asgob, 64, AK_ENTRY, adbox[n][5],
		stcrow, stccol, dspbuf, 14);

	advacur();
	return(SUCCESS);
}

