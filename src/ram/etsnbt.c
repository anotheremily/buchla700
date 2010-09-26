/*
   =============================================================================
	etsnbt.c -- beat field handlers
	Version 4 -- 1988-07-28 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "fields.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "stddefs.h"

#include "midas.h"
#include "scdsp.h"
#include "instdsp.h"

extern	unsigned	*obj8;

extern	short	clkrun;
extern	short	insmode;
extern	short	stccol;

extern	long	t_cur;

/* 
*/

/*
   =============================================================================
	et_snbt() -- load edit buffer
   =============================================================================
*/

short
et_snbt(n)
short n;
{
	register long curfr, beat;

	if (clkrun) {

		ebflag = FALSE;
		return(FAILURE);
	}

	curfr = t_cur;
	beat  = curfr / 48;

	sprintf(ebuf, "%05ld", beat);
	ebflag = TRUE;

	return(SUCCESS);
}

/*
   =============================================================================
	ef_snbt() -- parse edit buffer
   =============================================================================
*/

short
ef_snbt(n)
short n;
{
	register long ival;
	register short i;

	if (clkrun)
		return(FAILURE);

	ival = 0L;

	for (i = 0; i < 5; i++)
		ival = (ival * 10) + (ebuf[i] - '0');

	ebflag = FALSE;

	if (insmode) {

		icancel();
		dsimode();
	}

	sc_goto(fc_val = ival * 48L);
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_snbt() -- (re)display the field
   =============================================================================
*/

short
rd_snbt(n)
short n;
{
	register long curfr, beat;
	char	buf[8];

	curfr = t_cur;
	beat  = curfr / 48;
	sprintf(buf, "%05ld", beat);

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputs(obj8, 0, 11, buf, SDW00ATR);

	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_snbt() -- data entry function
   =============================================================================
*/

short
nd_snbt(n, k)
register short n, k;
{
	register short ec;

	if (clkrun)
		return(FAILURE);

	ec = stccol - cfetp->flcol;
	ebuf[ec]  = k + '0';

	if (v_regs[5] & 0x0180)
		vbank(0);

	vputc(obj8, 0, stccol, k + '0', SDW00DEA);
	advscur();

	return(SUCCESS);
}
