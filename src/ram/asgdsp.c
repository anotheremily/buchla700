/*
   =============================================================================
	asgdsp.c -- MIDAS assignment editor
	Version 50 -- 1988-10-04 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "memory.h"

#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "charset.h"
#include "fields.h"

#include "midas.h"
#include "asgdsp.h"

#define	PSG_ADDR	0
#define	PSG_READ	0
#define	PSG_WRIT	2

#define	PSG_IOEN	7
#define	PSG_IDLE	0xBF

#define	PSG_PRTB	15	/* PSG Port B */

#define	AUX_BIT		0x02	/* aux control bit in PSG port B */

#define	AK_WIDTH	115	/* width, in words, of keyboard icon */

/* 4 bit pixel patterns */

#define	AKW_0	0x0000
#define	AKW_1	0xF000
#define	AKW_2	0x0F00
#define	AKW_3	0xFF00
#define	AKW_4	0x00F0
#define	AKW_5	0xF0F0
#define	AKW_6	0x0FF0
#define	AKW_7	0xFFF0
#define	AKW_8	0x000F
#define	AKW_9	0xF00F
#define	AKW_A	0x0F0F
#define	AKW_B	0xFF0F
#define	AKW_C	0x00FF
#define	AKW_D	0xF0FF
#define	AKW_E	0x0FFF
#define	AKW_F	0xFFFF

/* 
*/

/* things defined elsewhere */

extern int (*point) ();
extern unsigned exp_c ();

extern short stcrow, stccol, cxval, cyval;
extern short curtun, tunmod;

#if	DEBUGIT
extern short debugsw;
#endif

extern unsigned *obj0, *obj2;

extern char bfs[];
extern char *adbxlb[];
extern char caname[];

extern short adbox[][8];

extern short admctl;		/* assignment menu control */
extern short adnamsw;		/* vitrual typewriter switch */
extern short asgfks;		/* first key selected */
extern short asghit;		/* row hit / assignment in progress */
extern short asgmod;		/* assignment number or table modified */
extern short auxctl;		/* aux control flag */
extern short curasg;		/* current assignment table */
extern short curmop;		/* current MIDI output port */
extern short curvce;		/* current voice being edited */
extern short prgchan;		/* MIDI program change channel  (port 1) */
extern short ps_dpth;		/* phase shifter -- depth */
extern short ps_intn;		/* phase shifter -- intensity */
extern short ps_rate;		/* phase shifter -- rate */
extern short submenu;		/* submenu flag */

extern short grpdyn[12];	/* group dynamics table (0..9) */
extern short ins2grp[12];	/* instrument to group table (00..NINST-1) */
extern short mctlnum[4];	/* MIDI controller number table (-1, 00..99) */
extern short s_inst[12];	/* instrument assignments */
extern short vce2grp[12];	/* voice to group table (-1, 1..12) */

extern short grp2prt[12][2];	/* group to port and channel table */
				/* port [0] = 0..4, channel [1] = -1, 1..16 */

extern short key2grp[88];	/* port 1 key to group assignment table */
				/* bit n = group n */

extern struct asgent asgtab[NASGS];	/* assignment table library */

extern unsigned *asgob;

extern struct octent *adoct;

/* 
*/

char *gprep[] = { " ", "1", "2", "L" };

char *asgsrc[] = {		/* source labels */

  "1 Pch/Hor",
  "2 Mod/Vrt",
  "3 Brth/LP",
  "4 GPC/CV1",
  "5 Pedal 1",
  "6 Key Prs"
};

/* keys are 5 pixels wide on top, except the last one, which is 8 pixels wide */

