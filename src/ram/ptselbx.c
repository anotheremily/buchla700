/*
   =============================================================================
	ptselbx.c -- MIDAS-VII Patch editor box selection functions
	Version 45 -- 1988-12-08 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0
#define	SNAPDUMP	0

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "graphdef.h"
#include "patch.h"
#include "macros.h"
#include "smdefs.h"
#include "curpak.h"

#include "midas.h"
#include "ptdisp.h"

#if	SNAPDUMP
short snapit = 0;
#endif

#if	(SNAPDUMP|DEBUGIT)
extern short debugsw;

short debug01 = 1;
#endif

#define	HT_0	0x01		/* definer, stimulus */
#define	HT_1	0x02		/* source */
#define	HT_2	0x04		/* destination */
#define	HT_3	0x08		/* oscillator */
#define	HT_4	0x10		/* level, index, freq, location, filter */
#define	HT_5	0x20		/* filq, dynamc, vout, depth, rate, inten, aux */
#define	HT_OFF	0x00		/* no submenu highlighted */

/* 
*/

extern short enterit (), nokey (), cnvp2c (), dec2fr ();

extern unsigned short exp_c ();

extern short CurLine;

extern short cents;
extern short cmtype;
extern short cxval;
extern short cyval;
extern short hitcx;
extern short hitcy;
extern short stccol;
extern short stcrow;
extern short submenu;
extern short vtccol;
extern short vtcrow;

extern short ndvals[];

extern unsigned short *obj11;

extern struct selbox *csbp;
extern struct selbox *curboxp;

extern char *smdata[];
extern char *ssdata[];
extern char *stdata[];
extern char *trdata[];

short ptfnbox ();

/* 
*/

short ptegood;			/* pte buffer -- entire entry valid flag */

short ptedfok;			/* pte buffer -- def valid flag */
short ptestok;			/* pte buffer -- stm valid flag */
short ptedsok;			/* pte buffer -- destination valid flag */
short ptedtok;			/* pte buffer -- datum valid flag */

short ptedef;			/* pte buffer -- definer number */
short ptestm;			/* pte buffer -- stimulus number */
short ptespec;			/* pte buffer -- address type */
short ptesuba;			/* pte buffer -- sub-address */
short ptedat1;			/* pte buffer -- data word 1 */
short ptedat2;			/* pte buffer -- data word 2 */

short ptedftp;			/* definer / stimulus entry flag */

short ptedfst;			/* current definer/stimulus data entry format */
short ptedest;			/* current destination data entry format */
short ptedata;			/* current datum data entry format */

short ptemenu;			/* current submenu highlighting */

short pteset;			/* pte buffer setup flag */
short ptbflag;			/* pdbuf setup flag */

short ptecpos;			/* current patch entry index */
short ptepred;			/* predecessor entry index */
short ptesucc;			/* successor entry index */

struct patch ptebuf;		/* pte buffer -- patch table format */

char ptdebuf[50];		/* patch edit buffer */

/* 
*/

char *rawdfst[] = {		/* def/stm field initial contents */

  "Key 001 1 01",
  "Rel 001 1 01",
  "Trig 01     ",
  "Pulse 1     "
};

char *rawdest[] = {		/* destination field initial contents */

  "??????? 00 ??",		/* ??????       00 */
  "K 001 1 01   ",		/* PA_KEY       01 */
  "Trigger 01   ",		/* PA_TRG       02 */
  "Pulse 1      ",		/* PA_PLS       03 */
  "LED A        ",		/* PA_LED       04 */
  "Seq 01 Lin   ",		/* PA_SLIN      05 */
  "Seq 01 Ctl   ",		/* PA_SCTL      06 */
  "Tune Table   ",		/* PA_TUNE      07 */
  "Reg 01 Set   ",		/* PA_RSET      08 */
  "Reg 01 Add   ",		/* PA_RADD      09 */
  "Instrument V1",		/* PA_INST      10 */
  "Osc 1 Int  V1",		/* PA_OSC       11 */
  "WaveshapeA V1",		/* PA_WAVA      12 */
  "WaveshapeB V1",		/* PA_WAVB      13 */
  "Configure  V1",		/* PA_CNFG      14 */
  "Level Srce V1",		/* PA_LEVL      15 */
  "Ind 1 Srce V1",		/* PA_INDX      16 */
  "Frq 1 Srce V1",		/* PA_FREQ      17 */
  "Filtr Srce V1",		/* PA_FILT      18 */
  "Fil Q Srce V1",		/* PA_FILQ      19 */
  "Loctn Srce V1",		/* PA_LOCN      20 */
  "Dynmc Srce V1",		/* PA_DYNM      21 */
  "Aux   Srce   ",		/* PA_AUX       22 */
  "Rate  Srce   ",		/* PA_RATE      23 */
  "Inten Srce   ",		/* PA_INTN      24 */
  "Depth Srce   ",		/* PA_DPTH      25 */
  "Vlt 1 Srce   "		/* PA_VOUT      26 */
};

