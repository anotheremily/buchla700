/*
   =============================================================================
	select.c -- field selection processing
	Version 9 -- 1988-12-13 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "fields.h"

#if	DEBUGIT
extern	short	debugsw;

short	debugsf = 1;;
#endif

extern	short	cxval, cyval, astat;

extern	short	cursbox;		/* currently selected box */
extern	short	hitbox;			/* box we just hit */
extern	short	hitcx, hitcy;		/* x,y of cursor when we hit the box */

extern	struct	selbox	*csbp;		/* current select box table pointer */
extern	struct	selbox	*curboxp;	/* current select box pointer */

/* 
*/

/*
   =============================================================================
	whatbox() -- determine which selection box a hit occured in
   =============================================================================
*/

short
whatbox()
{
	register struct selbox *sb;

	sb = csbp;
	hitbox = -1;

#if	DEBUGIT
	if (debugsw AND debugsf)
		printf("whatbox():  ENTRY  cxval=%d  cyval=%d  cursbox=%d  csbp=$%lX\n",
			cxval, cyval, cursbox, csbp);
#endif

	if (0L EQ sb)
		return(FALSE);

	while (sb->boxhit) {

		hitbox++;

		if ((cxval GE sb->sbxmin) AND
		    (cxval LE sb->sbxmax) AND
		    (cyval GE sb->sbymin) AND
		    (cyval LE sb->sbymax)) {

			hitcx = cxval;
			hitcy = cyval;
			curboxp = sb;

#if	DEBUGIT
	if (debugsw AND debugsf)
		printf("whatbox():  HIT  hitbox=%d, curboxp=$%lX, sbarg=$%04.4X\n",
			hitbox, curboxp, sb->sbarg);
#endif

			return(TRUE);
		}

		sb++;
	}

#if	DEBUGIT
	if (debugsw AND debugsf)
		printf("whatbox():  FAILED\n");
#endif

	hitbox = -1;
	return(FALSE);
}

/* 
*/

/*
   =============================================================================
	select() -- standard item selection processing
   =============================================================================
*/

select()
{
	if (astat) {	/* only when the E key goes down */

#if	DEBUGIT
	if (debugsw AND debugsf)
		printf("select():  ENTRY\n");
#endif

		if (whatbox()) {	/* see if we're in a box */

#if	DEBUGIT
	if (debugsw AND debugsf)
		printf("select():  HIT  hitbox = %d  curboxp $%lX\n",
			hitbox, curboxp);
#endif

			(*curboxp->boxhit)(curboxp->sbarg);	/* process it */
			cursbox = hitbox;

		} else {

#if	DEBUGIT
	if (debugsw AND debugsf)
		printf("select():  FAILED\n");
#endif

		}
	}
}
