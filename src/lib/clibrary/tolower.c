/*
   ============================================================================
	tolower.c -- convert character to lower case ASCII
	Version 1 -- 1987-09-11 -- D.N. Lynx Crowe
   ============================================================================
*/

#include "ctype.h"

/*
   ============================================================================
	tolower(c) -- convert c to lower case ASCII
   ============================================================================
*/

int
tolower (c)
     int c;
{
  int x;

  if (isascii (c))
    {

      if (isupper (c))
	x = _tolower (c);
      else
	x = c;

    }
  else
    {

      x = c;
    }

  return (x);
}
