/*
   =============================================================================
	rdline.c -- read a line into a buffer
	Version 1 -- 1988-12-29 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

/*
   =============================================================================
	rdline(fp, buf, maxc) -- read a line from a file into a buffer

	fp	FILE pointer
	buf	buffer pointer
	maxc	maximum line length  (not counting the terminating zero)

	The buffer must have room for maxc + 1 bytes.  The final byte will be
	a terminating zero.

	returns:
			 0	success		line read - complete with LF
			-1	failure		immediate EOF - no data read
			 1	failure		read to EOF - missing LF at end
			 2	failure		buffer full - missing LF at end
   =============================================================================
*/

short
rdline(fp, buf, maxc)
register FILE *fp;
register char *buf;
register short maxc;
{
	register short c, i;

	*buf = '\0';

	for (i = 0; i < maxc; i++) {

		if (EOF EQ (c = fgetc(fp)))
			return(i ? 1 : -1);

		*buf++ = c;
		*buf   = '\0';

		if (c EQ '\n')
			return(0);
	}

	return(2);
}
