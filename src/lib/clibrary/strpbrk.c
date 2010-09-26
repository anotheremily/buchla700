/*
   =============================================================================
	strpbrk.c -- find a 'break' character in a string
	Version 1 -- 1987-06-12

	Return ptr to first occurance of any character from `brkset'
	in the character string `string'; NULL if none exists.
   =============================================================================
*/

char *
strpbrk(string, brkset)
register char *string, *brkset;
{
	register char *p;

	do {

	for (p = brkset; *p != '\0' && *p != *string; ++p)
		;

	if (*p != '\0')
		return(string);
	}

	while(*string++)
		;

	return((char *)0);
}
