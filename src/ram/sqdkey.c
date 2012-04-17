/*
   =============================================================================
	sqdkey.c -- MIDAS-VII sequence display data entry functions
	Version 8 -- 1988-11-22 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "fields.h"
#include "curpak.h"
#include "patch.h"
#include "macros.h"

#include "midas.h"
#include "ptdisp.h"

extern short asig, astat;

extern short action;
extern short curslin;
extern short sqdeflg;
extern short stccol;
extern short stcrow;

extern char sqdebuf[50];

extern struct seqent seqbuf;
extern struct seqent seqtab[];

short actfmt;			/* action code format */

/* 
*/

char actlft[] = { 12, 24, 36 };	/* action field leftmost columns */

char seqdfmt[] = {		/* action data entry format by action */

  0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 0, 4, 0, 3
};

/* 
*/

/*
   =============================================================================
	seq2buf() -- load the edit buffer from the current sequence line
   =============================================================================
*/

seq2buf ()
{
  memcpyw (&seqbuf, &seqtab[curslin], NSEQW);
  dsqlin (sqdebuf, curslin);
  sqdeflg = TRUE;
}

/* 
*/

/*
   =============================================================================
	sqactde() -- sequence action digit data entry function
   =============================================================================
*/

sqactde (key)
     register short key;
{
  register short col;
  short defmt;
  unsigned short act, vtype;
  char buf[8];

  col = stccol - actlft[action];	/* get field data entry column */

  switch (action)
    {				/* get action code from sequence entry */

    case 0:			/* action 1 */

      act = seqbuf.seqact1;
      vtype = SQ_MTYP & seqbuf.seqdat1;
      break;

    case 1:			/* action 2 */

      act = seqbuf.seqact2;
      vtype = SQ_MTYP & seqbuf.seqdat2;
      break;

    case 2:			/* action 3 */

      act = seqbuf.seqact3;
      vtype = SQ_MTYP & seqbuf.seqdat3;
      break;

    default:			/* something weird got in here */

      return;
    }

  defmt = seqdfmt[SQ_MACT & act];	/* get data entry format code */
/* 
*/
  switch (defmt)
    {				/* switch off of data entry format */

    case 1:			/* key, port, chan */

      if (inrange (col, 3, 5))
	{			/* key */

	  if ((col == 3) && (key > 1))
	    return;

	  buf[0] = key + '0';

	}
      else if (col == 7)
	{			/* port */

	  if ((key == 1) || (key == 2))
	    {			/* MIDI */

	      buf[0] = key + '0';

	      UpdVid (7, stccol + 1, " 01", PTDATR);
	      memcpy (&sqdebuf[stccol + 1], " 01", 3);

	    }
	  else if (key == 3)
	    {			/* local */

	      buf[0] = 'L';

	      UpdVid (7, stccol + 1, "   ", PTDATR);
	      memset (&sqdebuf[stccol + 1], ' ', 3);

	    }
	  else
	    {

	      return;
	    }

	}
      else if (inrange (col, 9, 10))
	{			/* channel */

	  if ((col == 9) && (key > 1))
	    return;

	  buf[0] = key + '0';
	}

      buf[1] = '\0';
      sqdebuf[stccol] = buf[0];

      UpdVid (7, stccol, buf, PTDATR);

      if ((col == 5) || (col == 7))
	{			/* skip blanks */

	  ++stccol;
	  ++col;
	}

      if (col == 10)
	ctcon ();
      else
	movestc (stcrow, ++stccol);

      return;
/* 
*/
    case 2:			/* trigger */

      if (inrange (col, 9, 10))
	{

	  if ((col == 9) && (key > 1))
	    return;

	}
      else
	{

	  return;
	}

      buf[0] = key + '0';
      buf[1] = '\0';
      sqdebuf[stccol] = key + '0';

      UpdVid (7, stccol, buf, PTDATR);

      if (col == 10)
	ctcon ();
      else
	movestc (stcrow, ++stccol);

      return;
/* 
*/
    case 3:			/* register operations */

      if ((col == 7) && (act == SQ_AREG))
	{

	  if (key == 8)		/* - */
	    buf[0] = '-';
	  else if (key == 9)	/* + */
	    buf[0] = '+';
	  else
	    return;

	  buf[1] = '\0';
	  sqdebuf[stccol] = buf[0];
	  UpdVid (7, stccol, buf, PTDATR);
	  movestc (stcrow, ++stccol);
	  return;
	}

      switch (vtype)
	{

	case SQ_REG:		/* register contents */

	  if (inrange (col, 5, 6) || inrange (col, 9, 10))
	    {

	      if (((col == 5) || (col == 9)) && (key > 1))
		return;

	    }
	  else
	    {

	      return;
	    }

	  buf[0] = key + '0';
	  buf[1] = '\0';
	  sqdebuf[stccol] = key + '0';

	  UpdVid (7, stccol, buf, PTDATR);

	  if (col == 6)
	    {

	      col += 2;
	      stccol += 2;
	    }

	  if (col == 10)
	    ctcon ();
	  else
	    movestc (stcrow, ++stccol);

	  return;
/* 
*/
	case SQ_VAL:		/* constant value */

	  if (inrange (col, 5, 6) || inrange (col, 8, 9))
	    {

	      if ((col == 5) && (key > 1))
		return;

	    }
	  else
	    {

	      return;
	    }

	  buf[0] = key + '0';
	  buf[1] = '\0';
	  sqdebuf[stccol] = key + '0';

	  UpdVid (7, stccol, buf, PTDATR);

	  if (col == 6)
	    {

	      ++col;
	      ++stccol;
	    }

	  if (col == 9)
	    ctcon ();
	  else
	    movestc (stcrow, ++stccol);

	  return;
/* 
*/
	case SQ_VLT:		/* voltage input */

	  if (inrange (col, 5, 6) || (col == 9))
	    {

	      if ((col == 5) && (key > 1))
		return;
	      else if ((col == 9) && ((key < 1) || (key > 4)))
		return;

	    }
	  else
	    {

	      return;
	    }

	  buf[0] = key + '0';
	  buf[1] = '\0';
	  sqdebuf[stccol] = key + '0';

	  UpdVid (7, stccol, buf, PTDATR);

	  if (col == 6)
	    {

	      col += 2;
	      stccol += 2;
	    }

	  if (col == 9)
	    ctcon ();
	  else
	    movestc (stcrow, ++stccol);

	  return;
/* 
*/
	case SQ_RND:		/* random value */

	  if (inrange (col, 5, 6) || (col == 9))
	    {

	      if ((col == 5) && (key > 1))
		return;

	      if ((col == 9) && (key > 6))
		return;

	    }
	  else
	    {

	      return;
	    }

	  buf[0] = key + '0';
	  buf[1] = '\0';
	  sqdebuf[stccol] = key + '0';

	  UpdVid (7, stccol, buf, PTDATR);

	  if (col == 6)
	    {

	      col += 2;
	      stccol += 2;
	    }

	  if (col == 9)
	    ctcon ();
	  else
	    movestc (stcrow, ++stccol);

	  return;
	}
/* 
*/
    case 4:			/* sequence line */

      if (inrange (col, 0, 7))
	return;

      buf[0] = key + '0';
      buf[1] = '\0';
      sqdebuf[stccol] = key + '0';

      UpdVid (7, stccol, buf, PTDATR);

      if (col == 10)
	ctcon ();
      else
	movestc (stcrow, ++stccol);

      return;

    case 0:			/* -none- */
    default:

      return;
    }
}