/* 
*/

char *rawmode[] = { "Frq ", "Pch ", "Int ", "Rat " };	/* oscillator modes */
char *rawmdat[] = { "00.1 ", "0C 00", "+0000", "1/1  " };	/* oscillator data */

short omtabl[] = { PSO_FRQ, PSO_PCH, PSO_INT, PSO_RAT };	/* oscillator modes */
short omdtab[] = { 2, 320, 0, 0 };	/* oscillator data */
short omftab[] = { 6, 7, 4, 5 };	/* mode data format */

char *rawfpud[] = { "PchWh", "+0.00", "00001", "\2400.00", "Start" };

short fpudtab[] = { SM_HTPW, 0, 1, 0, 1 };
short fpuetab[] = { 11, 1, 2, 3, 9 };

short dfsttab[] = { 0x0000, 0x8000, 0x1100, 0x1180 };

short defcols[] = { 2, 6, 7, 8 };	/* def data entry columns */
short stmcols[] = { 15, 19, 20, 21 };	/* stm data entry columns */

short dstcols[] = {		/* destination data entry columns */

/*	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10	*/
  28, 30, 36, 34, 32, 32, 42, 39, 34, 32, 32, 34
};

short dfsttp[] = { 1, 1, 2, 3 };	/* def/stm formats */

short datasrc[] = {

  SM_HTPW, SM_VTMW, SM_LPBR, SM_PED1, SM_CTL1,
  SM_PTCH, SM_FREQ, SM_KVEL, SM_KPRS, SM_RAND
};

/* 
*/

short destype[] = {		/* destination type table */

  PA_KEY, -1, PA_TRG, PA_PLS, PA_LED,
  PA_SLIN, PA_SCTL, PA_TUNE, PA_RSET, PA_RADD,
  PA_INST, PA_OSC, PA_WAVA, PA_WAVB, PA_CNFG,
  PA_INDX, PA_FREQ, -1, -1, -1,
  PA_LEVL, PA_LOCN, PA_FILT, PA_FILQ, PA_DYNM,
  PA_VOUT, PA_DPTH, PA_RATE, PA_INTN, PA_AUX,
  -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1
};

char destfmt[] = {		/* destination data entry format table */

  0, -1, 1, 2, 3,		/*  1 ..  5 */
  4, 4, 5, 4, 4,		/*  6 .. 10 */
  6, 9, 6, 6, 6,		/* 11 .. 15 */
  9, 9, -1, -1, -1,		/* 16 .. 20 */
  10, 10, 10, 10, 10,		/* 21 .. 25 */
  8, 7, 7, 7, 7			/* 26 .. 30 */
};

char datafmt[] = {		/* datum data entry format table */

  8, -1, 9, 8, 14,
  13, 9, 18, 12, 12,
  15, 4, 16, 16, 17,
  11, 11, -1, -1, -1,
  11, 11, 11, 11, 11,
  11, 11, 11, 11, 11
};

/* 
*/

char desdatf[] = {		/* data entry default flags */

  2, 0, 2, 2, 1,
  1, 2, 1, 3, 3,
  1, 0, 1, 1, 1,
  2, 2, 0, 0, 0,
  2, 2, 2, 2, 2,
  2, 2, 2, 2, 2
};

short desdat1[] = {		/* data word 1 defaults */

  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0
};

short desdat2[] = {		/* data word 2 defaults */

  0, 0, 1, 0, 0,
  0, 1, 0, 0, 1,
  0, 0, 0, 0, 0,
  SM_HTPW, SM_HTPW, 0, 0, 0,
  SM_HTPW, SM_HTPW, SM_HTPW, SM_HTPW, SM_HTPW,
  SM_HTPW, SM_HTPW, SM_HTPW, SM_HTPW, SM_HTPW
};

/* 
*/

char *rawdata[] = {

  "Trans", "     ", "Start", "Trans", "000  ",
  "000  ", "Start", "0    ", "+00  ", "+01  ",
  "00   ", "+0000", "01   ", "01   ", "00   ",
  "PchWh", "PchWh", "     ", "     ", "     ",
  "PchWh", "PchWh", "PchWh", "PchWh", "PchWh",
  "PchWh", "PchWh", "PchWh", "PchWh", "PchWh"
};

