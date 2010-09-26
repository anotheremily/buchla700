/*
   =============================================================================
	now.c -- return the date and time as a string
	Version 2 -- 1988-10-28 -- D.N. Lynx Crowe
	(c) Copyright 1987, 1988 -- D.N. Lynx Crowe
	
	Version 3 -- 2010-07-03 -- zyoung
	
	Updated to use time functions available in time.h.
	
	@flag WORKING
	@flag TESTED
   =============================================================================
*/

#define	TESTER	0		/* define non-zero to get a test program */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
   =============================================================================
	now(p) -- return the date and time as a string in 'p'

	Returns a pointer to the string provided, which must be at least
	21 bytes long.  The string will be filled with the date and time
	in the format:

		yyyy-mm-dd  hh:mm:ss

	with a trailing zero byte.
   =============================================================================
*/

char *
now(p)
char *p;
{
	time_t curtime;
	struct tm *loctime;
	
	/* Get the current time.  */
	curtime = time (NULL);
	
	/* Convert it to local time representation.  */
	loctime = localtime (&curtime);

	strftime(p, 256, "%Y-%m-%d  %H:%M:%S", loctime);
	return(p);
}

/* */

#if	TESTER

char	x[22];		/* buffer for the returned string */

/* simple test program for the now() function */

int main()
{
	printf("Date/Time = %s\n", now(x));
	return 0;
}

#endif
