/*
   =============================================================================
	librw.c -- MIDAS librarian read / write functions
	Version 22 -- 1988-11-18 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "biosdefs.h"
#include "memory.h"
#include "stdio.h"
#include "errno.h"

#include "ascii.h"
#include "charset.h"
#include "fpu.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"
#include "graphdef.h"
#include "charset.h"
#include "fields.h"
#include "patch.h"
#include "score.h"
#include "scfns.h"

#include "midas.h"
#include "asgdsp.h"
#include "instdsp.h"
#include "libdsp.h"
#include "wsdsp.h"

/* 
*/

#if	DEBUGIT
extern short debugsw;
#endif

extern char *slotnam ();

extern long chksum ();

extern short ldslot, lderrsw, lrasw;
extern short tunlib[][128];

extern long lcsum, schksum, sntlreq;

extern char tunname[][32];
extern char ldfile[9];
extern char ldcmnt[38];

extern struct mlibhdr ldhead;
extern struct asgent asgtab[];
extern struct wstbl wslib[];
extern struct instdef idefs[];
extern struct s_time stimes[N_SCORES][N_SECTS];

/* 
*/

/*
   =============================================================================
	wrt_asg() -- write an assignment on the disk
   =============================================================================
*/

short
wrt_asg (slot)
     short slot;
{
  register FILE *fp;
  register int i;
  char cstemp[8];

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (slot, FT_ASG), "w");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't create a file",
	       " for the assignments", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      streset ();
      return (FAILURE);
    }

  makelh (FT_ASG);		/* make header */

  for (i = 0; i < NASGLIB; i++)
    lcsum += chksum (&asgtab[i + 1], (long) (sizeof (struct asgent)));

  sprintf (cstemp, "%08.8lX", lcsum);
  memcpy (ldhead.l_csum, cstemp, 8);

#if	DEBUGIT
  if (debugsw)
    printf ("wrt_asg():  hdr=[%-.56s]\n", &ldhead);
#endif

/* 
*/
  if (wr_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      streset ();
      return (FAILURE);
    }

  for (i = 0; i < NASGLIB; i++)
    {

      if (wr_ec (fp, &asgtab[i + 1], (long) (sizeof (struct asgent))))
	{

	  streset ();
	  return (FAILURE);
	}
    }

  fclose (fp);
  postio ();			/* restore LCD backlight */
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	get_asg() -- read an assignment library from the disk
   =============================================================================
*/

short
get_asg ()
{
  register FILE *fp;
  register int i;

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (ldslot, FT_ASG), "r");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't open the file",
	       " for the assignments", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      clrlsel ();
      return (FAILURE);
    }

  memcpy (ldfile, "        ", 8);
  memcpy (ldcmnt, "                                     ", 37);
  ldswin (3);
  ldswin (5);

/* 
*/
  if (rd_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      clrlsel ();
      return (FAILURE);
    }

#if	DEBUGIT
  if (debugsw)
    printf ("get_asg():  hdr=[%-.56s]\n", &ldhead);
#endif


  for (i = 0; i < NASGLIB; i++)
    {

      if (rd_ec (fp, &asgtab[i + 1], (long) (sizeof (struct asgent))))
	{

	  clrlsel ();
	  return (FAILURE);
	}
    }

  clrlsel ();
  fclose (fp);
  postio ();			/* restore LCD backlight */
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	wrt_orc() -- write an orchestra on the disk
   =============================================================================
*/

short
wrt_orc (slot, lorh)
     short slot, lorh;
{
  register FILE *fp;
  register int i;
  register struct instdef *ip;
  char cstemp[8];

  if (lorh)
    ldbusy ("       Writing Hi Orch");
  else
    ldbusy ("       Writing Lo Orch");

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (slot, FT_ORC), "w");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't create a file",
	       " for the orchestra", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      streset ();
      return (FAILURE);
    }

  makelh (FT_ORC);		/* make header */

  for (i = 0; i < NINORC; i++)
    {

      ip = &idefs[i + 1 + (lorh ? NINORC : 0)];

      lcsum += chksum (ip, (long) OR_LEN1);
      lcsum += chksum (ip->idhwvao, (long) OR_LEN2);
      lcsum += chksum (ip->idhwvbo, (long) OR_LEN2);
    }

  sprintf (cstemp, "%08.8lX", lcsum);
  memcpy (ldhead.l_csum, cstemp, 8);

#if	DEBUGIT
  if (debugsw)
    printf ("wrt_orc():  hdr=[%-.56s]\n", &ldhead);
#endif

