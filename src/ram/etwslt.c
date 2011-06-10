/*
   =============================================================================
	etwslt.c -- waveshape editor - waveshape slot field handlers
	Version 8 -- 1987-12-11 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"
#include "charset.h"

#include "midas.h"
#include "instdsp.h"
#include "wsdsp.h"

extern short advwcur (), wdswin (), newws (), dsnewws ();

extern unsigned *waveob;

extern short stcrow, stccol, curwslt;

extern short wdbox[][8];

extern char dspbuf[];

/* 
*/

/*
   =============================================================================
	et_wslt() -- load the edit buffer
   =============================================================================
*/

short
et_wslt (n)
     short n;
{
  sprintf (ebuf, "%c", curwslt + 'A');
  ebflag = TRUE;

  return (SUCCESS);
}

/*
   =============================================================================
	ef_wslt() -- parse (unload) the edit buffer
   =============================================================================
*/

short
ef_wslt (n)
     short n;
{
  ebuf[1] = '\0';		/* terminate the string in ebuf */
  ebflag = FALSE;

  curwslt = ebuf[0] - 'A';

#if DEBUGIT
  printf ("ef_wslt($%04X):  ebuf[%s], curwslt=%d\r\n", n, ebuf, curwslt);
#endif

  newws ();
  dsnewws ();
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	rd_wslt() -- (re)display the field
   =============================================================================
*/

short
rd_wslt (nn)
     short nn;
{
  register short n;

  n = nn & 0xFF;
  sprintf (dspbuf, "%c", curwslt + 'A');

  vbank (0);
  vcputsv (waveob, 64, wdbox[n][4], wdbox[n][5],
	   wdbox[n][6] + 1, wdbox[n][7] + WSLT_OFF, dspbuf, 14);

  return (SUCCESS);
}

/*
   =============================================================================
	nd_wslt() -- handle new data entry
   =============================================================================
*/

short
nd_wslt (nn, k)
     short nn;
     register short k;
{
  register short n;

  n = nn & 0xFF;

#if DEBUGIT
  printf ("nd_wslt($%04X, %d)\r\n", nn, k);
#endif

  if (k GT 1)
    return (FAILURE);

  ebuf[0] = k + 'A';
  ebuf[1] = '\0';

  dspbuf[0] = k + 'A';
  dspbuf[1] = '\0';

#if DEBUGIT
  printf ("nd_wslt($%04X, %d):  ebuf[%s]\r\n", nn, k, ebuf);
#endif

  vbank (0);
  vcputsv (waveob, 64, WS_ENTRY, wdbox[n][5], stcrow, stccol, dspbuf, 14);

  advwcur ();
  return (SUCCESS);
}
