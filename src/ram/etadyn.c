/*
   =============================================================================
	etadyn.c -- MIDAS assignment editor -- dynamics to group field handlers
	Version 7 -- 1988-07-13 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "fpu.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "asgdsp.h"
#include "instdsp.h"

extern	short	setsr();

extern	unsigned	*asgob;

extern	unsigned	curintp;

extern	short		stccol;
extern	short		stcrow;
extern	short		timemlt;

extern	char		dspbuf[];

extern	short		vce2grp[];
extern	short		grpdyn[];
extern	short		dyntab[];

extern	unsigned	expbit[];

extern	short		adbox[][8];

extern	struct instdef	vbufs[];

/* 
*/

/*
   =============================================================================
	et_adyn() -- load the edit buffer
   =============================================================================
*/

short
et_adyn(n)
short n;
{
	register short grp;

	grp = 0x00FF & (n >> 8);

	ebuf[0] = grpdyn[grp] + '0';
	ebuf[1] = '\0';

	ebflag = TRUE;

	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	setdyn() -- set dynamics for a group
   =============================================================================
*/

setdyn(grp, dyn)
short grp, dyn;
{
	register short g, i, val;
	register long ltmp;
	register unsigned *fpu;
	register struct idfnhdr *fp;
	unsigned fpmant, fpexp;
	short oldsr;
	short nop;

	grpdyn[grp] = dyn;
	val = dyntab[dyn];
	g = grp + 1;

	fpmant = (((long)curintp & 0x0000FFF0L) *
		  ((long)timemlt & 0x0000FFFFL)) >> 15;

	fpexp = expbit[curintp & 0x000F];

/* 
*/
	for (i = 0; i < 12; i++)
		if (g EQ vce2grp[i]) {

			fp = &vbufs[i].idhfnc[8];
			fpu = io_fpu + FPU_OFNC + (i << 8) + 0x0080;

			oldsr = setsr(0x2200);

/* ++++++++++++++++++++++++++++ FPU interrupts disabled +++++++++++++++++++++ */

			fp->idftmd ^= I_NVBITS;
			*(fpu + (long)FPU_TMNT) = fpmant;
			++nop;	++nop;	++nop;
			*(fpu + (long)FPU_TEXP) = fpexp;
			++nop;	++nop;	++nop;

			if (fp->idftmd & I_VNSUBN)
				*(fpu + (long)FPU_TNV1) = val;
			else
				*(fpu + (long)FPU_TNV0) = val;

			++nop;	++nop;	++nop;

			*(fpu + (long)FPU_TCTL) =
				(fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0001;

			setsr(oldsr);

/* ++++++++++++++++++++++++++++ Interrupts restored +++++++++++++++++++++++++ */

		}
}

/* 
*/

/*
   =============================================================================
	ef_adyn() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_adyn(n)
short n;
{
	register short dyn, grp;

	grp = 0x00FF & (n >> 8);

	ebuf[1] = '\0';			/* terminate the string in ebuf */
	ebflag = FALSE;

	dyn = ebuf[0] - '0';

	setdyn(grp, dyn);
	modasg();
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_adyn() -- (re)display the field
   =============================================================================
*/

short
rd_adyn(nn)
short nn;
{
	register short n, grp;

	n = 0x00FF & nn;
	grp = 0x00FF & (nn >> 8);

	dspbuf[0] = grpdyn[grp] + '0';
	dspbuf[1] = '\0';

	vbank(0);
	vcputsv(asgob, 64, adbox[n][4], adbox[n][5],
		cfetp->frow, cfetp->flcol, dspbuf, 14);

	return(SUCCESS);
}

/*
   =============================================================================
	nd_adyn() -- handle new data entry
   =============================================================================
*/

short
nd_adyn(nn, k)
short nn;
register short  k;
{
	register short n;

	n = nn & 0xFF;

	dspbuf[0] = ebuf[0] = k + '0';
	dspbuf[1] = ebuf[1] = '\0';

	vbank(0);
	vcputsv(asgob, 64, AK_ENTRY, adbox[n][5], stcrow, stccol, dspbuf, 14);

	return(SUCCESS);
}