short asgkble[88] = {		/* key left edge offsets */

  /* piano        MIDI */

  1, 6, 11,			/*  1..3         21..23 */
  17, 22, 27, 32, 37,		/*  4..8         24..28 */
  43, 48, 53, 58, 63, 68, 73,	/*  9..15        29..35 */
  79, 84, 89, 94, 99,		/* 16..20        36..40 */
  105, 110, 115, 120, 125, 130, 135,	/* 21..27        41..47 */
  141, 146, 151, 156, 161,	/* 28..32        48..52 */
  167, 172, 177, 182, 187, 192, 197,	/* 33..39        53..59 */
  203, 208, 213, 218, 223,	/* 40..44        60..64 */
  229, 234, 239, 244, 249, 254, 259,	/* 45..51        65..71 */
  265, 270, 275, 280, 285,	/* 52..56        72..76 */
  291, 296, 301, 306, 311, 316, 321,	/* 57..63        77..83 */
  327, 332, 337, 342, 347,	/* 64..68        84..88 */
  353, 358, 363, 368, 373, 378, 383,	/* 69..75        89..95 */
  389, 394, 399, 404, 409,	/* 76..80        96..100 */
  415, 420, 425, 430, 435, 440, 445,	/* 81..87       101..107 */
  451				/* 88           108 */
};

/* 
*/

short asgkbtp[AK_WIDTH] = {	/* keyboard icon top lines */

  AKW_7, AKW_C, AKW_1, AKW_F,
  AKW_7, AKW_C, AKW_1, AKW_F,
  AKW_0, AKW_7, AKW_D, AKW_F,
  AKW_0, AKW_7, AKW_C, AKW_1,
  AKW_F, AKW_0, AKW_7, AKW_D,
  AKW_F, AKW_0, AKW_7, AKW_C,
  AKW_1, AKW_F,

  AKW_7, AKW_C, AKW_1, AKW_F,
  AKW_0, AKW_7, AKW_C, AKW_1,
  AKW_F, AKW_7, AKW_C, AKW_1,
  AKW_F, AKW_0, AKW_7, AKW_D,
  AKW_F, AKW_0, AKW_7, AKW_C,
  AKW_1, AKW_F, AKW_0, AKW_7,
  AKW_D, AKW_F, AKW_0, AKW_7,
  AKW_C, AKW_1, AKW_F,

  AKW_7, AKW_C, AKW_1, AKW_F,
  AKW_0, AKW_7, AKW_C, AKW_1,
  AKW_F, AKW_7, AKW_C, AKW_1,
  AKW_F, AKW_0, AKW_7, AKW_D,
  AKW_F, AKW_0, AKW_7, AKW_C,
  AKW_1, AKW_F, AKW_0, AKW_7,
  AKW_D, AKW_F, AKW_0, AKW_7,
  AKW_C, AKW_1, AKW_F,

  AKW_7, AKW_C, AKW_1, AKW_F,
  AKW_0, AKW_7, AKW_C, AKW_1,
  AKW_F, AKW_7, AKW_C, AKW_1,
  AKW_F, AKW_0, AKW_7, AKW_D,
  AKW_F, AKW_0, AKW_7, AKW_C,
  AKW_1, AKW_F, AKW_0, AKW_7,
  AKW_D, AKW_F, AKW_E
};

/* 
*/

short asgkbbt[AK_WIDTH] = {	/* keyboard icon bottom lines */

  AKW_7, AKW_F, AKW_7, AKW_F,
  AKW_7, AKW_F, AKW_7, AKW_F,
  AKW_B, AKW_F, AKW_D, AKW_F,
  AKW_E, AKW_F, AKW_F, AKW_7,
  AKW_F, AKW_B, AKW_F, AKW_D,
  AKW_F, AKW_E, AKW_F, AKW_E,
  AKW_F, AKW_F,

  AKW_7, AKW_F, AKW_B, AKW_F,
  AKW_D, AKW_F, AKW_E, AKW_F,
  AKW_F, AKW_7, AKW_F, AKW_B,
  AKW_F, AKW_B, AKW_F, AKW_D,
  AKW_F, AKW_E, AKW_F, AKW_F,
  AKW_7, AKW_F, AKW_B, AKW_F,
  AKW_D, AKW_F, AKW_E, AKW_F,
  AKW_E, AKW_F, AKW_F,

  AKW_7, AKW_F, AKW_B, AKW_F,
  AKW_D, AKW_F, AKW_E, AKW_F,
  AKW_F, AKW_7, AKW_F, AKW_B,
  AKW_F, AKW_B, AKW_F, AKW_D,
  AKW_F, AKW_E, AKW_F, AKW_F,
  AKW_7, AKW_F, AKW_B, AKW_F,
  AKW_D, AKW_F, AKW_E, AKW_F,
  AKW_E, AKW_F, AKW_F,

  AKW_7, AKW_F, AKW_B, AKW_F,
  AKW_D, AKW_F, AKW_E, AKW_F,
  AKW_F, AKW_7, AKW_F, AKW_B,
  AKW_F, AKW_B, AKW_F, AKW_D,
  AKW_F, AKW_E, AKW_F, AKW_F,
  AKW_7, AKW_F, AKW_B, AKW_F,
  AKW_D, AKW_F, AKW_E
};

