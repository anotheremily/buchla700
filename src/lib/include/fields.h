/*
   =============================================================================
	fields.h -- field definition header
	Version 6 -- 1987-09-25 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	MAXEBUF	80		/* maximum length of data in the edit buffer */

struct fet
{				/* field definition structure */

  short frow;			/* row the field is in */
  short flcol;			/* leftmost column of field */
  short frcol;			/* rightmost column of field */
  short ftags;			/* field tags / parameters (used variously) */
  short (*ebto) ();		/* edit buffer 'to' (setup) function */
  short (*ebfrom) ();		/* edit buffer 'from' (parse) function */
  short (*redisp) ();		/* field (re)display function */
  short (*datain) ();		/* data entry function */
};

struct selbox
{				/* selection box structure */

  short sbxmin;			/* minimum x - left edge of box */
  short sbymin;			/* minimum y - top edge of box */
  short sbxmax;			/* maximum x - right edge of box */
  short sbymax;			/* maximum y - bottom edge of box */
  short sbarg;			/* select box argument */
  short (*boxhit) ();		/* box-hit function */
};

#ifndef	FET_DEFS
extern short ebflag;		/* edit buffer setup flag */
extern char ebuf[MAXEBUF + 1];	/* edit buffer */

extern struct fet *curfet;	/* current fet table pointer */
extern struct fet *cfetp, *infetp;	/* current and new fet entry pointers */
#endif
