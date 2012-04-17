/*
   =============================================================================
	sqexec.c -- MIDAS-VII sequence action execution code
	Version 12 -- 1988-12-13 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGSX		0
#define	DEBUGSP		0

#define	UPD_LINE	1

#include "stddefs.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "wordq.h"
#include "patch.h"
#include "timers.h"

#include "midas.h"
#include "ptdisp.h"

#if	(DEBUGSP|DEBUGSX)
extern short debugsw;
#endif

#if	DEBUGSP
short debugsp = 1;
#endif

#if	DEBUGSX
short debugsx = 1;
#endif

extern unsigned short setsr ();
extern long rand24 ();

extern char trgtab[];

extern short curslin;
extern short dsp_ok;
extern short ndisp;

extern unsigned short seqdupd;

extern unsigned short *obj10;

extern unsigned short cg3[];

extern struct wordq ptefifo;

short seqdspn;

short rtab[] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F,
  0x003F, 0x003F, 0x003F
};

/* 
*/

/*
   =============================================================================
	evaltv() -- evaluate a typed value
   =============================================================================
*/

unsigned short
evaltv (dat)
     register unsigned short dat;
{
  switch (SQ_MTYP & dat)
    {

    case SQ_REG:		/* register contents */

      return (sregval[SQ_MVAL & dat]);

    case SQ_VAL:		/* constant value */

      return (SQ_MVAL & dat);

    case SQ_VLT:		/* voltage input */

      return (0);

    case SQ_RND:		/* random number */

      return ((short) rand24 () & rtab[SQ_MVAL & dat]);

    default:			/* something weird got in here ... */

      return (0);
    }
}

/* 
*/

/*
   =============================================================================
	dosqact() -- do a sequence action
   =============================================================================
*/

short
dosqact (seq, act, dat)
     unsigned short seq;
     register unsigned short act, dat;
{
  register unsigned short obj, val;
  register short sv;

  obj = (SQ_MOBJ & act) >> 8;
  val = SQ_MVAL & dat;

  switch (SQ_MACT & act)
    {

    case SQ_NULL:		/* null action */

      return (0);

    case SQ_CKEY:		/* Key closure */

      putwq (&ptefifo, dat & TRG_MASK);
      return (0);

    case SQ_RKEY:		/* Key release */

      putwq (&ptefifo, dat | 0x8000);
      return (0);

    case SQ_TKEY:		/* Key transient */

      putwq (&ptefifo, dat & TRG_MASK);	/* closure */
      putwq (&ptefifo, dat | 0x8000);	/* release */
      return (0);

    case SQ_IKEY:		/* If key active */

      if (trgtab[TRG_MASK & dat])
	return (0);
      else
	return (1);
/* 
*/
    case SQ_STRG:		/* Trigger on */

      trstate[val] = 1;
      putwq (&ptefifo, (0x1100 | val));
      seqdupd |= (1 << val);
      return (0);

    case SQ_CTRG:		/* Trigger off */

      trstate[val] = 0;
      seqdupd |= (1 << val);
      return (0);

    case SQ_TTRG:		/* Trigger toggle */

      trstate[val] = trstate[val] ? 0 : 1;

      if (trstate[val])
	putwq (&ptefifo, (0x1100 | val));

      seqdupd |= (1 << val);
      return (0);

    case SQ_ITRG:		/* If trigger active */

      return (trstate[val] ? 0 : 1);
/* 
*/
    case SQ_SREG:		/* Set register */

      sregval[obj] = evaltv (dat);
      seqdupd |= (1 << obj);
      return (0);

    case SQ_AREG:		/* Increment register */

      if (dat & SQ_MFLG)
	{

	  sv = sregval[obj] - evaltv (dat);

	  if (sv < 0)
	    sv = 0;

	  sregval[obj] = sv;

	}
      else
	{

	  sv = sregval[obj] + evaltv (dat);

	  if (sv > 99)
	    sv = 99;

	  sregval[obj] = sv;

	}

      seqdupd |= (1 << obj);
      return (0);

    case SQ_IREQ:		/* If register = */

      val = evaltv (dat);

      if (sregval[obj] == val)
	return (0);
      else
	return (1);

    case SQ_IRLT:		/* If register < */

      val = evaltv (dat);

      if (sregval[obj] < val)
	return (0);
      else
	return (1);

    case SQ_IRGT:		/* If register > */

      val = evaltv (dat);

      if (sregval[obj] > val)
	return (0);
      else
	return (1);
/* 
*/
    case SQ_ISTM:		/* If stimulus active */

      if (trgtab[TRG_MASK & seqstim[seq]])
	return (0);
      else
	return (1);

    case SQ_JUMP:		/* Jump to sequence line */

      seqline[seq] = dat;
      seqtime[seq] = seqtab[dat].seqtime;
      seqflag[seq] |= SQF_CLK;

      seqdupd |= (1 << seq);
      return (-1);


    case SQ_STOP:		/* Stop sequence */
    default:

      seqflag[seq] = 0;
      seqtime[seq] = 0;

      seqdupd |= (1 << seq);
      return (-1);

    }
}

