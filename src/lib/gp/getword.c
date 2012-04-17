/*
   =============================================================================
	getword.c -- Get a string from a file
	Version 2 -- 1988-07-13 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "ctype.h"
#include "stddefs.h"

/*
   =============================================================================
	getword(string, ioptr) -- get a string from a file

	getword(string, ioptr) is equivalent to fscanf(ioptr, "%s", string)
	but much faster.  It returns a NULL pointer on EOF or the end
	of the string when one is found.  The return result can be used
	to find the length of the obtained string, or to append further
	characters to the returned string.
   =============================================================================
*/

char *
getword (string, ioptr)
     register char *string;
     register FILE *ioptr;
{
  register int c;

  while (((c = getc (ioptr)) != EOF) && isspace (c))
    ;

  if (c == EOF)
    return (NULL);

  do
    {
      *string++ = c;

    }
  while (((c = getc (ioptr)) != EOF) && (! isspace (c)));

  *string = '\0';		/* terminate the string */

  return (string);		/* return a pointer to the end of the string */
}
