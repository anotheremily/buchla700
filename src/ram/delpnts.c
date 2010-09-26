/*
   =============================================================================
	delpnts.c -- delete point(s) from (truncate) a function
	Version 14 -- 1989-12-19 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#define	D_INSPNT	0	/* debug inspnt() */

#include "stddefs.h"
#include "fpu.h"
#include "graphdef.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "smdefs.h"

#include "midas.h"
#include "instdsp.h"

#define	PT_SIZE	(sizeof (struct instpnt))

#if	DEBUGIT
extern	short	debugsw;

short	debugdf = 1;
#endif

extern	short	curfunc;
extern	short	curvce;
extern	short	pntsv;
extern	short	subj;

extern	short	finival[];
extern	short	fnoff[];

extern	struct	instdef	vbufs[];

/* 
*/

/*
   =============================================================================
	delpnts() -- deletes the current point and any points to the right
		of it in the current function in the current voice.
   =============================================================================
*/

short
delpnts()
{
	register struct instpnt *pp;
	register char *pp1, *pp2;
	register short np, pt1, i, pif, cf;
	struct idfnhdr *fp;
	struct instdef *vp;
	unsigned *fpu;
	short pt2, nmv, oldi;

	vp  = &vbufs[curvce];			/* voice buffer pointer */
	fp  = &vp->idhfnc[curfunc];		/* function pointer */

	pif = 0x00FF & fp->idfpif;		/* number of points in function */
	np  = pif - subj;			/* number of points to delete */
	pt1 = (0x00FF & fp->idfpt1) + subj;	/* first point to delete */

#if	DEBUGIT
	if (debugsw AND debugdf) {

		printf("delpnts():  curfunc = %d  curvce = %d\n",
			curfunc, curvce);

		printf("delpnts():  idfpt1=%d, pif=%d, np=%d, pt1=%d, vp=$%lX, fp=$%lX\n",
			(0x00FF & fp->idfpt1), pif, np, pt1, vp, fp);
	}
#endif

	if (np LE 0)			/* have to delete at least 1 point */
		return(FAILURE);

	if (subj GE pif)		/* make sure point number is valid */
		return(FAILURE);

	if ((pif - np) < 0)		/* make sure we have enough points */
		return(FAILURE);

	if ((subj + np) GE (pif + 1))	/* check the span */
		return(FAILURE);

	pt2 = pt1 + np;			/* move from point */
	nmv = NIPNTS - pt2;		/* move count */

#if	DEBUGIT
	if (debugsw AND debugdf) {

		printf("delpnts():  pt2=%d, nmv=%d\n", pt2, nmv);

		printf("  fnc   pif\n");

		for (cf = 0; cf < NFINST; cf++)
			printf("  %3d   %3d%s\n",
				cf, vp->idhfnc[cf].idfpif,
				(cf EQ curfunc) ? " <-- curfunc" : "");

		printf("\n");
	}
#endif

/* 
*/
	oldi = setipl(FPU_DI);		/* +++++ disable FPU interrupts +++++ */

	fpu = io_fpu + FPU_OFNC + (curvce << 8);	/* get fpu base */

	for (i = 0; i < NFINST; i++) {	/* stop all functions for this voice */

		fp  = &vp->idhfnc[i];	/* point at the function */

		*(fpu + (fnoff[i] << 4) + FPU_TCTL) =
		   (fp->idftmd = (fp->idftmd & ~3) | 1);
	}
					
	fp  = &vp->idhfnc[curfunc];	/* point at the function */

	if (subj) {			/* deleting trailing points */

		/* move points down */

		pp1 = &vp->idhpnt[pt1];
		pp2 = &vp->idhpnt[pt2];

		for (i = nmv * PT_SIZE; i > 0; i--)
			*pp1++ = *pp2++;

		/* adjust total points remaining */

		vp->idhplft += np;

		/* adjust number of points in this function */

		vp->idhfnc[curfunc].idfpif -= np;

		/* adjust starting points in other functions */

		for (cf = curfunc + 1; cf < NFINST; cf++)
			vp->idhfnc[cf].idfpt1 -= np;

		setipl(oldi);		/* +++++ restore interrupts +++++ */

		edfunc(curfunc);	/* set new current point */
		subj -= 1;
/* 
*/
	} else {			/* deleting all points */

		/* reset first point in function */

		pp = &vp->idhpnt[fp->idfpt1];

		pp->iptim  = FPU_MINT;
		pp->ipval  = finival[curfunc];
		pp->ipvmlt = 0;
		pp->ipvsrc = SM_NONE;
		pp->ipact  = AC_NULL;
		pp->ippar1 = 0;
		pp->ippar2 = 0;
		pp->ippar3 = 0;

		/* adjust functions */

		if (np > 1) {		/* if deleting more points than 1 ... */

			--nmv;		/* one less point to move */
			++pt1;		/* start one slot up */

			/* move points down */

			pp1 = &vp->idhpnt[pt1];
			pp2 = &vp->idhpnt[pt2];

			for (i = nmv * PT_SIZE; i > 0; i--)
				*pp1++ = *pp2++;

			/* adjust total points remaining */

			vp->idhplft += (np - 1);

			/* adjust number of points in this function */

			vp->idhfnc[curfunc].idfpif -= (np - 1);

			/* adjust starting points in other functions */

			for (cf = curfunc + 1; cf < NFINST; cf++)
				vp->idhfnc[cf].idfpt1 -= (np - 1);
		}

		setipl(oldi);		/* restore interrupts */

		edfunc(curfunc);	/* make point 0 current */
		subj = 0;
	}	

/* 
*/
#if	DEBUGIT
	if (debugsw AND debugdf) {

		printf("delpnts():  plft = %3d  pif = %3d  subj = %3d\n",
			vp->idhplft, vp->idhfnc[curfunc].idfpif, subj);

		printf("  fnc   pif\n");

		for (cf = 0; cf < NFINST; cf++)
			printf("  %3d   %3d%s\n",
				cf, vp->idhfnc[cf].idfpif,
				(cf EQ curfunc) ? " <-- curfunc" : "");

		printf("\n");
	}
#endif

	pntsel();
	pntsv = 0;
	showpt(1);
	modinst();

	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	inspnt() -- insert a new point into a function
   =============================================================================
*/

short
inspnt(ip, fn, inpnt)
register struct instdef *ip;
short fn, inpnt;
{
	register char *fp1, *fp2;
	register short i, j, k, l, npts;
	short topnt, frompt, oldi;

	if (ip->idhplft EQ 0)		/* see if instrument has points left */
		return(FALSE);

	if (ip->idhfnc[fn].idfpif EQ 99)	/* see if function is full */
		return(FALSE);

	topnt = NIPNTS - ip->idhplft;		/* calculate move parameters */
	frompt = topnt - 1;
	npts = frompt - inpnt;
	i = topnt;
	j = frompt;

#if D_INSPNT
	if (debugsw)
		printf("inspnt():  fn=%d, in=%d, to=%d, from=%d, npts=%d, pif=%d\r\n",
			fn, inpnt, topnt, frompt, npts, ip->idhfnc[fn].idfpif);
#endif

/* 
*/
	oldi = setipl(FPU_DI);		/* disable FPU interrupts */

/* ++++++++++++++++++++++++ FPU interrupts disabled +++++++++++++++++++++++++ */

	for (k = 0; k < npts; k++) {		/* move things up */

		fp1 = &ip->idhpnt[i--];
		fp2 = &ip->idhpnt[j--];

		for (l = 0; l < sizeof (struct instpnt); l++)
			*fp1++ = *fp2++;
	}

	for (i = fn + 1; i < NFINST; i++) {	/* update point numbers */

		++ip->idhfnc[i].idfpt1;		/* first point */
		++ip->idhfnc[i].idfcpt;		/* current point */
	}

	setipl(oldi);		/* restore interrupts */

/* ++++++++++++++++++++++++++ Interrupts restored +++++++++++++++++++++++++++ */

	++ip->idhfnc[fn].idfpif;	/* update point totals */
	--ip->idhplft;

#if D_INSPNT
	if (debugsw)
		printf("inspnt():  idfpif=%d, idhplft=%d\r\n",
			ip->idhfnc[fn].idfpif, ip->idhplft);
#endif

	return(TRUE);
}
