/*
   =============================================================================
	etival.c -- MIDAS-VI -- instrument editor - value field handlers
	Version 16 -- 1988-09-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"
#include "charset.h"
#include "smdefs.h"

#include "midas.h"
#include "instdsp.h"

#if	DEBUGIT
extern short debugsw;
#endif

extern short advicur (), edfunc (), dec2fr ();

extern char *dsimlt (), *fr2dec ();

extern unsigned *instob;

extern short stccol, curfunc;

extern short idbox[][8];

extern char dspbuf[];

extern struct instpnt *pntptr;

extern short vtcrow, vtccol, idsrcsw, submenu;

/* 
*/

/*
   =============================================================================
	et_ival() -- load the edit buffer
   =============================================================================
*/

short
et_ival (n)
     short n;
{
  register short vv, vh, vl;

  vv = pntptr->ipval >> 5;
  vh = vv / 100;
  vl = vv - (vh * 100);

  sprintf (ebuf, "%02d.%02d", vh, vl);
  ebuf[5] = '0' + pntptr->ipvsrc;
  fr2dec (pntptr->ipvmlt, &ebuf[6]);
  ebuf[10] = '\0';
  ebflag = TRUE;

#if DEBUGIT
  if (debugsw)
    printf ("et_ival():  ebuf=[%s]\n", ebuf);
#endif

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_ival() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_ival (n)
     short n;
{
  register short i, tmpval, srctmp;

  ebuf[2] = '.';
  ebuf[10] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;

#if DEBUGIT
  if (debugsw)
    printf ("ef_ival():  ebuf=[%s]\n", ebuf);
#endif

/* 
*/
  if (idsrcsw)
    {				/* entering the source */

      idsrcsw = FALSE;
      submenu = FALSE;

      if (vtcrow EQ 22)
	{

	  /*  'PchW/HT", 'Pch/Frq', 'Random', 'Ctl V1' */

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

	  /* 'Brth/LP', 'Key Prs' */

	  if (vtccol LT 24)
	    srctmp = SM_LPBR;
	  else if ((vtccol GT 24) AND (vtccol LT 32))
	    srctmp = SM_KPRS;
	  else
	    srctmp = SM_NONE;
	}

      pntptr->ipvsrc = srctmp;	/* set the source */
      objclr (TTCPRI);		/* turn off the menu cursor */
      idvlblc ();		/* blank the menu area */
      dswin (22);		/* refresh the screen */
      modinst ();
/* 
*/
    }
  else if (stccol LT 32)
    {				/* entering value */

      tmpval = 0;

      for (i = 0; i < 2; i++)	/* convert from ASCII to binary */
	tmpval = (tmpval * 10) + (ebuf[i] - '0');

      for (i = 3; i < 5; i++)	/* convert from ASCII to binary */
	tmpval = (tmpval * 10) + (ebuf[i] - '0');

      if (tmpval GT 1000)
	{

#if	DEBUGIT
	  if (debugsw)
	    printf ("ef_ival():  FAILURE - val - ebuf=[%s]\n", ebuf);
#endif

	  return (FAILURE);
	}

      pntptr->ipval = tmpval << 5;
      edfunc (curfunc);
      modinst ();

    }
  else if ((stccol GE 33) AND (stccol LE 39))
    {				/* selecting the source */

      idsrcsw = TRUE;		/* set the select switch */
      submenu = TRUE;
      idvlbld ();		/* load the menu area */
      dswin (22);		/* refresh the screen */
      SetPri (TTCURS, TTCPRI);	/* turn on the typewriter cursor */
      ttcpos (22, 17);		/* position the typewriter cusor */

    }
  else if ((stccol GE 41) AND (stccol LE 44))
    {				/* entering the multiplier */

      tmpval = dec2fr (&ebuf[6]);

      if (tmpval EQ 0xFFFF)
	{

#if	DEBUGIT
	  if (debugsw)
	    printf ("ef_ival():  FAILURE - mlt - ebuf=[%s]\n", ebuf);
#endif

	  return (FAILURE);

	}
      else
	{

	  pntptr->ipvmlt = tmpval;
	  modinst ();
	}

    }
  else
    return (FAILURE);

#if DEBUGIT
  if (debugsw)
    printf ("ef_ival():  SUCCESS\n");
#endif

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_ival() -- (re)display the field
   =============================================================================
*/

short
rd_ival (n)
     short n;
{
  register short vv, vh, vl;

  vv = pntptr->ipval >> 5;
  vh = vv / 100;
  vl = vv - (vh * 100);

  sprintf (dspbuf, "%02d.%02d ", vh, vl);
  dsimlt (&dspbuf[6], pntptr->ipvsrc, pntptr->ipvmlt);

  vbank (0);			/* display the value */

  vcputsv (instob, 64, idbox[n][4], idbox[n][5],
	   idbox[n][6] + 1, idbox[n][7], dspbuf, 14);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_ival() -- handle new data entry
   =============================================================================
*/

short
nd_ival (n, k)
     short n;
     register short k;
{
  if (idsrcsw)			/* not in source menu */
    return (FAILURE);

  if ((stccol GE 32) AND (stccol LE 40))	/* not in source */
    return (FAILURE);

  if (stccol EQ 29)		/* not in decimal point */
    return (FAILURE);

  ebuf[2] = '.';		/* setup the fixed stuff */
  ebuf[10] = '\0';

/* 
*/
  if (stccol LT 32)
    {				/* value */

      ebuf[stccol - 27] = k + '0';
      dspbuf[0] = k + '0';

    }
  else if (stccol EQ 41)
    {				/* mutiplier sign */

      if (k EQ 8)
	{			/* - */

	  ebuf[9] = '-';
	  dspbuf[0] = '-';

	}
      else if (k EQ 9)
	{			/* + */

	  ebuf[9] = '+';
	  dspbuf[0] = '+';

	}
      else
	return (FAILURE);

    }
  else if (stccol EQ 42)
    {				/* 1st digit */

      if (k EQ 0)
	{			/* 0 */

	  ebuf[6] = '0';
	  dspbuf[0] = '.';

	}
      else if (k EQ 1)
	{			/* 1 */

	  ebuf[6] = '1';
	  dspbuf[0] = SP_1P;

	}
      else
	return (FAILURE);

    }
  else
    {				/* 2nd or 3rd digit */

      ebuf[stccol - 36] = k + '0';
      dspbuf[0] = k + '0';
    }
/* 
*/
  dspbuf[1] = '\0';		/* terminate display buffer */

  vbank (0);

  vcputsv (instob, 64, ID_ENTRY, idbox[n][5],
	   idbox[n][6] + 1, stccol, dspbuf, 14);

  if ((stccol EQ 31) OR (stccol EQ 44))	/* last column of field ? */
    return (SUCCESS);

  advicur ();

  if (stccol EQ 29)		/* decimal point ? */
    advicur ();

  return (SUCCESS);
}