char hilitab[][8] = {		/* submenu highlight table */

  /* start, width, row1, row2, row3, row4, row5, pad */

  {2, 3, HT_0 | HT_2, HT_0, HT_0 | HT_2, HT_0 | HT_2, HT_2, 0},
  {7, 6, HT_2, HT_2, HT_2, HT_2, HT_2, 0},
  {15, 5, HT_2, HT_2, HT_2, HT_2, HT_2, 0},
  {22, 5, HT_2, HT_2 | HT_3, HT_3, HT_3, HT_3, 0},
  {29, 5, HT_2, HT_2, HT_2, HT_2, HT_2, 0},
  {36, 5, HT_2, HT_2, HT_2, HT_2, HT_2, 0},
  {43, 4, HT_4 | HT_5, HT_4 | HT_5, HT_4 | HT_5, HT_4 | HT_5, 0, 0},
  {49, 5, HT_1, HT_1, HT_1, HT_1, HT_1, 0},
  {56, 5, HT_1, HT_1, HT_1, HT_1, HT_1, 0}
};

struct selbox ptboxes[] = {	/* selection boxes */

  {CTOX (1), RTOY (DATAROW), CTOX (48) - 1, RTOY (1 + DATAROW) - 1, 0, ptfnbox},	/* current patch */

  {0, 0, 0, 0, 0, FN_NULL}
};

/* 
*/

/*
   =============================================================================
	hilitpm() -- highlight patch submenu
   =============================================================================
*/

hilitpm (n)
     register char n;
{
  register short chr, h, v, w;

  ptemenu = n;			/* set current submenu type */

  for (h = 0; h < 9; h++)
    {				/* scan the columns */

      for (v = 0; v < 5; v++)
	{			/* scan the rows */

	  chr = hilitab[h][0];	/* starting character */

	  for (w = hilitab[h][1]; w-- > 0;)
	    vsetcv (obj11, v + 1, chr++,
		    (hilitab[h][v + 2] & n) ? PTIATR : PTMATR, 64);
	}
    }
}

/* 
*/

/*
   =============================================================================
	aftercm() -- determine state after cursor motion
   =============================================================================
*/

aftercm ()
{
  switch (stccol)
    {				/* switch based on current cursor column */

    case 2:			/* definer */
    case 3:
    case 4:
      hilitpm (HT_0);
      break;

    case 15:			/* stimulus */
    case 16:
    case 17:

      hilitpm (HT_0);
      break;

    case 28:			/* destination */

      hilitpm (HT_2);
      break;

/* 
*/
    case 34:			/* may be a destination sub-address */
    case 35:
    case 36:
    case 37:

      if (ptedsok)
	{			/* if destination is good ... */

	  switch (PE_SPEC & ptespec)
	    {

	    case PA_OSC:

	      hilitpm (HT_3);
	      break;

	    case PA_LEVL:
	    case PA_INDX:
	    case PA_FREQ:
	    case PA_LOCN:
	    case PA_FILT:

	      hilitpm (HT_4);
	      break;

	    case PA_FILQ:
	    case PA_DYNM:
	    case PA_VOUT:
	    case PA_DPTH:
	    case PA_RATE:
	    case PA_INTN:
	    case PA_AUX:

	      hilitpm (HT_5);
	      break;

	    default:

	      hilitpm (HT_OFF);
	      break;
	    }

	}
      else
	{

	  hilitpm (HT_OFF);
	}

      break;
/* 
*/
    case 42:			/* datum */

      if (ptedtok)
	{			/* if datum is valid ... */

	  switch (PE_SPEC & ptespec)
	    {

	    case PA_LEVL:
	    case PA_INDX:
	    case PA_FREQ:
	    case PA_LOCN:
	    case PA_FILT:
	    case PA_FILQ:
	    case PA_DYNM:
	    case PA_VOUT:
	    case PA_DPTH:
	    case PA_RATE:
	    case PA_INTN:
	    case PA_AUX:

	      if (ptedat1 == PSA_SRC)
		hilitpm (HT_1);
	      else
		hilitpm (HT_OFF);

	      break;

	    default:

	      hilitpm (HT_OFF);
	      break;
	    }

	}
      else
	{

	  hilitpm (HT_OFF);
	}

      break;

    default:			/* not in a starting place */

      hilitpm (HT_OFF);
      break;
    }
}

/* 
*/

/*
   =============================================================================
	movectc() -- move cursor and reset highlighting
   =============================================================================
*/

movectc (r, c)
     short r, c;
{
  ctcpos (r, c);
  aftercm ();
}

/*
   =============================================================================
	endpsm() -- end patch submenu data entry
   =============================================================================
*/

