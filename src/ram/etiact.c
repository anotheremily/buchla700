/*
   =============================================================================
	etiact.c -- point action field handlers
	Version 16 -- 1988-08-31 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"
#include "charset.h"

#include "midas.h"
#include "instdsp.h"

/* functions defined elsewhere */

extern int advicur ();

extern char *strcpy ();

/* variables defined elsewhere */

extern short stcrow, stccol, curvce, curpnt, curfunc;
extern short debugsw;

extern short idbox[][8];

extern unsigned *instob;

extern char dspbuf[];

extern struct instdef vbufs[];

extern short aform;

/* 
*/

/*
   =============================================================================
	et_iact() -- load edit buffer
   =============================================================================
*/

short
et_iact (nn)
     short nn;
{
  register struct instpnt *pp;

  pp = &vbufs[curvce].idhpnt[curpnt];

  aform = pp->ipact;

  switch (aform)
    {

    case AC_NULL:
    case AC_SUST:
    case AC_ENBL:
    case AC_UNDF:

      ebuf[0] = '\0';
      break;

    case AC_JUMP:
    case AC_KYUP:
    case AC_KYDN:

      sprintf (ebuf, "%02d", pp->ippar1);
      break;

    case AC_LOOP:

      sprintf (ebuf, "%02d %02d", pp->ippar1, pp->ippar2);
      break;
    }

#if DEBUGIT
  if (debugsw)
    printf ("et_iact():  aform=%d, ebuf=[%s]\r\n", aform, ebuf);
#endif

  ebflag = TRUE;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_iact() -- unload (parse) edit buffer
   =============================================================================
*/

short
ef_iact (nn)
     short nn;
{
  register short i, tmp1, tmp2;
  register struct instpnt *pp;
  register struct idfnhdr *fp;

  fp = &vbufs[curvce].idhfnc[curfunc];
  pp = &vbufs[curvce].idhpnt[curpnt];
  ebflag = FALSE;

#if DEBUGIT
  if (debugsw)
    printf ("ef_iact():  aform=%d, fp=$%08lX, pp=$%08lX\r\n", aform, fp, pp);
#endif

  switch (aform)
    {

    case AC_NULL:
    case AC_UNDF:

      aform = AC_NULL;

    case AC_SUST:
    case AC_ENBL:

      pp->ipact = aform;

#if DEBUGIT
      if (debugsw)
	printf ("ef_iact():  aform=%d, ipact=%d, ippar1=%d, ippar2=%d\r\n",
		aform, pp->ipact, pp->ippar1, pp->ippar2);
#endif

      modinst ();
      return (SUCCESS);

    case AC_JUMP:
    case AC_KYUP:
    case AC_KYDN:

      tmp1 = 0;

      for (i = 0; i < 2; i++)
	tmp1 = (tmp1 * 10) + (ebuf[i] - '0');

      if (tmp1 >= fp->idfpif)
	return (FAILURE);

      pp->ipact = aform;
      pp->ippar1 = tmp1;
      pp->ippar2 = 0;
      pp->ippar3 = 0;

#if DEBUGIT
      if (debugsw)
	printf ("ef_iact():  aform=%d, ipact=%d, ippar1=%d, ippar2=%d\r\n",
		aform, pp->ipact, pp->ippar1, pp->ippar2);
#endif

      modinst ();
      return (SUCCESS);
/* 
*/
    case AC_LOOP:

      tmp1 = 0;

      for (i = 0; i < 2; i++)
	tmp1 = (tmp1 * 10) + (ebuf[i] - '0');

      if (tmp1 >= fp->idfpif)
	return (FAILURE);

      tmp2 = 0;

      for (i = 3; i < 5; i++)
	tmp2 = (tmp2 * 10) + (ebuf[i] - '0');

      pp->ipact = aform;
      pp->ippar1 = tmp1;
      pp->ippar2 = tmp2;
      pp->ippar3 = 0;

#if DEBUGIT
      if (debugsw)
	printf ("ef_iact():  aform=%d, ipact=%d, ippar1=%d, ippar2=%d\r\n",
		aform, pp->ipact, pp->ippar1, pp->ippar2);
#endif

      modinst ();
      return (SUCCESS);
    }
}

/* 
*/

/*
   =============================================================================
	rd_iact() -- (re)-display the field
   =============================================================================
*/

short
rd_iact (nn)
     short nn;
{
  register short pnt, par, n;
  register struct instpnt *pp;
  register char *s1;

  n = nn & 0x00FF;
  pp = &vbufs[curvce].idhpnt[curpnt];
  pnt = pp->ippar1;
  par = pp->ippar2;
  aform = pp->ipact;

#if DEBUGIT
  if (debugsw)
    printf ("rd_iact():  aform=%d, pp=$%08lX, pnt=%d, par=%d\r\n",
	    aform, pp, pnt, par);
#endif

  switch (aform)
    {

    case AC_NULL:

      s1 = "                 ";
      break;

    case AC_SUST:

      sprintf (dspbuf, "Pause if key %c  ", SP_DNA);
      s1 = dspbuf;
      break;

    case AC_ENBL:

      sprintf (dspbuf, "Stop if key %c   ", SP_UPA);
      s1 = dspbuf;
      break;

    case AC_JUMP:

      sprintf (dspbuf, "GoTo %02d forever ", pnt);
      s1 = dspbuf;
      break;

/* 
*/
    case AC_LOOP:

      sprintf (dspbuf, "GoTo %02d %02d times", pnt, par);

      if (dspbuf[8] == '9')
	dspbuf[8] = 'R';

      s1 = dspbuf;
      break;

    case AC_KYUP:

      sprintf (dspbuf, "GoTo %02d if key %c", pnt, SP_UPA);
      s1 = dspbuf;
      break;

    case AC_KYDN:

      sprintf (dspbuf, "GoTo %02d if key %c", pnt, SP_DNA);
      s1 = dspbuf;
      break;

    default:

      s1 = "????????????????";
      break;
    }

  vbank (0);
  vcputsv (instob, 64, idbox[n][4], idbox[n][5],
	   idbox[n][6] + 1, idbox[n][7], s1, 14);
}

/* 
*/

/*
   =============================================================================
	setactb() -- set action field data entry buffer format
   =============================================================================
*/

setactb (n)
     short n;
{
  register char *s1;

  switch (aform)
    {

    case AC_NULL:

      s1 = "                ";
      ebuf[0] = '\0';
      break;

    case AC_SUST:

      sprintf (dspbuf, "Pause if key %c  ", SP_DNA);
      s1 = dspbuf;
      ebuf[0] = '\0';
      break;

    case AC_ENBL:

      sprintf (dspbuf, "Stop if key %c   ", SP_UPA);
      s1 = dspbuf;
      ebuf[0] = '\0';
      break;

    case AC_JUMP:

      s1 = "GoTo 00 forever ";
      ebuf[0] = '0';
      ebuf[1] = '0';
      ebuf[2] = '\0';
      break;

/* 
*/
    case AC_LOOP:

      s1 = "GoTo 00 00 times";
      ebuf[0] = '0';
      ebuf[1] = '0';
      ebuf[2] = ' ';
      ebuf[3] = '0';
      ebuf[4] = '0';
      ebuf[5] = '\0';
      break;

    case AC_KYUP:

      sprintf (dspbuf, "GoTo 00 if key %c", SP_UPA);
      s1 = dspbuf;
      ebuf[0] = '0';
      ebuf[1] = '0';
      ebuf[2] = '\0';
      break;

    case AC_KYDN:

      sprintf (dspbuf, "GoTo 00 if key %c", SP_DNA);
      s1 = dspbuf;
      ebuf[0] = '0';
      ebuf[1] = '0';
      ebuf[2] = '\0';
      break;

    default:

      s1 = "????????????????";
      ebuf[0] = '\0';
      break;
    }

  vbank (0);
  vcputsv (instob, 64, ID_ENTRY, idbox[n][5],
	   idbox[n][6] + 1, idbox[n][7], s1, 14);
}

/* 
*/

/*
   =============================================================================
	nd_iact() -- process new data entry
   =============================================================================
*/

short
nd_iact (nn, k)
     short nn, k;
{
  register short n, ec;

  n = nn & 0xFF;

  if (stccol < (idbox[n][7] + 5))
    {

      if (k == 8)
	{

	  if (--aform < 0)
	    aform = AC_KYDN;

	  setactb (n);
	  return (SUCCESS);

	}
      else if (k == 9)
	{

	  if (++aform > AC_KYDN)
	    aform = AC_NULL;

	  setactb (n);
	  return (SUCCESS);
	}

      return (FAILURE);

/* 
*/
    }
  else
    {

      switch (aform)
	{

	case AC_NULL:
	case AC_UNDF:
	case AC_ENBL:
	case AC_SUST:

	  return (FAILURE);

	case AC_KYUP:
	case AC_KYDN:
	case AC_JUMP:

	  if ((stccol == (idbox[n][7] + 5)) || (stccol == (idbox[n][7] + 6)))
	    {

	      ebuf[stccol - (idbox[n][7] + 5)] = k + '0';
	      dspbuf[0] = k + '0';
	      dspbuf[1] = '\0';

	      vbank (0);
	      vcputsv (instob, 64, ID_ENTRY, idbox[n][5],
		       stcrow, stccol, dspbuf, 14);

	      advicur ();
	      return (SUCCESS);

	    }
	  else
	    {

	      return (FAILURE);
	    }
/* 
*/
	case AC_LOOP:

	  if ((stccol >= (idbox[n][7] + 5)) && (stccol <= (idbox[n][7] + 9)))
	    {

	      ec = stccol - (idbox[n][7] + 5);

	      if (ec == 2)
		return (FAILURE);

	      ebuf[ec] = k + '0';
	      dspbuf[0] = k + '0';
	      dspbuf[1] = '\0';

	      if ((ec == 3) && (k == 9))
		dspbuf[0] = 'R';

	      vbank (0);
	      vcputsv (instob, 64, ID_ENTRY, idbox[n][5],
		       stcrow, stccol, dspbuf, 14);

	      advicur ();

	      if (ec == 1)
		advicur ();

	      return (SUCCESS);

	    }
	  else
	    {

	      return (FAILURE);
	    }
	}
    }
}
