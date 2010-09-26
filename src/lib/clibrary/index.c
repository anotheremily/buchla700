/*
   ============================================================================
	index.c -- BSD style index function
	Version 1 -- 1987-02-11 -- D.N. Lynx Crowe
   ============================================================================
*/

char *
index(str, c)
register char *str;
register char c;
{
	while (*str) {

		if (*str == c)
			return(str);

		++str;
	}

	return((char *)0);
}