/* 
*/

short asgpal[16][3] = {		/* assignment editor color palette */

  {0, 0, 0},			/* 0 */
  {3, 3, 3},			/* 1 */
  {0, 0, 2},			/* 2 */
  {1, 0, 1},			/* 3 */
  {0, 1, 2},			/* 4 */
  {0, 1, 1},			/* 5 (was 0, 1, 0) */
  {1, 1, 2},			/* 6 */
  {0, 0, 1},			/* 7 */
  {2, 2, 2},			/* 8 */
  {0, 0, 0},			/* 9 */
  {2, 2, 2},			/* 10 (was 1, 1, 0) */
  {2, 3, 3},			/* 11 */
  {3, 3, 0},			/* 12 */
  {3, 0, 0},			/* 13 */
  {0, 0, 0},			/* 14 */
  {0, 2, 3}			/* 15 (was 0, 3, 2) */
};

short dyntab[10] = {		/* dynamics translation table */

  0,				/* 0 */
  (120 << 5),			/* 1 */
  (180 << 5),			/* 2 */
  (250 << 5),			/* 3 */
  (320 << 5),			/* 4 */
  (400 << 5),			/* 5 */
  (500 << 5),			/* 6 */
  (630 << 5),			/* 7 */
  (790 << 5),			/* 8 */
  (1000 << 5)			/* 9 */
};

/* 
*/

/*
   =============================================================================
	advacur() -- advance the assignment display text cursor
   =============================================================================
*/

advacur ()
{
  register short newcol;

  if (infield (stcrow, stccol, curfet))
    cfetp = infetp;
  else
    return;

  newcol = stccol + 1;

  if (newcol LE cfetp->frcol)
    itcpos (stcrow, newcol);

  cxval = stccol << 3;
  cyval = stcrow * 14;
}

/*
   =============================================================================
	bspacur() -- backspace the assignment display text cursor
   =============================================================================
*/

bspacur ()
{
  register short newcol;

  if (infield (stcrow, stccol, curfet))
    cfetp = infetp;
  else
    return;

  newcol = stccol - 1;

  if (newcol GE cfetp->flcol)
    itcpos (stcrow, newcol);

  cxval = stccol << 3;
  cyval = stcrow * 14;
}

/* 
*/

/*
   =============================================================================
	keycpyw() -- copy words into the keyboard object
   =============================================================================
*/

keycpyw (dest, src, len, wk, bk)
     register unsigned *dest, *src;
     register short len;
     unsigned wk, bk;
{
  register unsigned wkey, bkey, theword;
  register short i;

  wkey = exp_c (wk);
  bkey = exp_c (bk);

  for (i = 0; i < len; i++)
    {

      theword = *src++;
      *dest++ = (theword & wkey) | ((~theword) & bkey);
    }
}

/* 
*/

/*
   =============================================================================
	asgkb() -- draw the assignment keyboard icon
   =============================================================================
*/

