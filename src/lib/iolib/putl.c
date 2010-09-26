/*
   ============================================================================
	putl.c -- put a Motorola 68000 long word onto a stream file
	Version 2 -- 1987-10-15 -- D.N. Lynx Crowe

	WARNING:  This fails if the stream is ASCII.
   ============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

putl(w, stream)
register long w;
register FILE *stream;
{
	if (putc(((w >> 24) & 0xFF), stream) < 0 ) 
		return;

	if (putc(((w >> 16) & 0xFF), stream) < 0 ) 
		return;

	if (putc(((w >> 8) & 0xFF), stream) < 0 ) 
		return;

	putc((w & 0xFF), stream);
}
