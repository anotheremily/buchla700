/*
   =============================================================================
	ptdkey.c -- MIDAS-VII patch display data entry functions
	Version 29 -- 1988-12-08 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGPD		0
#define	DEBUGVP		0
#define	DUMPDEF		0
#define	DUMPSTM		0

#include "stddefs.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "fields.h"
#include "curpak.h"
#include "patch.h"
#include "macros.h"

#include "midas.h"
#include "ptdisp.h"

#if	DEBUGPD
short debugpd = 1;
short snappb = 0;
#endif

#if	DEBUGVP
short debugvp = 1;
#endif

#if	DUMPDEF
short dumpdef = 0;
#endif

#if	DUMPSTM
short dumpstm = 0;
#endif

#if	(DEBUGPD|DEBUGVP|DUMPDEF|DUMPSTM)
extern short debugsw;
#endif

extern short asig, astat;

extern short dpepred, dpecpos, dpesucc;

extern short ptegood, ptedfok, ptestok, ptedsok, ptedtok;

extern short ptedef, ptestm, ptespec, ptesuba, ptedat1, ptedat2;

extern short ptedftp, ptedfst, ptedest, ptedata;

extern short pteset, ptbflag, ptemenu;

extern short ptepred, ptecpos, ptesucc;

extern short stccol, stcrow;

extern char ptdebuf[50];

extern struct patch ptebuf;

/* 
*/

short dsdecol[5][2][2] = {	/* destination data entry column table */

  6, 8, 19, 21,			/* 0 -- key, rel -- key number */
  7, 8, 20, 21,			/* 1 -- trg -- trigger number */
  8, 8, 21, 21,			/* 2 -- pls -- pulse number */
  10, 10, 23, 23,		/* 3 -- key, rel -- port number */
  12, 13, 25, 26		/* 4 -- key, rel -- channel number */
};

short des2fmt[] = {		/* destination type to data entry format */

  -1, 0, 1, 2, 3, 4, 4, 5,
  4, 4, 6, 9, 6, 6, 6, 10,
  9, 9, 10, 10, 10, 10, 7, 7,
  7, 7, 8
};

short dtabl7[] = { 11, 1, 2, 3, 10 };	/* aux datum format table */

short dtabl9[] = { 11, 1, 2, 3, 9 };	/* FPU datum format table */

/* 
*/

/*
   =============================================================================
	SnapPTV() -- snap dump patch variables
   =============================================================================
*/

SnapPTV (s)
     char *s;
{
  register unsigned short stim;
  register short i;
  char dbuf[50];

  printf ("\n%s():  called SnapPTV()\n", s);

  memcpy (dbuf, ptdebuf, 48);

  for (i = 0; i < 48; i++)
    if (dbuf[i] EQ '\0')
      dbuf[i] = ' ';
    else if (dbuf[i] & 0x0080)
      dbuf[i] = '~';

  dbuf[48] = '\0';

  printf ("  ptdebuf = \"%s\"\n", dbuf);

  printf ("  pte  $%04.4X $%04.4X   $%04.4X $%04.4X   $%04.4X $%04.4X\n",
	  ptedef, ptestm, ptespec, ptesuba, ptedat1, ptedat2);

  printf ("  ptb  $%04.4X $%04.4X   $%04.4X $%04.4X   $%04.4X $%04.4X\n",
	  ptebuf.defnum, ptebuf.stmnum, ptebuf.paspec,
	  ptebuf.pasuba, ptebuf.padat1, ptebuf.padat2);

  printf ("  pte  dftp: %2d  dfst: %2d  dest: %2d  data: %2d  menu: %2d\n",
	  ptedftp, ptedfst, ptedest, ptedata, ptemenu);

  printf
    ("  pte  good: %d  dfok: %d  stok: %d  dsok: %d  dtok: %d  set: %d  flag: %d\n",
     ptegood, ptedfok, ptestok, ptedsok, ptedtok, pteset, ptbflag);

  printf ("  pte  pred: %3d  cpos: %3d  succ: %3d\n",
	  ptepred, ptecpos, ptesucc);

/* 
*/

#if	DUMPDEF

  if (dumpdef)
    {

      printf ("  dpe  pred: %3d  cpos: %3d  succ: %3d\n",
	      dpepred, dpecpos, dpesucc);

      for (stim = 0; stim < NDEFSTMS; stim++)
	{

	  if (defptr[stim])
	    {

	      printf ("  defptr[$%04.4X] = %3d%s\n",
		      stim, defptr[stim],
		      ((TRG_MASK & ptedef) EQ stim) ? " <-- ptedef" : "");
	    }
	}
    }
#endif

#if	DUMPSTM
  if (dumpstm)
    {

      for (stim = 0; stim < NDEFSTMS; stim++)
	if (stmptr[stim])
	  printf ("  stmptr[$%04.4X] = %3d%s\n",
		  stim, stmptr[stim],
		  ((TRG_MASK & ptestm) EQ stim) ? " <-- ptestm" : "");
    }
#endif

  printf ("\n");
}

