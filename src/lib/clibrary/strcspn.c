/*
   ============================================================================
	strcspn.c -- character scan function ala' Unix(tm)
	Version 1 -- 1987-02-11 -- D.N. Lynx Crowe

	Return the number of characters in the maximum leading segment
	of string which consists solely of characters ! from charset.
   ============================================================================
*/

int
strcspn (string, charset)
     char *string;
     register char *charset;
{
  register char *p, *q;
  register int n = 0;

  for (q = string; *q != '\0'; ++q, ++n)
    {

      for (p = charset; *p != '\0' && *p != *q; ++p)
	;

      if (*p != '\0')
	break;
    }

  return (n);
}
