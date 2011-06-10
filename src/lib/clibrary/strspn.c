/*
   ============================================================================
	strspn.c -- string scan function ala' Unix(tm)
	Version 2 -- 1987-06-12 -- D.N. Lynx Crowe

	Return the number of characters in the maximum leading segment
	of 'string' which consists solely of characters from 'charset'.
   ============================================================================
*/

int
strspn (string, charset)
     char *string;
     register char *charset;
{
  register char *p, *q;
  register int n = 0;

  for (q = string; *q != '\0'; ++q)
    {

      for (p = charset; *p != '\0' && *p != *q; ++p)
	;

      if (*p == '\0')
	break;

      n++;
    }

  return (n);
}
