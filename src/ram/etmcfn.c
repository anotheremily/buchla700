/*
   =============================================================================
	etmcfn.c -- instrument editor - ws/cf menu field handlers
	Version 7 -- 1988-08-26 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "fields.h"
#include "hwdefs.h"
#include "fpu.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "instdsp.h"
#include "wsdsp.h"

extern unsigned *instob;

extern short curvce;
extern short stccol;
extern short stcrow;
extern short wmcsel;
extern short wmctag;

extern char dspbuf[];

extern short idbox[][8];

extern struct instdef vbufs[];

extern struct wstbl wslib[];

/* 
*/

/*
   =============================================================================
	et_mcfn() -- load the edit buffer
   =============================================================================
*/

short
et_mcfn (n)
     short n;
{
  sprintf (ebuf, "%02d", vbufs[curvce].idhcfg);
  ebflag = TRUE;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_mcfn() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_mcfn (n)
     short n;
{
  register short tmpval;

  wmctag = FALSE;
  ebuf[2] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;

  tmpval = ((ebuf[0] - '0') * 10) + (ebuf[1] - '0');

  if (tmpval GE NUMCFG)		/* check against limit */
    return (FAILURE);

  vbufs[curvce].idhcfg = tmpval;
  dosync (curvce);
  wmctag = TRUE;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_mcfn() -- (re)display the field
   =============================================================================
*/

short
rd_mcfn (n)
     short n;
{
  sprintf (dspbuf, "%02d", vbufs[curvce].idhcfg);

  if (v_regs[5] & 0x0180)
    vbank (0);

  vcputsv (instob, 64, CFBX18, CBBX18, 24, 16, dspbuf, 14);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_mcfn() -- handle new data entry
   =============================================================================
*/

short
nd_mcfn (n, k)
     short n;
     register short k;
{
  register short ec, c;

  ec = stccol - cfetp->flcol;	/* setup edit buffer column */
  ebuf[ec] = k + '0';		/* enter new data in buffer */
  ebuf[2] = '\0';		/* make sure string is terminated */

  dspbuf[0] = k + '0';		/* setup for display */
  dspbuf[1] = '\0';

  if (v_regs[5] & 0x0180)
    vbank (0);

  /* display the new data */

  vcputsv (instob, 64, ID_ENTRY, CBBX18, stcrow, stccol, dspbuf, 14);

  advicur ();			/* advance cursor */

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	et_mwsn() -- load the edit buffer
   =============================================================================
*/

short
et_mwsn (nn)
     short nn;
{
  wmcsel = (nn & 0xFF00) ? 1 : 0;

  sprintf (ebuf, "%02d", 1 + (wmcsel ? vbufs[curvce].idhwsb
			      : vbufs[curvce].idhwsa));

  ebflag = TRUE;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_mwsn() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_mwsn (nn)
     short nn;
{
  register short tmpval;
  register short *fpuws;

  wmcsel = (nn & 0xFF00) ? 1 : 0;

  wmctag = FALSE;
  ebuf[2] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;

  /* convert from ASCII to binary */

  tmpval = ((ebuf[0] - '0') * 10) + (ebuf[1] - '0');

  if ((tmpval < 1) OR (tmpval > NUMWAVS))
    return (FAILURE);
/* 
*/
  if (wmcsel)
    {

      /* slot B */
      vbufs[curvce].idhwsb = tmpval - 1;

      memcpyw (vbufs[curvce].idhwvbf, &wslib[tmpval - 1],
	       NUMHARM + (2 * NUMWPNT));

      fpuws = io_fpu + FPU_OWST + (curvce << 9) + 1;

      memcpyw (fpuws, vbufs[curvce].idhwvbf, NUMWPNT);

      *(fpuws - 1) = vbufs[curvce].idhwvbf[0];
      *(fpuws + NUMWPNT) = vbufs[curvce].idhwvbf[NUMWPNT - 1];

    }
  else
    {

      /* slot A */

      vbufs[curvce].idhwsa = tmpval - 1;

      memcpyw (vbufs[curvce].idhwvaf, &wslib[tmpval - 1],
	       NUMHARM + (2 * NUMWPNT));

      fpuws = io_fpu + FPU_OWST + (curvce << 9) + 0x100 + 1;

      memcpyw (fpuws, vbufs[curvce].idhwvaf, NUMWPNT);

      *(fpuws - 1) = vbufs[curvce].idhwvaf[0];
      *(fpuws + NUMWPNT) = vbufs[curvce].idhwvaf[NUMWPNT - 1];
    }

  wmctag = TRUE;
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_mwsn() -- (re)display the field
   =============================================================================
*/

short
rd_mwsn (nn)
     short nn;
{
  wmcsel = (nn & 0xFF00) ? 1 : 0;

  sprintf (dspbuf, "%02d", 1 + (wmcsel ? vbufs[curvce].idhwsb
				: vbufs[curvce].idhwsa));

  if (v_regs[5] & 0x0180)
    vbank (0);

  vcputsv (instob, 64, CFBX23, CBBX23, 24, wmcsel ? 16 : 12, dspbuf, 14);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_mwsn() -- handle new data entry
   =============================================================================
*/

short
nd_mwsn (nn, k)
     short nn;
     register short k;
{
  register short ec, c;

  ec = stccol - cfetp->flcol;	/* setup edit buffer column */
  ebuf[ec] = k + '0';		/* enter new data in buffer */
  ebuf[2] = '\0';		/* make sure string is terminated */

  dspbuf[0] = k + '0';		/* setup for display */
  dspbuf[1] = '\0';

  if (v_regs[5] & 0x0180)
    vbank (0);			/* display the new data */

  vcputsv (instob, 64, ID_ENTRY, CBBX23, stcrow, stccol, dspbuf, 14);

  advicur ();			/* advance cursor */

  return (SUCCESS);
}
