/*
   =============================================================================
	etioas.c -- line 17 field handlers  (I/O Assignment thru Interpolate)
	Version 12 -- 1988-08-22 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "fields.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "stddefs.h"
#include "score.h"
#include "scfns.h"
#include "slice.h"

#include "midas.h"
#include "scdsp.h"

extern unsigned fromfpu (), tofpu ();

extern unsigned *obj8;
extern unsigned curintp;

extern short ctrsw;
extern short curasg;
extern short curtun;
extern short tmpoval;
extern short recsw;
extern short stccol;

extern char dspbuf[];

extern struct gdsel *gdstbc[];

/* 
*/

/*
   =============================================================================
	Assignment table field handlers
   =============================================================================
*/

/*
   =============================================================================
	et_ioas() -- load edit buffer
   =============================================================================
*/

short
et_ioas (n)
     short n;
{
  sprintf (ebuf, "%02.2d", curasg);
  ebflag = TRUE;

  return (SUCCESS);
}

/* 
*/
/*
   =============================================================================
	ef_ioas() -- parse edit buffer
   =============================================================================
*/

short
ef_ioas (n)
     short n;
{
  register short ival;
  register struct s_entry *ep;

  ebuf[2] = '\0';
  ival = ((ebuf[0] - '0') * 10) + (ebuf[1] - '0');

  ebflag = FALSE;

  if (ival >= NASGS)
    return (FAILURE);

  getasg (curasg = ival);
  mpcupd ();

  if (recsw)
    {

      if (E_NULL != (ep = findev (p_cur, t_cur, EV_ASGN, -1, -1)))
	{

	  ep->e_data1 = ival;

	}
      else if (E_NULL != (ep = e_alc (E_SIZE2)))
	{

	  ep->e_type = EV_ASGN;
	  ep->e_data1 = ival;
	  ep->e_time = t_cur;
	  p_cur = e_ins (ep, ep_adj (p_cur, 0, t_cur))->e_fwd;
	  eh_ins (ep, EH_ASGN);
	  ctrsw = TRUE;
	  se_disp (ep, D_FWD, gdstbc, 1);
	  scupd ();
	}
    }

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_ioas() -- (re)display the field
   =============================================================================
*/

short
rd_ioas (n)
     short n;
{
  sprintf (dspbuf, "%02.2d", curasg);

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputs (obj8, 1, 11, dspbuf, SDW04ATR);
  return (SUCCESS);
}

/*
   =============================================================================
	nd_ioas() -- data entry function
   =============================================================================
*/

short
nd_ioas (n, k)
     short n, k;
{
  register short ec;

  ec = stccol - cfetp->flcol;
  ebuf[ec] = k + '0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 1, stccol, k + '0', SDW04DEA);
  advscur ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	Tuning field handlers
   =============================================================================
*/

/*
   =============================================================================
	et_tune() -- load edit buffer
   =============================================================================
*/

short
et_tune (n)
     short n;
{
  ebuf[0] = '0' + curtun;
  ebuf[1] = '\0';
  ebflag = TRUE;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_tune() -- parse edit buffer
   =============================================================================
*/

short
ef_tune (n)
     short n;
{
  register short ival;
  register struct s_entry *ep;

  ebuf[1] = '\0';
  ival = ebuf[0] - '0';
  ebflag = FALSE;
  gettun (ival);

  if (recsw)
    {

      if (E_NULL != (ep = findev (p_cur, t_cur, EV_TUNE, -1, -1)))
	{

	  ep->e_data1 = ival;

	}
      else if (E_NULL != (ep = e_alc (E_SIZE2)))
	{

	  ep->e_type = EV_TUNE;
	  ep->e_data1 = ival;
	  ep->e_time = t_cur;
	  p_cur = e_ins (ep, ep_adj (p_cur, 0, t_cur))->e_fwd;
	  eh_ins (ep, EH_TUNE);
	  ctrsw = TRUE;
	  se_disp (ep, D_FWD, gdstbc, 1);
	  scupd ();
	}
    }

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_tune() -- (re)display the field
   =============================================================================
*/

short
rd_tune (n)
     short n;
{
  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 1, 19, curtun + '0', SDW05ATR);

  return (SUCCESS);
}

/*
   =============================================================================
	nd_tune() -- data entry function
   =============================================================================
*/

short
nd_tune (n, k)
     short n, k;
{
  register short ec;

  ebuf[0] = k + '0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 1, stccol, k + '0', SDW05DEA);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	Tempo field handlers
   =============================================================================
*/

/*
   =============================================================================
	et_tmpo() -- load edit buffer
   =============================================================================
*/

short
et_tmpo (n)
     short n;
{
  sprintf (ebuf, "%03.3d", tmpoval);
  ebflag = TRUE;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_tmpo() -- parse edit buffer
   =============================================================================
*/

short
ef_tmpo (n)
     short n;
{
  register short ival;
  register struct s_entry *ep;

  ebuf[3] = '\0';
  ival = ((ebuf[0] - '0') * 100) + ((ebuf[1] - '0') * 10) + (ebuf[2] - '0');

  ebflag = FALSE;

  if ((ival > 240) || (ival < 4))
    return (FAILURE);

  settmpo (ival);

  if (recsw)
    {

      if (E_NULL != (ep = findev (p_cur, t_cur, EV_TMPO, -1, -1)))
	{

	  ep->e_data1 = ival;

	}
      else if (E_NULL != (ep = e_alc (E_SIZE2)))
	{

	  ep->e_type = EV_TMPO;
	  ep->e_data1 = ival;
	  ep->e_time = t_cur;
	  p_cur = e_ins (ep, ep_adj (p_cur, 0, t_cur))->e_fwd;
	  eh_ins (ep, EH_TMPO);
	  ctrsw = TRUE;
	  se_disp (ep, D_FWD, gdstbc, 1);
	  scupd ();
	}
    }

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_tmpo() -- (re)display the field
   =============================================================================
*/

short
rd_tmpo (n)
     short n;
{
  sprintf (dspbuf, "%03.3d", tmpoval);

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputs (obj8, 1, 27, dspbuf, SDW06ATR);

  return (SUCCESS);
}

/*
   =============================================================================
	nd_tmpo() -- data entry function
   =============================================================================
*/

short
nd_tmpo (n, k)
     register short n, k;
{
  register short ec;

  ec = stccol - cfetp->flcol;
  ebuf[ec] = k + '0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 1, stccol, k + '0', SDW06DEA);
  advscur ();

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	Interpolate field handlers
   =============================================================================
*/

/*
   =============================================================================
	et_intp() -- load edit buffer
   =============================================================================
*/

short
et_intp (n)
     short n;
{
  register short th, tl;
  register long tt, sc, sf;

  sc = 1000L;
  sf = 100L;
  tt = fromfpu (curintp);
  th = tt / sc;
  tl = (tt - (th * sc)) / sf;

  sprintf (ebuf, "%02d.%d", th, tl);
  ebflag = TRUE;

  return (SUCCESS);
}

/*
   =============================================================================
	ef_intp() -- parse edit buffer
   =============================================================================
*/

short
ef_intp (n)
     short n;
{
  register short i;
  register unsigned tmpval;
  register struct s_entry *ep;

  ebuf[2] = '.';		/* add implied decimal point */
  ebuf[4] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;
  tmpval = 0;

  for (i = 0; i < 2; i++)	/* convert from ASCII to binary */
    tmpval = (tmpval * 10) + (ebuf[i] - '0');

  tmpval = ((tmpval * 10) + (ebuf[3] - '0')) * 100;

  if (tmpval > (unsigned) 64900)
    return (FAILURE);

  if (tmpval == 0)
    tmpval = 1;

  curintp = tofpu (tmpval);

  if (recsw)
    {

      if (E_NULL != (ep = findev (p_cur, t_cur, EV_INTP, -1, -1)))
	{

	  ep->e_data1 = (curintp >> 8);
	  ep->e_data2 = 0x00FF & curintp;

	}
      else if (E_NULL != (ep = e_alc (E_SIZE3)))
	{

	  ep->e_type = EV_INTP;
	  ep->e_time = t_cur;
	  ep->e_data1 = (curintp >> 8);
	  ep->e_data2 = 0x00FF & curintp;
	  p_cur = e_ins (ep, ep_adj (p_cur, 0, t_cur))->e_fwd;
	  eh_ins (ep, EH_INTP);
	  ctrsw = TRUE;
	  se_disp (ep, D_FWD, gdstbc, 1);
	  scupd ();
	}
    }

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_intp() -- (re)display the field
   =============================================================================
*/

short
rd_intp (n)
     short n;
{
  register short th, tl;
  register long tt, sc, sf;

  sc = 1000L;
  sf = 100L;
  tt = fromfpu (curintp);
  th = tt / sc;
  tl = (tt - (th * sc)) / sf;

  sprintf (dspbuf, "%02d.%d", th, tl);	/* convert to ASCII */

  vbank (0);			/* display the value */

  vputs (obj8, 1, 35, dspbuf, SDW07ATR);

  return (SUCCESS);
}

/*
   =============================================================================
	nd_intp() -- data entry function
   =============================================================================
*/

short
nd_intp (n, k)
     short n, k;
{
  register short ec;

  ec = stccol - cfetp->flcol;	/* setup edit buffer column */

  if (ec == 2)
    return (FAILURE);

  if ((ec == 0) && (k > 6))
    return (FAILURE);

  if ((ec == 1) && (ebuf[0] == '6') && (k > 4))
    return (FAILURE);

  ebuf[ec] = k + '0';
  ebuf[2] = '.';
  ebuf[4] = '\0';

  dspbuf[0] = k + '0';
  dspbuf[1] = '\0';

  vbank (0);

  vputs (obj8, 1, stccol, dspbuf, SDW07DEA);

  advscur ();

  if (stccol == 37)
    advscur ();

  return (SUCCESS);
}
