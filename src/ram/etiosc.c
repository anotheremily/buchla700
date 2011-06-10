/*
   =============================================================================
	etiosc.c -- MIDAS instrument editor -- oscillator field handlers
	Version 31 -- 1988-10-27 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"
#include "charset.h"

#include "midas.h"
#include "instdsp.h"

/* functions defined elsewhere */

extern int advicur (), dosync ();

extern char *strcpy ();

/* variables defined elsewhere */

extern short stcrow, stccol, curvce;

extern short idbox[][8];

extern unsigned *instob;

extern char dspbuf[];

extern struct instdef vbufs[];

extern short cents, bform;

/* 
*/

/* initialized variables */

/*		    C  C# D  D# E  F  F# G  G# A  A# B	*/
short notes[] = { 2, 2, 3, 3, 4, 5, 5, 6, 6, 0, 0, 1 };

/*		        A     B     C     D     E     F     G	*/
short pitches[] = { 900, 1100, 0, 200, 400, 500, 700 };

#include "ratio.h"		/* short ratio[]; */

/*		    C  C# D  D# E  F  F# G  G# A  A# B	*/
short sharps[] = { 7, 9, 7, 9, 7, 7, 9, 7, 9, 7, 9, 7 };

short shrpflt[] = { 0, -100, 100 };

char sfdsp[] = { ' ', D_FLAT, D_SHARP };

static char intstr[] = "+0000";
static char ratstr[] = "1/1";
static char frqstr[] = "00.0";
static char pchstr[] = "0C 00";

short ndvals[10] = { 0, 0, 1200, 1902, 2400, 2786, 3102, 3369, 3600, 3804 };


/* 
*/

/*
   =============================================================================
	int2rat() -- convert interval to ratio
   =============================================================================
*/

int2rat (rat)
     short rat;
{
  register short den, inum, num;

  ebuf[0] = '1';
  ebuf[1] = '/';
  ebuf[2] = '1';
  ebuf[3] = '\0';

  for (num = 1; num < 10; num++)
    {

      inum = 10 * num;

      for (den = 1; den < 10; den++)
	{

	  if (rat EQ ratio[inum + den])
	    {

	      ebuf[0] = num + '0';
	      ebuf[2] = den + '0';
	      return;
	    }
	}
    }
}

/* 
*/

/*
   =============================================================================
	cnvc2p() -- convert cents to pitch
   =============================================================================
*/

cnvc2p (buf, cv)
     char *buf;
     short cv;
{
  short rem, tmp;

  cv -= 160;
  buf[0] = cv / 1200;
  rem = cv - (buf[0] * 1200);
  tmp = rem / 100;
  rem -= (tmp * 100);
  buf[1] = notes[tmp];
  buf[2] = sharps[tmp];
  buf[3] = rem / 10;
  buf[4] = rem - (buf[3] * 10);
}

/* 
*/

/*
   =============================================================================
	cnvp2c() -- convert pitch to cents

	input in edit buffer:

		ebuf[0] = octave		0..9
		ebuf[1] = note			A..G  (0..6)
		ebuf[2] = type			natural, flat, sharp  (7..9)
		ebuf[3] = ms byte of offset	0..9
		ebuf[4] = ls byte of offset	0..9

	output in cents
   =============================================================================
*/

