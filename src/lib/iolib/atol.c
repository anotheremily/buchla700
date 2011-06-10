/*
   =============================================================================
	atol.c -- ascii to long conversion
	Version 3 -- 1987-06-29 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "ctype.h"

long
atol (cp)
     register char *cp;
{
  register long n;
  register short sign;

  sign = 0;

  if (*cp EQ '-')
    {

      ++cp;
      sign = 1;

    }
  else
    {

      if (*cp EQ '+')
	++cp;
    }

  for (n = 0; isdigit (*cp);)
    n = n * 10 + *cp++ - '0';

  return (sign ? -n : n);
}