endpsm (row, col)
     short row, col;
{
  submenu = FALSE;
  cmtype = CT_SMTH;

  ptegood = ptedfok && ptestok && ptedsok && ptedtok;

  mtcoff ();
  movectc (row, col);

  srdspte ();
}

/* 
*/

/*
   =============================================================================
	edefstm() -- enter a def or a stm field
   =============================================================================
*/

edefstm (du, n)
     short du, n;
{
  short chan, port, stim;
  register short i, left, right;
  register long ltemp;

#if	DEBUGIT
  if (debugsw && snapit)
    printf ("edefstm(%d):  pte stok=%d  dfok=%d\n", n, ptestok, ptedfok);
#endif

  if (n)
    {

      if (! ptestok)
	{

	  movectc (DATAROW, 15);
	  return;
	}

    }
  else
    {

      if (! ptedfok)
	{

	  movectc (DATAROW, 2);
	  return;
	}
    }

  port = ((n ? ptestm : ptedef) >> 11) & 0x0003;
  chan = ((n ? ptestm : ptedef) >> 7) & 0x000F;
  stim = (n ? ptestm : ptedef) & 0x007F;

/* 
*/

  if ((port == 0) || (port == 1) || ((port == 2) && (chan < 2)))
    {				/* key / rel */

      ltemp = 0;		/* stimulus number */
      left = n ? 19 : 6;
      right = n ? 22 : 9;

      for (i = left; i < right; i++)
	ltemp = (ltemp * 10) + ptdebuf[i] - '0';

      if ((ltemp == 0) || (ltemp > 128L))
	{

	  movectc (DATAROW, left);
	  return;
	}

      stim = ltemp - 1;

      if (port == 2)
	{

	  chan = 0;

	}
      else
	{

	  ltemp = 0;		/* channel */
	  left = n ? 25 : 12;
	  right = n ? 27 : 14;

	  for (i = left; i < right; i++)
	    ltemp = (ltemp * 10) + ptdebuf[i] - '0';

	  if ((ltemp == 0) || (ltemp > 16L))
	    {

	      movectc (DATAROW, left);
	      return;
	    }

	  chan = ltemp - 1;
	}

      if (n)
	ptestm = (ptestm & 0x8000) | (port << 11) | (chan << 7) | stim;
      else
	ptedef = (ptedef & 0x8000) | (port << 11) | (chan << 7) | stim;
/* 
*/
    }
  else if ((port == 2) && (chan == 2))
    {				/* Trig */

      ltemp = 0;
      left = n ? 20 : 7;
      right = n ? 22 : 9;

      for (i = left; i < right; i++)
	ltemp = (ltemp * 10) + ptdebuf[i] - '0';

      if ((ltemp == 0) || (ltemp > 16L))
	{

	  movectc (DATAROW, left);
	  return;
	}

      stim = ltemp - 1;

      if (n)
	ptestm = 0x1100 | stim;
      else
	ptedef = 0x1100 | stim;
/* 
*/
    }
  else if ((port == 2) && (chan == 3))
    {				/* Pulse */

      left = n ? 21 : 8;
      ltemp = ptdebuf[left] - '0';

      if ((ltemp == 0) || (ltemp > 2L))
	{

	  movectc (DATAROW, left);
	  return;
	}

      stim = ltemp - 1;

      if (n)
	ptestm = 0x1180 | stim;
      else
	ptedef = 0x1180 | stim;

    }
  else
    {

      movectc (DATAROW, n ? 15 : 2);
      return;
    }

  if (n)
    ptestok = TRUE;
  else
    ptedfok = TRUE;

#if	SNAPDUMP
  if (debugsw && snapit)
    SnapPTV ("edefstm");
#endif

  if (du)
    {				/* if display is to be updated ... */

      srdspte ();
      movectc (DATAROW, n ? 28 : 15);
    }
}

/* 
*/

/*
   =============================================================================
	ptdest() -- enter patch destination field
   =============================================================================
*/

