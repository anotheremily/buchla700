/*
   =============================================================================
	showcfg.c -- display a configuration diagram
	Version 14 -- 1988-08-26 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "hwdefs.h"

#include "midas.h"
#include "instdsp.h"
#include "wsdsp.h"

#define	MAXPAT		35

#include "configs.h"

/* instrument display configuration box parameters */

#define	LFT_EDGE	1	/* left edge of configuration box */
#define	TOP_EDGE	238	/* top edge of configuration box */
#define	RGT_EDGE	131	/* right edge of configuration box */
#define	BOT_EDGE	348	/* bottom edge of configuration box */

/* external functions */

extern unsigned exp_c ();

extern short enterit ();
extern short idpoint ();
extern short nokey ();

extern short et_mcfn (), ef_mcfn (), rd_mcfn (), nd_mcfn ();
extern short et_mwsn (), ef_mwsn (), rd_mwsn (), nd_mwsn ();

/* forward references */

short imfnbox ();

/* 
*/

/* variables defined elsewhere */

extern short (*point) ();	/* point plotting function pointer */
extern short (*x_key) ();	/* "X" key handler pointer */

extern short LftEdge;		/* left edge of current box */
extern short TopEdge;		/* top edge of current box */
extern short RgtEdge;		/* right edge of current box */
extern short BotEdge;		/* bottom edge of current box */

extern short curslim;		/* cursor changeover point */
extern short curvce;		/* currently edited voice */
extern short hitbox;		/* selected box number */
extern short hitcx;		/* selection cursor x value */
extern short hitcy;		/* selection cursor y value */
extern short stccol;		/* text cursor column */
extern short stcrow;		/* text cursor row */
extern short wcflag;		/* ws/cf menu select flag (cf=0, ws=1) */
extern short wcmcol;		/* ws/cf menu label column */
extern short wcmrow;		/* ws/cf menu label row */
extern short wcpage;		/* ws/cf menu page */
extern short wmcsel;		/* ws slot select */
extern short wmctag;		/* ws/cf display update flag */

extern unsigned *instob;	/* instrument display object pointer */

extern struct octent *idoct;	/* object control table pointer */

extern struct selbox *csbp;	/* current box select table pointer */

extern struct instdef vbufs[];	/* voice buffers */

extern struct wstbl wslib[];	/* waveshape library */

/* 
*/

struct fet id_fet2[] = {	/* waveshape number fet */

  {24, 12, 13, 0x0000, et_mwsn, ef_mwsn, rd_mwsn, nd_mwsn},
  {24, 16, 17, 0x0100, et_mwsn, ef_mwsn, rd_mwsn, nd_mwsn},
  {0, 0, 0, 0x0000, FN_NULL, FN_NULL, FN_NULL, FN_NULL}
};

struct fet id_fet3[] = {	/* configuration number fet */

  {24, 16, 17, 0, et_mcfn, ef_mcfn, rd_mcfn, nd_mcfn},
  {0, 0, 0, 0x0000, FN_NULL, FN_NULL, FN_NULL, FN_NULL}
};

struct selbox idmbox[] = {

  {1, 1, 127, 111, 0, imfnbox},	/* slot 00 */
  {129, 1, 255, 111, 1, imfnbox},	/* slot 01 */
  {257, 1, 383, 111, 2, imfnbox},	/* slot 02 */
  {385, 1, 510, 111, 3, imfnbox},	/* slot 03 */

  {1, 113, 127, 223, 4, imfnbox},	/* slot 04 */
  {129, 113, 255, 223, 5, imfnbox},	/* slot 05 */
  {257, 113, 383, 223, 6, imfnbox},	/* slot 06 */
  {385, 113, 510, 223, 7, imfnbox},	/* slot 07 */

  {1, 225, 127, 334, 8, imfnbox},	/* slot 08 */
  {129, 225, 255, 334, 9, imfnbox},	/* slot 09 */
  {257, 225, 383, 334, 10, imfnbox},	/* slot 10 */
  {385, 225, 510, 334, 11, imfnbox},	/* slot 11 */

  {1, 336, 510, 349, 12, imfnbox},	/* number */
};

