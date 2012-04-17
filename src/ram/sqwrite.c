/*
   =============================================================================
	sqwrite.c -- librarian - write sequence functions
	Version 2 -- 1988-11-17 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "stdio.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "patch.h"

#include "midas.h"
#include "libdsp.h"

#if	DEBUGIT
extern short debugsw;
#endif

extern long chksum ();

/* 
*/

/*
   =============================================================================
	sqsizer() -- return number of bytes necessary for storing
		active sequence lines
   =============================================================================
*/

long
sqsizer ()
{
  register short i, na;
  register long nb;

  nb = 0L;

  for (i = 0; i < NSLINES; i++)
    {

      na = 0;

      if (seqtab[i].seqtime)	/* check the time */
	na += 2L;

      if (seqtab[i].seqact1)	/* check action 1 */
	na += 4L;

      if (seqtab[i].seqact2)	/* check action 2 */
	na += 4L;

      if (seqtab[i].seqact3)	/* check action 3 */
	na += 4L;

      if (na)			/* tote up the result */
	nb += (na + 3);
    }

  if (nb)
    ++nb;

#if	DEBUGIT
  if (debugsw)
    printf ("sqsizer():  %ld bytes required\n", nb);
#endif

  return (nb);
}

/* 
*/

/*
   =============================================================================
	sqwrite() -- store a sequence table
   =============================================================================
*/

short
sqwrite (fp)
     register FILE *fp;
{
  short seq;
  char cb, zero;
  char scid[48];

#if	DEBUGIT
  if (debugsw)
    printf ("sqwrite($%08lX):  entered\n", fp);
#endif

  zero = 0;

  ldwmsg ("Busy -- Please stand by", (char *) 0L, "  writing sequences",
	  LCFBX10, LCBBX10);

  for (seq = 0; seq < NSLINES; seq++)
    {

      cb = 0x00;

      if (seqtab[seq].seqtime)
	cb |= 0x08;

      if (seqtab[seq].seqact1)
	cb |= 0x04;

      if (seqtab[seq].seqact2)
	cb |= 0x02;

      if (seqtab[seq].seqact3)
	cb |= 0x01;

      if (0 == cb)
	continue;

      if (wr_ec (fp, &cb, 1L))	/* Control byte */
	return (FAILURE);

      if (wr_ec (fp, &seq, 2L))	/* Line number */
	return (FAILURE);

      if (cb & 0x08)		/* Time */
	if (wr_ec (fp, &seqtab[seq].seqtime, 2L))
	  return (FAILURE);

      if (cb & 0x04)		/* Action 1 */
	if (wr_ec (fp, &seqtab[seq].seqact1, 4L))
	  return (FAILURE);

      if (cb & 0x02)		/* Action 2 */
	if (wr_ec (fp, &seqtab[seq].seqact2, 4L))
	  return (FAILURE);

      if (cb & 0x01)		/* Action 3 */
	if (wr_ec (fp, &seqtab[seq].seqact3, 4L))
	  return (FAILURE);
    }

  if (wr_ec (fp, &zero, 1L))	/* terminator */
    return (FAILURE);

#if	DEBUGIT
  if (debugsw)
    printf ("sqwrite():  SUCCESS\n");
#endif

  return (SUCCESS);
}