/* 
*/

/*
   =============================================================================
	buf2pte() -- pack the patch buffer
   =============================================================================
*/

buf2pte ()
{
  ptebuf.defnum = ptedef;
  ptebuf.stmnum = ptestm;
  ptebuf.paspec = ptespec;
  ptebuf.pasuba = ptesuba;
  ptebuf.padat1 = ptedat1;
  ptebuf.padat2 = ptedat2;

  pteset = TRUE;

#if	DEBUGPD
  if (debugsw AND debugpd)
    {

      if (snappb)
	SnapPTV ("buf2pte");

      printf ("buf2pte():  ptebuf setup\n");
    }
#endif

}

/* 
*/

/*
   =============================================================================
	voidpb() -- void the patch buffer
   =============================================================================
*/

voidpb ()
{
  memset (ptdebuf, ' ', 50);	/* clear data entry buffer */

  ptebuf.defnum = ptedef = NULL_DEF;	/* blank definer */
  ptebuf.stmnum = ptestm = 0x0000;
  ptebuf.paspec = ptespec = 0x0000;
  ptebuf.pasuba = ptesuba = 0x0000;
  ptebuf.padat1 = ptedat1 = 0x0000;
  ptebuf.padat2 = ptedat2 = 0x0000;

  ptbflag = FALSE;

  pteset = FALSE;

  ptegood = FALSE;

  ptedfok = TRUE;		/* definer is OK until it's screwed up */
  ptestok = FALSE;
  ptedsok = FALSE;
  ptedtok = FALSE;

  ptedfst = -1;
  ptedest = -1;
  ptedata = -1;

#if	DEBUGVP
  if (debugsw AND debugvp)
    {

      if (snappb)
	SnapPTV ("voidpb");

      printf ("voidpb():  patch buffer VOIDED\n");
    }
#endif

}

/* 
*/

/*
   =============================================================================

   =============================================================================
*/

setptcv ()
{
  register unsigned short spec;

  ptedest = des2fmt[spec = ptespec & PE_SPEC];

  switch (ptedest)
    {				/* setup datum entry format */

    case 0:			/* key */
    case 2:			/* pulse */

      ptedata = 8;
      break;

    case 1:			/* trigger */

      ptedata = 9;
      break;

    case 3:			/* led */

      ptedata = 14;
      break;
/* 
*/
    case 4:			/* seq, reg */

      if (spec EQ PA_SLIN)
	ptedata = 13;
      else if (spec EQ PA_SCTL)
	ptedata = 9;
      else
	ptedata = 12;

      break;

    case 5:			/* tuning */

      ptedata = 18;
      break;

    case 6:			/* inst, wave, config */

      if (spec EQ PA_INST)
	ptedata = 15;
      else if (spec EQ PA_CNFG)
	ptedata = 17;
      else
	ptedata = 16;

      break;

    case 7:			/* aux, rate, inten, dpth */
    case 8:			/* vout */

      ptedata = dtabl7[ptedat1];
      break;

    case 9:			/* osc, ind, frq */
    case 10:			/* level, filtr, fil q, loctn, dynmc */

      if (spec EQ PA_OSC)
	ptedata = ptedat1 + 4;
      else
	ptedata = dtabl9[ptedat1];

      break;

    default:			/* something weird got in here somehow ... */

#if	DEBUGPD
      if (debugsw AND debugpd)
	printf ("setptcv():  BAD ptedest ($%04.4X),  spec = $%04.4X\n",
		ptedest, spec);
#endif

      ptedata = -1;
      break;
    }
}

