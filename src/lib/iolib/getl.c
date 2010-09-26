/*
   =============================================================================
	getl.c -- get a 68000 long word from a stream file
	Version 2 -- 1987-10-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"

extern	int	fgetc();

/*
   =============================================================================
	getl(stream) -- get a long word from 'stream'

	Reads a long from 'stream' in Motorola 68000 byte order.
	No special alignment is assumed in the file.
   =============================================================================
*/

long
getl(stream)
register FILE *stream;
{
	long temp;
	register char *t;

	t = &temp;

	*t++ = fgetc(stream);
	*t++ = fgetc(stream);
	*t++ = fgetc(stream);
	*t = fgetc(stream);

	return(temp);
}
