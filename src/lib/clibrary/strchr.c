/*
   =============================================================================
	strchr.c -- find a character in a string

	Return the ptr in sp at which the character c appears;
	NULL if not found
   =============================================================================
*/

#define NULL    0

char *
strchr(sp, c)
register char *sp, c;
{
	do {
		if(*sp == c)
			return(sp);
	} while(*sp++);

	return((char *)NULL);
}
