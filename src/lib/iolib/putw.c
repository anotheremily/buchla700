/*
   ============================================================================
	putw.c -- put a word onto a stream file
	Version 3 -- 1987-10-15 -- D.N. Lynx Crowe

	WARNING:  This fails if the stream is ASCII.
   ============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

putw (w, stream)
     register unsigned w;
     FILE *stream;
{
  if (putc (((w >> 8) & 0xFF), stream) < 0)
    return;

  putc ((w & 0xFF), stream);
}