asgkb ()
{
  register unsigned *p;
  register unsigned akline;
  register short i, j;

  akline = exp_c (AK_LINE);
  p = asgob + (long) AKSTART;

  for (j = 0; j < 12; j++)
    {

      memsetw (p, akline, AK_WIDTH);
      p += 128L;

      for (i = 0; i < 13; i++)
	{

	  keycpyw (p, asgkbtp, AK_WIDTH, AK_WKEYT, AK_BKEYT);
	  p += 128L;
	}
    }

  memsetw (p, akline, AK_WIDTH);
  p += 128L;

  for (i = 0; i < 14; i++)
    {

      keycpyw (p, asgkbtp, AK_WIDTH, AK_WKEYB, AK_BKEYB);
      p += 128L;
    }

  for (i = 0; i < 11; i++)
    {

      keycpyw (p, asgkbbt, AK_WIDTH, AK_WKEYB, AK_BKEYB);
      p += 128L;
    }

  memsetw (p, akline, AK_WIDTH);
}

/* 
*/

/*
   =============================================================================
	drawk2g() -- display key assignments for a group
   =============================================================================
*/

drawk2g (grp)
     register short grp;
{
  register short i;
  register short n;
  register short key;
  register short line;
  register char *bfsp;
  register unsigned *lp;

  n = 7;			/* key to group window */

  line = (14 * grp) + AK_BASE;
  lp = asgob + ((long) line << 7) + 6L;

  for (i = 0; i < 4; i++)
    {

      keycpyw (lp, asgkbtp, AK_WIDTH, AK_WKEYT, AK_BKEYT);
      lp += 128L;
    }

  if (grp2prt[grp][0] EQ 1)
    {

      for (key = 0; key < 88; key++)
	if (key2grp[key] & (0x0001 << grp))
	  vbfill4 (asgob, 128, asgkble[key] + 24, line,
		   asgkble[key] + (key EQ 87 ? 31 : 28),
		   line + 3, exp_c (AK_SELC));

      sprintf (bfs, "%c", (grp > 8) ? (grp + 163) : (grp + '1'));
      bfsp = bfs;

    }
  else
    {

      bfsp = " ";
    }

  vcputsv (asgob, 64, adbox[n][4], adbox[n][5],
	   adbox[n][6] + 1 + grp, adbox[n][7], bfsp, 14);

  vcputsv (asgob, 64, adbox[n][4], adbox[n][5],
	   adbox[n][6] + 1 + grp, adbox[n][7] + 61, bfsp, 14);
}

/* 
*/

/*
   =============================================================================
	adpoint() -- plot a point for the lseg function
   =============================================================================
*/

adpoint (x, y, pen)
     short x, y, pen;
{
  if (v_regs[5] & 0x0180)
    vbank (0);

  vputp (adoct, x, y, exp_c (pen));
}

/*
   =============================================================================
	numblk() -- return a number string or a blank string
   =============================================================================
*/

char *
numblk (buf, n)
     char *buf;
     short n;
{
  if (n EQ - 1)
    {

      strcpy (buf, "  ");
      return (buf);

    }
  else
    {

      sprintf (buf, "%02.2d", n);
      return (buf);
    }
}

/* 
*/

/*
   =============================================================================
	adswin() -- display a window
   =============================================================================
*/

adswin (n)
     register short n;
{
  register short cx, i;
  register char *bfsp;
  char buf1[4], buf2[4];

  if ((n EQ 7) AND (admctl NE - 1))
    return;

  cx = exp_c (adbox[n][5]);
  point = adpoint;

  /* first, fill the box with the background color */

  vbank (0);
  vbfill4 (asgob, 128, adbox[n][0], adbox[n][1], adbox[n][2],
	   adbox[n][3], cx);

  /* put in the box label */

  tsplot4 (asgob, 64, adbox[n][4], adbox[n][6], adbox[n][7], adbxlb[n], 14);

/* 
*/
  switch (n)
    {				/* final text - overlays above stuff */

    case 0:			/* assignment table number and name */

      sprintf (bfs, "%02.2d", curasg);
      tsplot4 (asgob, 64, (asgmod ? AK_MODC : adbox[n][4]),
	       adbox[n][6], adbox[n][7] + 8, bfs, 14);

      sprintf (bfs, "%-10.10s", caname);
      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 1, adbox[n][7], bfs, 14);

      return;

    case 2:			/* output MIDI port number */

      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6], adbox[n][7] + 9,
	       gprep[curmop], 14);

      return;

    case 3:			/* MIDI program change channel  (always on port 1) */

      sprintf (bfs, "%02.2d", prgchan);
      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6], adbox[n][7] + 8, bfs, 14);

      return;
