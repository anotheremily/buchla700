/*
   =============================================================================
	etitim.c -- instrument editor - time field handlers
	Version 14 -- 1987-12-09 -- D.N. Lynx Crowe
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

extern short advicur (), edfunc (), pntsel ();

extern unsigned *instob;

extern short stccol, subj, curvce, curfunc, curpnt;
extern short pecase, npts, temin, temax, pntsv;

extern short idbox[][8];

extern char dspbuf[];

extern struct instdef vbufs[];

extern struct instpnt *pntptr;

/* 
*/

/*
   =============================================================================
	et_itim() -- load the edit buffer
   =============================================================================
*/

short
et_itim (n)
     short n;
{
  register short th, tl;
  register long tt, sc;

  pntsel ();			/* make sure edit limits are current */

  sc = 1000L;
  tt = timeto (curfunc, subj);
  th = tt / sc;
  tl = tt - (th * sc);

  sprintf (ebuf, "%02d.%03d", th, tl);
  ebflag = TRUE;
#if DEBUGIT
  printf ("et_itim(): voice=%d, func=%d, pnt=%d, ebuf=[%s]\r\n",
	  curvce, curfunc, curpnt, ebuf);
  printf ("et_itim(): npts=%d, subj=%d, case=%d, min=%d, max=%d\r\n",
	  npts, subj, pecase, temin, temax);
#endif
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_itim() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_itim (n)
     short n;
{
  register short i, endpnt, basept;
  register unsigned tmpval;
  register struct idfnhdr *fp;
  register struct instdef *ip;

  ip = &vbufs[curvce];		/* set instrument pointer */
  fp = &ip->idhfnc[curfunc];	/* set function pointer */

  ebuf[2] = '.';		/* add implied decimal point */
  ebuf[6] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;
#if DEBUGIT
  printf ("ef_itim():  voice=%d, func=%d, ebuf=[%s]\r\n",
	  curvce, curfunc, ebuf);
#endif
  tmpval = 0;

  for (i = 0; i < 2; i++)	/* convert from ASCII to binary */
    tmpval = (tmpval * 10) + (ebuf[i] - '0');

  for (i = 3; i < 6; i++)
    tmpval = (tmpval * 10) + (ebuf[i] - '0');
#if DEBUGIT
  printf ("ef_itim(): subj=%d, case=%d, min=%d, val=%d, max=%d\r\n",
	  subj, pecase, temin, tmpval, temax);
#endif
  if (tmpval > (unsigned) temax)
    return (FAILURE);

  if (tmpval < (unsigned) temin)
    return (FAILURE);

  setseg (subj, tmpval);

  if (pecase == 2)
    setseg (subj + 1, temax);
#if DEBUGIT
  printf ("ef_itim():  SUCCESS   time %d set at point %d <%d> = %d\r\n",
	  tmpval, subj, curpnt, timeto (curfunc, subj));
#endif
  modinst ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_itim() -- (re)display the field
   =============================================================================
*/

short
rd_itim (n)
     short n;
{
  register short th, tl;
  register long tt, sc;

  sc = 1000L;
  tt = timeto (curfunc, subj);
  th = tt / sc;
  tl = tt - (th * sc);

  sprintf (dspbuf, "%02d.%03d", th, tl);	/* convert to ASCII */

  vbank (0);			/* display the value */

  vcputsv (instob, 64, idbox[n][4], idbox[n][5],
	   idbox[n][6] + 1, idbox[n][7], dspbuf, 14);

  edfunc (curfunc);
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_itim() -- handle new data entry
   =============================================================================
*/

short
nd_itim (n, k)
     short n;
     register short k;
{
  register short ec;

  ec = stccol - cfetp->flcol;	/* setup edit buffer column */

  if (ec == 2)
    return (FAILURE);

  ebuf[ec] = k + '0';
  ebuf[2] = '.';
  ebuf[6] = '\0';

  dspbuf[0] = k + '0';
  dspbuf[1] = '\0';

  vbank (0);

  vcputsv (instob, 64, ID_ENTRY, idbox[n][5],
	   idbox[n][6] + 1, stccol, dspbuf, 14);

  advicur ();

  if (stccol == (idbox[n][7] + 2))
    advicur ();

  return (SUCCESS);
}