short
cnvp2c ()
{
  if (ebuf[0] EQ 9)		/* high limit is C9 00 */
    if (ebuf[1] GT 2)
      return (FAILURE);
    else if (ebuf[1] EQ 2)
      if (ebuf[2] NE 7)
	return (FAILURE);
      else if (ebuf[3] OR ebuf[4])
	return (FAILURE);

  cents = (ebuf[0] * 1200) + pitches[ebuf[1]] + shrpflt[ebuf[2] - 7]
    + (ebuf[3] * 10) + ebuf[4] + 160;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	et_iosc() -- load the edit buffer
   =============================================================================
*/

short
et_iosc (nn)
     short nn;
{
  register struct instdef *ip;
  register short val, ctl, fh, fl, v;

  v = (nn >> 8) & 0x03;
  ip = &vbufs[curvce];

  switch (v)
    {

    case 0:

      val = ip->idhos1v;
      ctl = ip->idhos1c;
      break;

    case 1:

      val = ip->idhos2v;
      ctl = ip->idhos2c;
      break;

    case 2:

      val = ip->idhos3v;
      ctl = ip->idhos3c;
      break;

    case 3:

      val = ip->idhos4v;
      ctl = ip->idhos4c;
      break;
    }
/* 
*/
  bform = ctl & OC_MOD;

  switch (bform)
    {

    case OC_INT:		/* interval */

      sprintf (ebuf, "%04d%c", ((val < 0 ? -val : val) >> 1),
	       (val < 0 ? '-' : '+'));

      ebflag = TRUE;
      return (SUCCESS);

    case OC_RAT:		/* ratio */

      int2rat (val >> 1);
      ebflag = TRUE;
      return (SUCCESS);

    case OC_FRQ:		/* frequency */

      fh = (val >> 1) / 10;
      fl = (val >> 1) - (fh * 10);

      sprintf (ebuf, "%02d.%d", fh, fl);
      ebflag = TRUE;
      return (SUCCESS);

    case OC_PCH:

      cnvc2p (ebuf, (val >> 1));
      ebflag = TRUE;
      return (SUCCESS);
    }
}

/* 
*/

/*
   =============================================================================
	setoval() -- set oscillator mode and pitch/ratio variables
   =============================================================================
*/

setoval (ip, v, val)
     register struct instdef *ip;
     register short v, val;
{
  switch (v)
    {

    case 0:

      ip->idhos1v = val << 1;
      ip->idhos1c = (ip->idhos1c & ~OC_MOD) | bform;
      break;

    case 1:

      ip->idhos2v = val << 1;
      ip->idhos2c = (ip->idhos2c & ~OC_MOD) | bform;
      break;

    case 2:

      ip->idhos3v = val << 1;
      ip->idhos3c = (ip->idhos3c & ~OC_MOD) | bform;
      break;

    case 3:

      ip->idhos4v = val << 1;
      ip->idhos4c = (ip->idhos4c & ~OC_MOD) | bform;
      break;
    }

  ip->idhfnc[v].idftmd = (ip->idhfnc[v].idftmd & ~I_NRATIO)
    | ((bform & 2) ? I_NRATIO : 0);
}

/* 
*/

/*
   =============================================================================
	ef_iosc() -- unload (parse) edit buffer
   =============================================================================
*/

short
ef_iosc (nn)
     short nn;
{
  register struct instdef *ip;
  register short v, i, tmp;

  v = (nn >> 8) & 3;
  ip = &vbufs[curvce];
  ebflag = FALSE;

  switch (bform)
    {

    case OC_INT:		/* interval */

      tmp = 0;

      for (i = 0; i < 4; i++)
	tmp = (tmp * 10) + (ebuf[i] - '0');

      if (ebuf[4] EQ '-')
	tmp = -tmp;

      setoval (ip, v, tmp);
      modinst ();
      return (SUCCESS);

    case OC_RAT:		/* ratio */

      tmp = ndvals[ebuf[0] - '0'] - ndvals[ebuf[2] - '0'];

      setoval (ip, v, tmp);
      modinst ();
      return (SUCCESS);
/* 
*/
    case OC_FRQ:		/* frequency */

      tmp = 0;

      for (i = 0; i < 2; i++)
	tmp = (tmp * 10) + ebuf[i] - '0';

      tmp = (tmp * 10) + ebuf[3] - '0';

      if (tmp GT 159)
	return (FAILURE);

      setoval (ip, v, tmp);
      modinst ();
      return (SUCCESS);

    case OC_PCH:		/* pitch */

      if (cnvp2c ()EQ FAILURE)
	return (FAILURE);

      setoval (ip, v, cents);
      modinst ();
      return (SUCCESS);
    }
}

/* 
*/

/*
   =============================================================================
	rd_iosc() -- (re)display the field
   =============================================================================
*/

short
rd_iosc (nn)
     short nn;
{
  register struct instdef *ip;
  register short val, ctl, fh, fl, v;
  short n;
  char ocs;

  v = (nn >> 8) & 0x03;
  n = nn & 0xFF;
  ip = &vbufs[curvce];

  switch (v)
    {

    case 0:

      val = ip->idhos1v;
      ctl = ip->idhos1c;
      break;

    case 1:

      val = ip->idhos2v;
      ctl = ip->idhos2c;
      break;

    case 2:

      val = ip->idhos3v;
      ctl = ip->idhos3c;
      break;

    case 3:

      val = ip->idhos4v;
      ctl = ip->idhos4c;
      break;
    }
/* 
*/
  bform = ctl & OC_MOD;
  ocs = ((v EQ 3) ? ' ' : (ctl & OC_SYN ? 'S' : 's'));

  switch (bform)
    {

    case OC_INT:		/* interval */

      sprintf (dspbuf, "Int %c%04d %c",
	       (val < 0 ? '-' : '+'), ((val < 0 ? -val : val) >> 1), ocs);
      break;

    case OC_RAT:		/* ratio */

      int2rat (val >> 1);
      sprintf (dspbuf, "Rat %c/%c   %c", ebuf[0], ebuf[2], ocs);
      break;

    case OC_FRQ:		/* frequency */

      fh = (val >> 1) / 10;
      fl = (val >> 1) - (fh * 10);

      sprintf (dspbuf, "Frq %02d.%d  %c", fh, fl, ocs);
      break;

    case OC_PCH:

      strcpy (dspbuf, "Pch ");
      cnvc2p (&dspbuf[4], (val >> 1));
      dspbuf[4] += '0';
      dspbuf[5] += 'A';
      dspbuf[6] = sfdsp[dspbuf[6] - 7];
      dspbuf[7] += '0';
      dspbuf[8] += '0';
      dspbuf[9] = ' ';
      dspbuf[10] = ocs;
      dspbuf[11] = '\0';
      break;
    }

  if (v_regs[5] & 0x0180)
    vbank (0);

  vcputsv (instob, 64, idbox[n][4], idbox[n][5], 18 + v, 36, dspbuf, 14);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	setoscb() -- change oscillator data entry buffer format
   =============================================================================
*/

setoscb (n, v)
     short n, v;
{
  register struct instdef *ip;
  register short bfm, ctl, val;
  register char ocs;

  ip = &vbufs[curvce];

  switch (v)
    {

    case 0:

      ctl = ip->idhos1c;
      val = ip->idhos1v;
      break;

    case 1:

      ctl = ip->idhos2c;
      val = ip->idhos2v;
      break;

    case 2:

      ctl = ip->idhos3c;
      val = ip->idhos3v;
      break;

    case 3:

      ctl = ip->idhos4c;
      val = ip->idhos4v;
      break;
    }

  ocs = ((v EQ 3) ? ' ' : (ctl & OC_SYN ? 'S' : 's'));

  bfm = ctl & OC_MOD;
/* 
*/
  switch (bform)
    {

    case OC_INT:		/* interval */

      if ((bfm EQ OC_RAT) OR (bfm EQ OC_INT))
	{

	  sprintf (ebuf, "%04d%c", ((val < 0 ? -val : val) >> 1),
		   (val < 0 ? '-' : '+'));

	  sprintf (dspbuf, "Int %c%04d %c",
		   (val < 0 ? '-' : '+'), ((val < 0 ? -val : val) >> 1), ocs);

	}
      else
	{

	  strcpy (ebuf, "0000+");
	  sprintf (dspbuf, "Int %s %c", intstr, ocs);
	}

      break;

    case OC_RAT:		/* ratio */

      if (bfm EQ OC_RAT)
	{

	  int2rat (val >> 1);

	  sprintf (dspbuf, "Rat %c/%c   %c", ebuf[0], ebuf[2], ocs);

	}
      else
	{

	  strcpy (ebuf, ratstr);
	  sprintf (dspbuf, "Rat %s   %c", ratstr, ocs);
	}

      break;
/* 
*/
    case OC_FRQ:		/* frequency */

      strcpy (ebuf, frqstr);
      sprintf (dspbuf, "Frq %s  %c", frqstr, ocs);
      break;

    case OC_PCH:		/* pitch */

      ebuf[0] = 0;		/* 0 */
      ebuf[1] = 2;		/* C */
      ebuf[2] = 7;		/*   */
      ebuf[3] = 0;		/* 0 */
      ebuf[4] = 0;		/* 0 */

      sprintf (dspbuf, "Pch %s %c", pchstr, ocs);
      break;
    }

  ebflag = TRUE;

  if (v_regs[5] & 0x0180)
    vbank (0);

  vcputsv (instob, 64, ID_ENTRY, idbox[n][5], stcrow, 36, dspbuf, 14);
}

/* 
*/

/*
   =============================================================================
	setosyn() -- set oscillator sync mode
   =============================================================================
*/

setosyn (n, v, t)
     short n, v, t;
{
  register struct instdef *ip;
  register char *sc;

  ip = &vbufs[curvce];

  if (stcrow EQ 21)
    return;

  sc = t ? "S" : "s";

  switch (v)
    {

    case 0:

      ip->idhos1c = (ip->idhos1c & ~OC_SYN) | (t ? OC_SYN : 0);
      break;

    case 1:

      ip->idhos2c = (ip->idhos2c & ~OC_SYN) | (t ? OC_SYN : 0);
      break;

    case 2:

      ip->idhos3c = (ip->idhos3c & ~OC_SYN) | (t ? OC_SYN : 0);
      break;

    case 3:

      return;
    }

  dosync (curvce);

  if (v_regs[5] & 0x0180)
    vbank (0);

  vcputsv (instob, 64, idbox[n][4], idbox[n][5], stcrow, stccol, sc, 14);
  modinst ();
}

/* 
*/

/*
   =============================================================================
	nd_iosc() -- handle new data entry
   =============================================================================
*/

short
nd_iosc (nn, k)
     short nn, k;
{
  register short v, n;

  n = nn & 0xFF;
  v = (nn >> 8) & 3;

  if (stccol LT 39)
    {				/* mode */

      if (k EQ 8)
	{			/* - */

	  if (--bform LT 0)
	    bform = 3;

	  setoscb (n, v);
	  return (SUCCESS);

	}
      else if (k EQ 9)
	{			/* + */

	  if (++bform GT 3)
	    bform = 0;

	  setoscb (n, v);
	  return (SUCCESS);

	}
      else
	return (FAILURE);
/* 
*/
    }
  else if (stccol EQ 46)
    {				/* sync */

      if (stcrow EQ 21)
	return (FAILURE);

      if (k EQ 8)
	{			/* - */

	  setosyn (n, v, 0);	/* off */
	  return (SUCCESS);

	}
      else if (k EQ 9)
	{			/* + */

	  setosyn (n, v, 1);	/* on */
	  return (SUCCESS);

	}
      else
	return (FAILURE);

/* 
*/
    }
  else if ((stccol GE 40) AND (stccol LE 44))
    {				/* value */

      switch (bform)
	{

	case OC_INT:		/* interval */

	  if (stccol EQ 40)
	    {			/* sign */

	      if (k EQ 8)
		{		/* - */

		  k = '-';
		  ebuf[4] = '-';

		}
	      else if (k EQ 9)
		{		/* + */

		  k = '+';
		  ebuf[4] = '+';

		}
	      else
		return (FAILURE);

	    }
	  else
	    {

	      ebuf[stccol - 41] = k + '0';
	    }

	  dspbuf[0] = (k > 9) ? k : (k + '0');
	  dspbuf[1] = '\0';

	  if (v_regs[5] & 0x0180)
	    vbank (0);

	  vcputsv (instob, 64, ID_ENTRY, idbox[n][5],
		   stcrow, stccol, dspbuf, 14);

	  if (stccol EQ 44)
	    return (SUCCESS);

	  advicur ();
	  return (SUCCESS);
/* 
*/
	case OC_RAT:		/* ratio */

	  if (stccol EQ 40)
	    {

	      if (k)
		{

		  ebuf[0] = dspbuf[0] = k + '0';
		  dspbuf[1] = '\0';

		  if (v_regs[5] & 0x0180)
		    vbank (0);

		  vcputsv (instob, 64, ID_ENTRY, idbox[n][5],
			   stcrow, stccol, dspbuf, 14);

		  advicur ();
		  advicur ();

		  return (SUCCESS);

		}
	      else
		return (FAILURE);

	    }
	  else if (stccol EQ 42)
	    {

	      if (k)
		{

		  ebuf[2] = dspbuf[0] = k + '0';
		  dspbuf[1] = '\0';

		  if (v_regs[5] & 0x0180)
		    vbank (0);

		  vcputsv (instob, 64, ID_ENTRY, idbox[n][5],
			   stcrow, stccol, dspbuf, 14);

		  return (SUCCESS);

		}
	      else
		return (FAILURE);
	    }
	  else
	    {

	      return (FAILURE);
	    }
/* 
*/
	case OC_FRQ:		/* frequency */

	  if (stccol EQ 42)
	    return (FAILURE);

	  ebuf[stccol - 40] = k + '0';
	  dspbuf[0] = k + '0';
	  dspbuf[1] = '\0';

	  if (v_regs[5] & 0x0180)
	    vbank (0);

	  vcputsv (instob, 64, ID_ENTRY, idbox[n][5],
		   stcrow, stccol, dspbuf, 14);

	  if (stccol EQ 44)
	    return (SUCCESS);

	  advicur ();

	  if (stccol EQ 42)
	    advicur ();

	  return (SUCCESS);
/* 
*/
	case OC_PCH:		/* pitch */

	  switch (stccol)
	    {

	    case 40:

	      ebuf[0] = k;
	      dspbuf[0] = k + '0';
	      break;

	    case 41:

	      if (k GT 6)
		return (FAILURE);

	      ebuf[1] = k;
	      dspbuf[0] = k + 'A';
	      break;

	    case 42:

	      if (k EQ 7)
		{		/* blank */

		  ebuf[2] = k;
		  dspbuf[0] = sfdsp[0];
		  break;

		}
	      else if (k EQ 8)
		{		/* flat */

		  ebuf[2] = k;
		  dspbuf[0] = sfdsp[1];
		  break;

		}
	      else if (k EQ 9)
		{		/* sharp */

		  ebuf[2] = k;
		  dspbuf[0] = sfdsp[2];
		  break;

		}
	      else
		return (FAILURE);
	    case 43:
	    case 44:

	      ebuf[stccol - 40] = k;
	      dspbuf[0] = k + '0';
	      break;
	    }
/* 
*/

	  dspbuf[1] = '\0';

	  if (v_regs[5] & 0x0180)
	    vbank (0);

	  vcputsv (instob, 64, ID_ENTRY, idbox[n][5],
		   stcrow, stccol, dspbuf, 14);

	  if (stccol EQ 44)
	    return (SUCCESS);

	  advicur ();
	  return (SUCCESS);
	}

    }
  else
    {

      return (FAILURE);
    }
}
