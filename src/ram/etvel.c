/*
   =============================================================================
	etvel.c -- velocity field handlers
	Version 2 -- 1988-06-13 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "score.h"
#include "scfns.h"

#include "midas.h"
#include "scdsp.h"
#include "instdsp.h"

extern unsigned *obj8;

extern short recsw;
extern short stccol;

extern short grpmode[];
extern short grpstat[];
extern short lastvel[];

/* 
*/

/*
   =============================================================================
	et_vel() -- load edit buffer
   =============================================================================
*/

short
et_vel (n)
     short n;
{
  sprintf (ebuf, "%03d", lastvel[n] / 252);

  ebflag = TRUE;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_vel() -- parse edit buffer
   =============================================================================
*/

short
ef_vel (n)
     short n;
{
  register short ival, i;
  register struct s_entry *ep;

  ival = 0;

  for (i = 0; i < 3; i++)
    ival = (ival * 10) + (ebuf[i] - '0');

  ebflag = FALSE;

  if (ival > 127)
    return (FAILURE);

  ival = SM_SCALE (ival);

  if (recsw && grpstat[n] && (2 == grpmode[n]))
    {

      lastvel[n] = ival;
      ep = ep_adj (p_cur, 1, t_cur);

      while (t_cur == ep->e_time)
	{

	  if ((EV_NBEG == (0x007F & ep->e_type)) && (ep->e_data2 == n))
	    {

	      ((struct n_entry *) ep)->e_vel = ival;
	    }

	  ep = ep->e_fwd;
	}
    }

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_vel() -- (re)display the field
   =============================================================================
*/

short
rd_vel (n)
     short n;
{
  char buf[6];

  sprintf (buf, "%03d", lastvel[n] / 252);

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputs (obj8, 5, 6 + (n * 5), buf, SDW11ATR);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_vel() -- data entry function
   =============================================================================
*/

short
nd_vel (n, k)
     register short n, k;
{
  register short ec;

  ec = stccol - cfetp->flcol;	/* setup edit buffer column */
  ebuf[ec] = k + '0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 5, stccol, k + '0', SDW11DEA);
  advscur ();

  return (SUCCESS);
}
