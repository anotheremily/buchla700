/*
   =============================================================================
	addfpu.c -- FPU time functions
	Version 1 -- 1987-09-14 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"

#define	MAXSEGT		32767

extern unsigned tofpu (), fromfpu ();

/* 
*/

/*
   =============================================================================
	addfpu() -- find next higher FPU time 'k' milliseconds greater than 't'
   =============================================================================
*/

unsigned
addfpu (t, k)
     unsigned t;
     short k;
{
  register short x, z;
  register unsigned y;

  x = fromfpu (t);

  if (x == MAXSEGT)
    return (t);

  y = t;
  z = x + k;

  if (z >= MAXSEGT)
    return (tofpu (MAXSEGT));

  while ((y == t) || (z > x))
    y = tofpu (++x);

  return (y);
}

/* 
*/

/*
   =============================================================================
	subfpu() -- find next lower FPU time 'k' milliseconds less than 't'
   =============================================================================
*/

unsigned
subfpu (t, k)
     unsigned t;
     short k;
{
  register short x, z;
  register unsigned y;

  x = fromfpu (t);

  if (x == 1)
    return (t);

  y = t;
  z = x - k;

  if (z <= 1)
    return (tofpu (1));

  while ((y == t) || (z < x))
    y = tofpu (--x);

  return (y);
}
