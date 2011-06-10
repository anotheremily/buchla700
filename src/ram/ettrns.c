/*
   =============================================================================
	ettrns.c -- transposition field handlers
	Version 20 -- 1988-07-12 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT	0

#include "stddefs.h"
#include "fields.h"
#include "charset.h"
#include "hwdefs.h"
#include "vsdd.h"
#include "graphdef.h"
#include "score.h"
#include "scfns.h"

#include "midas.h"
#include "scdsp.h"

extern short advscur ();

extern unsigned *obj8;

extern short ctrsw, recsw, stccol, stcrow;

extern short s_trns[12];	/* current transposition values */

extern char dspbuf[65];

extern short grpdyn[], grpmode[], grpstat[];

extern struct gdsel *gdstbc[];

/* 
*/

/*
   =============================================================================
	et_trns() -- load the edit buffer
   =============================================================================
*/

short
et_trns (n)
     short n;
{
  register short trval;
  register char trsign;

  trval = s_trns[n];

  if (trval < 0)
    {

      trval = (-trval);
      trsign = '-';

    }
  else
    {

      trsign = '+';
    }

  sprintf (ebuf, "%04.4d%c", trval, trsign);

  ebflag = TRUE;

#if	DEBUGIT
  printf ("et_trns(0x%04.4x)  [%s]\r\n", n, ebuf);
#endif

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_trns() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_trns (n)
     short n;
{
  register short i, trval;
  register struct s_entry *ep, *trnval;

  ebuf[5] = '\0';		/* terminate the string in ebuf */

#if	DEBUGIT
  printf ("ef_trns(0x%04.4x) [%s]\r\n", n, ebuf);
#endif

  ebflag = FALSE;
  trval = 0;

  for (i = 0; i < 4; i++)	/* convert from ASCII to binary */
    trval = (trval * 10) + (ebuf[i] - '0');

  if (trval GT 1200)		/* check against limit */
    return (FAILURE);

  if (ebuf[4] EQ '-')		/* fixup sign of value */
    trval = (-trval);

  s_trns[n] = trval;		/* store new value */
  settune ();			/* update FPU */

  if (recsw AND grpmode[n] AND (2 EQ grpmode[n]))
    {

      trnval = (struct s_entry *) ((long) trval << 16);

      if (E_NULL NE (ep = findev (p_cur, t_cur, EV_TRNS, n, -1)))
	{

	  ep->e_lft = trnval;

	}
      else if (E_NULL NE (ep = e_alc (E_SIZE3)))
	{

	  ep->e_type = EV_TRNS;
	  ep->e_time = t_cur;
	  ep->e_data1 = n;
	  ep->e_lft = trnval;
	  p_cur = e_ins (ep, ep_adj (p_cur, 0, t_cur))->e_fwd;
	  eh_ins (ep, EH_TRNS);
	  ctrsw = TRUE;
	  se_disp (ep, D_FWD, gdstbc, 1);
	  scupd ();
	}
    }

#if	DEBUGIT
  printf ("   SUCCESS: %d\r\n", trval);
#endif
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_trns() -- (re)display the value
   =============================================================================
*/

short
rd_trns (n)
     short n;
{
  register short trval, i;
  register char trsign;

  trval = s_trns[n];		/* get the value */

  if (trval < 0)
    {				/* adjust for the sign */

      trsign = '-';
      trval = (-trval);

    }
  else
    {

      trsign = '+';
    }

  sprintf (dspbuf, "%04.4d", trval);	/* convert to ASCII */

#if	DEBUGIT
  printf ("rd_trns:  %d <%d> [%s] -> ", n, s_trns[n], dspbuf);
#endif

  if (trsign EQ '-')
    {				/* handle +1, -1 cases */

      if (dspbuf[0] EQ '1')
	dspbuf[0] = SP_M1;	/* -1 */
      else
	dspbuf[0] = '-';

    }
  else
    {

      if (dspbuf[0] EQ '1')
	dspbuf[0] = SP_P1;	/* +1 */
      else
	dspbuf[0] = '+';
    }

#if	DEBUGIT
  printf ("{%02x %02x %02x %02x}\r\n",
	  dspbuf[0], dspbuf[1], dspbuf[2], dspbuf[3]);
#endif

  if (v_regs[5] & 0x0180)
    vbank (0);			/* display the value */

  vputs (obj8, 3, (5 * (n + 1)), dspbuf, SDW11ATR);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ds_trns() -- display all transposition values
   =============================================================================
*/

ds_trns ()
{
  register short i;

  for (i = 0; i < 12; i++)	/* display each of the groups */
    rd_trns (i);
}

/* 
*/

/*
   =============================================================================
	nd_trns() -- handle new data entry
   =============================================================================
*/

short
nd_trns (n, k)
     short n;
     register short k;
{
  register short ec, c, advsw;

  ec = stccol - cfetp->flcol;	/* setup edit buffer column */

#if	DEBUGIT
  printf ("nd_trns(0x%04.4x, 0x%02.2x) ec = %d, tp = 0x%08.8lx\r\n",
	  n, k, ec, tp);
#endif

  advsw = TRUE;

  if (ec EQ 0)
    {				/* first column of field ? */

      switch (k)
	{			/* what are we entering ? */

	case 0:		/* digit 0 */

	  ebuf[0] = '0';
	  k = ebuf[4];
	  break;

	case 1:		/* digit 1 */

	  if (ebuf[4] EQ '+')
	    k = SP_P1;		/* +1 */
	  else
	    k = SP_M1;		/* -1 */

	  ebuf[0] = '1';
	  break;

	case 8:		/* - */

	  if (ebuf[0] EQ '0')
	    k = '-';
	  else
	    k = SP_M1;

	  ebuf[4] = '-';
	  advsw = FALSE;
	  break;


	case 9:		/* + */

	  if (ebuf[0] EQ '0')
	    k = '+';
	  else
	    k = SP_P1;

	  ebuf[4] = '+';
	  advsw = FALSE;
	  break;

	default:		/* anything else is an error */

	  return (FAILURE);
	}

    }
  else
    {				/* any other column */

      ebuf[ec] = k + '0';
    }

  dspbuf[0] = (k > 9) ? k : (k + '0');
  dspbuf[1] = '\0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputs (obj8, 3, stccol, dspbuf, SDW11DEA);

#if	DEBUGIT
  printf ("nd_trns: char=0x%02.2x, col=%d\n", dspbuf[0], stccol);
#endif

  if (advsw)
    advscur ();

  return (SUCCESS);
}