/* 
*/

short patctab[MAXPAT] = {	/* pattern colors */

  0,				/* 1 */
  0,				/* 2 */
  0,				/* 3 */
  0,				/* 4 */
  0,				/* 5 */
  0,				/* 6 */
  12,				/* 7 */
  12,				/* 8 */
  12,				/* 9 */
  12,				/* 10 */
  0,				/* 11 */
  0,				/* 12 */
  0,				/* 13 */
  0,				/* 14 */
  13,				/* 15 */
  8,				/* 16 */
  0,				/* 17 */
  0,				/* 18 */
  2,				/* 19 */
  2,				/* 20 */
  2,				/* 21 */
  2,				/* 22 */
  2,				/* 23 */
  2,				/* 24 */
  2,				/* 25 */
  2,				/* 26 */
  0,				/* 27 */
  0,				/* 28 */
  0,				/* 29 */
  0,				/* 30 */
  0,				/* 31 */
  0,				/* 32 */
  0,				/* 33 */
  0,				/* 34 */
  4				/* 35 */
};

/* 
*/

short pat[MAXPAT][16] = {	/* configuration pattern elements */

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 1 */
   0x0080, 0x0180, 0x0080, 0x0080,
   0x0080, 0x0080, 0x0080, 0x01C0,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 2 */
   0x03C0, 0x0420, 0x0020, 0x0020,
   0x03C0, 0x0400, 0x0400, 0x07E0,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 3 */
   0x03C0, 0x0420, 0x0020, 0x01C0,
   0x0020, 0x0020, 0x0420, 0x03C0,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 4 */
   0x0040, 0x00C0, 0x0140, 0x0240,
   0x0440, 0x07E0, 0x0040, 0x0040,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 5 */
   0x03C0, 0x0200, 0x0200, 0x03C0,
   0x0020, 0x0020, 0x0420, 0x03C0,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 6 */
   0x03C0, 0x0400, 0x0400, 0x07C0,
   0x0420, 0x0420, 0x0420, 0x03C0,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x03C0, 0x07E0, 0x0FF0,	/* 7 */
   0x1FF8, 0x1FF8, 0x3FFC, 0x3FFC,
   0x7FFE, 0x7FFE, 0x7FFE, 0xFFFF,
   0xFFFF, 0xFFFF, 0x0000, 0x0000},

  {0x0000, 0x0000, 0xFFFF, 0xFFFF,	/* 8 */
   0xFFFF, 0x7FFE, 0x7FFE, 0x7FFE,
   0x3FFC, 0x3FFC, 0x1FF8, 0x1FF8,
   0x0FF0, 0x07E0, 0x03C0, 0x0000},

  {0x3800, 0x3F00, 0x3FC0, 0x3FF0,	/* 9 */
   0x3FF8, 0x3FFC, 0x3FFE, 0x3FFE,
   0x3FFE, 0x3FFE, 0x3FFC, 0x3FF8,
   0x3FF0, 0x3FC0, 0x3F00, 0x3800},

  {0x001C, 0x00FC, 0x03FC, 0x0FFC,	/* 10 */
   0x1FFC, 0x3FFC, 0x7FFC, 0x7FFC,
   0x7FFC, 0x7FFC, 0x3FFC, 0x1FFC,
   0x0FFC, 0x03FC, 0x00FC, 0x001C},
