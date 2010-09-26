/*
   =============================================================================
	strfill() -- fill a string with a constant byte and zero terminate it
	Version 1 -- 1988-08-19 -- D.N. Lynx Crowe

	Where:
		s = string pointer
		c = constant byte
		n = string length (not including the trailing zero)

	Returns a pointer to the string.
   =============================================================================
*/

char *
strfill(s, c, n)
register char *s;
register char c;
register unsigned n;
{
	register unsigned i;
	char *p;

	p = s;

	for (i = n; i--; )
		*s++ = c;

	*s = '\0';
	return(p);
}

