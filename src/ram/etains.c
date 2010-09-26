/*
   =============================================================================
	etains.c -- MIDAS assignment editor -- instrument to groups
	Version 10 -- 1988-07-11 -- D.N. Lynx Crowe
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

extern	short	asgmod;
extern	short	stccol;
extern	short	stcrow;

extern	char	dspbuf[];

extern	short	ins2grp[];

extern	short	adbox[][8];

/* 
*/

/*
   =============================================================================
	modasg() -- indicate that the assignment table was modified
   =============================================================================
*/

modasg()
{
	if (NOT asgmod) {

		asgmod = TRUE;
		adswin(0);
	}
}

/*
   =============================================================================
	et_ains() -- load the edit buffer
   =============================================================================
*/

short
et_ains(n)
short n;
{
	register short grp;

	char buf[4];

	grp = 0x00FF & (n >> 8);

	sprintf(ebuf, "%02.2d", ins2grp[grp] & 0x00FF);
	ebflag = TRUE;

	return(SUCCESS);
}

/*
   =============================================================================
	ef_ains() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_ains(n)
short n;
{
	register short tmpval, grp, i;

	grp = 0x00FF & (n >> 8);
	ebuf[2] = '\0';
	ebflag = FALSE;
	tmpval = 0;

	for (i = 0; i < 2; i++)
		tmpval = (tmpval * 10) + (ebuf[i] - '0');

	if (tmpval GE NINST )
		return(FAILURE);

	ins2grp[grp] = tmpval | (ins2grp[grp] & 0xFF00);
	modasg();
	setv2gi(grp);
	setinst();
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_ains() -- (re)display the field
   =============================================================================
*/

short
rd_ains(nn)
short nn;
{
	register short n, grp;
	char buf[4];

	n = 0x00FF & nn;
	grp = 0x00FF & (nn >> 8);

	sprintf(dspbuf, "%02.2d", ins2grp[grp] & 0x00FF);

	vbank(0);
	vcputsv(asgob, 64, adbox[n][4], adbox[n][5],
		cfetp->frow, cfetp->flcol, dspbuf, 14);

	return(SUCCESS);
}

/*
   =============================================================================
	nd_ains() -- handle new data entry
   =============================================================================
*/

short
nd_ains(nn, k)
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

