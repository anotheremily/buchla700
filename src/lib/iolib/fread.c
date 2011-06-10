/*
   =============================================================================
	fread.c -- read a stream file
	Version 3 -- 1987-06-29 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

/*
   =============================================================================
	fread(buffer, size, number, stream) -- read 'number' items of size
	'size' bytes from file 'stream' into 'buffer'.
   =============================================================================
*/

int
fread (buffer, size, number, stream)
     register char *buffer;
     unsigned size;
     int number;
     FILE *stream;
{
  int total;
  register int c, i;

  for (total = 0; total < number; ++total)
    {

      for (i = size; i; --i)
	{

	  if ((c = getc (stream)) EQ EOF)
	    return (total);

	  *buffer++ = c;
	}
    }
  return (total);
}
