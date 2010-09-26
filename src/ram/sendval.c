/*
   =============================================================================
	sendval.c -- MIDAS-VII -- FPU output and clear functions
	Version 4 -- 1988-09-27 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "m7ver.h"
#include "stddefs.h"
#include "biosdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "hwdefs.h"
#include "memory.h"
#include "fpu.h"
#include "panel.h"

#include "midas.h"
#include "instdsp.h"
#include "score.h"
#include "scdsp.h"
#include "scfns.h"
#include "wsdsp.h"

/* functions defined elsewhere */

extern	unsigned	tofpu();

extern	int	setipl();

extern	short	amplval, ext_cv1, ext_cv2, ext_cv3, ext_cv4, ext_mod;
extern	short	lstbgnc, lstendc, ps_intn, ps_rate, ps_dpth, sliders;

extern	short	lastart[];
extern	short	dyntab[];
extern	short	expbit[];
extern	short	fnoff[];
extern	short	fp_resv[];
extern	short	grpdyn[];
extern	short	grploc[];
extern	short	lastvce[];
extern	short	prstab[];
extern	short	vce2grp[];
extern	short	vce2trg[];
extern	short	veltab[];

extern	short	sigtab[128][2];

extern	char	keystat[];
extern	char	mpsust[];
extern	char	trgtab[];

extern	struct	n_entry	*lstbgns[NLSTENTS];
extern	struct	n_entry	*lstends[NLSTENTS];

/* 
*/

/*
   =============================================================================
	sendval() -- send a value to a voice parameter
   =============================================================================
*/

sendval(voice, par, ival)
short voice, par, ival;
{
	register unsigned *fpu;
	register short oldi, val;
	register long ltmp;
	short nop;

	fpu = io_fpu + FPU_OFNC + (voice << 8) + (par << 4);

	switch (par) {

	case 1:		/* freq 1 */
	case 3:		/* freq 2 */
	case 5:		/* freq 3 */
	case 7:		/* freq 4 */

		val = addpch(ival, 0);
		break;

	case 4:		/* location */

		val = (ival << 1) ^ 0x8000;
		break;

	case 10:	/* filter */

		ltmp = ((long)ival >> 1) + ((long)ival >>2);

		if (ltmp GT (long)VALMAX)
			ltmp = (long)VALMAX;
		else if (ltmp LT (long)VALMIN)
			ltmp = (long)VALMIN;

		val = (short)ltmp;
		break;
/* 
*/
	case 2:		/* level */

		val = (((short)ival >> 5) - 500) << 6;
		break;

	default:

		val = ival;
	}

	oldi = setipl(FPU_DI);

/* ++++++++++++++++++++++++++++ FPU interrupts disabled +++++++++++++++++++++ */

	*(fpu + (long)FPU_TNV0) = val;
	++nop;	++nop;	++nop;
	*(fpu + (long)FPU_TNV1) = val;
	++nop;	++nop;	++nop;
	*(fpu + (long)FPU_TCTL) = 0x0015;

	setipl(oldi);

/* ++++++++++++++++++++++++++++ Interrupts restored +++++++++++++++++++++++++ */

}

/* 
*/

/*
   =============================================================================
	clearer() -- clear the FPU
   =============================================================================
*/

clearer(stat, sig)
short stat, sig;
{
	register unsigned *fpu;
	register unsigned fpexp, fpmant, fptime;
	register short oldi, i;
	short nop, olds;

	if (stat) {

		fp_resv[0]  = 0x8300;			/* amplitude (off) */
		fp_resv[1]  = (ps_intn * 10) << 5;	/* intensity */
		fp_resv[2]  = (ps_rate * 10) << 5;	/* sweep rate */
		fp_resv[3]  = (ps_dpth * 10) << 5;	/* sweep depth */
		fp_resv[4]  = 0;			/* - unused - */
		fp_resv[5]  = 0;			/* - unused - */
		fp_resv[6]  = ext_cv3;			/* CV-3 */
		fp_resv[7]  = 0;			/* - unused - */
		fp_resv[8]  = ext_cv4;			/* CV-4 */
		fp_resv[9]  = ext_mod;			/* Aux Signal Mod */
		fp_resv[10] = ext_cv2;			/* CV-2 */
		fp_resv[11] = ext_cv1;			/* CV-1 */

		fpu    = io_fpu + FPU_OFNC;	/* setup to set slew rate */
		fptime = tofpu(AMP_TIME);
		fpexp  = expbit[fptime & 0x000F];
		fpmant = fptime & 0xFFF0;
/* 
*/
		oldi = setipl(FPU_DI);		/* disable FPU interrupts */

/* ++++++++++++++++++++++++++++ FPU interrupts disabled +++++++++++++++++++++ */

		fpuclr();			/* reset the FPU to nominal */

		*(fpu + FPU_TMNT) = fpmant;	/* set amplitude slew rate */
		++nop;	++nop;
		*(fpu + FPU_TEXP) = fpexp;

		/* reset execution control tables */

		memset(trgtab,   0, NTRIGS);		/* trigger table */
		memset(keystat,  0, 24);		/* local key status */

		memsetw(prstab,  0, NTRIGS);		/* pressure table */
		memsetw(veltab, SM_SCALE(64), NTRIGS);	/* velocity table */

		clrpfl();			/* clear pendant functions */

		for (i = 0; i < 48; i++)	/* sustain pedal table */
			mpsust[i] = 0;

		for (i = 0; i < 12; i++) {

			vce2trg[i] = -1;	/* voice to trigger table */
			lastvce[i] = 0;		/* last voice used table */
		}

		olds = sliders;			/* reset articulation pots */
		sliders = LS_PRMTR;
		l_init(1, 0);
		sliders = LS_OTHER;
		l_init(1, 0);
		sliders = olds;

		setipl(oldi);			/* restore interrupts */

/* ++++++++++++++++++++++++++++ Interrupts restored +++++++++++++++++++++++++ */

/* 
*/
		lstbgnc = 0;		/* reset last note begin count */
		lstendc = 0;		/* reset last note end count */

		memsetw(lstbgns, 0, (NLSTENTS << 1));	/* clear begin table */
		memsetw(lstends, 0, (NLSTENTS << 1));	/* clear end table */

	 	/* re-establish dynamics and locations */

		for (i = 0; i < 12; i++) {

			setdyn(i, grpdyn[i]);
			setloc(i, grploc[i]);
		}

		settune();			/* set tuning */
		sendval(0, 0, amplval);		/* set amplitude */
	}
}
