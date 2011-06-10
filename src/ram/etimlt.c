/*
   =============================================================================
	etimlt.c -- instrument editor - global source / multiplier field
	Version 14 -- 1988-09-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "graphdef.h"
#include "charset.h"
#include "smdefs.h"

#include "midas.h"
#include "instdsp.h"

#if	DEBUGIT
extern short debugsw;
#endif

extern short advicur (), dec2fr (), dswin (), idvlbld (), ttcpos ();

extern char *fr2dec ();

extern unsigned *instob;

extern short stcrow, stccol, curvce, curfunc, idsrcsw;
extern short vtcrow, vtccol, submenu;

extern short idbox[][8];

extern char dspbuf[];

extern char *srctbl[];

extern struct instpnt *pntptr;

extern struct instdef vbufs[];

/* 
*/

/*
   =============================================================================
	dsimlt() -- decode a source / multiplier pair for display
   =============================================================================
*/

char *
dsimlt (buf, src, mltval)
     char *buf;
     short src;
     register short mltval;
{
  char mltstr[5], mlttmp[5];

  fr2dec (mltval, mlttmp);	/* convert to ASCII from binary */

  mltstr[0] = mlttmp[3];	/* sign */

  if (mlttmp[0] EQ '0')		/* 1st digit & decimal point */
    mltstr[1] = '.';
  else
    mltstr[1] = SP_1P;

  mltstr[2] = mlttmp[1];	/* 2nd digit */
  mltstr[3] = mlttmp[2];	/* 3rd digit */
  mltstr[4] = '\0';		/* terminate the string */

  sprintf (buf, "%7.7s %s", srctbl[src], mltstr);

#if	DEBUGIT
  if (debugsw)
    printf ("dsimlt($%08.8lX, %d, $%04.4X):  [%s]\n", buf, src, mltval, buf);
#endif

  return (buf);
}

/* 
*/

/*
   =============================================================================
	et_imlt() -- load the edit buffer
   =============================================================================
*/

