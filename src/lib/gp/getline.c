/*
   =============================================================================
	getline.c -- get a line from stdin
	Version 1 -- 1988-05-03 -- D.N. Lynx Crowe
	Does line editing.  Acts like gets().
   =============================================================================
*/

#include "stdio.h"
#include "ascii.h"
#include "stddefs.h"

char *
getline (buf, nb)
     char *buf;			/* buffer pointer */
     int nb;			/* buffer limit */
{
  register char *bp;
  register int c;
  register int bc;

  bc = 0;			/* number of characters currently in buffer */
  bp = buf;			/* current buffer pointer */
  *bp = '\0';			/* initial null into buffer */

  while (bc < nb)
    {				/* read up to limit */

      c = getchar ();

      switch (c)
	{

	case A_BS:
	case A_DEL:

	  if (bc == 0)
	    {

	      putchar (A_BEL);	/* complain -- nothing there */
	      break;

	    }
	  else
	    {

	      bc--;		/* decrement byte count */
	      bp--;		/* back-up the pointer */
	      *bp = '\0';	/* terminate the string */

	      putchar (A_BS);	/* erase character from screen */
	      putchar (' ');
	      putchar (A_BS);

	      break;
	    }

	case A_CR:
	case A_LF:

	  *bp = '\0';		/* terminate line with null */
	  return (buf);		/* return -- CR or LF hit */

/*  */

	case CTL ('X'):

	  *buf = '\0';		/* clear the buffer */

	  while (bc--)
	    {

	      putchar (A_BS);
	      putchar (' ');
	      putchar (A_BS);
	    }

	  bc = 0;		/* reset byte count */
	  bp = buf;		/* reset buffer pointer */
	  break;

	case EOF:

	  return (NULL);

	default:

	  *bp++ = (char) c;	/* put character in buffer */
	  *bp = '\0';		/* terminate line with null */
	  putchar (c);		/* echo the character */
	  bc++;			/* update character count */
	}
    }

  buf[nb - 1] = '\0';		/* terminate buffer */
  return (buf);			/* buffer full error */
}
