/*
   =============================================================================
	setgc.c -- set cursors and keep them in sync
	Version 2 -- 1988-08-19 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "curpak.h"
#include "fields.h"

extern short infield ();

extern short cmtype;
extern short cxval;
extern short cyval;
extern short sgcsw;
extern short stccol;
extern short stcrow;

/*
   =============================================================================
	setgc() -- set graphics cursor
   =============================================================================
*/

setgc (xv, yv)
     register short xv, yv;
{
  gcurpos (xv, yv);

  cxval = xv;
  cyval = yv;

  stcrow = yv / 14;
  stccol = xv >> 3;

  sgcsw = TRUE;
  cmtype = CT_GRAF;

  if (infield (stcrow, stccol, curfet))
    cfetp = infetp;
  else
    cfetp = (struct fet *) NULL;
}

/* 
*/

/*
   =============================================================================
	settc() -- set text cursor
   =============================================================================
*/

settc (rv, cv)
     register short rv, cv;
{
  itcpos (rv, cv);

  stcrow = rv;
  stccol = cv;

  cyval = rv * 14;
  cxval = cv << 3;

  sgcsw = FALSE;
  cmtype = CT_TEXT;

  if (infield (stcrow, stccol, curfet))
    cfetp = infetp;
  else
    cfetp = (struct fet *) NULL;
}
