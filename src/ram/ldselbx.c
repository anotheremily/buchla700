/*
   =============================================================================
	ldselbx.c -- librarian box selection functions
	Version 46 -- 1988-11-18 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stdio.h"
#include "stddefs.h"
#include "fields.h"
#include "graphdef.h"
#include "glcfns.h"
#include "glcdefs.h"
#include "graphdef.h"
#include "hwdefs.h"
#include "lcdline.h"
#include "panel.h"
#include "vsdd.h"
#include "vsddvars.h"

#include "midas.h"
#include "instdsp.h"
#include "libdsp.h"
#include "score.h"
#include "scdsp.h"
#include "scfns.h"

extern short ldswin (), advlcur (), bsplcur (), ttcpos (), fcindex (),
storit ();
extern short showsiz (), get_asg (), get_orc (), get_tun (), get_wav (),
nokey ();
extern short ldline (), lin2slt (), dslslot (), vtdisp (), ldpoint (),
rd_ec ();
extern short fcreset (), ftkind (), ldwmsg (), ldbusy (), ckstor ();
extern short get_pat (), get_scr ();

extern unsigned exp_c ();

extern char *slotnam ();

/* 
*/

#if DEBUGIT
extern short debugsw;
#endif

extern short (*point) ();

extern short asmode;
extern short catin;
extern short cxrate;
extern short cxval;
extern short cyrate;
extern short cyval;
extern short errno;
extern short gomode;
extern short hitcx;
extern short hitcy;
extern short ismode;
extern short lasgsw;
extern short ldelsw;
extern short lderrsw;
extern short ldkind;
extern short ldrow;
extern short ldslot;
extern short lksel;
extern short lmwtype;
extern short lorchl;
extern short lorchsw;
extern short lorclsw;
extern short ldpass;
extern short lpatsw;
extern short lrasw;
extern short lscrsw;
extern short lselsw;
extern short lseqsw;
extern short lstrsw;
extern short ltagged;
extern short ltunsw;
extern short lwavsw;
extern short ndisp;
extern short oldpk;
extern short oldsl;
extern short pkctrl;
extern short sliders;
extern short stcrow;
extern short stccol;
extern short tagslot;

extern unsigned *librob;

extern short ldmap[];
extern short scsizes[][2];

extern short ldbox[][8];

extern struct scndx sindex[];

extern struct selbox *csbp;
extern struct selbox *curboxp;

extern
PFS (*swpt)[];
     extern PFS (*oldsw)[];
     extern PFS t_libr[];

     extern char bfs[];
     extern char ldfile[];
     extern char ldcmnt[];

     extern char loadedf[][8];
     extern char loadedc[][37];

/* forward references */

     short bx_null (), ldfnbox ();

/* 
*/

     short ft2lt[] = {		/* file type to load type map */

       LT_ASG,
       LT_ORL,
       LT_ORH,
       LT_SCR,
       LT_TUN,
       LT_WAV,
       LT_ORL,
       LT_PAT,
       LT_SEQ
     };

struct selbox ldboxes[] = {

  {1, 1, 510, 13, 0, ldfnbox},	/*  0 - index area label */
  {1, 14, 510, 293, 1, ldfnbox},	/*  1 - index area */
  {1, 294, 78, 307, 2, ldfnbox},	/*  2 - file name label */
  {80, 294, 143, 307, 3, ldfnbox},	/*  3 - file name */
  {145, 294, 214, 307, 4, ldfnbox},	/*  4 - comment label */
  {216, 294, 510, 307, 5, ldfnbox},	/*  5 - comment */
  {1, 308, 70, 321, 6, ldfnbox},	/*  6 - fetch */
  {1, 322, 70, 335, 7, ldfnbox},	/*  7 - replace / append */
  {1, 336, 70, 349, 8, ldfnbox},	/*  8 - lo orch / hi orch */
  {72, 308, 255, 349, 9, ldfnbox},	/*  9 - store */
  {257, 308, 510, 349, 10, ldfnbox},	/* 10 - message window */

