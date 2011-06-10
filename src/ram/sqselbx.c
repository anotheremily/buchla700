/*
   =============================================================================
	sqselbx.c -- MIDAS-VII sequence editor box selection functions
	Version 19 -- 1988-11-18 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "graphdef.h"
#include "patch.h"
#include "macros.h"
#include "curpak.h"

#include "midas.h"
#include "ptdisp.h"

#define	ST_0		0x01	/* initial Action 1 or Action 2 */
#define	ST_1		0x02	/* operand type */
#define	ST_2		0x04	/* initial Action 3 */

#define	ST_OFF		0x00	/* cancel highlighting */

/* 
*/

extern short cmtype;
extern short curslin;
extern short stccol;
extern short stcrow;
extern short submenu;
extern short vtccol;
extern short vtcrow;

extern char actlft[];

extern short sqatype[];

extern unsigned short *obj11;

extern struct seqent seqtab[];

short action;			/* current action column */
short sqdeflg;			/* sequence data entry buffer filled */
short sqmenu;			/* sqeuence submenu state */

char sqdebuf[50];		/* sequence data entry buffer */

struct seqent seqbuf;		/* sequence line buffer */

short sqfnbox ();

/* 
*/

char sqhilit[][8] = {		/* submenu highlight table */

  /* start, width, row1, row2, row3, row4, row5, pad */

  {2, 13, ST_0 | ST_2, ST_0 | ST_2, ST_0 | ST_2, ST_0, 0, 0},
  {17, 11, ST_0 | ST_2, ST_0 | ST_2, ST_0 | ST_2, ST_0, 0, 0},
  {30, 11, ST_0 | ST_2, ST_0 | ST_2, 0, ST_0, 0, 0},
  {43, 8, ST_0 | ST_2, ST_0 | ST_2, ST_0, ST_0, ST_0, 0},
  {53, 8, ST_1, ST_1, ST_1, ST_1, 0, 0},
};

struct selbox sqboxes[] = {	/* selection boxes */

  {CTOX (1), RTOY (DATAROW), CTOX (48) - 1, RTOY (1 + DATAROW) - 1, 0, sqfnbox},	/* current patch */

  {0, 0, 0, 0, 0, FN_NULL}
};

char sqopreq[] = {		/* action needs operand type flag table */

  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1
};

char nextact[] = { 24, 36, 6 };	/* next column after action entry */

/* 
*/

short sqidata[] = {		/* intial data by box */

  0x0000,			/*      "           "   0 */
  0x0000,			/*      "Kt 001 1 01"   1 */
  0x0000,			/*      "Kc 001 1 01"   2 */
  0x0000,			/*      "Kr 001 1 01"   3 */
  0x0000,			/*      "If 001 1 01"   4 */
  0x0000,			/*      "Trig on  01"   5 */
  0x0000,			/*      "Trig off 01"   6 */
  0x0000,			/*      "Trig tgl 01"   7 */
  0x0000,			/*      "If trig  01"   8 */
  0x0000,			/*      "Stop       "   9 */
  0x0000,			/*      "Jump to 000"   10 */
  0x0000,			/*      "???????????"   11 */
  0x0000,			/*      "If stim act"   12 */
  SQ_VAL,			/*      "Set R01=00 "   13 */
  SQ_VAL,			/*      "Inc R01+00 "   14 */
  SQ_VAL,			/*      "If  R01=00 "   15 */
  SQ_VAL,			/*      "If  R01<00 "   16 */
  SQ_VAL			/*      "If  R01>00 "   17 */
};

/* 
*/

char actcol[] = {		/* action data entry column by action */

  0, 3, 3, 3, 3, 9, 9, 9, 9, 5, 5, 5, 5, 10, 8, 3, 5
};

char sqvcol[] = { 9, 8, 9, 9 };	/* value entry column - type order */

char seqvcol[] = { 8, 8, 9, 9, 9 };	/* value entry column - box order */

short sqndata[] = {		/* data types in box order */

  SQ_VAL, SQ_REG, SQ_VLT, SQ_RND
};

/* 
*/

/*
   =============================================================================
	hilitsq() -- highlight sequence submenu
   =============================================================================
*/

hilitsq (n)
     register char n;
{
  register short chr, h, v, w;

  sqmenu = n;			/* set current submenu type */

  for (h = 0; h < 5; h++)
    {				/* scan the columns */

      for (v = 0; v < 5; v++)
	{			/* scan the rows */

	  chr = sqhilit[h][0];	/* starting character */

	  for (w = sqhilit[h][1]; w-- > 0;)
	    vsetcv (obj11, v + 1, chr++,
		    (sqhilit[h][v + 2] & n) ? PTIATR : PTMATR, 64);
	}
    }
}

/* 
*/

