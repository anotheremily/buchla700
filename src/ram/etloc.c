/*
   =============================================================================
	etloc.c -- location field handlers
	Version 11 -- 1988-07-13 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "hwdefs.h"
#include "fpu.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "score.h"
#include "scfns.h"

#include "midas.h"
#include "scdsp.h"
#include "instdsp.h"

extern short setsr ();

extern unsigned *obj8;

extern unsigned curintp;

extern short ctrsw;
extern short timemlt;
extern short recsw;
extern short stccol;

extern short grploc[];
extern short grpmode[];
extern short grpstat[];
extern short loctab[];
extern short vce2grp[];

extern unsigned expbit[];

extern struct gdsel *gdstbc[];

extern struct instdef vbufs[];

/* 
*/

/*
   =============================================================================
	et_loc() -- load edit buffer
   =============================================================================
*/

short
et_loc (n)
     short n;
{
  ebuf[0] = grploc[n] + 1 + '0';
  ebuf[1] = '\0';
  ebflag = TRUE;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	setloc() -- set location
   =============================================================================
*/

setloc (grp, loc)
     short grp, loc;
{
  register short g, i, val;
  register long ltmp;
  register unsigned *fpu;
  register struct idfnhdr *fp;
  unsigned fpmant, fpexp;
  short oldsr;
  short nop;

  grploc[grp] = loc;
  val = (loctab[loc] << 1) ^ 0x8000;
  g = grp + 1;

  fpmant = (((long) curintp & 0x0000FFF0L) *
	    ((long) timemlt & 0x0000FFFFL)) >> 15;

  fpexp = expbit[curintp & 0x000F];

/* 
*/
  for (i = 0; i < 12; i++)
    if (g == vce2grp[i])
      {

	fp = &vbufs[i].idhfnc[4];
	fpu = io_fpu + FPU_OFNC + (i << 8) + 0x0040;

	oldsr = setsr (0x2200);

/* ++++++++++++++++++++++++++++ FPU interrupts disabled +++++++++++++++++++++ */

	fp->idftmd ^= I_NVBITS;

	*(fpu + (long) FPU_TMNT) = fpmant;
	++nop;
	++nop;
	++nop;
	*(fpu + (long) FPU_TEXP) = fpexp;
	++nop;
	++nop;
	++nop;

	if (fp->idftmd & I_VNSUBN)
	  *(fpu + (long) FPU_TNV1) = val;
	else
	  *(fpu + (long) FPU_TNV0) = val;

	++nop;
	++nop;
	++nop;

	*(fpu + (long) FPU_TCTL) =
	  (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0001;

	setsr (oldsr);

/* ++++++++++++++++++++++++++++ Interrupts restored +++++++++++++++++++++++++ */

      }
}

/* 
*/

/*
   =============================================================================
	ef_loc() -- parse edit buffer
   =============================================================================
*/

short
ef_loc (n)
     short n;
{
  register short ival;
  register struct s_entry *ep;

  ebuf[1] = '\0';
  ival = ebuf[0] - '0';
  ebflag = FALSE;

  if (ival == 0)
    return (FAILURE);

  --ival;

  setloc (n, ival);

  if (recsw && grpstat[n] && (2 == grpmode[n]))
    {

      if (E_NULL != (ep = findev (p_cur, t_cur, EV_LOCN, n, -1)))
	{

	  ep->e_data2 = ival;

	}
      else if (E_NULL != (ep = e_alc (E_SIZE2)))
	{

	  ep->e_type = EV_LOCN;
	  ep->e_data1 = n;
	  ep->e_data2 = ival;
	  ep->e_time = t_cur;
	  p_cur = e_ins (ep, ep_adj (p_cur, 0, t_cur))->e_fwd;
	  eh_ins (ep, EH_LOCN);
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
	rd_loc() -- (re)display the field
   =============================================================================
*/

short
rd_loc (n)
     short n;
{
  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 4, 8 + (n * 5), (grploc[n] + 1 + '0'), SDW11ATR);

  return (SUCCESS);
}

/*
   =============================================================================
	ds_loc() -- display all location to group assignments
   =============================================================================
*/

ds_loc ()
{
  register short i;

  for (i = 0; i < 12; i++)
    rd_loc (i);
}

/* 
*/

/*
   =============================================================================
	nd_loc() -- data entry function
   =============================================================================
*/

short
nd_loc (n, k)
     register short n, k;
{
  ebuf[0] = k + '0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  vputc (obj8, 4, stccol, k + '0', SDW11DEA);
  advscur ();

  return (SUCCESS);
}