/* 
*/

/*
   =============================================================================
	sqexec() -- execute a line for a sequence
   =============================================================================
*/

sqexec (seq)
     register unsigned short seq;
{
  register unsigned short act, dat, line;
  register struct seqent *sp;
  register short rc;

  line = seqline[seq];
  sp = &seqtab[line];

  act = sp->seqact1;		/* do Action 1 */
  dat = sp->seqdat1;

  rc = dosqact (seq, act, dat);

#if	DEBUGSX
  if (debugsw && debugsx)
    printf ("sqexec(%02u):  Line %03u  Act 1 $%04.4X $%04.4X  $%04.4X %d\n",
	    seq, line, act, dat, seqflag[seq], rc);
#endif

  if (rc == 1)			/* skip action 2 */
    goto act3;
  else if (rc == - 1)		/* jump or stop */
    return;

  act = sp->seqact2;		/* do Action 2 */
  dat = sp->seqdat2;

  rc = dosqact (seq, act, dat);

#if	DEBUGSX
  if (debugsw && debugsx)
    printf ("sqexec(%02u):  Line %03u  Act 2 $%04.4X $%04.4X  $%04.4X %d\n",
	    seq, line, act, dat, seqflag[seq], rc);
#endif

  if (rc == 1)			/* skip action 3 */
    goto nxtline;
  else if (rc == - 1)		/* jump or stop */
    return;

act3:
  act = sp->seqact3;		/* do Action 3 */
  dat = sp->seqdat3;

  rc = dosqact (seq, act, dat);

#if	DEBUGSX
  if (debugsw && debugsx)
    printf ("sqexec(%02u):  Line %03u  Act 3 $%04.4X $%04.4X  $%04.4X %d\n",
	    seq, line, act, dat, seqflag[seq], rc);
#endif

  if (rc == - 1)		/* jump or stop */
    return;

nxtline:			/* increment line counter */

  if (++seqline[seq] >= NSLINES)
    seqline[seq] = 0;

  seqtime[seq] = seqtab[seqline[seq]].seqtime;
  seqflag[seq] |= SQF_CLK;

#if	DEBUGSX
  if (debugsw && debugsx)
    printf ("sqexec(%02u):  Next %03u  %5u  $%04.4X\n",
	    seq, line, seqtime[seq], seqflag[seq]);
#endif

  seqdupd |= (1 << seq);
}

/* 
*/

/*
   =============================================================================
	seqproc() -- process sequences
   =============================================================================
*/

seqproc ()
{
  register unsigned short oldsr, seq;
  register short dspn;
  register unsigned short *fp;
  char linbuf[66];

  if (0 == timers[SQTIMER])
    {

      for (seq = 0; seq < 16; seq++)
	{

	  fp = &seqflag[seq];

	  if ((SQF_RUN | SQF_CLK) == ((SQF_RUN | SQF_CLK) & *fp))
	    {

	      if (seqtime[seq])
		{

		  if (0 EQ-- seqtime[seq])
		    *fp &= ~SQF_CLK;

		}
	      else
		{

		  *fp &= ~SQF_CLK;
		}
	    }
	}

      oldsr = setsr (0x2700);
      timers[SQTIMER] = SEQTIME;
      setsr (oldsr);
    }

  for (seq = 0; seq < 16; seq++)
    if (SQF_RUN == ((SQF_RUN | SQF_CLK) & seqflag[seq]))
      sqexec (seq);
/* 
*/
  if (((ndisp == 1) || (ndisp == 3)) && dsp_ok && seqdupd)
    {

#if	DEBUGSX
      if (debugsw && debugsx)
	printf ("seqproc():  ndisp = %d  seqdupd = $%04.4X\n",
		ndisp, seqdupd);
#endif
      if (seqdupd & (1 << seqdspn))
	{

	  dspn = seqdspn;

	  if (v_regs[5] & 0x0180)
	    vbank (0);

	  sprintf (linbuf, "    %03d %02d %c ",
		   seqline[dspn], sregval[dspn], '0' + trstate[dspn]);

	  vvputsv (obj10, 16, PDSEQFG, PDSEQBG, dspn, 1, linbuf, 14, 14, cg3);

#if	UPD_LINE
	  sprintf (linbuf, "%02d", dspn + 1);

	  vvputsv (obj10, 16,
		   (seqflag[dspn] & SQF_RUN) ?
		   PDSEQRN : PDSEQFG, PDSEQBG, dspn, 2, linbuf, 14, 14, cg3);
#else
	  vsetcv (obj10, dspn, 2,
		  (((seqflag[dspn] & SQF_RUN) ?
		    PDSEQRN : PDSEQFG) << 4) | PDSEQBG, 16);
#endif
	  seqdupd &= ~(1 << dspn);
	}

      if (++seqdspn > 15)
	seqdspn = 0;
    }
}