/* 
*/
  if (wr_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      streset ();
      return (FAILURE);
    }

  for (i = 0; i < NINORC; i++)
    {

      ip = &idefs[i + 1 + (lorh ? NINORC : 0)];

      if (wr_ec (fp, ip, (long) OR_LEN1))
	{			/* functions */

	  streset ();
	  return (FAILURE);
	}

      if (wr_ec (fp, ip->idhwvao, (long) OR_LEN2))
	{			/* WS A */

	  streset ();
	  return (FAILURE);
	}

      if (wr_ec (fp, ip->idhwvbo, (long) OR_LEN2))
	{			/* WS B */

	  streset ();
	  return (FAILURE);
	}
    }

  fclose (fp);
  postio ();			/* restore LCD backlight */
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	get_orc() -- read an orchestra from the disk
   =============================================================================
*/

short
get_orc (lorh, kind)
     short lorh, kind;
{
  register FILE *fp;
  register int i;
  register struct instdef *ip;

  if (lorh)
    ldbusy ("       Reading Hi Orch");
  else
    ldbusy ("       Reading Lo Orch");

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (ldslot, kind), "r");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't open the file",
	       " for the orchestra", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      clrlsel ();
      return (FAILURE);
    }

  memcpy (ldfile, "        ", 8);
  memcpy (ldcmnt, "                                     ", 37);
  ldswin (3);
  ldswin (5);

/* 
*/
  if (rd_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      clrlsel ();
      return (FAILURE);
    }

#if	DEBUGIT
  if (debugsw)
    printf ("get_orc():  hdr=[%-.56s]\n", &ldhead);
#endif


  for (i = 0; i < NINORC; i++)
    {

      ip = &idefs[i + 1 + (lorh ? NINORC : 0)];

      if (rd_ec (fp, ip, (long) OR_LEN1))
	{			/* functions */

	  clrlsel ();
	  return (FAILURE);
	}

      if (rd_ec (fp, ip->idhwvao, (long) OR_LEN2))
	{			/* WS A */

	  clrlsel ();
	  return (FAILURE);
	}

      if (rd_ec (fp, ip->idhwvbo, (long) OR_LEN2))
	{			/* WS B */

	  clrlsel ();
	  return (FAILURE);
	}

      /* unpack offsets (and eventually harmonics) into finals */

      memcpyw (ip->idhwvaf, ip->idhwvao, NUMWPNT);
      memcpyw (ip->idhwvbf, ip->idhwvbo, NUMWPNT);
    }

  clrlsel ();
  fclose (fp);
  postio ();			/* restore LCD backlight */
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	wrt_tun() -- write a tuning library on the disk
   =============================================================================
*/

short
wrt_tun (slot)
     short slot;
{
  register FILE *fp;
  register int i;
  char cstemp[8];

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (slot, FT_TUN), "w");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't create a file",
	       " for the tunings", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      streset ();
      return (FAILURE);
    }

  makelh (FT_TUN);		/* make header */

  for (i = 0; i < NTUNSLIB; i++)
    {

      lcsum += chksum (&tunlib[i + 1], 256L);
      lcsum += chksum (&tunname[i + 1], 32L);
    }

  sprintf (cstemp, "%08.8lX", lcsum);
  memcpy (ldhead.l_csum, cstemp, 8);

#if	DEBUGIT
  if (debugsw)
    printf ("wrt_tun():  hdr=[%-.56s]\n", &ldhead);
#endif

/* 
*/
  if (wr_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      streset ();
      return (FAILURE);
    }

  for (i = 0; i < NTUNSLIB; i++)
    {

      if (wr_ec (fp, &tunlib[i + 1], 256L))
	{

	  streset ();
	  return (FAILURE);
	}

      if (wr_ec (fp, &tunname[i + 1], 32L))
	{

	  streset ();
	  return (FAILURE);
	}
    }

  fclose (fp);
  postio ();			/* restore LCD backlight */
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	get_tun() -- read a tuning library from the disk
   =============================================================================
*/

short
get_tun ()
{
  register FILE *fp;
  register int i;

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (ldslot, FT_TUN), "r");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't open the file",
	       " for the tunings", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      clrlsel ();
      return (FAILURE);
    }

  memcpy (ldfile, "        ", 8);
  memcpy (ldcmnt, "                                     ", 37);
  ldswin (3);
  ldswin (5);

/* 
*/
  if (rd_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      clrlsel ();
      return (FAILURE);
    }

#if	DEBUGIT
  if (debugsw)
    printf ("get_tun():  hdr=[%-.56s]\n", &ldhead);
#endif


  for (i = 0; i < NTUNSLIB; i++)
    {

      if (rd_ec (fp, &tunlib[i + 1], 256L))
	{

	  clrlsel ();
	  return (FAILURE);
	}

      if (rd_ec (fp, &tunname[i + 1], 32L))
	{

	  clrlsel ();
	  return (FAILURE);
	}
    }

  clrlsel ();
  fclose (fp);
  postio ();			/* restore LCD backlight */
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	wrt_pat() -- write a patch file on the disk
   =============================================================================
