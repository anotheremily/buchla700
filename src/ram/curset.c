/*
   =============================================================================
	curset.c -- cursor control and data entry parameter setup
	Version 14 -- 1988-12-13 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "graphdef.h"
#include "curpak.h"

#include "midas.h"

#if	DEBUGIT
extern	short	debugsw;

short	debugcs = 1;

char	*C_TYPE[] = {

	"CT_GRAF",	/*	0	graphics */
	"CT_TEXT",	/*	1	text -- general */
	"CT_VIRT",	/*	2	virtual -- graphic */
	"CT_SCOR",	/*	3	text -- score */
	"CT_SMTH",	/*	4	text -- smooth scroll */
	"CT_MENU"	/*	5	virtual -- character */
};
#endif

short	trkonly = FALSE;		/* set TRUE to force use of trackball */

extern	short		txstd(), tystd(), cmvgen();

extern	short		(*curmove)();
extern	short		(*curtype)();
extern	short		(*cx_key)();
extern	short		(*cx_upd)();
extern	short		(*cy_key)();
extern	short		(*cy_upd)();
extern	short		(*d_key)();
extern	short		(*e_key)();
extern	short		(*m_key)();
extern	short		(*not_fld)();
extern	short		(*premove)();
extern	short		(*pstmove)();
extern	short		(*x_key)();
extern	short		(*xy_dn)();
extern	short		(*xy_up)();

extern	short		*cratex;
extern	short		*cratey;

/* 
*/

extern	short		chtime;
extern	short		chwait;
extern	short		cmfirst;
extern	short		cmtype;
extern	short		curhold;
extern	short		curslim;
extern	short		cvtime;
extern	short		cvwait;
extern	short		cxval;
extern	short		cyval;
extern	short		hcwval;
extern	short		nchwait;
extern	short		ncvwait;
extern	short		stccol;
extern	short		stcrow;
extern	short		submenu;
extern	short		syrate;
extern	short		thcwval;
extern	short		tvcwval;
extern	short		vcwval;

extern	struct fet	*infetp;
extern	struct fet	*cfetp;
extern	struct fet	*curfet;

extern	struct selbox	*csbp;

/* 
*/

#if	DEBUGIT

/*
   =============================================================================
	SnapCT() -- snap dump curpak variables
   =============================================================================
*/

SnapCT()
{
	printf("\ncurpak variables:\n");
	printf("   curtype = $%lX\n", curtype);
	printf("   premove = $%lX\n", premove);
	printf("   pstmove = $%lX\n", pstmove);
	printf("   cx_key  = $%lX\n", cx_key);
	printf("   cy_key  = $%lX\n", cy_key);
	printf("   cx_upd  = $%lX\n", cx_upd);
	printf("   cy_upd  = $%lX\n", cy_upd);
	printf("   xy_up   = $%lX\n", xy_up);
	printf("   xy_dn   = $%lX\n", xy_dn);
	printf("   x_key   = $%lX\n", x_key);
	printf("   e_key   = $%lX\n", e_key);
	printf("   m_key   = $%lX\n", m_key);
	printf("   d_key   = $%lX\n", d_key);
	printf("   not_fld = $%lX\n", not_fld);
	printf("   curfet  = $%lX\n", curfet);
	printf("   csbp    = $%lX\n", csbp);
	printf("   cratex  = $%lX\n", cratex);
	printf("   cratey  = $%lX\n", cratey);
	printf("   cmtype  = %d\n", cmtype);
	printf("   cxval   = %d\n", cxval);
	printf("   cyval   = %d\n", cyval);
	printf("\n");
}

#endif

/* 
*/

/*
   =============================================================================
	stdctp0() -- cursor type - text, virtual
   =============================================================================
*/

short
stdctp0()
{
	return(submenu ? CT_VIRT : CT_TEXT);
}

/*
   =============================================================================
	stdctp1() -- cursor type -- graphic, text, virtual -- text if > curslim
   =============================================================================
*/

short
stdctp1()
{
	if (submenu)
		return(CT_VIRT);

	return((cyval > curslim) ? CT_TEXT : CT_GRAF);
}

/*
   =============================================================================
	stdctp2() -- cursor type -- graphic
   =============================================================================
*/

short
stdctp2()
{
	return(CT_GRAF);
}

/* 
*/

/*
   =============================================================================
	stdctp3() -- cursor type -- graphic, score text, virtual
   =============================================================================
*/

short
stdctp3()
{
	if (submenu)
		return(CT_VIRT);

	return(((cyval < 14) OR (cyval > 237)) ? CT_SCOR : CT_GRAF);
}

/*
   =============================================================================
	stdctp4() -- cursor type -- graphic, text, virtual -- text if < curslim
   =============================================================================
*/

short
stdctp4()
{
	if (submenu)
		return(CT_VIRT);

	return((cyval < curslim) ? CT_TEXT : CT_GRAF);
}

/*
   =============================================================================
	stdctp5() -- cursor type - text, virtual - character objects
   =============================================================================
*/

short
stdctp5()
{
	return(submenu ? CT_MENU : CT_SMTH);
}

/* 
*/

/*
   =============================================================================
	curset() -- setup the cursor control and data entry parameters
   =============================================================================
*/

curset(s)
register struct curpak *s;
{
#if	DEBUGIT
	if (debugsw AND debugcs)
		printf("curset($%lX): ENTRY old cmtype=%d\n", s, cmtype);
#endif

	curtype = s->curtype;
	premove = s->premove;
	pstmove = s->pstmove;

	curmove = cmvgen;

	if (trkonly) {

		cx_key  = txstd;
		cy_key  = tystd;

	} else {

		cx_key  = s->cx_key;
		cy_key  = s->cy_key;
	}

	cx_upd  = s->cx_upd;
	cy_upd  = s->cy_upd;
	xy_up   = s->xy_up;
	xy_dn   = s->xy_dn;
	x_key   = s->x_key;
	e_key   = s->e_key;
	m_key   = s->m_key;
	d_key   = s->d_key;
	not_fld = s->not_fld;
	curfet  = s->curfet;
	csbp    = s->csbp;
	cratex  = s->cratex;
	cratey  = s->cratey;
	cmtype  = s->cmtype;
	cxval   = s->cxval;
	cyval   = s->cyval;

#if	DEBUGIT
	if (debugsw AND debugcs)
		printf("curset($%lX): new cmtype=%d\n", s, cmtype);
#endif

/* 
*/
	if (cmtype EQ CT_GRAF) {	/* graphics */

		chtime  = hcwval;
		cvtime  = vcwval;

	} else {			/* text of some sort */

		chtime  = thcwval;
		cvtime  = (cmtype EQ CT_SMTH) ? syrate : tvcwval;
	}

	stccol  = XTOC(cxval);
	stcrow  = YTOR(cyval);

	chwait  = chtime;
	cvwait  = cvtime;

	nchwait = curhold;
	ncvwait = curhold;

	cmfirst = TRUE;

	ebflag = FALSE;
	memset(ebuf, '\0', sizeof ebuf);

	cfetp  = (struct fet *)NULL;
	infetp = (struct fet *)NULL;

#if	DEBUGIT
	if (debugsw AND debugcs) {

		printf("curset($%lX): EXIT cmtype=%d=%s chtime=%d  cvtime=%d  curhold=%d\n",
			s, cmtype,
			(cmtype < 6) ? C_TYPE[cmtype] : "UNKNOWN",
			chtime, cvtime, curhold);

		SnapCT();
	}
#endif

}