/* 
*/

/*
   =============================================================================
	pte2buf() -- convert ptebuf to ptdebuf format and load edit variables
   =============================================================================
*/

pte2buf ()
{
  register unsigned short spec;

  memset (ptdebuf, ' ', 50);

  ptbflag = TRUE;

  if (pteset)
    {				/* if ptebuf is valid ... */

      ptedef = ptebuf.defnum;	/* ... setup the edit variables */
      ptestm = ptebuf.stmnum;
      ptespec = ptebuf.paspec;
      ptesuba = ptebuf.pasuba;
      ptedat1 = ptebuf.padat1;
      ptedat2 = ptebuf.padat2;

      setptcv ();		/* setup control variables */

      dspdfst (&ptdebuf[2], ptedef);	/* setup the patch buffer */
      dspdfst (&ptdebuf[15], ptestm);
      dspdest (&ptdebuf[28], &ptebuf);
      ptdebuf[0] = ' ';
      ptdebuf[1] = ' ';
      ptdebuf[14] = ' ';
      ptdebuf[27] = ' ';

/* 
*/
      ptegood = TRUE;		/* validate the patch buffer */
      ptedfok = TRUE;
      ptestok = TRUE;
      ptedsok = TRUE;
      ptedtok = TRUE;

#if	DEBUGPD
      if (debugsw AND debugpd)
	{

	  if (snappb)
	    SnapPTV ("pte2buf");

	  printf ("pte2buf():  patch buffer LOADED\n");
	}
#endif

    }
  else
    {

      voidpb ();		/* void the patch buffer */

#if	DEBUGPD
      if (debugsw AND debugpd)
	printf ("pte2buf():  patch buffer VOIDED\n");
#endif

    }
}

/* 
*/

/*
   =============================================================================
	ptde_ds() -- digit data entry for definer / stimulus fields
   =============================================================================
*/

ptde_ds (n, key)
     register short n, key;
{
  register short chan, port, stim;
  char buf[8], buf2[8];

  if (n ? ptestok : ptedfok)
    {

      if ((n EQ 0) AND (ptedef EQ 0xFFFF))
	return;

      port = ((n ? ptestm : ptedef) >> 11) & 0x0003;
      chan = ((n ? ptestm : ptedef) >> 7) & 0x000F;
      stim = (n ? ptestm : ptedef) & 0x007F;

      if ((port EQ 0) OR (port EQ 1) OR ((port EQ 2) AND (chan < 2)))
	{

	  /* Key / Rel */

	  if (inrange (stccol, dsdecol[0][n][0], dsdecol[0][n][1]) OR
	      inrange (stccol, dsdecol[3][n][0], dsdecol[3][n][1]) OR
	      inrange (stccol, dsdecol[4][n][0], dsdecol[4][n][1]))
	    {

	      if (stccol EQ dsdecol[3][n][1])
		{

		  if ((key < 1) OR (key > 3))
		    return;

		  if (key EQ 3)
		    {

		      buf[0] = 'L';
		      UpdVid (7, n ? 25 : 12, "  ", PTDATR);

		      if (n)
			ptestm &= 0xF87F;
		      else
			ptedef &= 0xF87F;

		    }
		  else
		    {

		      sprintf (buf2, "%02d", 1 + chan);
		      buf[0] = key + '0';
		      UpdVid (7, n ? 25 : 12, buf2, PTDATR);
		    }
/* 
*/
		  port = key - 1;

		  if (n)
		    ptestm = (ptestm & 0xE7FF) | (port << 11);
		  else
		    ptedef = (ptestm & 0xE7FF) | (port << 11);

		}
	      else
		{

		  buf[0] = key + '0';
		}

	      ptdebuf[stccol] = buf[0];
	      buf[1] = '\0';

	      UpdVid (7, stccol, buf, PTDATR);

	      if (stccol EQ dsdecol[4][n][1])
		{

		  ctcon ();
		  return;
		}

	      if ((stccol EQ dsdecol[0][n][1]) OR
		  (stccol EQ dsdecol[3][n][1]))
		++stccol;

	      movectc (stcrow, ++stccol);
	    }

	  return;
/* 
*/
	}
      else if ((port EQ 2) AND (chan EQ 2))
	{

	  /* Trig */

	  if (inrange (stccol, dsdecol[1][n][0], dsdecol[1][n][1]))
	    {

	      ptdebuf[stccol] = key + '0';
	      buf[0] = key + '0';
	      buf[1] = '\0';

	      UpdVid (7, stccol, buf, PTDATR);

	      if (stccol EQ dsdecol[1][n][1])
		{

		  ctcon ();
		  return;
		}

	      movectc (stcrow, ++stccol);
	    }

	  return;

	}
      else if ((port EQ 2) AND (chan EQ 3))
	{

	  /* Pulse */

	  if (stccol EQ dsdecol[2][n][1])
	    {

	      if ((key < 1) OR (key > 2))
		return;

	      ptdebuf[stccol] = key + '0';
	      buf[0] = key + '0';
	      buf[1] = '\0';

	      UpdVid (7, stccol, buf, PTDATR);
	      ctcon ();
	    }

	  return;
	}
    }
}