/* 
*/
  {0x0000, 0x0000, 0x0000, 0x0000,	/* 11 */
   0x0080, 0x0180, 0x0080, 0x0000,
   0x0000, 0x0000, 0x0000, 0x01C0,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 12 */
   0x03C0, 0x0420, 0x0020, 0x0000,
   0x03C0, 0x0400, 0x0000, 0x07E0,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 13 */
   0x03C0, 0x0420, 0x0020, 0x01C0,
   0x0020, 0x0000, 0x0420, 0x03C0,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 14 */
   0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x07E0, 0x1FF8, 0x3FFC,	/* 15 */
   0x7FFE, 0x7FFE, 0xFFFF, 0xFFFF,
   0xFFFF, 0xFFFF, 0x7FFE, 0x7FFE,
   0x3FFC, 0x1FF8, 0x07E0, 0x0000},

  {0x0000, 0x7FFE, 0x7FFE, 0x7FFE,	/* 16 */
   0x7FFE, 0x7FFE, 0x7FFE, 0x7FFE,
   0x7FFE, 0x7FFE, 0x7FFE, 0x7FFE,
   0x7FFE, 0x7FFE, 0x7FFE, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 17 */
   0x03C0, 0x0420, 0x0420, 0x0420,
   0x07E0, 0x0420, 0x0420, 0x0420,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 18 */
   0x07C0, 0x0220, 0x0220, 0x03C0,
   0x0220, 0x0220, 0x0220, 0x07C0,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 19 */
   0x0000, 0x0000, 0x0000, 0x007F,
   0x00FF, 0x01C0, 0x0180, 0x0180,
   0x0180, 0x0180, 0x0180, 0x0180},

  {0x0180, 0x0180, 0x0180, 0x0180,	/* 20 */
   0x0180, 0x0180, 0x0380, 0xFF00,
   0xFE00, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000},
/* 
*/
  {0x0000, 0x0000, 0x0000, 0x0000,	/* 21 */
   0x0000, 0x0000, 0x0000, 0xFE00,
   0xFF00, 0x0380, 0x0180, 0x0180,
   0x0180, 0x0180, 0x0180, 0x0180},

  {0x0180, 0x0180, 0x0180, 0x0180,	/* 22 */
   0x0180, 0x0180, 0x01C0, 0x00FF,
   0x007F, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 23 */
   0x0000, 0x0000, 0x0000, 0xFFFF,
   0xFFFF, 0x0000, 0x0000, 0x0000,
   0x0000, 0x0000, 0x0000, 0x0000},

  {0x0180, 0x0180, 0x0180, 0x0180,	/* 24 */
   0x0180, 0x0180, 0x0180, 0x0180,
   0x0180, 0x0180, 0x0180, 0x0180,
   0x0180, 0x0180, 0x0180, 0x0180},

  {0x0003, 0x0007, 0x000E, 0x001C,	/* 25 */
   0x0038, 0x0070, 0x00E0, 0x01C0,
   0x0380, 0x0700, 0x0E00, 0x1C00,
   0x3800, 0x7000, 0xE000, 0xC000},

  {0xC000, 0xE000, 0x7000, 0x3800,	/* 26 */
   0x1C00, 0x0E00, 0x0700, 0x0380,
   0x01C0, 0x00E0, 0x0070, 0x0038,
   0x001C, 0x000E, 0x0007, 0x0003}

  {0x0000, 0x0000, 0x0000, 0x03C0,	/* 27 */
   0x07E0, 0x0E70, 0x0C30, 0x0C30,
   0x0FF0, 0x0FF0, 0x0C30, 0x0C30,
   0x0C30, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0FE0,	/* 28 */
   0x0FF0, 0x0630, 0x0630, 0x07E0,
   0x07E0, 0x0630, 0x0630, 0x0FF0,
   0x0FE0, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0080,	/* 29 */
   0x0180, 0x0380, 0x0180, 0x0180,
   0x0180, 0x0180, 0x0180, 0x03C0,
   0x03C0, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x03C0,	/* 30 */
   0x07E0, 0x0660, 0x0060, 0x03E0,
   0x07C0, 0x0600, 0x0600, 0x07E0,
   0x07E0, 0x0000, 0x0000, 0x0000},
/* 
*/
  {0x0000, 0x0000, 0x0000, 0x03C0,	/* 31 */
   0x07E0, 0x0660, 0x0060, 0x01C0,
   0x01C0, 0x0060, 0x0660, 0x07E0,
   0x03C0, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x0000,	/* 32 */
   0x0060, 0x0660, 0x0660, 0x0660,
   0x07F0, 0x07F0, 0x0060, 0x0060,
   0x0060, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x07C0,	/* 33 */
   0x07C0, 0x0600, 0x0600, 0x07C0,
   0x07E0, 0x0060, 0x0060, 0x07E0,
   0x03C0, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x0000, 0x0000, 0x03C0,	/* 34 */
   0x07C0, 0x0600, 0x0600, 0x07C0,
   0x07E0, 0x0660, 0x0660, 0x07E0,
   0x03C0, 0x0000, 0x0000, 0x0000},

  {0x0000, 0x7FFE, 0x7FFE, 0x7FFE,	/* 35 */
   0x7FFE, 0x7FFE, 0x7FFE, 0x7FFE,
   0x7FFE, 0x7FFE, 0x7FFE, 0x7FFE,
   0x7FFE, 0x7FFE, 0x7FFE, 0x0000}
};

