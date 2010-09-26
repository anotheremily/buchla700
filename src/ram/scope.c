/*
   =============================================================================
	scope.c -- MIDAS-VII diagnostic scope functions
	Version 6 -- 1988-10-12 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "graphdef.h"
#include "fields.h"
#include "curpak.h"

#include "midas.h"

#define	SCOPEOLD	0x0014		/* scope old text attribute */
#define	SCOPENEW	0x0054		/* scope new text attribute */

short		scopec;			/* scope col position */
short		scopef;			/* scope control flag */
short		scoper;			/* scope row position */

unsigned	scopev;			/* last scope value */

unsigned	*scopeob;		/* scope display object */

char		scopebf[65];		/* scope display buffer */

extern	short	nokey(), stdmkey();

extern	short	astat;
extern	short	ndisp;

extern	short	scorpal[][3];

extern	short	crate1[];

/* forward reference */

short	scptogl();

/* 
*/

struct curpak dt_flds = {

	nokey,			/* curtype */
	nokey,			/* premove */
	nokey,			/* pstmove */
	nokey,			/* cx_key */
	nokey,			/* cy_key */
	nokey,			/* cx_upd */
	nokey,			/* cy_upd */
	nokey,			/* xy_up */
	nokey,			/* xy_dn */
	nokey,			/* x_key */
	scptogl,		/* e_key */
	stdmkey,		/* m_key */
	nokey,			/* d_key */
	nokey,			/* not_fld */
	(struct fet *)NULL,	/* curfet */
	(struct selbox *)NULL,	/* csbp */
	crate1,			/* cratex */
	crate1,			/* cratey */
	CT_GRAF,		/* cmtype */
	0,			/* cxval */
	0			/* cyval */
};

/* 
*/

/*
   =============================================================================
	scope() -- display a value in binary on the crt
   =============================================================================
*/

scope(val)
register unsigned val;
{
	register short i;
	register char *bp = scopebf;

	if ((ndisp NE 11) OR (NOT scopef))
		return;

	vputs(scopeob, scoper++, scopec, scopebf, SCOPEOLD);

	*bp++ = ' ';

	if (scoper > 24) {

		scoper = 0;
		scopec += 12;

		if (scopec GE 60)
			scopec = 0;
	}

	scopev = val;

	for (i = 0; i < 4; i++)
		if (val & (0x0080 >> i))
			*bp++ = '1';
		else
			*bp++ = '0';

	*bp++ = ' ';

	for (i = 4; i < 8; i++)
		if (val & (0x0080 >> i))
			*bp++ = '1';
		else
			*bp++ = '0';

	*bp++ = ' ';
	*bp = 0;

	vputs(scopeob, scoper, scopec, scopebf, SCOPENEW);
}

/* 
*/

/*
   =============================================================================
	scptogl() -- toggle "scope" status
   =============================================================================
*/

scptogl()
{
	if (NOT astat)
		return;

	scopef = NOT scopef;
}

/* 
*/

/*
   =============================================================================
	scopeon() -- put up the "scope" display
   =============================================================================
*/

scopeon()
{
	scopeob = &v_score[0];

	curset(&dt_flds);

	scopec  = 0;
	scopef  = TRUE;
	scoper  = 0;
	scopev  = 0;

	strcpy(scopebf, " ???? ???? ");

	dswap();

	vbank(0);

	memsetw(scopeob, 0, 32767);
	memsetw(scopeob+32767L, 0, 12033);

	SetObj(0, 1, 0, scopeob, 512, 350, 0, 0, CHR3, -1);
	vsndpal(scorpal);

	SetPri(0, 15);
}