short
et_imlt (n)
     short n;
{
  register struct instdef *ip;
  register struct idfnhdr *fp;

  ip = &vbufs[curvce];
  fp = &ip->idhfnc[curfunc];

  ebuf[0] = '0' + fp->idfsrc;
  fr2dec (fp->idfmlt, &ebuf[1]);
  ebuf[5] = '\0';

  ebflag = TRUE;

#if DEBUGIT
  if (debugsw)
    printf ("et_imlt():  ebuf=[%s]\n", ebuf);
#endif

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_imlt() -- parse (unload) the edit buffer  (called by 'E' key)
   =============================================================================
*/

short
ef_imlt (n)
     short n;
{
  register short i, tmpval, srctmp;
  register struct instdef *ip;
  register struct idfnhdr *fp;

  ip = &vbufs[curvce];
  fp = &ip->idhfnc[curfunc];

  ebuf[5] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;

#if DEBUGIT
  if (debugsw)
    printf ("ef_imlt():  ebuf=[%s]\n", ebuf);
#endif

/* 
*/
  if (idsrcsw)
    {				/* entering the source */

      idsrcsw = FALSE;
      submenu = FALSE;

      if (vtcrow EQ 22)
	{

	  /*  'PchW/HT', 'Pitch', 'Random', 'GPC/CV1' */

	  if (vtccol LT 24)
	    srctmp = SM_HTPW;
	  else if ((vtccol GT 24) AND (vtccol LT 28))
	    srctmp = SM_PTCH;
	  else if ((vtccol GT 28) AND (vtccol LT 32))
	    srctmp = SM_FREQ;
	  else if ((vtccol GT 32) AND (vtccol LT 40))
	    srctmp = SM_RAND;
	  else
	    srctmp = SM_CTL1;

	}
      else if (vtcrow EQ 23)
	{

	  /* 'ModW/VT', 'Key Vel', 'Pedal 1' */

	  if (vtccol LT 24)
	    srctmp = SM_VTMW;
	  else if ((vtccol GT 24) AND (vtccol LT 32))
	    srctmp = SM_KVEL;
	  else if ((vtccol GT 32) AND (vtccol LT 40))
	    srctmp = SM_PED1;
	  else
	    srctmp = SM_NONE;

	}
      else
	{			/* must be row 24 */

	  /* 'PchW/LP', 'Key Prs' */

	  if (vtccol LT 24)
	    srctmp = SM_LPBR;
	  else if ((vtccol GT 24) AND (vtccol LT 32))
	    srctmp = SM_KPRS;
	  else
	    srctmp = SM_NONE;
	}

      fp->idfsrc = srctmp;	/* set the source */
      objclr (TTCPRI);		/* turn off the menu cursor */
      idvlblc ();		/* blank the menu area */
      modinst ();
      dswin (22);		/* refresh the screen */
/* 
*/
    }
  else if (stccol LT 8)
    {				/* selecting the source */

      idsrcsw = TRUE;		/* set the select switch */
      submenu = TRUE;
      idvlbld ();		/* load the menu area */
      dswin (22);		/* refresh the screen */
      SetPri (TTCURS, TTCPRI);	/* turn on the typewriter cursor */
      ttcpos (22, 17);		/* position the typewriter cusor */

    }
  else if (stccol GT 8)
    {				/* entering the multiplier */

      tmpval = dec2fr (&ebuf[1]);

      if (tmpval EQ 0xFFFF)
	return (FAILURE);
      else
	fp->idfmlt = tmpval;

      modinst ();

    }
  else
    return (FAILURE);

#if DEBUGIT
  if (debugsw)
    printf ("ef_imlt():  SUCCESS\n");
#endif

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_imlt() -- (re)display the field
   =============================================================================
*/

short
rd_imlt (n)
     short n;
{
  register struct instdef *ip;
  register struct idfnhdr *fp;

  ip = &vbufs[curvce];
  fp = &ip->idhfnc[curfunc];

  dsimlt (dspbuf, fp->idfsrc, fp->idfmlt);

  vcputsv (instob, 64, idbox[n][4], idbox[n][5],
	   idbox[n][6] + 1, idbox[n][7], dspbuf, 14);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_imlt() -- handle new data entry
   =============================================================================
*/

short
nd_imlt (n, k)
     short n;
     register short k;
{
  register short ec;

  if (idsrcsw)			/* not if the source menu is up */
    return (FAILURE);

  if (stccol LT 9)		/* only in the multiplier field */
    return (FAILURE);

  ec = stccol - 9;		/* calculate edit buffer column */

  switch (ec)
    {

    case 0:			/* sign position */

      if (k EQ 8)
	{			/* - */

	  ebuf[4] = '-';
	  dspbuf[0] = '-';	/* setup display buffer */

	}
      else if (k EQ 9)
	{			/* + */

	  ebuf[4] = '+';
	  dspbuf[0] = '+';	/* setup display buffer */

	}
      else
	return (FAILURE);

      break;

    case 1:			/* 1st digit position  (0 or 1) */

      if (k EQ 0)
	{			/* '0' -> '.' */

	  ebuf[1] = '0';
	  dspbuf[0] = '.';	/* setup display buffer */

	}
      else if (k EQ 1)
	{			/* '1' -> '1.' */

	  ebuf[1] = '1';
	  dspbuf[0] = SP_1P;	/* setup display buffer */

	}
      else
	return (FAILURE);

      break;

    case 2:			/* 2nd digit position */
    case 3:			/* 3rd digit position */

      ebuf[ec] = k + '0';
      dspbuf[0] = k + '0';	/* setup display buffer */
      break;

    default:
      return (FAILURE);
    }

  ebuf[5] = '\0';
  dspbuf[1] = '\0';

#if	DEBUGIT
  if (debugsw)
    printf ("nd_imlt(): OK - k=%d, ec=%d, ebuf=[%s]\n", k, ec, ebuf);
#endif

  vbank (0);

  vcputsv (instob, 64, ID_ENTRY, idbox[n][5],
	   idbox[n][6] + 1, stccol, dspbuf, 14);

  if (ec EQ 4)
    return (SUCCESS);

  advicur ();

  return (SUCCESS);
}
