/*
   =============================================================================
	fwrite.c -- write a stream file
	Version 3 -- 1987-07-09 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

/*
   =============================================================================
	fwrite(buffer, size, number, stream) -- write 'number' items of sixe
	'size' bytes from 'buffer' onto 'stream'.  Returns the number of items
	written.  If 'number' or 'size' is negative, nothing is written, and
	0 is returned.  EOF may cause the last item written to be imcomplete.
	Check with feof() to detect an EOF condition.  Use ferror() to check
	for error conditions.
   =============================================================================
*/

int
fwrite (buffer, size, number, stream)
     register char *buffer;
     register int size, number;
     register FILE *stream;
{
  register int i, j;

  if (size < 0)			/* check size for validity */
    return (0);

  if (number < 0)		/* check number for validity */
    return (0);

  for (i = 0; i < number; ++i)
    for (j = 0; j < size; ++j)
      if (putc (*buffer++, stream) == EOF)
	return (i);

  return (number);
}
