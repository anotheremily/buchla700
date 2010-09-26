/*
   =============================================================================
	etipnt.c -- instrument editor - point number field handlers
	Version 15 -- 1988-01-13 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "instdsp.h"

extern	short	advicur(), pntsel(), showpt(), timeto(), setseg();

extern	unsigned	*instob;

extern	short	stccol, subj, curvce, curfunc, curpnt;

extern	short	idbox[][8];

extern	char	dspbuf[];

extern	struct	instdef	vbufs[];

extern	struct	instpnt	*pntptr;

/* 
*/

/*
   =============================================================================
	et_ipnt() -- load the edit buffer
   =============================================================================
*/

short
et_ipnt(n)
short n;
{
	sprintf(ebuf, "%02d", subj);
	ebflag = TRUE;

	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_ipnt() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_ipnt(n)
short n;
{
	register short i, tmpval, endpnt, basept;
	register struct idfnhdr *fp;
	register struct instdef *ip;

	ip = &vbufs[curvce];		/* set instrument pointer */
	fp = &ip->idhfnc[curfunc];	/* set function pointer */

	ebuf[2] = '\0';			/* terminate the string in ebuf */
	ebflag = FALSE;

	tmpval = 0;

	for (i = 0; i < 2; i++)		/* convert from ASCII to binary */
		tmpval = (tmpval * 10) + (ebuf[i] - '0');

	basept = fp->idfpt1;
	endpnt = fp->idfpt1 + fp->idfpif - 1;	/* calculate last point */
#if DEBUGIT
	printf("ef_ipnt():  subj=%d, basept=%d, tmpval=%d, endpnt=%d, idpif=%d\r\n",
		subj, basept, tmpval, endpnt, fp->idfpif);
#endif
	if (tmpval GE fp->idfpif) {	/* see if we want a new point */

		if (fp->idfpif EQ 100) {	/* can't do it if we're full */

			showpt(1);
			return(FAILURE);
		}
#if DEBUGIT
	printf("ef_ipnt():  NEW POINT REQUESTED\r\n");
#endif
		if (FALSE EQ inspnt(ip, curfunc, endpnt)) {	/* try to add it */
#if DEBUGIT
	printf("ef_ipnt():  inspnt() returned FALSE -- no point allocated\r\n");
#endif
			showpt(1);
			return(FAILURE);

		} else {

			subj = fp->idfpif - 1;	/* make it the current point */
			pntsel();
			memset(pntptr, 0, sizeof (struct instpnt));
			pntptr->ipval = (pntptr - 1)->ipval;
			setseg(subj, timeto(curfunc, subj - 1) + 1);
			showpt(1);
#if DEBUGIT
	printf("ef_ipnt():  NEW POINT SELECTED  curpnt=%d, subj=%d, idpif=%d\r\n",
		curpnt, subj, fp->idfpif);
#endif
			modinst();
			return(SUCCESS);
		}

	} else {	/* old point requested */
#if DEBUGIT
	printf("ef_ipnt():  OLD POINT REQUESTED\r\n");
#endif
		if ((tmpval + basept) > endpnt)		/* check range */
			return(FAILURE);

		subj = tmpval;			/* make it current */
		pntsel();
		showpt(1);
#if DEBUGIT
	printf("ef_ipnt():  OLD POINT SELECTED  curpnt=%d, subj=%d\r\n",
		curpnt, subj);
#endif
		return(SUCCESS);
	}
}

/* 
*/

/*
   =============================================================================
	rd_ipnt() -- (re)display the point number
   =============================================================================
*/

short
rd_ipnt(n)
short n;
{
	sprintf(dspbuf, "%02d", subj);	/* convert to ASCII */

	vbank(0);			/* display the value */

	vcputsv(instob, 64, idbox[n][4], idbox[n][5],
		idbox[n][6] + 1, idbox[n][7], dspbuf, 14);

	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_ipnt() -- handle new data entry
   =============================================================================
*/

short
nd_ipnt(n, k)
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
		idbox[n][6] + 1, stccol, dspbuf, 14);

	advicur();

	return(SUCCESS);
}
