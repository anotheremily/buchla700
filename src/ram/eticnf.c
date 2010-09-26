/*
   =============================================================================
	eticnf.c -- instrument editor - configuration number field handlers
	Version 16 -- 1988-08-23 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "hwdefs.h"
#include "fpu.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "instdsp.h"

#define	CFG_OFF		10	/* display offset into configuration field */

extern	unsigned	*instob;

extern	short	stccol, curvce;
extern	short	idbox[][8];

extern	char	dspbuf[];

extern	struct	instdef	vbufs[];

/* 
*/

/*
   =============================================================================
	et_icnf() -- load the edit buffer
   =============================================================================
*/

short
et_icnf(n)
short n;
{
	sprintf(ebuf, "%02d", vbufs[curvce].idhcfg);
	ebflag = TRUE;

	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_icnf() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_icnf(n)
short n;
{
	register short i, tmpval;

	ebuf[2] = '\0';			/* terminate the string in ebuf */
	ebflag = FALSE;

	tmpval = 0;

	for (i = 0; i < 2; i++)		/* convert from ASCII to binary */
		tmpval = (tmpval * 10) + (ebuf[i] - '0');

	if (tmpval GE NUMCFG)		/* check against limit */
		return(FAILURE);

	vbufs[curvce].idhcfg = tmpval;
	dosync(curvce);
	showcfg(tmpval);
	modinst();
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_icnf() -- (re)display the field
   =============================================================================
*/

short
rd_icnf(n)
short n;
{
	sprintf(dspbuf, "%02d", vbufs[curvce].idhcfg);	/* convert */

	if (v_regs[5] & 0x0180)
		vbank(0);

	vcputsv(instob, 64, idbox[n][4], idbox[n][5],		/* display */
		idbox[n][6], idbox[n][7] + CFG_OFF, dspbuf, 14);

	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_icnf() -- handle new data entry
   =============================================================================
*/

short
nd_icnf(n, k)
short n;
register short  k;
{
	register short ec, c;

	ec = stccol - cfetp->flcol;	/* setup edit buffer column */
	ebuf[ec] = k + '0';		/* enter new data in buffer */
	ebuf[2] = '\0';			/* make sure string is terminated */

	dspbuf[0] = k + '0';		/* setup for display */
	dspbuf[1] = '\0';

	if (v_regs[5] & 0x0180)
		vbank(0);

	/* display the new data */

	vcputsv(instob, 64, ID_ENTRY, idbox[n][5],
		idbox[n][6], stccol, dspbuf, 14);

	advicur();			/* advance cursor */

	return(SUCCESS);
}

