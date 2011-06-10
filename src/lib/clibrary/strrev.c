/*
   =============================================================================
	strrev.c -- reverse a string
	Version 1 -- 1988-02-03 -- D.N. Lynx Crowe

	char *
	strrev(s1, s2)
	char *s1;
	char *s2;

		Copies string 's2' into 's1'.  's1' must be large enough
		to hold 's2'.  Returns 's1'.
   =============================================================================
*/

char *
strrev (s1, s2)
     char *s1;
     register char *s2;
{
  register char *s3;
  register long i;

  i = 0L;			/* initialize string length */
  s3 = s1;			/* initialize target left end pointer */

  while (*s2)
    {				/* find right end of source string */

      ++s2;
      ++i;
    }

  --s2;				/* adjust source right end pointer */

  while (i--)			/* copy string in reverse */
    *s3++ = *s2--;

  *s3 = '\0';			/* terminate the string */
  return (s1);			/* return address of reversed string */
}