*/

short
wrt_pat (slot)
     short slot;
{
  register FILE *fp;
  char cstemp[8];

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (slot, FT_PAT), "w");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't create a file",
	       " for the patches", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      streset ();
      return (FAILURE);
    }

  makelh (FT_PAT);		/* make header */

  sprintf (cstemp, "%08.8lX", lcsum);
  memcpy (ldhead.l_csum, cstemp, 8);

#if	DEBUGIT
  if (debugsw)
    printf ("wrt_pat():  hdr=[%-.56s]\n", &ldhead);
#endif

/* 
*/
  if (wr_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      streset ();
      return (FAILURE);
    }

  if (ptwrite (fp))
    {				/* patches */

      streset ();
      return (FAILURE);
    }

  fclose (fp);
  postio ();			/* restore LCD backlight */
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	get_pat() -- read a patch file from the disk
   =============================================================================
*/

short
get_pat ()
{
  register FILE *fp;

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (ldslot, FT_PAT), "r");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't open the file",
	       " for the patches", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      clrlsel ();
      return (FAILURE);
    }

  memcpy (ldfile, "        ", 8);
  memcpy (ldcmnt, "                                     ", 37);
  ldswin (3);
  ldswin (5);

/* 
*/
  if (rd_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      clrlsel ();
      return (FAILURE);
    }

#if	DEBUGIT
  if (debugsw)
    printf ("get_pat():  hdr=[%-.56s]\n", &ldhead);
#endif

  if (lrasw EQ 0)		/* clear all patches if in replace mode */
    initpt ();

  if (ptread (fp))
    {

      clrlsel ();
      return (FAILURE);
    }

  fclose (fp);
  postio ();			/* restore LCD backlight */
  clrlsel ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	wrt_scr() -- write a score file on the disk
   =============================================================================
*/

short
wrt_scr (slot)
     short slot;
{
  register FILE *fp;
  register int i;
  long tnb;
  char cstemp[8];

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (slot, FT_SCR), "w");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't create a file",
	       " for the scores", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      streset ();
      return (FAILURE);
    }

  makelh (FT_SCR);		/* make header */

  lcsum += sntlreq;

  sprintf (cstemp, "%08.8lX", lcsum);
  memcpy (ldhead.l_csum, cstemp, 8);

#if	DEBUGIT
  if (debugsw)
    printf ("wrt_scr():  hdr=[%-.56s]\n", &ldhead);
#endif

/* 
*/
  if (wr_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      streset ();
      return (FAILURE);
    }

  if (wr_ec (fp, &sntlreq, 4L))
    {				/* total longs required */

      streset ();
      return (FAILURE);
    }

  for (i = 0; i < N_SCORES; i++)
    {				/* scores */

      if (scwrite (i, fp))
	{

	  streset ();
	  return (FAILURE);
	}
    }

  fclose (fp);
  postio ();			/* restore LCD backlight */
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	get_scr() -- read a score file from the disk
   =============================================================================
*/

short
get_scr ()
{
  register FILE *fp;
  register int i;
  long tnl;

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (ldslot, FT_SCR), "r");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't open the file",
	       " for the scores", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      clrlsel ();
      return (FAILURE);
    }

  memcpy (ldfile, "        ", 8);
  memcpy (ldcmnt, "                                     ", 37);
  ldswin (3);
  ldswin (5);

/* 
*/
  if (rd_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      clrlsel ();
      return (FAILURE);
    }

#if	DEBUGIT
  if (debugsw)
    printf ("get_scr():  hdr=[%-.56s]\n", &ldhead);
#endif


  if (rd_ec (fp, &tnl, 4L))
    {				/* longs required */

      clrlsel ();
      return (FAILURE);
    }

  if (lrasw EQ 0)		/* clear all scores if in replace mode */
    scinit ();

  for (i = 0; i < N_SCORES; i++)
    {				/* read scores */

      if (scread (i, fp))
	{

	  clrlsel ();
	  return (FAILURE);
	}
    }

  fclose (fp);
  postio ();			/* restore LCD backlight */

  p_bak = p_cur = p_ctr = p_fwd = scp = E_NULL;
  t_cur = t_ctr = 0L;
  t_bak = t_cur - TO_BAK;
  t_fwd = t_cur + TO_FWD;

  selscor (0);

  clrlsel ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	wrt_seq() -- write a sequence file on the disk
   =============================================================================
*/

