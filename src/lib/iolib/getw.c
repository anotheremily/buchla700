/*
   =============================================================================
	getw.c -- get a 68000 word from a stream file
	Version 2 -- 1987-10-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"

extern int fgetc ();

/*
   =============================================================================
	getw(stream) -- get a word from 'stream'

	Reads a word from 'stream' in Motorola 68000 byte order.
	No special alignment is assumed in the file.
   =============================================================================
*/

int
getw (stream)
     register FILE *stream;
{
  int temp;
  register char *t;

  t = &temp;

  *t++ = fgetc (stream);
  *t = fgetc (stream);

  return (temp);
}
