/*
   =============================================================================
	printf.c -- printf function
	Version 6 -- 1987-08-13 -- D.N. Lynx Crowe

	RECOVERED From:  Version 5 -- 1987-06-16 -- D.N. Lynx Crowe

	"Crufty code" Warning:
		Since this isn't Unix(tm), we prepend a '\r' when we see a '\n'.
		We also return a long, since this is a 32 bit address machine.
		(Well, almost 32 bits.  Too big for an Alcyon 16 bit int anyway.)
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"
#include "varargs.h"

extern long dofmt_ ();

/*
   =============================================================================
	printf(fmt, args) -- output 'args' according to 'fmt' on CON_DEV
   =============================================================================
*/

long
printf (fmt, va_alist)
     char *fmt;
     va_dcl
{
  register long count;
  int fpsub ();
  va_list aptr;

  va_start (aptr);
  count = dofmt_ (fpsub, fmt, aptr);
  va_end (aptr);
  return (count);
}

/*
   =============================================================================
	fpsub(c) -- output 'c' to CON_DEV
   =============================================================================
*/

static int
fpsub (c)
     int c;
{
  /* KLUDGE:  since we aren't Unix(tm) we prepend a CR to LF's */

  if (c == '\n')
    BIOS (B_PUTC, CON_DEV, '\r');

  BIOS (B_PUTC, CON_DEV, c);
  return (c);
}
