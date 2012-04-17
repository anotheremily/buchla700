/*
   =============================================================================
	sqdisp.c -- MIDAS-VII sequence editor display functions
	Version 24 -- 1989-11-16 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGSQ		0

#include "stddefs.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "patch.h"

#include "midas.h"
#include "ptdisp.h"

#if	DEBUGSQ
extern short debugsw;

short debugsq = 1;
#endif

extern short nokey ();

extern char *(*BakLine) (), *(*FwdLine) ();

extern unsigned short *obj8, *obj9, *obj10, *obj11;
extern unsigned short *LineBuf, *ScObAdr, *OldLine;
extern unsigned short LineAtr;

extern short CurLine, CurScan;
extern short ScrlObj, SmScNsl;
extern short PdScDnF, PdScUpF;
extern short ctcsw, submenu;
extern short stccol, stcrow;
extern short cxval;

extern unsigned short cg3[];

extern char TheBuf[];		/* display build buffer */

short curslin;			/* current sequence line */

/* 
*/

char sqhead[] =
  "\260 Lin Time  Action 1    Action 2    Action 3    \260 No Seq Rg T \260";

char *sqmcon[] = {

  "\260 Key transient  Trigger on   Stop         Set reg   Value    \260",
  "\260 Key closure    Trigger off  Jump to lin  Inc Reg   Register \260",
  "\260 Key release    Trig toggle               If reg =  Voltage  \260",
  "\260 If key active  If trig act  If stim act  If reg <  Random   \260",
  "\260                                          If reg >           \260"
};

short seqpal[16][3] = {		/* sequence display palette */

  {0, 0, 0},			/*  0 - black */
  {2, 3, 3},			/*  1 - white */
  {0, 0, 1},			/*  2 - dark blue #1 */
  {2, 0, 2},			/*  3 - dark violet */
  {0, 0, 3},			/*  4 - dark blue #2 */
  {3, 0, 0},			/*  5 - red */
  {3, 3, 0},			/*  6 - yellow */
  {1, 2, 0},			/*  7 - dark green */
  {0, 3, 3},			/*  8 - light blue */
  {2, 2, 2},			/*  9 - gray */
  {0, 3, 0},			/* 10 - light green */
  {3, 1, 2},			/* 11 - light violet */
  {0, 2, 3},			/* 12 - medium blue */
  {2, 0, 0},			/* 13 - dark red */
  {1, 1, 2},			/* 14 - electric purple */
  {3, 3, 3}			/* 15 - bright white */
};

/* 
*/

short sqatype[] = {		/* sequence action types by box number */

  SQ_NULL,			/*      "           "   0 */
  SQ_TKEY,			/*      "Kt 001 1 01"   1 */
  SQ_CKEY,			/*      "Kc 001 1 01"   2 */
  SQ_RKEY,			/*      "Kr 001 1 01"   3 */
  SQ_IKEY,			/*      "If 001 1 01"   4 */
  SQ_STRG,			/*      "Trig on  01"   5 */
  SQ_CTRG,			/*      "Trig off 01"   6 */
  SQ_TTRG,			/*      "Trig tgl 01"   7 */
  SQ_ITRG,			/*      "If trig act"   8 */
  SQ_STOP,			/*      "Stop       "   9 */
  SQ_JUMP,			/*      "Jump to 000"   10 */
  SQ_NULL,			/*      "???????????"   11 */
  SQ_ISTM,			/*      "If stim act"   12 */
  SQ_SREG,			/*      "Set R01=00 "   13 */
  SQ_AREG,			/*      "Inc R01+00 "   14 */
  SQ_IREQ,			/*      "If  R01=00 "   15 */
  SQ_IRLT,			/*      "If  R01<00 "   16 */
  SQ_IRGT			/*      "If  R01>00 "   17 */
};

/* 
*/

char *sqdsptb[] = {		/* sequence action display strings by action */

  "           ",		/* SQ_NULL      0x0000  null action */
  "Kc 001 1 01",		/* SQ_CKEY      0x0001  Key closure */
  "Kr 001 1 01",		/* SQ_RKEY      0x0002  Key release */
  "Kt 001 1 01",		/* SQ_TKEY      0x0003  Key transient */
  "If 001 1 01",		/* SQ_IKEY      0x0004  If key active */
  "Trig on  01",		/* SQ_STRG      0x0005  Trigger on */
  "Trig off 01",		/* SQ_CTRG      0x0006  Trigger off */
  "Trig tgl 01",		/* SQ_TTRG      0x0007  Trigger toggle */
  "If trig  01",		/* SQ_ITRG      0x0008  If trigger active */
  "Set R01=00 ",		/* SQ_SREG      0x0009  Set register */
  "If  R01=00 ",		/* SQ_IREQ      0x000A  If register = */
  "If  R01<00 ",		/* SQ_IRLT      0x000B  If register < */
  "If  R01>00 ",		/* SQ_IRGT      0x000C  If register > */
  "If stim act",		/* SQ_ISTM      0x000D  If stimulus active */
  "Jump to 000",		/* SQ_JUMP      0x000E  Jump to sequence line */
  "Stop       ",		/* SQ_STOP      0x000F  Stop sequence */
  "Inc R01+00 "			/* SQ_AREG      0x0010  Increment register */
};