ptdest (du)
     short du;
{
  register long ltemp;
  register short i, left, right, stim;
  short port, chan;

#if	DEBUGIT
  if (debugsw && debug01)
    printf ("ptdest(%d): entry -- ptedest = %d\n", du, ptedest);
#endif

  switch (ptedest)
    {

    case 0:			/* key */

      ltemp = 0;
      left = 30;
      right = 33;

      for (i = left; i < right; i++)
	ltemp = (ltemp * 10) + ptdebuf[i] - '0';

      if ((ltemp == 0) || (ltemp > 128L))
	{

	  movectc (DATAROW, left);
	  return;
	}

      stim = ltemp - 1;
      ltemp = 0;
      left = 36;
      right = 38;

      for (i = left; i < right; i++)
	ltemp = (ltemp * 10) + ptdebuf[i] - '0';

      if ((ltemp == 0) || (ltemp > 16L))
	{

	  movectc (DATAROW, left);
	  return;
	}

      chan = ltemp - 1;
/* 
*/
      if (ptdebuf[34] == 'L')
	{

	  port = 2;
	  chan = 0;

	}
      else
	{

	  port = ptdebuf[34] - '1';
	}

      ptesuba = (port << 11) | (chan << 7) | stim;
      break;
/* 
*/
    case 1:			/* trigger */

      ltemp = 0;
      left = 36;
      right = 38;

      for (i = left; i < right; i++)
	ltemp = (ltemp * 10) + ptdebuf[i] - '0';

      if ((ltemp == 0) || (ltemp > 16L))
	{

	  movectc (DATAROW, left);
	  return;
	}

      stim = ltemp - 1;
      ptesuba = 0x1100 | stim;
      break;

    case 2:			/* pulse */

      ptesuba = ptdebuf[34] - '1';
      break;

    case 3:			/* led */

      ptesuba = ptdebuf[32] - 'A';
      break;

    case 4:			/* seq / reg */

      ltemp = 0;
      left = 32;
      right = 34;

      for (i = left; i < right; i++)
	ltemp = (ltemp * 10) + ptdebuf[i] - '0';

      if ((ltemp == 0) || (ltemp > 16L))
	{

	  movectc (DATAROW, left);
	  return;
	}

      ptesuba = ltemp - 1;
      break;

/* 
*/
    case 5:			/* tuning */
    case 7:			/* aux, rate, inten, depth */

      break;

    case 6:			/* inst, wave, config */
    case 10:			/* level, filtr, fil Q, loctn, dynmc */

      if (ptdebuf[39] == 'V')
	stim = 0;
      else
	stim = 12;

      i = ptdebuf[40] & 0x00FF;

      if (i >= '\254')
	stim += i - '\242';
      else
	stim += i - '0';

      --stim;
      ptesuba = stim << 8;
      break;

    case 8:			/* vlt */

      ptesuba = ptdebuf[32] - '0';
      break;
/* 
*/
    case 9:			/* osc, ind, frq */

      if (ptdebuf[39] == 'V')
	stim = 0;
      else
	stim = 12;

      i = ptdebuf[40] & 0x00FF;

      if (i >= '\254')
	stim += i - '\242';
      else
	stim += i - '0';

      --stim;
      ptesuba = (stim << 8) | (ptdebuf[32] - '0');
      break;

    default:			/* something weird got in here ... */

      movectc (DATAROW, 28);
      return;
    }

  ptedsok = TRUE;

#if	SNAPDUMP
  if (debugsw && snapit)
    SnapPTV ("ptdest");
#endif

  if (du)
    {

      srdspte ();
      movectc (DATAROW, 42);
    }

#if	DEBUGIT
  if (debugsw && debug01)
    printf ("ptdest(%d): exit\n", du);
#endif
}

/* 
*/

/*
   =============================================================================
	epatch() -- enter patch and reposition cursor to the DEF field
   =============================================================================
*/

epatch ()
{
#if	DEBUGIT
  if (debugsw && debug01)
    printf ("epatch(): entry\n");
#endif

  ptedtok = TRUE;		/* data just got parsed or we wouldn't be here */
  edefstm (0, 0);		/* parse def */
  edefstm (0, 1);		/* parse stm */
  ptdest (0);			/* parse dest */
  entrpte ();			/* enter the patch */
  srdspte ();			/* adjust the display */
  movectc (DATAROW, 2);		/* reposition the cursor */

#if	SNAPDUMP
  if (debugsw && snapit)
    SnapPTV ("epatch");
#endif

#if	DEBUGIT
  if (debugsw && debug01)
    printf ("epatch(): exit\n");
#endif
}

/* 
*/

/*
   =============================================================================
	badpdat() -- position cursor at bad data field
   =============================================================================
*/

badpdat ()
{
  ptedtok = FALSE;
  movectc (DATAROW, 42);
}

/*
   =============================================================================
	setsubm() -- setup submenu
   =============================================================================
*/

setsubm (r, c)
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
	ptfnbox() -- patch display box hit processor
   =============================================================================
*/