/* 
*/
    case 4:			/* groups to voices */

      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 1, adbox[n][7],
	       "of Groups", 14);

      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 2, adbox[n][7],
	       "to Voices", 14);

      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 3, adbox[n][7],
	       "V Gr  V Gr", 14);

      lseg (CTOX (13), RTOY (3) + 13,
	    CTOX (13) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (15), RTOY (3) + 13,
	    CTOX (16) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (19), RTOY (3) + 13,
	    CTOX (19) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (21), RTOY (3) + 13,
	    CTOX (22) + 7, RTOY (3) + 13, adbox[n][4]);

      for (i = 0; i < 6; i++)
	{

	  sprintf (bfs, "%c %s", i + '1', numblk (buf1, vce2grp[i]));

	  tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + i + 4,
		   adbox[n][7], bfs, 14);

	  sprintf (bfs, "%c %s", (i > 2 ? (i + 169) : (i + '7')),
		   numblk (buf2, vce2grp[i + 6]));

	  tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + i + 4,
		   adbox[n][7] + 6, bfs, 14);
	}

      return;
/* 
*/
    case 5:			/* MIDI controller number assignments */

      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 1, adbox[n][7],
	       "Sources and", 14);

      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 2, adbox[n][7],
	       "Controllers", 14);

      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 3, adbox[n][7],
	       "# Source  CN", 14);

      lseg (CTOX (25), RTOY (3) + 13,
	    CTOX (25) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (27), RTOY (3) + 13,
	    CTOX (33) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (35), RTOY (3) + 13,
	    CTOX (36) + 7, RTOY (3) + 13, adbox[n][4]);

      for (i = 0; i < 6; i++)
	tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + i + 4,
		 adbox[n][7], asgsrc[i], 14);

      for (i = 0; i < 4; i++)
	{

	  sprintf (bfs, "%s", numblk (buf1, (mctlnum[i] & 0x00FF)));

	  tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + i + 5,
		   adbox[n][7] + 10, bfs, 14);

	  if ((mctlnum[i] NE - 1) AND (mctlnum[i] & CTAG1))
	    {

	      bfs[0] = '2' + i;
	      bfs[1] = '\0';

	      tsplot4 (asgob, 64, AK_MODC, adbox[n][6] + i + 5,
		       adbox[n][7], bfs, 14);
	    }
	}

      return;
/* 
*/
    case 6:			/* instruments, dynamics, MIDI ports and channels to groups */

      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 1, adbox[n][7],
	       "Dynamics, MIDI Ports and", 14);

      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 2, adbox[n][7],
	       "Channels to Groups", 14);

      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 3, adbox[n][7],
	       "G In D I Ch  G In D I Ch", 14);

      lseg (CTOX (39), RTOY (3) + 13,
	    CTOX (39) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (41), RTOY (3) + 13,
	    CTOX (42) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (44), RTOY (3) + 13,
	    CTOX (44) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (46), RTOY (3) + 13,
	    CTOX (46) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (48), RTOY (3) + 13,
	    CTOX (49) + 7, RTOY (3) + 13, adbox[n][4]);


      lseg (CTOX (52), RTOY (3) + 13,
	    CTOX (52) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (54), RTOY (3) + 13,
	    CTOX (55) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (57), RTOY (3) + 13,
	    CTOX (57) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (59), RTOY (3) + 13,
	    CTOX (59) + 7, RTOY (3) + 13, adbox[n][4]);

      lseg (CTOX (61), RTOY (3) + 13,
	    CTOX (62) + 7, RTOY (3) + 13, adbox[n][4]);

