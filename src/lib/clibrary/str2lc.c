/*
   ============================================================================
	str2lc(s) -- convert string s to lower case ASCII
	Version 2 -- 1987-06-12 -- Copyright 1987 -- D.N. Lynx Crowe
   ============================================================================
*/

char *
str2lc(s)
register char *s;
{
	register char c;
	register char *r = s;

	while (c = *s)
		*s++ = tolower(c);

	return(r);
}