/* 
*/

/*
   =============================================================================
	putpat() -- display a pattern in a configuration diagram
   =============================================================================
*/

putpat (pn, px, py, patc)
     short pn, px, py, patc;
{
  register short xp, yp, pr, pc;
  register unsigned pw;

  if (v_regs[5] & 0x0180)
    vbank (0);

  for (pr = 0; pr < 16; pr++)
    {

      pw = pat[pn - 1][pr];
      yp = py - 8 + pr + TopEdge;

      if ((yp >= TopEdge) && (yp <= BotEdge))
	{

	  for (pc = 0; pc < 16; pc++)
	    {

	      xp = px - 8 + pc + LftEdge;

	      if ((xp >= LftEdge) &&
		  (xp <= RgtEdge) && (pw & (1 << (15 - pc))))
		{

		  vputp (idoct, xp, yp, patc);
		}
	    }
	}
    }
}

/* 
*/

/*
   =============================================================================
	dispcfg() -- display a configuration on the screen
   =============================================================================
*/

dispcfg (nn)
     register short nn;
{
  register short i, np;
  short idbuf[8];
  register char *cfgdat, *cfp;

  if (v_regs[5] & 0x0180)
    vbank (0);

  tsplot4 (instob, 64, CFBX18, wcmrow, 4 + wcmcol, "Config #", 14);

  sprintf (idbuf, "%02d", nn);
  tsplot4 (instob, 64, CFBX18, wcmrow, 12 + wcmcol, idbuf, 14);

  cfgdat = cfgptr[nn];

  for (i = 0; i < 64; i++)
    {

      cfp = cfgdat + (i << 1) + i;
      np = *cfp;

      if (np)
	putpat (np, *(cfp + 1), *(cfp + 2), patctab[np - 1]);
    }
}

/* 
*/

/*
   =============================================================================
	showcfg() -- display a configuration in the configuration window
   =============================================================================
*/

showcfg (nn)
     short nn;
{
  LftEdge = LFT_EDGE;
  TopEdge = TOP_EDGE;
  RgtEdge = RGT_EDGE;
  BotEdge = BOT_EDGE;
  wcmrow = 17;
  wcmcol = 0;

  if (v_regs[5] & 0x0180)
    vbank (0);

  vbfill4 (instob, 128, LftEdge, TopEdge, RgtEdge, BotEdge, exp_c (CBBX18));

  dispcfg (nn);
}

/* 
*/

/*
   =============================================================================
	dispws() -- display a waveshape
   =============================================================================
*/

dispws (ws)
     short ws;
{
  register unsigned *wsp;
  register short i, x, y;
  char buf[64];

  if (ws >= NUMWAVS)		/* number must be valid */
    return;

  wsp = wslib[ws].final;	/* point at the data */

  /* label the waveshape */

  sprintf (buf, "Waveshape #%02d", ws + 1);
  tsplot4 (instob, 64, CFBX21, wcmrow, wcmcol + 2, buf, 14);

  for (i = 1; i < 254; i++)
    {				/* draw the points */

      x = LftEdge + (i >> 1);
      y = BotEdge - ((wsp[i] ^ 0x8000) / 676);

      idpoint (x, y, WSBFC);
    }
}

/* 
*/

/*
   =============================================================================
	wcmenu() -- setup the display for a waveshape or configuration menu
   =============================================================================
*/

