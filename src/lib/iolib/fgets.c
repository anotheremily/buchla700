/*
   =============================================================================
	fgets.c -- get a string from an ASCII stream file
	Version 2 -- 1987-07-09 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

#define	EATCHAR	'\n'		/* character to be "eaten" on input */

int
agetc (ptr)
     register FILE *ptr;
{
  register int c;

top:
  if ((c = getc (ptr)) != EOF)
    {

      switch (c &= 0x7F)
	{

	case 0x1a:		/* {x}DOS EOF */
	  --ptr->_bp;
	  return (EOF);

	case EATCHAR:		/* CR or LF */
	case 0:		/* NUL */
	  goto top;
	}
    }

  return (c);
}

char *
gets (line)
     char *line;
{
  register char *cp;
  register int i;

  cp = line;

  while ((i = getchar ())NE EOF && i != '\n')
    *cp++ = i;

  *cp = 0;			/* terminate the line */

  if ((i == EOF) && (cp == line))
    return (NULL);

  return (line);
}

char *
fgets (s, n, fp)
     char *s;
     int n;
     FILE *fp;
{
  register c;
  register char *cp;

  cp = s;

  while (--n > 0 && (c = agetc (fp)) != EOF)
    {

      *cp++ = c;

      if (c == '\n')
	break;
    }

  *cp = 0;

  if (c == EOF && cp == s)
    return (NULL);

  return (s);
}
