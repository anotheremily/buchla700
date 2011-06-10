/*
   ============================================================================
	flread.c -- read a long buffer
	Version 2 -- 1987-07-09 -- D.N. Lynx Crowe
   ============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

#define	CHUNK	32256		/* largest practical sized chunk of data */

/*
   ============================================================================
	flread(buff, len, fp) -- Reads 'len' bytes into 'buff' from file 'fp'.

	Returns SUCCESS (0) if OK, EOF (-1) on end of file.
   ============================================================================
*/

int
flread (buff, len, fp)
     register char *buff;
     register long len;
     FILE *fp;
{
  register int ilen;

  while (len > 0L)
    {

      if (len GE (long) CHUNK)
	{

	  if (1 NE fread (buff, CHUNK, 1, fp))
	    return (EOF);

	  buff += (long) CHUNK;
	  len -= (long) CHUNK;

	}
      else
	{

	  ilen = len;

	  if (1 NE fread (buff, ilen, 1, fp))
	    return (EOF);

	  len = 0L;
	}
    }

  return (SUCCESS);
}
