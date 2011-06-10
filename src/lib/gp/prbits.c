/*
   =============================================================================
	prbits.c -- print groups of bits from a number
	Version 1 -- 1988-09-21 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

/*
   =============================================================================
	prbits(n, l, r, gs) -- print selected groups of bits of 'n'

		'n'	number to print bits from
		'r'	rightmost bit to use 		starting with the LSB
		'l'	leftmost bit to use		starting with the LSB
		'gs'	bits in each group		starting on the right
   =============================================================================
*/

prbits (n, l, r, gs)
     register long n;
     register int l, r, gs;
{
  register char *bp;
  register int i;
  char buf[66];

  if ((l < 0) OR (l > 31) OR
      (r < 0) OR (r > 31) OR (gs < 1) OR (gs > 32) OR (r > l))
    return;

  bp = &buf[65];
  *bp-- = '\0';

  for (i = r; i < (l + 1); i++)
    {

      if ((i NE 0) AND (0 EQ (i % gs)))
	*bp-- = ' ';

      if (n & (1L << i))
	*bp-- = '1';
      else
	*bp-- = '0';
    }

  ++bp;

  if (*bp EQ ' ')
    ++bp;

  while ('\0' NE (c = *bp++))
    putchar (c);
}
