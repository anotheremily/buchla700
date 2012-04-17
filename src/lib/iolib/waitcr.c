/*
   =============================================================================
	waitcr.c -- wait for a CR from the console and allow for debugging
	Version 3 -- 1987-07-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "biosdefs.h"
#include "stddefs.h"

extern int xtrap15 ();		/* the ROMP trap */

/*
   =============================================================================
	waitcr() -- wait for a CR or a CTL-G.  CR returns control to the
	caller, CTL-G invokes xtrap15(), which if it returns, returns to
	the caller.  The xtrap15() escape is to allow a convenient debug
	point whenever we wait on the console.  The character that caused
	the return  (CR or CTL-G) is returned as the value of the function.
   =============================================================================
*/

int
waitcr ()
{
  int c;

  BIOS (B_PUTC, CON_DEV, '\007');	/* wake up the operator */

  /* await a CR, in which case we just exit */

  while ('\r' != (c = (0x007F & (int) BIOS (B_GETC, CON_DEV))))
    if (c == '\007')		/* ... or a control-G */
      xtrap15 ();		/* ... in which case we trap first */

  return (c);			/* return the character that caused us to exit */
}