/*
   =============================================================================
	postcm() -- determine state after cursor motion
   =============================================================================
*/

postcm ()
{
  if (inrange (stccol, 12, 22))
    {

      if (inrange (stccol, 12, 13))
	hilitsq (ST_0);
      else if ((sqopreq[seqbuf.seqact1 & SQ_MACT]) AND (stccol EQ 19))
	hilitsq (ST_1);
      else
	hilitsq (ST_OFF);

      return;

    }
  else if (inrange (stccol, 24, 34))
    {

      if (inrange (stccol, 24, 25))
	hilitsq (ST_0);
      else if ((sqopreq[seqbuf.seqact2 & SQ_MACT]) AND (stccol EQ 31))
	hilitsq (ST_1);
      else
	hilitsq (ST_OFF);

      return;

    }
  else if (inrange (stccol, 36, 46))
    {

      if (inrange (stccol, 36, 37))
	hilitsq (ST_2);
      else if ((sqopreq[seqbuf.seqact3 & SQ_MACT]) AND (stccol EQ 43))
	hilitsq (ST_1);
      else
	hilitsq (ST_OFF);

      return;

    }
  else
    {

      hilitsq (ST_OFF);
      return;
    }
}

/* 
*/

/*
   =============================================================================
	movestc() -- move cursor and reset highlighting
   =============================================================================
*/

movestc (r, c)
     short r, c;
{
  ctcpos (r, c);
  postcm ();
}

/*
   =============================================================================
	endssm() -- end patch submenu data entry
   =============================================================================
*/

endssm (row, col)
     short row, col;
{
  submenu = FALSE;
  cmtype = CT_SMTH;

  mtcoff ();
  ctcon ();
  movestc (row, col);
}

/* 
*/

/*
   =============================================================================
	setsqm() -- setup submenu
   =============================================================================
*/

setsqm (r, c)
     short r, c;
{
  submenu = TRUE;
  cmtype = CT_MENU;
  mtcpos (r, c);
}

/* 
*/

/*
   =============================================================================
	sqenter() -- enter an action
   =============================================================================
*/

