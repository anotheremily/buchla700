/*
   =============================================================================
	micons.c -- motorola / intel format conversion functions
	Version 3 -- 1987-06-11 -- D.N. Lynx Crowe

	short
	micons(wi)
	short wi;

		Convert between motorola and intel format for a short.

	int
	miconi(wi)
	int wi;

		Convert between motorola and intel format for an int.

	long
	miconl(wi)
	long wi;

		Convert between motorola and intel format for a long.
   =============================================================================
*/

#define	TESTER	0		/* define non-zero for a test program */

/*  */

/*
   =============================================================================
	micons(wi) -- Convert between motorola and intel format for a short.
   =============================================================================
*/

short
micons (wi)
     short wi;
{
  return ((short) (((wi << 8) & 0xFF00) | ((wi >> 8) & 0x00FF)));
}

/*
   =============================================================================
	miconi(wi) -- Convert between motorola and intel format for an int.
   =============================================================================
*/

int
miconi (wi)
     int wi;
{
  if (sizeof (int) == 4)
    return (((wi << 24) & 0xFF000000L) | ((wi << 8) & 0x00FF0000L) |
	    ((wi >> 8) & 0x0000FF00L) | ((wi >> 24) & 0x000000FFL));
  else
    return (((wi << 8) & 0xFF00) | ((wi >> 8) & 0x00FF));
}

/*
   =============================================================================
	miconl(wi) -- Convert between motorola and intel format for a long.
   =============================================================================
*/

long
miconl (wi)
     long wi;
{
  return (((wi << 24) & 0xFF000000L) | ((wi << 8) & 0x00FF0000L) |
	  ((wi >> 8) & 0x0000FF00L) | ((wi >> 24) & 0x000000FFL));
}

/*  */

#if	TESTER

#include "stdio.h"

/*
   =============================================================================
	test program for micon functions
   =============================================================================
*/

main ()
{
  printf ("micons(0x1234) returned 0x%04x\n", micons (0x1234));

  if (sizeof (int) == 4)
    printf ("miconi(0x1234) returned 0x%04x\n", miconi (0x1234));
  else
    printf ("miconi(0x12345678L) returned 0x%08lx\n", miconi (0x12345678L));

  printf ("miconl(0x12345678L) returned 0x%08lx\n", miconl (0x12345678L));
}

#endif