wcmenu (wc)
     short wc;
{
  register short i;
  char buf[32];

  wcflag = wc;			/* set menu page type */

  if (v_regs[5] & 0x0180)
    vbank (0);

  vbfill4 (instob, 128, 0, 0, 511, 349, exp_c (CBBX18));

  point = idpoint;

  lseg (0, 0, 511, 0, CBORD);	/* outside border */
  lseg (511, 0, 511, 349, CBORD);
  lseg (511, 349, 0, 349, CBORD);
  lseg (0, 349, 0, 0, CBORD);

  lseg (0, 112, 511, 112, CBORD);	/* horizontal lines */
  lseg (0, 224, 511, 224, CBORD);
  lseg (0, 335, 511, 335, CBORD);

  lseg (128, 0, 128, 335, CBORD);	/* vertical lines */
  lseg (256, 0, 256, 335, CBORD);
  lseg (384, 0, 384, 335, CBORD);

  ebflag = FALSE;		/* clear edit buffer */
  memset (ebuf, '\0', sizeof ebuf);

  curslim = RTOY (24) - 1;	/* text cursor in bottom row */

  x_key = nokey;
  csbp = idmbox;

/* 
*/
  if (wc)
    {				/* waveshape */

      for (i = 0; i < 12; i++)
	{			/* fill in waveshapes */

	  LftEdge = ((i % 4) * 128) + 1;
	  BotEdge = ((i / 4) * 112) + 111;
	  wcmrow = (i / 4) << 3;
	  wcmcol = (i % 4) << 4;

	  dispws (i + (wcpage * 12));
	}

      /* label data entry area and show page number */

      sprintf (buf, "Waveshape A%02d B%02d Page",
	       1 + vbufs[curvce].idhwsb, 1 + vbufs[curvce].idhwsa);

      tsplot4 (instob, 64, CFBX18, 24, 1, buf, 14);

      tsplot4 (instob, 64, wcpage ? CFBX18 : ID_ENTRY, 24, 26, "1", 14);

      tsplot4 (instob, 64, wcpage ? ID_ENTRY : CFBX18, 24, 28, "2", 14);

      curfet = id_fet2;
      settc (24, 12);
/* 
*/
    }
  else
    {				/* configuration */

      for (i = 0; i < 12; i++)
	{			/* fill in configurations */

	  LftEdge = ((i % 4) * 128) + 1;
	  TopEdge = ((i / 4) * 112) + 1;
	  RgtEdge = LftEdge + 128;
	  BotEdge = TopEdge + 112;
	  wcmrow = (i / 4) << 3;
	  wcmcol = (i % 4) << 4;

	  dispcfg (i);
	}

      /* label data entry area */

      tsplot4 (instob, 64, CFBX18, 24, 1, "Configuration #", 14);

      sprintf (buf, "%02d", vbufs[curvce].idhcfg);
      tsplot4 (instob, 64, CFBX18, 24, 16, buf, 14);

      curfet = id_fet3;
      settc (24, 16);
    }
}

/* 
*/

/*
   =============================================================================
	imfnbox() -- instrument menu box hit processor
   =============================================================================
*/

imfnbox (n)
     short n;
{
  register short row, col;

  row = hitcy / 14;
  col = hitcx >> 3;

  if (hitbox == 12)
    {

      if (wcflag)
	{			/* waveshape */

	  if (col < 10)
	    {			/* take down menu */

	      reshowi ();
	      settc (17, wmcsel ? 61 : 57);

	    }
	  else if ((col >= 19) && (col <= 22))
	    {

	      /* toggle waveshape display page */

	      if (wcpage)
		wcpage = 0;	/* to page 1 */
	      else
		wcpage = 1;	/* to page 2 */

	      wcmenu (1);
	      return;

	    }
	  else
	    {			/* data entry */

	      wmctag = FALSE;
	      enterit ();

	      if (wmctag)
		modinst ();

	      return;
	    }
/* 
*/
	}
      else
	{			/* configuration */

	  if (col < 14)
	    {			/* take down menu */

	      reshowi ();
	      settc (17, 12);

	    }
	  else
	    {			/* data entry */

	      wmctag = FALSE;
	      enterit ();

	      if (wmctag)
		modinst ();

	      return;
	    }
	}
    }
}
