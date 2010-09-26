/*
   =============================================================================
	strcmp.c -- compare strings
	Version 2 -- 1987-06-15

	Compare strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0
   =============================================================================
*/

int
strcmp(s1, s2)
register char *s1, *s2;
{
	if(s1 == s2)
		return(0);

	while(*s1 == *s2++)
		if(*s1++ == '\0')
			 return(0);

	return((int)*s1 - (int)*--s2);
}
