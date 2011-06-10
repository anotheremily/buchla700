/*
   =============================================================================
	strrevi.c -- reverse a string in place
	Version 1 -- 1988-02-03 -- D.N. Lynx Crowe

	char *
	strrevi(s)
	char *s;

		Reverses string 's1' in place.  Returns 's1'.
   =============================================================================
*/

char *
strrevi (s)
     char *s;
{
  register char *p1, *p2;
  register long i;
  register char c;

  p1 = s;			/* initialize left end pointer */
  p2 = s;			/* initialize right end pointer */
  i = 0L;			/* initialize character count */

  while (*p2)
    {				/* find the right end of the string */

      ++i;
      ++p2;
    }

  --p2;				/* adjust right end pointer */
  i >>= 1;			/* calculate swap count */

  while (i--)
    {				/* for each pair of characters ... */

      c = *p2;			/* ... swap right and left characters */
      *p2-- = *p1;
      *p1++ = c;
    }

  return (s);			/* return pointer to reversed string */
}