short
ptfnbox (n)
     short n;
{
  register short i, box, ptd, ptt;
  register long ltemp;

  if (! submenu)
    {				/* PATCH DATA ENTRY LINE */

      if (inrange (stccol, 2, 13))
	{			/* DEFINER */

	  ptedftp = 1;		/* def */

	  if (inrange (stccol, 2, 4))
	    {			/* select */

	      setsubm (19, 2);

	    }
	  else
	    {			/* enter */

	      edefstm (0, 1);
	      ptdest (0);
	      edefstm (1, 0);
	    }

	  return (SUCCESS);

	}
      else if (inrange (stccol, 15, 26))
	{			/* STIMULUS */

	  ptedftp = 2;		/* stm */

	  if (inrange (stccol, 15, 17))
	    {			/* select */

	      setsubm (19, 2);

	    }
	  else
	    {			/* enter */

	      edefstm (0, 0);
	      ptdest (0);
	      edefstm (1, 1);
	    }

	  return (SUCCESS);
/* 
*/
	}
      else if (inrange (stccol, 28, 40))
	{			/* DESTINATION */

	  ptedftp = 0;		/* not def/stm */

	  if (stccol == 28)
	    {			/* select */

	      setsubm (19, 2);

	    }
	  else if (ptemenu == HT_3)
	    {

	      setsubm (20, 22);

	    }
	  else if ((ptemenu == HT_4) || (ptemenu == HT_5))
	    {

	      setsubm (19, 43);

	    }
	  else
	    {			/* enter */

	      edefstm (0, 0);
	      edefstm (0, 1);
	      ptdest (1);
	    }

	  return (SUCCESS);
/* 
*/
	}
      else if (inrange (stccol, 42, 46))
	{			/* DATUM */

	  ptedftp = 0;		/* not def/stm */
	  setptcv ();		/* setup control variables */

#if	DEBUGIT
	  if (debugsw && debug01)
	    SnapPTV ("ptfnbox");
#endif
	  switch (ptedata)
	    {			/* switch on Datum format */

	    case 0:		/* -none- */

	      ptedtok = FALSE;
	      return (FAILURE);

	    case 1:		/* multiplier -- +1.00 */

	      ebuf[0] = ptdebuf[43];
	      ebuf[1] = ptdebuf[45];
	      ebuf[2] = ptdebuf[46];
	      ebuf[3] = ptdebuf[42];

	      if (0xFFFF == (i = dec2fr (ebuf)))
		{

		  badpdat ();
		  return (FAILURE);
		}

	      ptedat2 = i;
	      epatch ();	/* enter -- multiplier */
	      return (SUCCESS);
/* 
*/
	    case 2:		/* time -- 32767 */

	      ltemp = 0;

	      for (i = 42; i < 47; i++)
		ltemp = (ltemp * 10) + (ptdebuf[i] - '0');

	      if (ltemp > 32767L)
		{

		  badpdat ();
		  return (FAILURE);
		}

	      ptedat2 = tofpu ((unsigned short) ltemp);
	      epatch ();	/* enter -- time */
	      return (SUCCESS);

	    case 3:		/* value -- +10.00 */

	      i = ptdebuf[42] & 0x00FF;

	      if ((i == '\240') || (i == '\241'))
		ltemp = 1000;
	      else
		ltemp = 0;

	      ltemp += ((ptdebuf[43] - '0') * 100) +
		((ptdebuf[45] - '0') * 10) + (ptdebuf[46] - '0');

	      if (ltemp > 1000L)
		{

		  badpdat ();
		  return (FAILURE);
		}

	      if ((i == '-') || (i == '\241'))
		ltemp = -ltemp;

	      ptedat2 = ltemp << 5;
	      epatch ();	/* enter -- value */
	      return (SUCCESS);

/* 
*/
	    case 4:		/* interval -- +1200 */

	      ltemp = 0;

	      for (i = 43; i < 47; i++)
		ltemp = (ltemp * 10) + (ptdebuf[i] - '0');

	      if (ltemp > 1200L)
		{

		  badpdat ();
		  return (FAILURE);
		}

	      if (ptdebuf[42] == '-')
		ltemp = -ltemp;

	      ptedat2 = ltemp << 1;
	      epatch ();	/* enter -- interval */
	      return (SUCCESS);
/* 
*/
	    case 5:		/* ratio -- 9/9 */

	      ptedat2 = ndvals[ptdebuf[42] - '0'] - ndvals[ptdebuf[44] - '0'];

	      epatch ();	/* enter -- ratio */
	      return (SUCCESS);

	    case 6:		/* frequency -- 15.9 */

	      ltemp = ((ptdebuf[42] - '0') * 100) +
		((ptdebuf[43] - '0') * 10) + (ptdebuf[45] - '0');

	      if (ltemp > 159L)
		{

		  badpdat ();
		  return (FAILURE);
		}

	      ptedat2 = ltemp << 1;
	      epatch ();	/* enter -- frequency */
	      return (SUCCESS);

	    case 7:		/* pitch -- 9C#99 */

	      memcpy (ebuf, &ptdebuf[42], 5);

	      if (FAILURE == cnvp2c ())
		{

		  badpdat ();
		  return (FAILURE);
		}

	      ptedat2 = cents;
	      epatch ();	/* enter - pitch */
	      return (SUCCESS);
/* 
*/
	    case 8:		/* trans/stop/start */
	    case 9:		/* stop/start */
	    case 10:		/* off/on */

	      epatch ();	/* enter -- control */
	      return (SUCCESS);

	    case 11:		/* source */

	      if (stccol == 42)
		setsubm (19, 49);	/* select */
	      else
		epatch ();	/* enter -- source */

	      return (SUCCESS);

	    case 12:		/* register/value -- R16 | +99 */

	      ltemp = ((ptdebuf[43] - '0') * 10) + (ptdebuf[44] - '0');

	      if (ptdebuf[42] == 'R')
		{

		  if ((ltemp == 0) || (ltemp > 16L))
		    {

		      badpdat ();
		      return (FAILURE);
		    }

		  --ltemp;
		  ptedat1 = 1;

		}
	      else
		{

		  ptedat1 = 0;
		}

	      ptedat2 = ltemp;
	      epatch ();	/* enter -- register | value */
	      return (SUCCESS);
/* 
*/
	    case 13:		/* sequence line */

	      ltemp = 0;

	      for (i = 42; i < 45; i++)
		ltemp = (ltemp * 10) + (ptdebuf[i] - '0');

	      ptedat1 = ltemp;
	      epatch ();	/* enter  -- sequence line */
	      return (SUCCESS);

	    case 14:		/* LED controls */

	      ltemp = 0;

	      if (ptesuba & 0x0001)
		{

		  for (i = 42; i < 46; i++)
		    ltemp = (ltemp << 2) | (ptdebuf[i] - '0');

		  ptedat1 = ltemp << 8;

		}
	      else
		{

		  for (i = 42; i < 45; i++)
		    ltemp = (ltemp << 2) | (ptdebuf[i] - '0');

		  ptedat1 = ltemp << 10;
		}

	      epatch ();	/* enter -- LED controls */
	      return (SUCCESS);

/* 
*/
	    case 15:		/* instrument number */

	      ltemp = ((ptdebuf[42] - '0') * 10) + (ptdebuf[43] - '0');

	      if (ltemp > 40L)
		{

		  badpdat ();
		  return (FAILURE);
		}

	      ptedat1 = ltemp;
	      epatch ();	/* enter -- instrument number */
	      return (SUCCESS);

	    case 16:		/* waveshape number */

	      ltemp = ((ptdebuf[42] - '0') * 10) + (ptdebuf[43] - '0');

	      if ((ltemp == 0) || (ltemp > 20L))
		{

		  badpdat ();
		  return (FAILURE);
		}

	      ptedat1 = ltemp;
	      epatch ();	/* enter -- waveshape number */
	      return (SUCCESS);
/* 
*/
	    case 17:		/* configuration number */

	      ltemp = ((ptdebuf[42] - '0') * 10) + (ptdebuf[43] - '0');

	      if (ltemp > 11L)
		{

		  badpdat ();
		  return (FAILURE);
		}

	      ptedat1 = ltemp;
	      epatch ();	/* enter -- configuration number */
	      return (SUCCESS);

	    case 18:		/* tuning table number */

	      ptedat1 = ptdebuf[42] - '0';
	      epatch ();	/* enter -- tuning table number */
	      return (SUCCESS);

	    default:		/* something weird got in here ... */

	      ptedtok = FALSE;
	      return (FAILURE);
	    }
	}

      return (FAILURE);
/* 
*/
    }
  else
    {				/* SUBMENU SELECTION */

      /* determine the "box" we're pointing at */

      if (inrange (vtccol, 2, 4))
	box = vtcrow - 18;
      else if (inrange (vtccol, 7, 12))
	box = vtcrow - 13;
      else if (inrange (vtccol, 15, 19))
	box = vtcrow - 8;
      else if (inrange (vtccol, 22, 26))
	box = vtcrow - 3;
      else if (inrange (vtccol, 29, 33))
	box = vtcrow + 2;
      else if (inrange (vtccol, 36, 40))
	box = vtcrow + 7;
      else if (inrange (vtccol, 43, 46))
	box = vtcrow + 12;
      else if (inrange (vtccol, 49, 53))
	box = vtcrow + 17;
      else if (inrange (vtccol, 56, 60))
	box = vtcrow + 22;
      else
	return (FAILURE);

/* 
*/
      switch (ptemenu)
	{			/* switch on submenu type */

	case HT_0:		/* DEFINERS / STIMULLI */

	  if (inrange (box, 1, 4))
	    {			/* Key, Rel, Trg, Pls */

	      i = box - 1;
	      ptedfst = dfsttp[i];

	      if (ptedftp == 1)
		{		/* def */

		  ptedfok = TRUE;
		  ptbflag = TRUE;

		  ptedef = dfsttab[i];

		  strcpy (&ptdebuf[2], rawdfst[i]);

		  UpdVid (7, 2, rawdfst[i], PTDATR);
		  endpsm (DATAROW, defcols[ptedfst]);

		  return (SUCCESS);

		}
	      else if (ptedftp == 2)
		{		/* stm */

		  ptestok = TRUE;
		  ptbflag = TRUE;

		  ptestm = dfsttab[i];

		  strcpy (&ptdebuf[15], rawdfst[i]);

		  UpdVid (7, 15, rawdfst[i], PTDATR);
		  endpsm (DATAROW, stmcols[ptedfst]);

		  return (SUCCESS);
		}
	    }

	  return (FAILURE);
/* 
*/
	case HT_1:		/* SOURCES */

	  if (inrange (box, 36, 45))
	    {

	      ptedtok = TRUE;
	      ptbflag = TRUE;

	      ptedat2 = datasrc[box - 36];

	      strcpy (&ptdebuf[42], smdata[ptedat2]);

	      UpdVid (7, 42, smdata[ptedat2], PTDATR);
	      endpsm (DATAROW, 46);

	      return (SUCCESS);
	    }

	  return (FAILURE);
/* 
*/
	case HT_2:		/* DESTINATIONS */

	  i = box - 1;
	  ptt = destype[i];

	  if (ptt != - 1)
	    {

	      ptedsok = TRUE;
	      ptedtok = TRUE;
	      ptbflag = TRUE;

	      ptedest = destfmt[i];
	      ptedata = datafmt[i];
	      ptespec = ptt;
	      ptesuba = 0x0000;
	      ptedat1 = 0x0000;
	      ptedat2 = 0x0000;

	      if (ptd = desdatf[i])
		{

		  if (ptd & 1)
		    ptedat1 = desdat1[i];

		  if (ptd & 2)
		    ptedat2 = desdat2[i];
		}

	      strcpy (&ptdebuf[28], rawdest[ptt]);
	      strcpy (&ptdebuf[42], rawdata[i]);

	      UpdVid (7, 28, rawdest[ptt], PTDATR);
	      UpdVid (7, 42, rawdata[i], PTDATR);
	      endpsm (DATAROW, dstcols[1 + ptedest]);

	      return (SUCCESS);
	    }

	  return (FAILURE);
/* 
*/
	case HT_3:		/* OSCILLATOR MODES */

	  if (inrange (box, 17, 20))
	    {

	      i = box - 17;

	      ptedsok = TRUE;
	      ptedtok = TRUE;
	      ptbflag = TRUE;

	      ptedat1 = omtabl[i];
	      ptedat2 = omdtab[i];

	      ptedata = omftab[i];

	      strcpy (&ptdebuf[34], rawmode[i]);
	      strcpy (&ptdebuf[42], rawmdat[i]);

	      UpdVid (7, 34, rawmode[i], PTDATR);
	      UpdVid (7, 42, rawmdat[i], PTDATR);
	      endpsm (DATAROW, 39);

	      return (SUCCESS);
	    }

	  return (FAILURE);
/* 
*/
	case HT_5:		/* FPU OUTPUTS WITHOUT FUNCTIONS */

	  if (! inrange (box, 31, 34))
	    return (FAILURE);

	case HT_4:		/* FPU OUTPUTS WITH FUNCTIONS */

	  if (! inrange (box, 31, 35))
	    return (FAILURE);

	  i = box - 31;

	  ptedsok = TRUE;
	  ptedtok = TRUE;
	  ptbflag = TRUE;

	  ptedat1 = i;
	  ptedat2 = fpudtab[i];

	  ptedata = fpuetab[i];

	  strcpy (&ptdebuf[34], stdata[i]);
	  strcpy (&ptdebuf[42], rawfpud[i]);

	  UpdVid (7, 34, stdata[i], PTDATR);
	  UpdVid (7, 42, rawfpud[i], PTDATR);
	  endpsm (DATAROW, 42);

	  return (SUCCESS);

	default:		/* -BOGUS- */

	  endpsm (stcrow, stccol);

	  return (FAILURE);
	}
    }

  return (FAILURE);
}
