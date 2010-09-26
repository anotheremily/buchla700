/*
   =============================================================================
	memcpyw.c -- copy words
	Version 2 -- 1987-06-15 -- D.N. Lynx Crowe

	Copy s2 to s1, always copy n words.
	Return pointer to s1.
   =============================================================================
*/

short *
memcpyw(s1, s2, n)
register short *s1, *s2;
register int n;
{
        register short *os1 = s1;

        while (--n >= 0)
                *s1++ = *s2++;
        return (os1);
}