/* 
*/

/*
   =============================================================================
	ptdkey() -- patch digit data entry function
   =============================================================================
*/

ptdkey ()
{
  register short key, val, vg;
  char buf[8];

  if (NOT astat)		/* only do this on key closures */
    return;

  if (NOT ptbflag)		/* load up the edit buffer */
    pte2buf ();

  key = asig - 60;

  if (inrange (stccol, 2, 13))
    {				/* DEFINER */

      ptde_ds (0, key);
      return;

    }
  else if (inrange (stccol, 15, 26))
    {				/* STIMULUS */

      ptde_ds (1, key);
      return;
/* 
*/
    }
  else if (inrange (stccol, 28, 40))
    {				/* DESTINATION */

      if (ptedsok)
	{

	  switch (ptedest)
	    {

	    case 0:		/* key */

	      if (inrange (stccol, 30, 32) OR
		  (stccol EQ 34) OR inrange (stccol, 36, 37))
		{

		  if (stccol EQ 34)
		    {

		      if ((key < 1) OR (key > 3))
			return;

		      if (key EQ 3)
			buf[0] = 'L';
		      else
			buf[0] = key + '0';

		    }
		  else
		    {

		      buf[0] = key + '0';
		    }

		  ptdebuf[stccol] = buf[0];
		  buf[1] = '\0';

		  UpdVid (7, stccol, buf, PTDATR);

		  if ((stccol EQ 32) OR (stccol EQ 34))
		    ++stccol;

		  if (stccol EQ 37)
		    ctcon ();
		  else
		    movectc (stcrow, ++stccol);
		}

	      return;
/* 
*/
	    case 1:		/* trg */

	      if (inrange (stccol, 36, 37))
		{

		  ptdebuf[stccol] = key + '0';
		  buf[0] = key + '0';
		  buf[1] = '\0';

		  UpdVid (7, stccol, buf, PTDATR);

		  if (stccol EQ 37)
		    ctcon ();
		  else
		    movectc (stcrow, ++stccol);
		}

	      return;

	    case 2:		/* pls */

	      if (stccol EQ 34)
		{

		  if ((key < 1) OR (key > 2))
		    return;

		  ptdebuf[stccol] = key + '0';
		  buf[0] = key + '0';
		  buf[1] = '\0';

		  UpdVid (7, stccol, buf, PTDATR);
		  ctcon ();
		}

	      return;
/* 
*/
	    case 3:		/* led */

	      if (stccol EQ 32)
		{

		  if (key > 6)
		    return;

		  ptdebuf[stccol] = key + 'A';
		  buf[0] = key + 'A';
		  buf[1] = '\0';

		  UpdVid (7, stccol, buf, PTDATR);
		  ctcon ();
		}

	      return;

	    case 4:		/* seqlin, seqctl, regset, regadd */

	      if (inrange (stccol, 32, 33))
		{

		  ptdebuf[stccol] = key + '0';
		  buf[0] = key + '0';
		  buf[1] = '\0';

		  UpdVid (7, stccol, buf, PTDATR);

		  if (stccol EQ 33)
		    ctcon ();
		  else
		    movectc (stcrow, ++stccol);
		}

	      return;
/* 
*/
	    case 5:		/* tune */
	    case 7:		/* aux, rate, inten, depth */

	      return;

	    case 8:		/* v out */

	      if (stccol EQ 32)
		{

		  if ((key < 1) OR (key > 5))
		    return;

		  ptdebuf[stccol] = key + '0';
		  buf[0] = key + '0';
		  buf[1] = '\0';

		  UpdVid (7, stccol, buf, PTDATR);
		  movectc (DATAROW, 34);
		}

	      return;

	    case 9:		/* osc, index, freq */

	      if (stccol EQ 32)
		{

		  if ((key < 1) OR (key > 4))
		    return;

		  ptdebuf[stccol] = key + '0';
		  buf[0] = key + '0';
		  buf[1] = '\0';

		  UpdVid (7, stccol, buf, PTDATR);
		  movectc (DATAROW, 34);
		  return;
		}

	      /* FALL-THROUGH to handle v/g in columns 39..40 */
/* 
*/
	    case 6:		/* inst, wava, wavb, conf */
	    case 10:		/* level, filtr, fil q, loctn, dynmc */

	      if (inrange (stccol, 39, 40))
		{

		  vg = (ptesuba >> 8) & 0x00FF;

		  if (stccol EQ 39)
		    {

		      if ((key EQ 8) OR (key EQ 9))
			{

			  if (vg > 11)
			    {

			      vg -= 12;
			      val = 'V';

			    }
			  else
			    {

			      vg += 12;
			      val = 'G';
			    }

			  ptesuba = (ptesuba & 0x00FF) | (vg << 8);

			}
		      else
			{

			  return;
			}
/* 
*/
		    }
		  else
		    {		/* column 40 */

		      if (((vg EQ 0) OR (vg EQ 12)) AND
			  ((key EQ 0) OR (key EQ 1) OR (key EQ 2)))
			key += 10;
		      else if (key EQ 0)
			return;

		      val = key + ((key > 9) ? '\242' : '0');

		      --key;

		      if (vg > 11)
			vg = key + 12;
		      else
			vg = key;

		      ptesuba = (ptesuba & 0x00FF) | (vg << 8);
		    }

		  ptdebuf[stccol] = val;
		  buf[0] = val;
		  buf[1] = '\0';

		  UpdVid (7, stccol, buf, PTDATR);

		  if (stccol EQ 40)
		    ctcon ();
		  else
		    movectc (stcrow, ++stccol);
		}

	      return;

	    default:		/* Eh ? */

	      return;
	    }
	}
/* 
*/
    }
  else if (inrange (stccol, 42, 46))
    {				/* DATUM */

      switch (ptedata)
	{

	case 1:		/* multiplier -- +1.00 */

	  switch (stccol)
	    {

	    case 42:		/* + | - */

	      if (key EQ 8)
		buf[0] = '-';
	      else if (key EQ 9)
		buf[0] = '+';
	      else
		return;

	      break;

	    case 43:		/* 0 | 1 */

	      if ((key EQ 0) OR (key EQ 1))
		buf[0] = key + '0';
	      else
		return;

	      break;

	    case 45:		/* 0..9 */
	    case 46:		/* 0..9 */

	      buf[0] = key + '0';
	      break;

	    case 44:		/* . */
	    default:

	      return;
	    }
/* 
*/
	  buf[1] = '\0';
	  ptdebuf[stccol] = buf[0];

	  UpdVid (7, stccol, buf, PTDATR);

	  if (stccol EQ 43)
	    ++stccol;

	  if (stccol EQ 46)
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;
/* 
*/
	case 2:		/* time -- 32767 */

	  if ((stccol EQ 42) AND (key > 3))
	    return;

	  buf[0] = key + '0';
	  buf[1] = '\0';
	  ptdebuf[stccol] = buf[0];

	  UpdVid (7, stccol, buf, PTDATR);

	  if (stccol EQ 46)
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;

/* 
*/
	case 3:		/* value -- +10.00 */

	  switch (stccol)
	    {

	    case 42:

	      if (key EQ 8)
		{

		  if (ptdebuf[42] EQ '\240')
		    buf[0] = '\241';
		  else if (ptdebuf[42] EQ '\241')
		    buf[0] = '\241';
		  else
		    buf[0] = '-';

		}
	      else if (key EQ 9)
		{

		  if (ptdebuf[42] EQ '\240')
		    buf[0] = '\240';
		  else if (ptdebuf[42] EQ '\241')
		    buf[0] = '\240';
		  else
		    buf[0] = '+';

		}
	      else if (key EQ 0)
		{

		  if (ptdebuf[42] EQ '\240')
		    buf[0] = '+';
		  else if (ptdebuf[42] EQ '\241')
		    buf[0] = '-';
		  else
		    return;

		}
	      else if (key EQ 1)
		{

		  if (ptdebuf[42] EQ '\240')
		    buf[0] = '\240';
		  else if (ptdebuf[42] EQ '\241')
		    buf[0] = '\241';
		  else if (ptdebuf[42] EQ '+')
		    buf[0] = '\240';
		  else if (ptdebuf[42] EQ '-')
		    buf[0] = '\241';
		  else
		    return;

		}
	      else
		return;

	      break;
/* 
*/
	    case 43:
	    case 45:
	    case 46:

	      buf[0] = key + '0';
	      break;

	    case 44:
	    default:
	      return;
	    }

	  buf[1] = '\0';
	  ptdebuf[stccol] = buf[0];

	  UpdVid (7, stccol, buf, PTDATR);

	  if (stccol EQ 43)
	    ++stccol;

	  if (stccol EQ 46)
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;
/* 
*/
	case 4:		/* interval -- +1200 */

	  if (stccol EQ 42)
	    {

	      if (key EQ 8)
		buf[0] = '-';
	      else if (key EQ 9)
		buf[0] = '+';
	      else
		return;

	    }
	  else if (stccol EQ 43)
	    {

	      if (key > 1)
		return;
	      else
		buf[0] = key + '0';

	    }
	  else
	    {

	      buf[0] = key + '0';
	    }

	  ptdebuf[stccol] = buf[0];
	  buf[1] = '\0';

	  UpdVid (7, stccol, buf, PTDATR);

	  if (stccol EQ 46)
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;
/* 
*/
	case 5:		/* ratio -- 9/9 */

	  switch (stccol)
	    {

	    case 42:
	    case 44:

	      buf[0] = key + '0';
	      break;

	    case 43:
	    case 45:
	    case 46:

	      return;
	    }

	  ptdebuf[stccol] = buf[0];
	  buf[1] = '\0';

	  UpdVid (7, stccol, buf, PTDATR);

	  if (stccol EQ 42)
	    ++stccol;

	  if (stccol EQ 44)
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;
/* 
*/
	case 6:		/* frequency -- 15.9 */

	  switch (stccol)
	    {

	    case 42:

	      if (key > 1)
		return;

	      /* FALL-THROUGH */

	    case 43:
	    case 45:

	      buf[0] = key + '0';
	      break;

	    case 44:
	    case 46:

	      return;
	    }

	  ptdebuf[stccol] = buf[0];
	  buf[1] = '\0';

	  UpdVid (7, stccol, buf, PTDATR);

	  if (stccol EQ 43)
	    ++stccol;

	  if (stccol EQ 45)
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;
/* 
*/
	case 7:		/* pitch -- 9C#99 */

	  switch (stccol)
	    {

	    case 43:

	      if (key > 6)
		return;

	      buf[0] = key + 'A';
	      break;

	    case 44:

	      if (key EQ 7)
		buf[0] = ' ';
	      else if (key EQ 8)
		buf[0] = '\251';
	      else if (key EQ 9)
		buf[0] = '\250';
	      else
		return;

	      break;

	    case 42:
	    case 45:
	    case 46:

	      buf[0] = key + '0';
	      break;
	    }

	  ptdebuf[stccol] = buf[0];
	  buf[1] = '\0';

	  UpdVid (7, stccol, buf, PTDATR);

	  if (stccol EQ 46)
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;
/* 
*/
	case 8:		/* trans | stop | start */

	  switch (key)
	    {

	    case 7:		/* trans */

	      strcpy (buf, "Trans");
	      ptedat2 = 0;
	      break;

	    case 8:		/* stop */

	      strcpy (buf, "Stop ");
	      ptedat2 = 1;
	      break;

	    case 9:		/* start */

	      strcpy (buf, "Start");
	      ptedat2 = 2;
	      break;

	    default:

	      return;
	    }

	  strcpy (&ptdebuf[42], buf);
	  UpdVid (7, 42, buf, PTDATR);
	  movectc (stcrow, 46);
	  return;
/* 
*/
	case 9:		/* stop | start */

	  if (key EQ 8)
	    {			/* stop */

	      strcpy (buf, "Stop ");
	      ptedat2 = 0;

	    }
	  else if (key EQ 9)
	    {			/* start */

	      strcpy (buf, "Start");
	      ptedat2 = 1;

	    }
	  else
	    {

	      return;
	    }

	  strcpy (&ptdebuf[42], buf);
	  UpdVid (7, 42, buf, PTDATR);
	  movectc (stcrow, 46);
	  return;

	case 10:		/* off | on */

	  if (key EQ 8)
	    {			/* off */

	      strcpy (buf, "Off   ");
	      ptedat2 = 0;

	    }
	  else if (key EQ 9)
	    {			/* on */

	      strcpy (buf, "On   ");
	      ptedat2 = 1;

	    }
	  else
	    {

	      return;
	    }

	  strcpy (&ptdebuf[42], buf);
	  UpdVid (7, 42, buf, PTDATR);
	  movectc (stcrow, 46);
	  return;
/* 
*/
	case 11:		/* source */

	  return;

	case 12:		/* register | value */

	  switch (stccol)
	    {

	    case 42:

	      if (key EQ 7)
		buf[0] = 'R';
	      else if (key EQ 8 AND ((PE_SPEC & ptespec) NE PA_RSET))
		buf[0] = '-';
	      else if (key EQ 9)
		buf[0] = '+';
	      else
		return;

	      break;

	    case 43:
	    case 44:

	      buf[0] = key + '0';
	      break;

	    default:

	      return;
	    }

	  ptdebuf[stccol] = buf[0];
	  buf[1] = '\0';

	  UpdVid (7, stccol, buf, PTDATR);

	  if (stccol EQ 44)
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;
/* 
*/
	case 13:		/* sequence line */

	  if (stccol > 44)
	    return;

	  buf[0] = key + '0';
	  ptdebuf[stccol] = buf[0];
	  buf[1] = '\0';

	  UpdVid (7, stccol, buf, PTDATR);

	  if (stccol EQ 44)
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;

	case 14:		/* LED controls */

	  if ((key > 3) OR (stccol EQ 46))
	    return;

	  if ((stccol EQ 45) AND (0 EQ (ptesuba & 0x0001)))
	    return;

	  buf[0] = key + '0';
	  buf[1] = '\0';
	  ptdebuf[stccol] = buf[0];

	  UpdVid (7, stccol, buf, PTDATR);

	  if (((ptesuba & 0x0001) AND (stccol EQ 45)) OR
	      ((0 EQ ptesuba & 0x0001) AND (stccol EQ 44)))
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;
/* 
*/
	case 15:		/* instrument number */

	  if (stccol > 43)
	    return;

	  if ((stccol EQ 42) AND (key > 4))
	    return;

	  buf[0] = key + '0';
	  buf[1] = '\0';
	  ptdebuf[stccol] = buf[0];

	  UpdVid (7, stccol, buf, PTDATR);

	  if (stccol EQ 43)
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;
/* 
*/
	case 16:		/* waveshape number */

	  if (stccol > 43)
	    return;

	  if ((stccol EQ 42) AND (key > 2))
	    return;

	  buf[0] = key + '0';
	  buf[1] = '\0';
	  ptdebuf[stccol] = buf[0];

	  UpdVid (7, stccol, buf, PTDATR);

	  if (stccol EQ 43)
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;

	case 17:		/* configuration number */

	  if (stccol > 43)
	    return;

	  if ((stccol EQ 42) AND (key > 1))
	    return;

	  buf[0] = key + '0';
	  buf[1] = '\0';
	  ptdebuf[stccol] = buf[0];

	  UpdVid (7, stccol, buf, PTDATR);

	  if (stccol EQ 43)
	    ctcon ();
	  else
	    movectc (stcrow, ++stccol);

	  return;
/* 
*/
	case 18:		/* tuning table number */

	  if (stccol > 42)
	    return;

	  buf[0] = key + '0';
	  buf[1] = '\0';
	  ptdebuf[stccol] = buf[0];

	  UpdVid (7, stccol, buf, PTDATR);

	  ctcon ();
	  return;

	default:

	  return;
	}
    }
}