/* 
*/
      for (i = 0; i < 6; i++)
	{

	  sprintf (bfs, "%c %02.2d %d %s %s  %c %02.2d %d %s %s",
		   i + '1',
		   (ins2grp[i] & 0x00FF), grpdyn[i],
		   gprep[grp2prt[i][0]],
		   numblk (buf1, grp2prt[i][1]),
		   ((i > 2) ? (i + 169) : (i + '7')),
		   (ins2grp[i + 6] & 0x00FF), grpdyn[i + 6],
		   gprep[grp2prt[i + 6][0]],
		   numblk (buf2, grp2prt[i + 6][1]));

	  tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + i + 4,
		   adbox[n][7], bfs, 14);

	  if (GTAG1 & ins2grp[i])
	    {

	      bfs[1] = '\0';

	      tsplot4 (asgob, 64, AK_MODC, adbox[n][6] + i + 4,
		       adbox[n][7], bfs, 14);
	    }

	  if (GTAG1 & ins2grp[i + 6])
	    {

	      bfs[14] = '\0';

	      tsplot4 (asgob, 64, AK_MODC, adbox[n][6] + i + 4,
		       adbox[n][7] + 13, &bfs[13], 14);
	    }
	}

      return;
/* 
*/
    case 7:			/* port 1 key to group assignments */

      lseg (8, 153, 15, 153, exp_c (adbox[n][4]));	/* underlines */
      lseg (496, 153, 503, 153, exp_c (adbox[n][4]));

      asgkb ();			/* icon */

      for (i = 0; i < 12; i++)	/* assignments */
	drawk2g (i);

      return;

    case 8:			/* aux control */

      tsplot4 (asgob, 64, (auxctl ? AK_MODC : adbox[n][4]),
	       adbox[n][6], adbox[n][7], "Aux", 14);

      return;

    case 9:			/* tuning table */

      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6], adbox[n][7], "Tun", 14);

      sprintf (bfs, "%d", curtun);
      tsplot4 (asgob, 64, (tunmod ? AK_MODC : adbox[n][4]),
	       adbox[n][6], adbox[n][7] + 4, bfs, 14);

      return;

    case 10:			/* phase shifter variables -- intensity, rate depth */

      sprintf (bfs, "Intnsty %02.2d", ps_intn);
      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 1, adbox[n][7], bfs, 14);

      sprintf (bfs, "ModRate %02.2d", ps_rate);
      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 2, adbox[n][7], bfs, 14);

      sprintf (bfs, "ModDpth %02.2d", ps_dpth);
      tsplot4 (asgob, 64, adbox[n][4], adbox[n][6] + 3, adbox[n][7], bfs, 14);

      return;
    }
}

/* 
*/

/*
   =============================================================================
	initat() -- initialize assignment table
   =============================================================================
*/