short
wrt_seq (slot)
     short slot;
{
  register FILE *fp;
  char cstemp[8];

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (slot, FT_SEQ), "w");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't create a file",
	       " for the sequences", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      streset ();
      return (FAILURE);
    }

  makelh (FT_SEQ);		/* make header */

  sprintf (cstemp, "%08.8lX", lcsum);
  memcpy (ldhead.l_csum, cstemp, 8);

#if	DEBUGIT
  if (debugsw)
    printf ("wrt_seq():  hdr=[%-.56s]\n", &ldhead);
#endif

/* 
*/
  if (wr_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      streset ();
      return (FAILURE);
    }

  if (sqwrite (fp))
    {				/* sequences */

      streset ();
      return (FAILURE);
    }

  fclose (fp);
  postio ();			/* restore LCD backlight */
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	get_seq() -- read a sequence file from the disk
   =============================================================================
*/

short
get_seq ()
{
  register FILE *fp;

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (ldslot, FT_SEQ), "r");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't open the file",
	       " for the sequences", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      clrlsel ();
      return (FAILURE);
    }

  memcpy (ldfile, "        ", 8);
  memcpy (ldcmnt, "                                     ", 37);
  ldswin (3);
  ldswin (5);

/* 
*/
  if (rd_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      clrlsel ();
      return (FAILURE);
    }

#if	DEBUGIT
  if (debugsw)
    printf ("get_seq():  hdr=[%-.56s]\n", &ldhead);
#endif

  initsq ();

  if (sqread (fp))
    {

      clrlsel ();
      return (FAILURE);
    }

  fclose (fp);
  postio ();			/* restore LCD backlight */
  clrlsel ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	wrt_wav() -- write a waveshape library on the disk
   =============================================================================
*/

short
wrt_wav (slot)
     short slot;
{
  register FILE *fp;
  register int i;
  char cstemp[8];
  register struct wstbl *wp;

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (slot, FT_WAV), "w");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't create a file",
	       " for the waveshapes", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      streset ();
      return (FAILURE);
    }

  makelh (FT_WAV);		/* make header */

  for (i = 0; i < NUMWAVS; i++)
    {

      wp = &wslib[i];

      lcsum += chksum (wp->offset, (long) (NUMWPNT * 2));
      lcsum += chksum (wp->harmon, (long) (NUMHARM * 2));
    }

  sprintf (cstemp, "%08.8lX", lcsum);
  memcpy (ldhead.l_csum, cstemp, 8);

#if	DEBUGIT
  if (debugsw)
    printf ("wrt_wav():  hdr=[%-.56s]\n", &ldhead);
#endif

/* 
*/
  if (wr_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      streset ();
      return (FAILURE);
    }

  for (i = 0; i < NUMWAVS; i++)
    {

      wp = &wslib[i];

      if (wr_ec (fp, wp->offset, (long) (NUMWPNT * 2)))
	{

	  streset ();
	  return (FAILURE);
	}

      if (wr_ec (fp, wp->harmon, (long) (NUMHARM * 2)))
	{

	  streset ();
	  return (FAILURE);
	}
    }

  fclose (fp);
  postio ();			/* restore LCD backlight */
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	get_wav() -- read a waveshape library from the disk
   =============================================================================
*/

short
get_wav ()
{
  register FILE *fp;
  register int i;
  register struct wstbl *wp;

  preio ();			/* kill LCD backlight */

  fp = fopenb (slotnam (ldslot, FT_WAV), "r");

  if ((FILE *) NULL EQ fp)
    {

      ldermsg ("Couldn't open the file",
	       " for the waveshapes", (char *) NULL, LD_EMCF, LD_EMCB);

      postio ();		/* restore LCD backlight */
      clrlsel ();
      return (FAILURE);
    }

  memcpy (ldfile, "        ", 8);
  memcpy (ldcmnt, "                                     ", 37);
  ldswin (3);
  ldswin (5);

/* 
*/
  if (rd_ec (fp, &ldhead, (long) LH_LEN))
    {				/* header */

      clrlsel ();
      return (FAILURE);
    }

#if	DEBUGIT
  if (debugsw)
    printf ("get_wav():  hdr=[%-.56s]\n", &ldhead);
#endif


  for (i = 0; i < NUMWAVS; i++)
    {

      wp = &wslib[i];

      if (rd_ec (fp, wp->offset, (long) (NUMWPNT * 2)))
	{

	  clrlsel ();
	  return (FAILURE);
	}

      if (rd_ec (fp, wp->harmon, (long) (NUMHARM * 2)))
	{

	  clrlsel ();
	  return (FAILURE);
	}

      /* unpack offsets (and eventually harmonics) into finals */

      memcpyw (wp->final, wp->offset, NUMWPNT);
    }

  clrlsel ();
  fclose (fp);
  postio ();			/* restore LCD backlight */
  return (SUCCESS);
}
