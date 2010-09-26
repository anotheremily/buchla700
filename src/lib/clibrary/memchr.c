/*
   =============================================================================
	memchr.c -- search a string for a character
	Version 1 -- 1987-06-12

	Return the ptr in sp at which the character c appears;
	NULL if not found in n chars; don't stop at \0.
   =============================================================================
*/

char *
memchr(sp, c, n)
register char *sp, c;
register int n;
{
        while (--n >= 0)
                if (*sp++ == c)
                        return(--sp);
        return((char *)0);
}
