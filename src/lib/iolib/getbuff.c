/*
   ============================================================================
	getbuff.c -- Get a buffer for a file for the Buchla 700 C I/O Library
	Version 4 -- 1987-06-24 -- D.N. Lynx Crowe
   ============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

getbuff (ptr)
     register FILE *ptr;
{
  char *buffer;

  if (ptr->_buflen == 1)
    {				/* see if we want the small buffer */

      ptr->_buff = &ptr->_bytbuf;
      return;
    }

  if (Stdbufs)
    {				/* see if we have any standard buffers left */

      buffer = (char *) Stdbufs;
      Stdbufs = *(long **) Stdbufs;

    }
  else
    {				/* ... if not, use the small one */

      ptr->_buff = &ptr->_bytbuf;
      ptr->_buflen = 1;
      return;
    }

  ptr->_flags |= _ALLBUF;	/* say we allocated a standard buffer */
  ptr->_buff = buffer;
  return;
}