short
sqenter ()
{
  register short i, lcol;
  register long ltemp;
  register unsigned short *ap, *dp;
  unsigned short theact, port, chan, key, val, obj, dtype;

  switch (action)
    {

    case 0:

      ap = &seqbuf.seqact1;
      dp = &seqbuf.seqdat1;
      break;

    case 1:

      ap = &seqbuf.seqact2;
      dp = &seqbuf.seqdat2;
      break;

    case 2:

      ap = &seqbuf.seqact3;
      dp = &seqbuf.seqdat3;
      break;

    default:

      action = 0;
      movestc (DATAROW, actlft[action]);
      return (FAILURE);
    }

/* 
*/
  lcol = actlft[action];
  theact = SQ_MACT & *ap;

  switch (theact)
    {

    case SQ_TKEY:		/*      "Kt 001 1 01" */
    case SQ_CKEY:		/*      "Kc 001 1 01" */
    case SQ_RKEY:		/*      "Kr 001 1 01" */
    case SQ_IKEY:		/*      "If 001 1 01" */

      ltemp = 0;

      for (i = lcol + 3; i < lcol + 6; i++)
	ltemp = (ltemp * 10) + (sqdebuf[i] - '0');

      if ((ltemp < 1L) OR (ltemp > 128L))
	{

	  movestc (DATAROW, actlft[action] + 3);
	  return (FAILURE);
	}

      key = ltemp - 1;
/* 
*/
      i = sqdebuf[lcol + 7];

      if ((i EQ '1') OR (i EQ '2'))
	{

	  port = i - '1';

	}
      else if (i EQ 'L')
	{

	  port = 2;

	}
      else
	{

	  movestc (DATAROW, actlft[action] + 7);
	  return (FAILURE);
	}

      ltemp = ((sqdebuf[lcol + 9] - '0') * 10) + (sqdebuf[lcol + 10] - '0');

      if ((ltemp < 1L) OR (ltemp > 16L))
	{

	  movestc (DATAROW, actlft[action] + 9);
	  return (FAILURE);
	}

      chan = ltemp - 1;
      *dp = (port << 11) | (chan << 7) | key;

      break;
/* 
*/
    case SQ_STRG:		/*      "Trig on  01" */
    case SQ_CTRG:		/*      "Trig off 01" */
    case SQ_TTRG:		/*      "Trig tgl 01" */

      ltemp = ((sqdebuf[lcol + 9] - '0') * 10) + (sqdebuf[lcol + 10] - '0');

      if ((ltemp < 1L) OR (ltemp > 16))
	{

	  movestc (DATAROW, actlft[action] + 9);
	  return (FAILURE);
	}

      *dp = ltemp - 1;

      break;

    case SQ_NULL:		/*      "           " */
    case SQ_ITRG:		/*      "If trig act" */
    case SQ_STOP:		/*      "Stop       " */
    case SQ_ISTM:		/*      "If stim act" */

      break;

    case SQ_JUMP:		/*      "Jump to 000" */

      ltemp = 0;

      for (i = lcol + 8; i < lcol + 11; i++)
	ltemp = (ltemp * 10) + (sqdebuf[i] - '0');

      *dp = ltemp;

      break;
/* 
*/
    case SQ_SREG:		/*      "Set R01=00 " */
    case SQ_AREG:		/*      "Inc R01+00 " */
    case SQ_IREQ:		/*      "If  R01=00 " */
    case SQ_IRLT:		/*      "If  R01<00 " */
    case SQ_IRGT:		/*      "If  R01>00 " */

      dtype = *dp & SQ_MTYP;

      ltemp = ((sqdebuf[lcol + 5] - '0') * 10) + (sqdebuf[lcol + 6] - '0');

      if ((ltemp < 1) OR (ltemp > 16))
	{

	  movestc (DATAROW, actlft[action] + 5);
	  return (FAILURE);
	}

      obj = ltemp - 1;

      switch (dtype)
	{

	case SQ_REG:		/* register */

	  ltemp = ((sqdebuf[lcol + 9] - '0') * 10) +
	    (sqdebuf[lcol + 10] - '0');

	  if ((ltemp < 1) OR (ltemp > 16))
	    {

	      movestc (DATAROW, actlft[action] + 9);
	      return (FAILURE);
	    }

	  val = ltemp - 1;
	  break;

	case SQ_VAL:		/* constant value */

	  val = ((sqdebuf[lcol + 8] - '0') * 10) + (sqdebuf[lcol + 9] - '0');

	  break;
/* 
*/
	case SQ_VLT:		/* control voltage */

	  val = (sqdebuf[lcol + 9] - '0') - 1;
	  break;

	case SQ_RND:		/* random value */

	  val = sqdebuf[lcol + 9] - '0';
	  break;

	default:

	  movestc (DATAROW, actlft[action] + 8);
	  return (FAILURE);
	}

      if (((*ap & SQ_MACT) EQ SQ_AREG) AND (sqdebuf[lcol + 7] EQ '-'))
	val |= 0x1000;

      *ap = (*ap & SQ_MACT) | (obj << 8);
      *dp = (*dp & SQ_MTYP) | val;

      break;

    default:

      movestc (DATAROW, actlft[action]);
      return (FAILURE);
    }
/* 
*/
  memcpyw (&seqtab[curslin], &seqbuf, NSEQW);
  dsqlin (sqdebuf, curslin);
  sqdeflg = TRUE;
  dcursq ();
  movestc (DATAROW, nextact[action]);
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	sqfnbox() -- sequence display box hit processor
   =============================================================================
*/

short
sqfnbox (n)
     short n;
{
  short act, vtype;
  register short box;
  register short i;
  register long ltemp;

  if (NOT submenu)
    {				/* SEQUENCE DATA ENTRY LINE */

      if (inrange (stccol, 2, 4))
	{			/* Line */

	  ltemp = 0;

	  for (i = 2; i < 5; i++)
	    ltemp = (ltemp * 10) + (sqdebuf[i] - '0');

	  curslin = ltemp;
	  sqdeflg = FALSE;
	  dstw ();
	  movestc (DATAROW, 2);
	  return (SUCCESS);

	}
      else if (inrange (stccol, 6, 10))
	{			/* Time */

	  ltemp = 0;

	  for (i = 6; i < 8; i++)
	    ltemp = (ltemp * 10) + (sqdebuf[i] - '0');

	  for (i = 9; i < 11; i++)
	    ltemp = (ltemp * 10) + (sqdebuf[i] - '0');

	  seqtab[curslin].seqtime = ltemp;
	  seqbuf.seqtime = ltemp;
	  dsqlin (sqdebuf, curslin);
	  sqdeflg = TRUE;
	  dcursq ();
	  movestc (DATAROW, 12);
	  return (SUCCESS);
/* 
*/
	}
      else if (inrange (stccol, 12, 22))
	{			/* Action 1 */

	  action = 0;

	  if (inrange (stccol, 12, 13))
	    {

	      setsqm (19, 2);
	      return (SUCCESS);

	    }
	  else if ((sqopreq[seqbuf.seqact1 & SQ_MACT]) AND (stccol EQ 19))
	    {

	      setsqm (19, 53);
	      return (SUCCESS);

	    }
	  else
	    {

	      return (sqenter ());
	    }

	}
      else if (inrange (stccol, 24, 34))
	{			/* Action 2 */

	  action = 1;

	  if (inrange (stccol, 24, 25))
	    {

	      setsqm (19, 2);
	      return (SUCCESS);

	    }
	  else if ((sqopreq[seqbuf.seqact2 & SQ_MACT]) AND (stccol EQ 31))
	    {

	      setsqm (19, 53);
	      return (SUCCESS);

	    }
	  else
	    {

	      return (sqenter ());
	    }
/* 
*/
	}
      else if (inrange (stccol, 36, 46))
	{			/* Action 3 */

	  action = 2;

	  if (inrange (stccol, 36, 37))
	    {

	      setsqm (19, 2);
	      return (SUCCESS);

	    }
	  else if ((sqopreq[seqbuf.seqact3 & SQ_MACT]) AND (stccol EQ 43))
	    {

	      setsqm (19, 53);
	      return (SUCCESS);

	    }
	  else
	    {

	      return (sqenter ());
	    }

	}
      else
	return (FAILURE);
/* 
*/
    }
  else
    {				/* SUBMENU SELECTION */

      /* determine the "box" we're pointing at */

      if (inrange (vtccol, 2, 14))
	box = vtcrow - 18;
      else if (inrange (vtccol, 17, 27))
	box = vtcrow - 14;
      else if (inrange (vtccol, 30, 40))
	box = vtcrow - 10;
      else if (inrange (vtccol, 43, 50))
	box = vtcrow - 6;
      else if (inrange (vtccol, 53, 60))
	box = vtcrow - 1;
      else
	return (FAILURE);
/* 
*/
      switch (sqmenu)
	{			/* switch on submenu type */

	case ST_0:		/* Action 1 or Action 2 type */
	case ST_2:		/* Action 3 type */

	  if (inrange (box, 18, 21))
	    return (FAILURE);

	  switch (action)
	    {

	    case 0:		/* action 1 */

	      act = SQ_MACT & (seqbuf.seqact1 = sqatype[box]);
	      vtype = 0x000F & ((seqbuf.seqdat1 = sqidata[box]) >> 8);
	      break;

	    case 1:		/* action 2 */

	      act = SQ_MACT & (seqbuf.seqact2 = sqatype[box]);
	      vtype = 0x000F & ((seqbuf.seqdat2 = sqidata[box]) >> 8);
	      break;

	    case 2:		/* action 3 */

	      if ((box EQ 4) OR (box EQ 8) OR (box EQ 12) OR
		  inrange (box, 15, 17))
		return (FAILURE);

	      act = SQ_MACT & (seqbuf.seqact3 = sqatype[box]);
	      vtype = 0x000F & ((seqbuf.seqdat3 = sqidata[box]) >> 8);
	      break;
	    }

	  memcpyw (&seqtab[curslin], &seqbuf, NSEQW);
	  dsqlin (sqdebuf, curslin);
	  sqdeflg = TRUE;
	  dcursq ();

	  if (sqopreq[act])
	    endssm (stcrow, actlft[action] + sqvcol[vtype]);
	  else if ((act EQ SQ_ISTM) OR (act EQ SQ_STOP))
	    endssm (stcrow, nextact[action]);
	  else
	    endssm (stcrow, actlft[action] + actcol[act]);

	  return (SUCCESS);
/* 
*/
	case ST_1:		/* operand type */

	  if (NOT inrange (box, 18, 21))
	    return (FAILURE);

	  switch (action)
	    {

	    case 0:		/* action 1 */

	      if (NOT sqopreq[act = SQ_MACT & seqbuf.seqact1])
		return (FAILURE);

	      seqbuf.seqdat1 = (seqbuf.seqdat1 & SQ_MFLG) | sqndata[box - 18];
	      break;

	    case 1:		/* action 2 */

	      if (NOT sqopreq[act = SQ_MACT & seqbuf.seqact2])
		return (FAILURE);

	      seqbuf.seqdat2 = (seqbuf.seqdat2 & SQ_MFLG) | sqndata[box - 18];
	      break;

	    case 2:		/* action 3 */

	      if (NOT sqopreq[act = SQ_MACT & seqbuf.seqact3])
		return (FAILURE);

	      seqbuf.seqdat3 = (seqbuf.seqdat3 & SQ_MFLG) | sqndata[box - 18];
	      break;
	    }

	  break;

	default:		/* -BOGUS- */

	  endssm (stcrow, stccol);
	  return (FAILURE);
	}

      memcpyw (&seqtab[curslin], &seqbuf, NSEQW);
      dsqlin (sqdebuf, curslin);
      sqdeflg = TRUE;
      dcursq ();
      endssm (stcrow, actlft[action] + seqvcol[box - 18]);
      return (SUCCESS);
    }

  return (FAILURE);
}
