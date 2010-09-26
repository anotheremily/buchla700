/*
   =============================================================================
	strrchr.c -- find last occurrence of a character in a string
	Version 1 -- 1987-06-12

	Return the ptr in sp at which the character c last
	appears; NULL if not found.
   =============================================================================
*/

#define	NULL	(char *)0

char *
strrchr(sp, c)
register char *sp, c;
{
	register char *r;

	r = NULL;

	do {

	if (*sp == c)
		r = sp;

	} while(*sp++);

	return(r);
}