  {0, 0, 0, 0, 0, FN_NULL}	/* end of table */
};

/* 
*/

/*
   =============================================================================
	skperr() -- complain about an error while skipping a score
   =============================================================================
*/

skperr (sn)
     short sn;
{
  char scid[32];
  char erms[64];

  clrlsel ();

  sprintf (scid, "  score %d", sn + 1);
  sprintf (erms, "  errno = %d", errno);

  ldermsg ("Couldn't skip", scid, erms, LD_EMCF, LD_EMCB);
}

/* 
*/

/*
   =============================================================================
	skp_ec() -- skip with error checking
   =============================================================================
*/

short
skp_ec (fp, len)
     register FILE *fp;
     register long len;
{
  register long count;
  register int c;
  char errbuf[64];

  for (count = 0; count < len; count++)
    {

      errno = 0;

      if (EOF == (c = getc (fp)))
	{

	  sprintf (errbuf, "errno = %d", errno);

	  ldermsg ("Unexpected EOF", errbuf, (char *) NULL, LD_EMCF, LD_EMCB);

#if	DEBUGIT
	  if (debugsw)
	    FILEpr (fp);
#endif

	  fclose (fp);
	  postio ();		/* restore LCD backlight */
	  return (FAILURE);
	}
    }

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	scskip() -- skip a score starting with its section list
   =============================================================================
*/

short
scskip (fp, ns)
     register FILE *fp;
     short ns;
{
  register short ehdr, go;
  char etype;
  char erms[64];

  go = TRUE;

  if (skp_ec (fp, (long) (N_SECTS * 12)))
    {				/* skip section times */

      skperr (ns);
      return (FAILURE);
    }

  if (rd_ec (fp, &etype, 1L))
    {				/* read first score header event */

      skperr (ns);
      return (FAILURE);
    }

  if (etype != EV_SCORE)
    {				/* complain if it's not a score event */

      sprintf (erms, "  score %d  etype = %d", ns + 1, etype);

      ldermsg ("Bad score --", "  1st event is wrong",
	       erms, LD_EMCF, LD_EMCB);

      return (FAILURE);
    }

  if (skp_ec (fp, (long) (scsizes[etype][1] - 1)))
    {				/* skip data */

      skperr (ns);
      return (FAILURE);
    }

/* 
*/
  do
    {				/* skip remaining score events */

      if (rd_ec (fp, &etype, 1L))
	{			/* get event type */

	  skperr (ns);
	  return (FAILURE);
	}

      /* skip the event's data */

      if (skp_ec (fp, (long) (scsizes[etype][1] - 1)))
	{

	  skperr (ns);
	  return (FAILURE);
	}

      if (etype == EV_FINI)	/* check for score end */
	go = FALSE;

    }
  while (go);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ldermsg() -- display an error message if none is up already
   =============================================================================
*/

ldermsg (p1, p2, p3, p4, p5)
     char *p1, *p2, *p3;
     unsigned p4, p5;
{
  char msgbuf[64];

  if (! lderrsw)
    {				/* put up new messages only */

      strcpy (msgbuf, "ERROR: ");
      strcat (msgbuf, p1);

      ldwmsg (p1, p2, p3, p4, p5);
    }

  lderrsw = TRUE;		/* set error state */
}

/*
   =============================================================================
	clrerms() -- clear an error message from the message window
   =============================================================================
*/

clrerms ()
{
  if (lderrsw)
    {

      lderrsw = FALSE;
      lmwclr ();
      ldswin (10);
    }
}

/* 
*/

/*
   =============================================================================
	clrlsel() -- clear library selection
   =============================================================================
*/

clrlsel ()
{
  if (lselsw)
    {

      if (lrasw)
	{

	  lksel = -1;
	  ldpass = 0;
	  pkctrl = oldpk;
	  sliders = oldsl;
	  swpt = oldsw;
	  lcdlbls ();
	  setleds ();
	  fcindex ();

	}
      else
	{

	  dslslot (ldslot, exp_c (ldbox[1][4]), ldrow);
	}
    }

  fcreset ();
}

/*
   =============================================================================
	endltyp() -- end function for virtual typewriter
   =============================================================================
*/

endltyp ()
{
  lmwclr ();
  ldswin (10);
}

/* 
*/

/*
   =============================================================================
	savefc() -- save name and comment from loaded or stored file
   =============================================================================
*/

savefc (kind)
     short kind;
{
  short fi;

  fi = ft2lt[kind - 1];

  if (kind == FT_ORC)
    fi = lorchl ? LT_ORH : LT_ORL;

  memcpy (loadedf[fi], ldfile, 8);
  memcpy (loadedc[fi], ldcmnt, 37);
}

/* 
*/

/*
   =============================================================================
	lcancel() -- cancel librarian selections
   =============================================================================
*/

short
lcancel (lct)
     short lct;
{
  short rc;

  rc = FALSE;

  if ((lct != 0) && lselsw)
    {

      rc = TRUE;
      clrlsel ();
    }

  if ((lct != 1) && (lstrsw || (! ckstor ())))
    {

      rc = TRUE;
      streset ();
    }

  if ((lct != 2) && ldelsw)
    {

      rc = TRUE;
      ldelsw = FALSE;
      dslslot (ldslot, exp_c (ldbox[1][4]), ldrow);
    }

  return (rc);
}

/* 
*/

/*
   =============================================================================
	dpy_scr() -- display score contents entry
   =============================================================================
*/

dpy_scr (color, ns)
     unsigned color;
     short ns;
{
  char buf[40];
  long scl;

  if (ndisp != 0)
    return;

  if (v_regs[5] & 0x0180)
    vbank (0);

  if (ldmap[ns] == - 1)
    strcpy (buf, "  ");
  else
    sprintf (buf, "%02d", 1 + ldmap[ns]);

  vcputsv (librob, 64, ldbox[1][4], ldbox[1][5], 1 + ns, 1, buf, 14);

  if (-1L == (scl = sindex[ns].sclen))
    strcpy (buf, "{ empty score  }      0");
  else
    sprintf (buf, "%-16.16s  %5ld", sindex[ns].scfnm, scl);

  vcputsv (librob, 64, color, ldbox[1][5], 1 + ns, 4, buf, 14);
}

/* 
*/

/*
   =============================================================================
	lst_scr() -- list the score contents directory
   =============================================================================
*/

lst_scr ()
{
  register short i;
  unsigned cx;

  if (ndisp != 0)
    return;

  point = ldpoint;

  cx = exp_c (ldbox[0][5]);

  if (v_regs[5] & 0x0180)
    vbank (0);

  vbfill4 (librob, 128, ldbox[0][0], ldbox[0][1],
	   ldbox[0][2], ldbox[0][3], cx);

  tsplot4 (librob, 64, ldbox[0][4], ldbox[0][6], ldbox[0][7],
	   "No Score Name       Length", 14);

  lseg (8, 13, 23, 13, LUNDRLN);
  lseg (32, 13, 159, 13, LUNDRLN);
  lseg (168, 13, 215, 13, LUNDRLN);

  cx = exp_c (ldbox[1][5]);

  vbfill4 (librob, 128, ldbox[1][0], ldbox[1][1],
	   ldbox[1][2], ldbox[1][3], cx);

  for (i = 0; i < N_SCORES; i++)
    dpy_scr (ldbox[1][4], i);
}

/* 
*/

/*
   =============================================================================
	ndx_scr() -- display the table of contents for a score
   =============================================================================
*/

short
ndx_scr (slot)
     register short slot;
{
  register FILE *fp;
  register short i;
  register long rlen;
  long rdlen;
  char msgbuf1[64];
  char msgbuf2[64];

  ldpass = 0;

  for (i = 0; i < N_SCORES; i++)
    {

      sindex[i].sclen = -1L;
      memset (sindex[i].scfnm, ' ', 16);
    }

  errno = 0;

  if ((FILE *) NULL == (fp = fopenb (slotnam (slot, FT_SCR), "r")))
    {

      sprintf (msgbuf2, "  errno = %d", errno);

      ldermsg ("Couldn't open the file",
	       "  for the scores", msgbuf2, LD_EMCF, LD_EMCB);

      clrlsel ();
      return (FAILURE);
    }

  errno = 0;

  if (fseek (fp, 60L, 1))
    {				/* seek past header */

      sprintf (msgbuf2, "  errno = %d", errno);

      ldermsg ("Seek failure", (char *) NULL, msgbuf2, LD_EMCF, LD_EMCB);


#if DEBUGIT
      if (debugsw)
	FILEpr (fp);
#endif

      fclose (fp);
      postio ();		/* restore LCD backlight */
      clrlsel ();
      return (FAILURE);
    }

/* 
*/
#if DEBUGIT
  if (debugsw)
    FILEpr (fp);
#endif

  for (i = 0; i < N_SCORES; i++)
    {

      if (rd_ec (fp, &rdlen, 4L))
	{

	  sprintf (msgbuf1, "  of score %d", i + 1);
	  sprintf (msgbuf2, "  errno = %d", errno);

	  ldermsg ("Unable to read the length",
		   msgbuf1, msgbuf2, LD_EMCF, LD_EMCB);


#if DEBUGIT
	  if (debugsw)
	    FILEpr (fp);
#endif

	  clrlsel ();
	  return (FAILURE);
	}

      sindex[i].sclen = rdlen;

      if (-1L != rdlen)
	{

	  if (rd_ec (fp, sindex[i].scfnm, 16L))
	    {

	      sprintf (msgbuf1, "  of score %d", i + 1);
	      sprintf (msgbuf2, "  errno = %d", errno);

	      ldermsg ("Unable to read the name",
		       msgbuf1, msgbuf2, LD_EMCF, LD_EMCB);


#if DEBUGIT
	      if (debugsw)
		FILEpr (fp);
#endif

	      clrlsel ();
	      return (FAILURE);
	    }

	  errno = 0;

	  if (scskip (fp, i))
	    {

	      sprintf (msgbuf1, "  score %d", i + 1);
	      sprintf (msgbuf2, "  errno=%d  rlen=%ld", errno, rlen);

	      ldermsg ("Unable to skip past",
		       msgbuf1, msgbuf2, LD_EMCF, LD_EMCB);


#if DEBUGIT
	      if (debugsw)
		FILEpr (fp);
#endif

	      fclose (fp);
	      postio ();	/* restore LCD backlight */
	      clrlsel ();
	      return (FAILURE);
	    }
	}
    }

  fclose (fp);
  postio ();			/* restore LCD backlight */
  ldpass = 1;
  lst_scr ();

/* 
*/
  point = GLCplot;
  GLCcurs (G_ON);

  if (ismode != IS_NULL)
    {				/* cancel inst. mode */

      ismode = IS_NULL;
      pkctrl = oldpk;
      sliders = oldsl;
      swpt = oldsw;
      lcdlbls ();
    }

  if (gomode != GO_NULL)
    {				/* cancel goto mode */

      gomode = GO_NULL;
      pkctrl = oldpk;
      lseg (GOTO_XL, GOTO_Y, GOTO_XR, GOTO_Y, 0);
    }

  if (asmode)
    {				/* cancel assign mode */

      asmode = 0;
      pkctrl = oldpk;
      swpt = oldsw;
      lseg (ASGN_XL, ASGN_Y, ASGN_XR, ASGN_Y, 0);
    }

  if ((pkctrl == PK_PFRM) || (pkctrl == PK_NOTE))
    oldpk = pkctrl;

  if (sliders != LS_LIBR)
    oldsl = sliders;

  oldsw = swpt;
  swpt = t_libr;
  pkctrl = PK_LIBR;
  sliders = LS_LIBR;

  lcdlbls ();
  setleds ();

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	getit() -- read selected file
   =============================================================================
*/

short
getit ()
{
  ldkind = ftkind (ldslot);

  if (ldkind == - 1)
    {

      ldermsg ("Unknown file type",
	       (char *) NULL, (char *) NULL, LD_EMCF, LD_EMCB);

      clrlsel ();
      return (FAILURE);
    }

  ldbusy ("         Reading file");
/* 
*/
  switch (ldkind)
    {

    case FT_ASG:

      if (get_asg ())
	return (FAILURE);

      break;

    case FT_ORH:
    case FT_ORL:
    case FT_ORC:

      if (get_orc (lorchl, ldkind))
	return (FAILURE);

      break;

    case FT_PAT:

      if (get_pat ())
	return (FAILURE);

      break;

    case FT_SCR:

      if (get_scr ())
	return (FAILURE);

      break;

    case FT_SEQ:

      if (get_seq ())
	return (FAILURE);

      break;

    case FT_TUN:

      if (get_tun ())
	return (FAILURE);

      break;

    case FT_WAV:

      if (get_wav ())
	return (FAILURE);

      break;

    default:

      ldermsg ("ldkind bad", (char *) NULL, (char *) NULL, LD_EMCF, LD_EMCB);

      clrlsel ();
      return (FAILURE);
    }
/* 
*/

  memcpy (ldfile, filecat[ldslot].fcname, 8);
  memcpy (ldcmnt, filecat[ldslot].fccmnt, 37);
  savefc (ldkind);

  clrlsel ();

  if (lrasw)
    {

      ldswin (0);
      ldswin (8);
    }

  ldswin (3);
  ldswin (5);
  showsiz ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ldfnbox() -- librarian display box hit processor
   =============================================================================
*/

short
ldfnbox (n)
     short n;
{
  register short col, i, slot, sn;

  col = hitcx >> 3;

  if (lderrsw)
    clrerms ();

  switch (n)
    {

    case 1:			/* index area */

      if (lcancel (0))
	return (SUCCESS);

      if (lselsw)
	{			/* something already selected ? */

	  if (lrasw)
	    {			/* content mode ? */

	      if (ldkind == FT_SCR)
		{		/* score */

		  sn = ldline (hitcy) - 1;

		  if (sindex[sn].sclen != - 1L)
		    {

		      if ((1 + sn) != ldrow)
			{

			  dpy_scr (ldbox[1][4], ldrow - 1);
			  ldrow = 1 + sn;
			}

		      dpy_scr (LD_SELC, sn);
		      lksel = sn;

		    }
		  else
		    {

		      lksel = -1;
		    }

		  return (SUCCESS);
		}
	    }

	  if (ldrow != ldline (hitcy))
	    {

	      clrlsel ();
	      return (SUCCESS);
	    }

	  return (getit ());
/* 
*/
	}
      else
	{			/* nothing selected yet */

	  if (0 != (ldrow = ldline (hitcy)))
	    {

	      if (col == 11)
		{

		  if (catin && ltagged)
		    {

		      slot = lin2slt (ldrow);

		      if (slot == tagslot)
			{

			  putcat ();
			  ltagged = FALSE;
			  showcat ();
			}
		    }

		}
	      else
		{

		  if (-1 != (ldslot = lin2slt (ldrow)))
		    {

		      lselsw = TRUE;
		      ldswin (8);
		      ldkind = ftkind (ldslot);

		      for (i = 0; i < N_SCORES; i++)
			ldmap[i] = i;

		      if (lrasw && (ldkind == FT_SCR))
			return (ndx_scr (ldslot));

		      dslslot (ldslot, exp_c (LD_SELC), ldrow);
		      return (SUCCESS);
		    }
		}
	    }

	  clrlsel ();
	  return (FAILURE);
	}
/* 
*/
    case 3:			/* file name field */

      if (lcancel (3))
	return (SUCCESS);

      if (lmwtype != 1)
	{

	  lmwvtyp ();		/* setup for the typewriter */
	  ldswin (10);		/* display the typewriter */

	  vtsetup (librob, vtdisp, 10, ldfile, 22, 33,
		   advlcur, bsplcur, nokey, nokey, endltyp,
		   ldbox[n][4], ldbox[n][5]);

	}
      else
	{

	  vtyper ();
	}

      return (SUCCESS);

    case 5:			/* comment field */

      if (lcancel (3))
	return (SUCCESS);

      if (lmwtype != 1)
	{

	  lmwvtyp ();		/* setup for the typewriter */
	  ldswin (10);		/* display the typewriter */

	  vtsetup (librob, vtdisp, 27, ldcmnt, 22, 33,
		   advlcur, bsplcur, nokey, nokey, endltyp,
		   ldbox[n][4], ldbox[n][5]);

	}
      else
	{

	  vtyper ();
	}

      return (SUCCESS);

/* 
*/

    case 6:			/* "Index" */

      if (lcancel (0))
	return (SUCCESS);

      clrlsel ();
      return (fcindex ());

    case 7:			/* "Content" */

      if (lselsw && lrasw)
	return (getit ());

      lrasw = ! lrasw;
      ldswin (7);
      return (SUCCESS);

    case 8:			/* "Hi Orch" / "Lo Orch" */

      lorchl = ! lorchl;
      ldswin (8);
      return (SUCCESS);

/* 
*/
    case 9:			/* "Store" */

      if (lcancel (1))
	return (SUCCESS);

      if (cyval < 321)
	{

	  /* row 22:  "Store", "Score", or "Hi Orch" */

	  if (cxval < 120)
	    {

	      /* "Store" */

	      if (lstrsw)
		{

		  storit ();

		}
	      else
		{

		  lstrsw = TRUE;
		  ldswin (9);
		}

	      return (SUCCESS);

	    }
	  else if ((cxval > 135) && (cxval < 176))
	    {

	      /* "Score" */

	      lscrsw = ! lscrsw;
	      ldswin (9);

	      if (lstrsw)
		storit ();

	      return (SUCCESS);

	    }
	  else if (cxval > 191)
	    {

	      /* "Hi Orch" */

	      lorchsw = ! lorchsw;
	      ldswin (9);

	      if (lstrsw)
		storit ();

	      return (SUCCESS);
	    }
/* 
*/
	}
      else if ((cyval > 321) && (cyval < 335))
	{

	  /* row 23:  "Waves",  "Patch",  or "Lo Orch" */

	  if (cxval < 120)
	    {			/* "Waves" */

	      lwavsw = ! lwavsw;
	      ldswin (9);

	      if (lstrsw)
		storit ();

	      return (SUCCESS);

	    }
	  else if ((cxval > 135) && (cxval < 176))
	    {

	      /* "Patch" */

	      lpatsw = ! lpatsw;
	      ldswin (9);

	      if (lstrsw)
		storit ();

	      return (SUCCESS);

	    }
	  else if (cxval > 191)
	    {			/* "Lo Orch" */

	      lorclsw = ! lorclsw;
	      ldswin (9);

	      if (lstrsw)
		storit ();

	      return (SUCCESS);
	    }
/* 
*/
	}
      else if (cyval > 335)
	{

	  /* row 24:  "Assgn", "Seqnc", or "Tunings" */

	  if (cxval < 120)
	    {			/* "Assgn" */

	      lasgsw = ! lasgsw;
	      ldswin (9);

	      if (lstrsw)
		storit ();

	      return (SUCCESS);

	    }
	  else if ((cxval > 135) && (cxval < 176))
	    {

	      /* "Seqnc" */

	      lseqsw = ! lseqsw;
	      ldswin (9);

	      if (lstrsw)
		storit ();

	      return (SUCCESS);

	    }
	  else if (cxval > 191)
	    {			/* "Tunings" */

	      ltunsw = ! ltunsw;
	      ldswin (9);

	      if (lstrsw)
		storit ();

	      return (SUCCESS);
	    }
	}

      return (FAILURE);

    default:

      lcancel (3);
      return (FAILURE);
    }
}
