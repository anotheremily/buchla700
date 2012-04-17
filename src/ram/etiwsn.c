/*
   =============================================================================
	etiwsn.c -- MIDAS instrument editor - waveshape number handlers
	Version 6 -- 1988-04-19 -- D.N. Lynx Crowe
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

extern short stcrow, stccol, curvce;
extern short idbox[][8];

extern char dspbuf[];

extern struct instdef vbufs[];
extern struct wstbl wslib[];

/* 
*/

/*
   =============================================================================
	et_iwsn() -- load the edit buffer
   =============================================================================
*/

short
et_iwsn (nn)
     short nn;
{
  register short m;

  m = nn >> 8;

  sprintf (ebuf, "%02d", m ? vbufs[curvce].idhwsb + 1
	   : vbufs[curvce].idhwsa + 1);
  ebflag = TRUE;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ef_iwsn() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_iwsn (nn)
     short nn;
{
  register short i, tmpval, m;
  register short *fpuws;

  m = nn >> 8;
  ebuf[2] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;

  tmpval = 0;

  for (i = 0; i < 2; i++)	/* convert from ASCII to binary */
    tmpval = (tmpval * 10) + (ebuf[i] - '0');

  if ((tmpval == 0) || (tmpval > NUMWAVS))
    return (FAILURE);

  if (m)
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

  dswin (21);
  modinst ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_iwsn() -- (re)display the field
   =============================================================================
*/

short
rd_iwsn (nn)
     short nn;
{
  register short m, n;

  m = (nn >> 8) & 0x00FF;
  n = nn & 0x00FF;

  sprintf (dspbuf, "%02d", m ? vbufs[curvce].idhwsb + 1
	   : vbufs[curvce].idhwsa + 1);

  vbank (0);

  vcputsv (instob, 64, (m ? WSBFC : WSAFC), idbox[n][5],
	   cfetp->frow, cfetp->flcol, dspbuf, 14);

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	nd_iwsn() -- handle new data entry
   =============================================================================
*/

short
nd_iwsn (nn, k)
     short nn;
     register short k;
{
  register short ec, c, n;

  n = nn & 0x00FF;

  ec = stccol - cfetp->flcol;	/* setup edit buffer column */
  ebuf[ec] = k + '0';		/* enter new data in buffer */
  ebuf[2] = '\0';		/* make sure string is terminated */

  dspbuf[0] = k + '0';		/* setup for display */
  dspbuf[1] = '\0';

  vbank (0);			/* display the new data */
  vcputsv (instob, 64, ID_ENTRY, idbox[n][5], stcrow, stccol, dspbuf, 14);

  advicur ();			/* advance cursor */

  return (SUCCESS);
}
