/*
   ============================================================================
	str2uc(s) -- converts string s to upper case ASCII
	Version 2 -- 1987-06-12 -- (c) Copyright 1987 -- D.N. Lynx Crowe
   ============================================================================
*/

char *
str2uc(s)
register char *s;
{
	register char c;
	register char *r = s;

	while (c = *s)
		*s++ = toupper(c);

	return(r);
}