/* 
*/

/*
   =============================================================================
	sqdkey() -- sequence digit data entry control function
   =============================================================================
*/

sqdkey ()
{
  register short key;
  char buf[8];

  if (! astat)		/* only do this on key closures */
    return;

  if (! sqdeflg)		/* load up the edit buffer */
    seq2buf ();

  key = asig - 60;

  if (inrange (stccol, 2, 4))
    {				/* line */

      buf[0] = key + '0';
      buf[1] = '\0';

      sqdebuf[stccol] = key + '0';

      UpdVid (7, stccol, buf, PTDATR);

      if (stccol == 4)
	ctcon ();
      else
	movestc (stcrow, ++stccol);

      return;
/* 
*/
    }
  else if (inrange (stccol, 6, 10))
    {				/* time */

      if (stccol == 8)
	return;

      buf[0] = key + '0';
      buf[1] = '\0';

      sqdebuf[stccol] = key + '0';

      UpdVid (7, stccol, buf, PTDATR);

      if (stccol == 7)
	++stccol;

      if (stccol == 10)
	ctcon ();
      else
	movestc (stcrow, ++stccol);

      return;

    }
  else if (inrange (stccol, 12, 22))
    {				/* action 1 */

      action = 0;
      sqactde (key);
      return;

    }
  else if (inrange (stccol, 24, 34))
    {				/* action 2 */

      action = 1;
      sqactde (key);
      return;

    }
  else if (inrange (stccol, 36, 46))
    {				/* action 3 */

      action = 2;
      sqactde (key);
      return;
    }
}
