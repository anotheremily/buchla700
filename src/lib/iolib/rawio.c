/*
   ============================================================================
	rawio.c -- Some raw serial I/O routines for ROMP, et al
	Version 5 -- 1987-06-11 -- D.N. Lynx Crowe

	Defines:	getln(), getrln(), readln(), writeln()

	All of the following use raw BIOS calls to do their I/O.

	int
	getln(unit,nb,buf)
	int unit;	logical unit number 1..4
	int nb;		buffer limit
	char *buf;	buffer pointer

		Reads a line from unit into buf, with a limit of nb
	bytes.  Does standard input editing  (BS. DEL, ^X).  Returns
	on CR, LF, ^X, or buffer full.  Returns the byte that stopped
	the input, or ERR01 for buffer full.  Echoes the characters as
	it reads them, such that "What you see is what you get" on a crt.

	int
	getrln(unit, nb, buf)
	int unit;	logical unit number 1..4
	int nb;		buffer limit
	char *buf;	buffer pointer

		Reads a line from unit into buf, with a limit of nb bytes.
	Allows input to be cancelled by ^X.  Returns	on CR, LF, ^X,
	^Z, or buffer full.  Returns the byte that stopped the input,
	or ERR01 for buffer full.


	int
	readln(unit,nc,ctl,nb,buf)
	int unit;	logical unit number 1..4
	int nc;		length of control string ctl
	char *ctl;	control string pointer
	int nb;		buffer limit
	char *buf;	buffer pointer (at least nb+1 bytes)

		Reads characters from unit into buf until: 1) nb bytes
	have been transferred, or, 2) one of the characters in the string
	at ctl has been read.  The terminating character will be in the
	buffer, followed by a null byte  (even if the character from ctl
	was a null byte).  Returns: 1) the terminating character as its value,
	or 2) ERR01 if stopped by the count in nb, or 3) ERR15 for an
	invalid unit.  Echoes characters as it reads them, unless the
	character is one of those in ctl.

	void
	writeln(unit,buf)
	int unit;	logical unit number 0..4
	char *buf;	buffer pointer

		Writes the zero terminated string from buf onto unit.

   ============================================================================
*/

/*  */

#include "stddefs.h"
#include "errdefs.h"
#include "ascii.h"
#include "biosdefs.h"


int
readln (unit, nc, ctl, nb, buf)
     int unit;			/* logical unit number 1..4 */
     int nc;			/* length of control string ctl */
     char *ctl;			/* control string pointer */
     int nb;			/* buffer limit */
     char *buf;			/* buffer pointer (at least nb+1 bytes) */
{
  register char *cp;
  register int i, j;
  register char *bp;
  register char c;


  if (unit LT 1 OR unit GT 4)	/* verify unit number is in range */
    return (ERR15);		/* return ERR15 if not */

  bp = buf;			/* setup buffer pointer */

  for (i = 0; i < nb; i++)
    {				/* main read loop */

      c = BIOS (B_GETC, unit) & 0xFF;	/* get a byte from the unit */

      *bp++ = c;		/* add it to the buffer */
      *bp = '\0';		/* ... followed by a zero byte */

      cp = ctl;			/* setup to scan ctl for the character */

      for (j = 0; j < nc; j++)	/* scan each byte of ctl */
	if (*ctl++ EQ c)	/* done if we find it */
	  return ((int) c);

      BIOS (B_PUTC, unit, c);	/* echo the character */
    }

  return (ERR01);		/* buffer full */
}

/*  */

int
getln (unit, nb, buf)
     int unit;			/* logical unit number 1..4 */
     int nb;			/* buffer limit */
     char *buf;			/* buffer pointer */
{
  register char *bp;
  register char c;
  register int bc;

  bc = 0;			/* number of characters currently in buffer */
  bp = buf;			/* current buffer pointer */
  *bp = '\0';			/* initial null into buffer */

  while (bc LT nb)
    {

      c = BIOS (B_GETC, unit) & 0xFF;

      switch (c)
	{

	case A_BS:
	case A_DEL:

	  if (bc EQ 0)
	    {

	      BIOS (B_PUTC, unit, A_BEL);
	      break;

	    }
	  else
	    {

	      bc--;
	      bp--;
	      *bp = '\0';
	      BIOS (B_PUTC, unit, A_BS);
	      BIOS (B_PUTC, unit, ' ');
	      BIOS (B_PUTC, unit, A_BS);
	      break;
	    }

	case A_CR:
	case A_LF:

	  *bp++ = c;		/* put character in buffer */
	  *bp = '\0';		/* terminate line with null */
	  return ((int) c);	/* return -- CR or LF hit */

/*  */

	case CTL ('X'):

	  *buf = '\0';		/* clear the buffer */
	  return ((int) c);	/* return -- line cancelled */

	default:

	  *bp++ = c;		/* put character in buffer */
	  *bp = '\0';		/* terminate line with null */
	  BIOS (B_PUTC, unit, c);	/* echo the character */
	  bc++;			/* update character count */
	}
    }

  return (ERR01);		/* buffer full error */
}

/*  */

int
getrln (unit, nb, buf)
     int unit;			/* logical unit number 1..4 */
     int nb;			/* buffer limit */
     char *buf;			/* buffer pointer */
{
  register char *bp;
  register char c;
  register int bc;

  bc = 0;			/* number of characters currently in buffer */
  bp = buf;			/* current buffer pointer */
  *bp = '\0';			/* initial null into buffer */

  while (bc LT nb)
    {

      c = BIOS (B_GETC, unit) & 0xFF;

      switch (c)
	{

	case A_CR:
	case A_LF:
	case CTL ('Z'):

	  *bp++ = c;		/* put character in buffer */
	  *bp = '\0';		/* terminate line with null */
	  return ((int) c);	/* return -- CR, LF, or ^Z hit */

	case CTL ('X'):

	  *buf = '\0';		/* clear the buffer */
	  return ((int) c);	/* return -- line cancelled */

	default:

	  *bp++ = c;		/* put character in buffer */
	  *bp = '\0';		/* terminate line with null */
	  bc++;			/* update character count */
	}
    }

  return (ERR01);		/* buffer full error */
}

/*  */

writeln (unit, buf)
     int unit;			/* logical unit number 0..4 */
     char *buf;			/* buffer pointer */
{
  register char *bp;
  register char c;

  bp = buf;			/* setup buffer pointer */

  while (c = *bp++)		/* send the string, a byte at a time */
    BIOS (B_PUTC, unit, c);
}
