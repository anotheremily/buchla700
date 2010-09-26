/*
   ============================================================================
	rindex.c -- BSD style rindex function
	Version 2 -- 1987-06-12 -- D.N. Lynx Crowe
   ============================================================================
*/

char *
rindex(str, c)
register char *str;
char c;
{
	register char *cp;

	for (cp = str ;*cp++ ; )
		;

	while (cp > str)
		if (*--cp == c)
			return(cp);

	return((char *)0);
}
