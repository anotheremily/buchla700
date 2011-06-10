/*
   ============================================================================
	setbuf.c -- set a stream buffer
	Version 1 -- 1987-06-26 -- D.N. Lynx Crowe
   ============================================================================
*/

#include "stdio.h"

setbuf (stream, buffer)
     register FILE *stream;
     char *buffer;
{
  if (stream->_buff)
    return;

  if (buffer)
    {

      stream->_buff = buffer;
      stream->_buflen = BUFSIZ;

    }
  else
    {

      stream->_buff = &stream->_bytbuf;
      stream->_buflen = 1;
    }
}
