/*
   =============================================================================
	strlen.c -- string length function ala' Unix(tm)
	Version 1 -- 1987-02-11 -- D.N. Lynx Crowe

	Returns the number of non-NULL bytes in string argument.
   =============================================================================
*/


int
strlen(s)
register char *s;
{
        register int n = 0;

        while (*s++ != '\0')
               ++n;

        return (n);
}
