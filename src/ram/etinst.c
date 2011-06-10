/*
   =============================================================================
	etinst.c -- instrument field handlers
	Version 23 -- 1988-07-11 -- D.N. Lynx Crowe
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

extern short ctrsw;
extern short recsw;
extern short stccol;

extern short grpmode[];
extern short grpstat[];
extern short ins2grp[];

extern struct gdsel *gdstbc[];

/*
   =============================================================================
	et_inst() -- load edit buffer
   =============================================================================
*/

short
et_inst (n)
     short n;
{
  sprintf (ebuf, "%02.2d", ins2grp[n] & 0x00FF);
  ebflag = TRUE;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_inst() -- parse edit buffer
   =============================================================================
*/

short
ef_inst (n)
     short n;
{
  register short ival;
  register struct s_entry *ep;

  ebuf[2] = '\0';
  ival = ((ebuf[0] - '0') * 10) + (ebuf[1] - '0');
  ebflag = FALSE;

  if (ival GE NINST)
    return (FAILURE);

  ins2grp[n] = ival | (ins2grp[n] & 0xFF00);
  setv2gi (n);
  setinst ();

  if (recsw AND grpstat[n] AND (2 EQ grpmode[n]))
    {

      if (E_NULL NE (ep = findev (p_cur, t_cur, EV_INST, n, -1)))
	{

	  ep->e_data2 = ival;

	}
      else if (E_NULL NE (ep = e_alc (E_SIZE2)))
	{

	  ep->e_type = EV_INST;
	  ep->e_data1 = n;
	  ep->e_data2 = ival;
	  ep->e_time = t_cur;
	  p_cur = e_ins (ep, ep_adj (p_cur, 0, t_cur))->e_fwd;
	  eh_ins (ep, EH_INST);
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
	rd_inst() -- (re)display the field
   =============================================================================
*/

short
rd_inst (n)
     short n;
{
  char buf[4];

  sprintf (buf, "%02.2d", ins2grp[n] & 0x00FF);

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputs (obj8, 2, 7 + (n * 5), buf, SDW11ATR);

  return (SUCCESS);
}

/*
   =============================================================================
	ds_inst() -- display all instrument to group assignments
   =============================================================================
*/

ds_inst ()
{
  register short i;

  for (i = 0; i < 12; i++)
    rd_inst (i);
}

/* 
*/

/*
   =============================================================================
	nd_inst() -- data entry function
   =============================================================================
*/

short
nd_inst (n, k)
     register short n, k;
{
  register short ec;

  ec = stccol - cfetp->flcol;
  ebuf[ec] = k + '0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 2, stccol, k + '0', SDW11DEA);
  advscur ();

  return (SUCCESS);
}