/* 
*/

/*
   =============================================================================
	initsq() -- initialize the sequence data structures
   =============================================================================
*/

initsq ()
{
  register short i;

  for (i = 0; i < 16; i++)
    {

      seqflag[i] = 0;
      seqline[i] = 0;
      sregval[i] = 0;
      trstate[i] = 0;
      seqstim[i] = 0;
      seqtime[i] = 0;
    }

  memsetw (&seqtab, 0, (NSEQW * NSLINES));
}

/* 
*/

/*
   =============================================================================
	dsact() -- convert sequence action code and data to display format
   =============================================================================
*/

dsact (buf, act, dat)
     char *buf;
     unsigned short act, dat;
{
  unsigned short sqa, sqd, sqf, sqr, sqt, t1, t2, val;
  unsigned short chan, i, key, port;

  sqa = SQ_MACT & act;

  strcpy (buf, sqdsptb[sqa]);

  switch (sqa)
    {

    case SQ_CKEY:		/* Key closure */
    case SQ_RKEY:		/* Key release */
    case SQ_TKEY:		/* Key transient */
    case SQ_IKEY:		/* If key active */

      port = 0x0003 & (dat >> 11);
      chan = 0x000F & (dat >> 7);
      key = 0x007F & dat;

      if (port == 2)
	sprintf (&buf[3], "%03u L   ", 1 + key);
      else
	sprintf (&buf[3], "%03u %u %02u", 1 + key, 1 + port, 1 + chan);

      break;

    case SQ_STRG:		/* Trigger on */
    case SQ_CTRG:		/* Trigger off */
    case SQ_TTRG:		/* Trigger toggle */
    case SQ_ITRG:		/* If trigger active */

      sprintf (&buf[9], "%02u", 1 + dat);
      break;
/* 
*/
    case SQ_AREG:		/* Increment register */

      sqf = (dat & SQ_MFLG) ? '-' : '+';
      goto doval;

    case SQ_SREG:		/* Set register */
    case SQ_IREQ:		/* If register = */

      sqf = '=';
      goto doval;

    case SQ_IRLT:		/* If register < */

      sqf = '<';
      goto doval;

    case SQ_IRGT:		/* If register > */

      sqf = '>';

    doval:

      sqr = 1 + ((SQ_MOBJ & act) >> 8);
      sqd = SQ_MVAL & dat;
      sqt = SQ_MTYP & dat;

      switch (sqt)
	{

	case SQ_REG:		/* register */

	  sprintf (&buf[4], "R%02u%cR%02u", sqr, sqf, 1 + sqd);
	  break;

	case SQ_VAL:		/* value */

	  sprintf (&buf[4], "R%02u%c%02u ", sqr, sqf, sqd);
	  break;

	case SQ_VLT:		/* voltage */

	  sprintf (&buf[4], "R%02u%cV%u ", sqr, sqf, 1 + sqd);
	  break;

	case SQ_RND:		/* random */

	  sprintf (&buf[4], "R%02u%c?%u ", sqr, sqf, sqd);
	  break;
	}

      break;
/* 
*/
    case SQ_JUMP:		/* Jump to sequence line */

      sprintf (&buf[8], "%03u", dat);
      break;

    case SQ_NULL:		/* null action */
    case SQ_ISTM:		/* If stimulus active */
    case SQ_STOP:		/* Stop sequence */

      break;
    }

  for (i = 0; i < 12; i++)
    if (buf[i] == '\0')
      buf[i] = ' ';
}

/* 
*/

/*
   =============================================================================
	dsqlin() -- convert a sequence line to display format
   =============================================================================
*/

dsqlin (buf, slin)
     char *buf;
     short slin;
{
  register struct seqent *sp;
  register short i, t1, t2;

  sp = &seqtab[slin];

  t1 = sp->seqtime / 100;
  t2 = sp->seqtime - (t1 * 100);

  sprintf (buf, "  %03u %02u.%02u", slin, t1, t2);

  dsact (&buf[12], sp->seqact1, sp->seqdat1);
  dsact (&buf[24], sp->seqact2, sp->seqdat2);
  dsact (&buf[36], sp->seqact3, sp->seqdat3);

  buf[0] = '\260';

  for (i = 0; i < 48; i++)
    if (buf[i] == '\0')
      buf[i] = ' ';

  buf[48] = '\0';
}

