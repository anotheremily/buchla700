/*
   =============================================================================
	memcmp.c -- compare memory areas
	Version 2 -- 1987-06-15

	Compare n bytes:  s1>s2: >0  s1==s2: 0  s1<s2: <0
   =============================================================================
*/

int
memcmp(s1, s2, n)
register char *s1, *s2;
register int n;
{
	register int	diff;

        if (s1 != s2)
                while (--n >= 0)
                        if (diff = *s1++ - *s2++)
                                return(diff);
        return(0);
}
