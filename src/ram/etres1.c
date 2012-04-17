/*
   =============================================================================
	etres1.c -- analog variable field handlers
	Version 16 -- 1988-06-28 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "smdefs.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "score.h"
#include "scfns.h"
#include "macros.h"

#include "midas.h"
#include "scdsp.h"

extern short ancmsw;
extern short angroup;
extern short ctrsw;
extern short recsw;
extern short stccol;

extern unsigned *obj8;

extern short anrs[8][16];
extern short grpmode[12];
extern short grpstat[12];
extern short var2src[6];
extern short varmode[8][16];

extern struct gdsel *gdstbc[];
extern struct valent valents[];

/*
   =============================================================================
	et_res1() -- load edit buffer
   =============================================================================
*/

short
et_res1 (n)
     short n;
{
  ebuf[0] = anrs[n][abs (angroup) - 1];
  ebflag = TRUE;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_res1() -- parse edit buffer
   =============================================================================
*/

short
ef_res1 (n)
     short n;
{
  register short ival, grp, vg;
  register struct s_entry *ep;

  ebflag = FALSE;
  grp = abs (angroup) - 1;
  ival = ebuf[0] & 0x00FF;

  if ((ival < 3) || (ival > 8))
    return (FAILURE);

  anrs[n][grp] = ival;

  if (recsw && grpstat[grp] &&
      (2 == (ancmsw ? varmode[n][grp] : grpmode[grp])))
    {

      vg = ((n << 4) | grp);

      if (E_NULL != (ep = findev (p_cur, t_cur, EV_ANRS, vg, -1)))
	{

	  ep->e_data2 = ival;

	}
      else if (E_NULL != (ep = e_alc (E_SIZE2)))
	{

	  ep->e_type = EV_ANRS;
	  ep->e_data1 = vg;
	  ep->e_data2 = ival;
	  ep->e_time = t_cur;
	  p_cur = e_ins (ep, ep_adj (p_cur, 0, t_cur))->e_fwd;
	  eh_ins (ep, EH_ANRS);
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
	rd_res1() -- (re)display the field
   =============================================================================
*/

short
rd_res1 (n)
     short n;
{
  register short grp, chr;

  grp = abs (angroup) - 1;

  chr = anrs[n][grp] + '0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 7, 6 + (n * 9), chr, SDW12ATR);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_res1() -- data entry function
   =============================================================================
*/

short
nd_res1 (n, k)
     short n, k;
{
  ebuf[0] = k;

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 7, stccol, k + '0', SDW12DEA);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	et_aval() -- load edit buffer
   =============================================================================
*/

short
et_aval (n)
     short n;
{
  register short val, grp, gs;
  short val1, val2, val3;

  grp = abs (angroup) - 1;
  gs = (grp << 4) | var2src[n];
  val = (valents[gs].val) >> 5;

  if (val < 0)
    {

      ebuf[0] = 8;
      val = -val;

    }
  else
    {

      ebuf[0] = 9;
    }

  val1 = val / 100;
  ebuf[4] = (val - (val1 * 100)) / 10;
  val2 = val1 / 10;
  ebuf[2] = val1 - (val2 * 10);
  ebuf[1] = val2;

  ebflag = TRUE;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_res1() -- parse edit buffer
   =============================================================================
*/

short
ef_aval (n)
     short n;
{
  register short val, grp, src, vg;
  register struct s_entry *ep;

  ebflag = FALSE;
  grp = abs (angroup) - 1;
  src = var2src[n];
  val = ((ebuf[1] * 100) + (ebuf[2] * 10) + ebuf[4]) * 10;

  if (val > 1000)
    return (FAILURE);

  if (ebuf[0] == 8)
    val = -val;

  val = val << 5;

  setsv (grp, src, val);

  if (recsw && grpstat[grp] &&
      (2 == (ancmsw ? varmode[n][grp] : grpmode[grp])))
    {

      vg = (n << 4) | grp;

      if (E_NULL != (ep = findev (p_cur, t_cur, EV_ANVL, vg, -1)))
	{

	  ep->e_dn = (struct s_entry *) ((long) val << 16);

	}
      else if (E_NULL != (ep = e_alc (E_SIZE2)))
	{

	  ep->e_type = EV_ANVL;
	  ep->e_data1 = vg;
	  ep->e_dn = (struct s_entry *) ((long) val << 16);
	  ep->e_time = t_cur;
	  p_cur = e_ins (ep, ep_adj (p_cur, 0, t_cur))->e_fwd;
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
	rd_aval() -- (re)display the field
   =============================================================================
*/

short
rd_aval (n)
     short n;
{
  register short grp, val, gs;
  short val1, val2;
  char buf[6];

  grp = abs (angroup) - 1;
  gs = (grp << 4) | var2src[n];
  val = (valents[gs].val) >> 5;

  if (val < 0)
    {

      buf[0] = '-';
      val = -val;

    }
  else
    {

      buf[0] = '+';
    }

  val1 = val / 100;
  buf[4] = '0' + ((val - (val1 * 100)) / 10);
  val2 = val1 / 10;
  buf[2] = '0' + (val1 - (val2 * 10));
  buf[1] = '0' + val2;
  buf[3] = '.';
  buf[5] = '\0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputs (obj8, 7, 8 + (n * 9), buf, SDW12ATR);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_aval() -- data entry function
   =============================================================================
*/

short
nd_aval (n, k)
     register short n, k;
{
  register short ec;

  ec = stccol - cfetp->flcol;

  if (ec == 3)
    return (FAILURE);

  ebuf[ec] = k;

  if (ec == 0)
    k = (k == 8) ? '-' : '+';
  else
    k += '0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 7, stccol, k, SDW12DEA);

  if (ec == 4)
    return (SUCCESS);

  advscur ();

  if (ec == 2)
    advscur ();

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	et_agrp() -- load edit buffer
   =============================================================================
*/

short
et_agrp (n)
     short n;
{
  sprintf (ebuf, "%02.2d", abs (angroup));
  ebflag = TRUE;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_agrp() -- parse edit buffer
   =============================================================================
*/

short
ef_agrp (n)
     short n;
{
  register short ival;

  ebflag = FALSE;
  ival = (10 * (ebuf[0] - '0')) + (ebuf[1] - '0');

  if ((ival > 12) || (ival < 1))
    return (FAILURE);

  angroup = sign (angroup, ival);

  ds_anmd ();
  ds_anrs ();
  ds_anvl ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_agrp() -- (re)display the field
   =============================================================================
*/

short
rd_agrp (n)
     short n;
{
  char buf[4];

  sprintf (buf, "%02.2d", abs (angroup));

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputs (obj8, 7, 61, buf, SDW12ATR);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_agrp() -- data entry function
   =============================================================================
*/

short
nd_agrp (n, k)
     short n, k;
{
  register short ec;

  ec = stccol - cfetp->flcol;

  ebuf[ec] = k + '0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 7, stccol, k + '0', SDW12DEA);
  advscur ();

  return (SUCCESS);
}
