/*
   =============================================================================
	ptdisp.c -- MIDAS-VII Patch editor display functions
	Version 32 -- 1989-11-16 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "patch.h"

#include "midas.h"
#include "ptdisp.h"

extern short LineBak (), LineFwd ();

extern short (*BakLine) (), (*FwdLine) ();

extern unsigned short pt_alc ();

extern unsigned short *obj8, *obj9, *obj10, *obj11;
extern unsigned short *LineBuf, *ScObAdr, *OldLine;
extern unsigned short LineAtr;

extern short CurLine, CurScan;
extern short ScrlObj, SmScNsl;
extern short PdScDnF, PdScUpF;
extern short ctcsw, submenu;
extern short cxval;

extern unsigned short cg3[];

unsigned short seqflag[16];	/* sequence flags */
unsigned short seqline[16];	/* current sequencer line */
unsigned short seqstim[16];	/* sequence stimulus */
unsigned short seqtime[16];	/* sequence timers */
unsigned short sregval[16];	/* register values */
unsigned short trstate[16];	/* trigger states */

struct seqent seqtab[NSLINES];	/* sequence table */

/* 
*/

char pthead[] =
  "\260 Definer p ch Stmulus p ch Destnation \275\276 Datum \260 No Seq Rg T \260";

char *ptmcon[] = {

  "\260 Key  SeqLin  Instr  Index  Level  V Out  Srce  PchWh  Pitch \260",
  "\260 Rel  SeqCtl  Osc    Freq   Loctn  Depth  Mult  ModWh  Freq  \260",
  "\260 Trg  Tuning  WaveA  Pitch  Filtr  Rate   Time  Breth  KeyVl \260",
  "\260 Pls  RegSet  WaveB  Intvl  Fil Q  Inten  Valu  Pedal  KeyPr \260",
  "\260 LED  RegAdd  Confg  Ratio  Dynmc  Aux    Func  GPC/V  Randm \260"
};

short ptabpal[16][3] = {	/* patch display palette */

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

/*
   =============================================================================
	ptwin() -- fill in a patch table display window
   =============================================================================
*/

ptwin (n)
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
      atrbuf[14] = PTHATR;
      atrbuf[27] = PTHATR;
      atrbuf[41] = PTHATR;
      atrbuf[47] = PTHATR;
      atrbuf[48] = PTBATR;
      atrbuf[49] = PTHATR;
      atrbuf[52] = PTHATR;
      atrbuf[56] = PTHATR;
      atrbuf[59] = PTHATR;
      atrbuf[61] = PTHATR;
      atrbuf[62] = PTBATR;
      atrbuf[63] = 0x0000;

      vputsa (obj8, 1, 0, pthead, atrbuf);
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
    case 1:			/* patches */

      SetDTop (0, 13);

      for (i = 0; i < 16; i++)
	{

	  for (j = 0; j < 47; j++)
	    linbuf[j] = ' ';

	  linbuf[47] = '\0';

	  UpdVid (i, 0, "\260", PTBATR);

	  UpdVid (i, 1, linbuf, (i EQ 7) ? PTEATR : PTPATR);
	}

      dptw ();

      break;
/* 
*/
    case 2:			/* sequence status */

      for (i = 0; i < 16; i++)
	{

	  sprintf (linbuf, "    %03d %02d %d ",
		   seqline[i], sregval[i], trstate[i]);

	  vvputsv (obj10, 16, PDBORFG, PDSEQBG, i, 0, "\260", 14, 14, cg3);

	  vvputsv (obj10, 16, PDSEQFG, PDSEQBG, i, 1, linbuf, 14, 14, cg3);

	  vvputsv (obj10, 16, PDBORFG, PDSEQBG, i, 14, "\260", 14, 14, cg3);

	  if (i EQ 7)
	    {

	      if (48 EQ XTOC (cxval))
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
	vputsa (obj11, (i + 1), 0, ptmcon[i], atrbuf);

      break;
    }
}

/* 
*/

/*
   =============================================================================
	ptwins() -- draw the patch table display
   =============================================================================
*/

ptwins ()
{
  register short i;

  for (i = 0; i < 4; i++)
    ptwin (i);
}

/*
   =============================================================================
	ptdisp() -- setup the patch table display
   =============================================================================
*/

ptdisp ()
{
  dswap ();			/* clear the video display */

  BakLine = LineBak;
  FwdLine = LineFwd;

  /* setup object pointers */

  obj8 = &v_score[OB08LOC];	/*  8 - headings */
  obj9 = &v_score[OB09LOC];	/*  9 - patches */
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

  voidpb ();			/* void the patch buffer */
  ptwins ();			/* fill up the windows */

/* 
*/
  /* display some objects */

  SetPri (8, 8);		/* headings */
  SetPri (9, 9);		/* patches */
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
  aftercm ();			/* set initial submenu */

  vsndpal (ptabpal);		/* set the palette */
}