initat (n)
     short n;
{
  register struct asgent *ap;
  register short i;

  ap = &asgtab[n];
  ap->a_mop = 0;		/* output to NULL */
  ap->a_tun = 0;		/* tuning = default */
  ap->a_aux = 0;		/* aux ctl = OFF */
  ap->a_intn = 70;		/* intensity */
  ap->a_rate = 0;		/* rate */
  ap->a_dpth = 70;		/* depth */

  for (i = 0; i < 12; i++)
    {				/* groups 1..12 */

      ap->a_i2grp[i] = 0;	/* instrument */
      ap->a_gpdyn[i] = 9;	/* dynamics */
    }

  for (i = 0; i < 8; i++)	/* voices 1..8 to group 1 */
    ap->a_v2grp[i] = 1;

  for (i = 8; i < 12; i++)	/* voices 9..12 to group 2 */
    ap->a_v2grp[i] = 2;

  ap->a_mctln[0] = 1;		/* modulation wheel */
  ap->a_mctln[1] = 2;		/* breath controller */
  ap->a_mctln[2] = 80;		/* general controller 1 */
  ap->a_mctln[3] = 4;		/* pedal controller 1 */

  ap->a_g2prt[0][0] = 1;	/* group 1:  port 1 input */
  ap->a_g2prt[0][1] = 1;	/* group 1:  channel 1 in and out */
  ap->a_g2prt[1][0] = 3;	/* group 2:  local input */
  ap->a_g2prt[1][1] = 1;	/* group 2:  channel 1 in and out */

  for (i = 2; i < 12; i++)
    {				/* groups 3..12 */

      ap->a_g2prt[i][0] = 0;	/* no input port */
      ap->a_g2prt[i][1] = -1;	/* no channel */
    }

  memsetw (ap->a_k2grp, 0x0001, 88);	/* all keys in group 1 */

  memcpy (ap->a_name, n ? "{unused}        " : "{Default}       ", 16);
}

/* 
*/

/*
   =============================================================================
	setaux() -- set aux control
   =============================================================================
*/

setaux (aux)
     register short aux;
{
  register short psgdata;
  register char *psg;

  auxctl = aux;
  psg = &io_tone;

  *(psg + PSG_ADDR) = PSG_IOEN;	/* setup PSG I/O controls */
  *(psg + PSG_WRIT) = PSG_IDLE;

  *(psg + PSG_ADDR) = PSG_PRTB;	/* read current psg data */
  psgdata = *(psg + PSG_READ) & ~AUX_BIT;

  *(psg + PSG_ADDR) = PSG_PRTB;	/* send out updated aux data */
  *(psg + PSG_WRIT) = psgdata | (aux ? 0 : AUX_BIT);
}

/* 
*/

/*
   =============================================================================
	getasg() -- get an assignment table from the library
   =============================================================================
*/

getasg (n)
     short n;
{
  register struct asgent *ap;
  register short i, grp, vce;

  ap = &asgtab[n];
  curmop = ap->a_mop;
  gettun (ap->a_tun);
  setaux (ap->a_aux);
  ps_intn = ap->a_intn;
  ps_rate = ap->a_rate;
  ps_dpth = ap->a_dpth;
  memcpyw (ins2grp, ap->a_i2grp, sizeof ins2grp / 2);
  memcpyw (grpdyn, ap->a_gpdyn, sizeof grpdyn / 2);
  memcpyw (vce2grp, ap->a_v2grp, sizeof vce2grp / 2);
  memcpyw (mctlnum, ap->a_mctln, sizeof mctlnum / 2);
  memcpyw (grp2prt, ap->a_g2prt, sizeof grp2prt / 2);
  memcpyw (key2grp, ap->a_k2grp, sizeof key2grp / 2);
  memcpy (caname, ap->a_name, 16);

  for (i = 0; i < 12; i++)	/* fix old tables */
    if (grp2prt[i][0] EQ 4)
      grp2prt[i][0] = 3;

  sendval (1, 0, (ps_intn * 10) << 5);
  sendval (2, 0, (ps_rate * 10) << 5);
  sendval (3, 0, (ps_dpth * 10) << 5);

  for (vce = 0; vce < 12; vce++)
    {

      grp = vce2grp[vce];

      if (grp NE - 1)
	{

	  s_inst[vce] = ins2grp[grp - 1] & 0x00FF;
	  execins (vce, s_inst[vce], 1);
	  sendval (vce, 8, dyntab[grpdyn[grp - 1]]);
	}
    }

  newvce (curvce);
  asgmod = FALSE;
}

/* 
*/

/*
   =============================================================================
	putasg() -- put an assignment table into the library
   =============================================================================
*/