/* 
*/

/*
   =============================================================================
	dcursq() -- display current sequence line
   =============================================================================
*/

dcursq ()
{
  dsqlin (TheBuf, curslin);
  UpdVid (7, 0, TheBuf, PTEATR);
  ctcon ();
}

/*
   =============================================================================
	dstw() -- display sequence table window around current sequence line
   =============================================================================
*/

dstw ()
{
  register short slin, srow;

  slin = curslin - 7;

  if (slin < 0)
    slin += NSLINES;

  for (srow = 0; srow < 16; srow++)
    {

      dsqlin (TheBuf, slin);
      UpdVid (srow, 0, TheBuf, (srow == 7) ? PTEATR : PTPATR);

      if (++slin >= NSLINES)
	slin -= NSLINES;
    }

  seq2buf ();
  ctcon ();
}

/* 
*/

/*
   =============================================================================
	sqwin() -- fill in a sequence display window
   =============================================================================
*/

sqwin (n)
     short n;
{
  register short i, j;
  short atrbuf[64];
  char linbuf[66];

  if (v_regs[5] & 0x0180)
    vbank (0);

  switch (n)
    {

    case 0:			/* headings and box outline */

      /* row 0 */

      memset (linbuf, '\261', 63);
      linbuf[0] = '\272';
      linbuf[48] = '\267';
      linbuf[62] = '\273';
      linbuf[63] = '\0';

      memsetw (atrbuf, PTBATR, 63);
      atrbuf[63] = 0x0000;

      vputsa (obj8, 0, 0, linbuf, atrbuf);

      /* row 1 */

      memsetw (atrbuf, PTHATR + 0x0100, 64);
      atrbuf[0] = PTBATR;
      atrbuf[1] = PTHATR;
      atrbuf[5] = PTHATR;
      atrbuf[11] = PTHATR;
      atrbuf[23] = PTHATR;
      atrbuf[35] = PTHATR;
      atrbuf[47] = PTHATR;
      atrbuf[48] = PTBATR;
      atrbuf[49] = PTHATR;
      atrbuf[52] = PTHATR;
      atrbuf[56] = PTHATR;
      atrbuf[59] = PTHATR;
      atrbuf[61] = PTHATR;
      atrbuf[62] = PTBATR;
      atrbuf[63] = 0x0000;

      vputsa (obj8, 1, 0, sqhead, atrbuf);
/* 
*/
      /* row 18 */

      memset (linbuf, '\261', 63);
      linbuf[0] = '\266';
      linbuf[48] = '\265';
      linbuf[62] = '\264';
      linbuf[63] = '\0';

      memsetw (atrbuf, PTBATR, 63);
      atrbuf[63] = 0x0000;

      vputsa (obj11, 0, 0, linbuf, atrbuf);

      /* row 24 */

      memset (linbuf, '\261', 63);
      linbuf[0] = '\271';
      linbuf[62] = '\270';
      linbuf[63] = '\0';

      memsetw (atrbuf, PTBATR, 63);
      atrbuf[63] = 0x0000;

      vputsa (obj11, 6, 0, linbuf, atrbuf);

      break;
/* 
*/
    case 1:			/* sequences */

      SetDTop (0, 13);
      dstw ();
      break;

    case 2:			/* sequence status */

      for (i = 0; i < 16; i++)
	{

	  sprintf (linbuf, "    %03d %02d %d ",
		   seqline[i], sregval[i], trstate[i]);

	  vvputsv (obj10, 16, PDBORFG, PDSEQBG, i, 0, "\260", 14, 14, cg3);

	  vvputsv (obj10, 16, PDSEQFG, PDSEQBG, i, 1, linbuf, 14, 14, cg3);

	  vvputsv (obj10, 16, PDBORFG, PDSEQBG, i, 14, "\260", 14, 14, cg3);

	  if (i == 7)
	    {

	      if (48 == XTOC (cxval))
		{

		  vsplot4 (obj10, 16, PDPTRFG, i, 0, "\277", 14, 14, cg3);

		}
	      else
		{

		  vsplot4 (obj10, 16, PDPTRFG, i, 0, "\274", 14, 14, cg3);
		}
	    }

	  sprintf (linbuf, "%02d", i + 1);

	  vvputsv (obj10, 16,
		   (seqflag[i] & SQF_RUN) ? PDSEQRN : PDSEQFG, PDSEQBG,
		   i, 2, linbuf, 14, 14, cg3);
	}

      break;

    case 3:			/* menu */

      memsetw (atrbuf, PTMATR, 64);
      atrbuf[0] = PTBATR;
      atrbuf[62] = PTBATR;
      atrbuf[63] = 0x0000;

      for (i = 0; i < 5; i++)
	vputsa (obj11, (i + 1), 0, sqmcon[i], atrbuf);

      break;
    }
}

