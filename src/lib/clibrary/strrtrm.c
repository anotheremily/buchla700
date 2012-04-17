/*
   =============================================================================
	strrtrm() -- trim trailing blanks from a string, in-place
	Version 1 -- 1988-08-19 -- D.N. Lynx Crowe

	Returns a pointer to the string.
   =============================================================================
*/

#include "stddefs.h"

char *
strrtrm (s)
     char *s;
{
  register char *lp, *rp;

  lp = s;
  rp = s;

  while (*rp)
    ++rp;

  while ((long) (--rp) >= (long) lp)
    {

      if (*rp == ' ')
	*rp = '\0';
      else
	break;
    }

  return (lp);
}
