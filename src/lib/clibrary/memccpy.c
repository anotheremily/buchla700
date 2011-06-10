/*
   =============================================================================
	memccpy.c -- copy bytes until character seen
	Version 1 -- 1987-06-12

	Copy s2 to s1, stopping if character c is copied.
	Copy no more than n bytes.
	Return a pointer to the byte after character c in the copy,
	or NULL if c is not found in the first n bytes.
   =============================================================================
*/

char *
memccpy (s1, s2, c, n)
     register char *s1, *s2;
     register char c;
     register int n;
{
  while (--n >= 0)
    if ((*s1++ = *s2++) == c)
      return (s1);
  return ((char *) 0);
}