putasg (n)
     short n;
{
  register struct asgent *ap;
  register short i;

  for (i = 0; i < 12; i++)	/* fix old tables */
    if (grp2prt[i][0] EQ 4)
      grp2prt[i][0] = 3;

  ap = &asgtab[n];
  ap->a_mop = curmop;
  ap->a_tun = curtun;
  ap->a_aux = auxctl;
  ap->a_intn = ps_intn;
  ap->a_rate = ps_rate;
  ap->a_dpth = ps_dpth;
  memcpyw (ap->a_i2grp, ins2grp, sizeof ins2grp / 2);
  memcpyw (ap->a_gpdyn, grpdyn, sizeof grpdyn / 2);
  memcpyw (ap->a_v2grp, vce2grp, sizeof vce2grp / 2);
  memcpyw (ap->a_mctln, mctlnum, sizeof mctlnum / 2);
  memcpyw (ap->a_g2prt, grp2prt, sizeof grp2prt / 2);
  memcpyw (ap->a_k2grp, key2grp, sizeof key2grp / 2);
  memcpy (ap->a_name, caname, 16);
  asgmod = FALSE;
}

/* 
*/

/*
   =============================================================================
	awins() -- display all assignment editor windows
   =============================================================================
*/

awins ()
{
  register short i;

  for (i = 0; i < 11; i++)
    adswin (i);
}

/*
   =============================================================================
	inital() -- initialize assignment library
   =============================================================================
*/

inital ()
{
  register short n;

  for (n = 0; n < NASGS; n++)
    initat (n);

  getasg (0);
  prgchan = 1;
}

/* 
*/

/*
   =============================================================================
	adbord() -- draw the border for the  display
   =============================================================================
*/

adbord ()
{
  point = adpoint;

  lseg (0, 0, 511, 0, AK_BORD);	/* outer border */
  lseg (511, 0, 511, 349, AK_BORD);
  lseg (511, 349, 0, 349, AK_BORD);
  lseg (0, 349, 0, 0, AK_BORD);

  lseg (0, 41, 95, 41, AK_BORD);	/* windows - H lines */
  lseg (0, 55, 95, 55, AK_BORD);
  lseg (0, 69, 95, 69, AK_BORD);
  lseg (0, 83, 95, 83, AK_BORD);
  lseg (0, 139, 511, 139, AK_BORD);

  lseg (39, 69, 39, 83, AK_BORD);	/* windows - V lines */
  lseg (95, 0, 95, 139, AK_BORD);
  lseg (191, 0, 191, 139, AK_BORD);
  lseg (303, 0, 303, 139, AK_BORD);
}

/* 
*/

/*
   =============================================================================
	asgdsp() -- put up the assignment display
   =============================================================================
*/

asgdsp ()
{
  asgob = v_score;		/* setup object pointer */
  obj0 = v_curs0;		/* setup cursor object pointer */
  obj2 = v_tcur;		/* setup typewriter object pointer */
  adoct = &v_obtab[ASGOBJ];	/* setup object control table pointer */

  adnamsw = FALSE;		/* virtual typewriter not up */
  submenu = FALSE;		/* no submenu cursor up */
  admctl = -1;			/* no submenu up */

  dswap ();			/* initialize display */

  vbank (0);			/* clear the display */
  memsetw (asgob, 0, 32767);
  memsetw (asgob + 32767L, 0, 12033);

  SetObj (ASGOBJ, 0, 0, asgob, 512, 350, 0, 0, ASGNFL, -1);
  SetObj (0, 0, 1, obj0, 16, 16, CTOX (9), RTOY (0), OBFL_00, -1);
  SetObj (TTCURS, 0, 1, obj2, 16, 16, 0, 0, TTCCFL, -1);

  arcurs (AK_CURS);		/* setup arrow cursor object */
  itcini (AK_CURS);		/* setup text cursor object */
  ttcini (AK_CURS);		/* setup virtual typewriter cursor object */

  adbord ();			/* draw the border */
  awins ();			/* fill in the windows */

  SetPri (ASGOBJ, ASGPRI);	/* enable screen object */

  settc (0, 9);			/* display text cursor */

  vsndpal (asgpal);		/* set the palette */
}
