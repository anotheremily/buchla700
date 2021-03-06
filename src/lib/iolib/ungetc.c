/*
   ============================================================================
	ungetc.c -- 'unget' a character
	Version 2 -- 1987-06-26 -- D.N. Lynx Crowe
   ============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

int
ungetc (c, ptr)
     register int c;
     register FILE *ptr;
{
  if ((c == EOF) || (ptr->_bp <= ptr->_buff))
    return (EOF);

  *--ptr->_bp = c;

  return (c);
}