/* 
*/

/*
   =============================================================================
	SqBakLn() -- return the next sequence line in the backward direction
   =============================================================================
*/

char *
SqBakLn ()
{
  register short slin;

  if (--curslin < 0)
    curslin += NSLINES;

  slin = curslin - 7;

  if (slin < 0)
    slin += NSLINES;

  dsqlin (TheBuf, slin);
  return (TheBuf);
}

/* 
*/

/*
   =============================================================================
	SqFwdLn() -- return the next sequence line in the forward direction
   =============================================================================
*/

char *
SqFwdLn ()
{
  register short slin;

  if (++curslin >= NSLINES)
    curslin -= NSLINES;

  slin = curslin + 8;

  if (slin >= NSLINES)
    slin -= NSLINES;

  dsqlin (TheBuf, slin);
  return (TheBuf);
}

/*
   =============================================================================
	sqwins() -- draw the sequence display
   =============================================================================
*/

sqwins ()
{
  register short i;

  for (i = 0; i < 4; i++)
    sqwin (i);
}

/* 
*/

/*
   =============================================================================
	sqdisp() -- setup the sequence display
   =============================================================================
*/

sqdisp ()
{
#if	DEBUGSQ
  if (debugsw && debugsq)
    printf ("sqdisp(): ENTRY\n");
#endif

  dswap ();			/* clear the video display */

  BakLine = SqBakLn;
  FwdLine = SqFwdLn;

  /* setup object pointers */

  obj8 = &v_score[OB08LOC];	/*  8 - headings */
  obj9 = &v_score[OB09LOC];	/*  9 - sequences */
  obj10 = &v_score[OB10LOC];	/* 10 - status */
  obj11 = &v_score[OB11LOC];	/* 11 - menu */

  ScrlObj = 9;
  ScObAdr = obj9;
  LineBuf = obj9;
  OldLine = (unsigned short *) NULL;
  LineAtr = PTPATR;

  CurLine = 0;
  CurScan = 13;

  PdScDnF = FALSE;
  PdScUpF = FALSE;

  /* ---------------- initialize object table -------------------- */
  /*     obj,  typ, bnk,  base, xpix, ypix,  x0,  y0,   flags, pri */

  SetObj (8, 1, 0, obj8, 512, 28, 0, 0, PDFL_08, -1);
  SetObj (9, 1, 0, obj9, 384, 224, 0, 28, PDFL_09, -1);
  SetObj (10, 0, 0, obj10, 128, 224, 384, 28, PDFL_10, -1);
  SetObj (11, 1, 0, obj11, 512, 98, 0, 252, PDFL_11, -1);

  if (v_regs[5] & 0x0180)
    vbank (0);

  memsetw (v_score, 0, 32767);	/* clear display objects */
  memsetw (v_score + 32767L, 0, 24577);

  sqwins ();			/* fill up the windows */

/* 
*/
  /* display some objects */

  SetPri (8, 8);		/* headings */
  SetPri (9, 9);		/* sequences */
  SetPri (10, 10);		/* sequence status */
  SetPri (11, 11);		/* menu */

  if (v_regs[5] & 0x0180)	/* select bank 0 */
    vbank (0);

  memcpyw (v_cgtab, cg3, 3584);	/* setup character generator */
  v_regs[1] = (v_regs[1] & 0x0FFF) | 0xE000;

  /* fix the initial scan line specs in the object descriptor table */

  v_odtab[8][0] = (v_odtab[8][0] & 0x0FFF) | 0xD000;
  v_odtab[9][0] = (v_odtab[9][0] & 0x0FFF) | 0xD000;

  v_odtab[11][0] = (v_odtab[11][0] & 0x0FFF) | 0xD000;

  submenu = FALSE;

  ctcsw = TRUE;			/* enable cursor */
  ctcpos (DATAROW, 2);		/* set initial cursor */
  postcm ();			/* set initial submenu */

  vsndpal (seqpal);		/* set the palette */

#if	DEBUGSQ
  if (debugsw && debugsq)
    printf ("sqdisp():  EXIT\n");
#endif

}
