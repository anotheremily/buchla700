/*
   =============================================================================
	sintest.c -- test the sin() function and floating point stuff
	Version 1 -- 1987-08-10 -- D.N. Lynx Crowe
	Setup for MetaComCo/Lattice C on the Atari 1040ST
   =============================================================================
*/

#include "stdio.h"
#include "limits.h"
#include "math.h"

#define NPTS	360
#define PI2	(2.0 * PI)
#define XI	(PI2 / (float)NPTS)

double points[NPTS];		/* array of y=sin(x) values */
double xvals[NPTS];		/* array of x values */

main ()
{
  double x;			/* temporary value */

  int i;			/* temporary value */

  /* create an array of NPTS points of sin(x) from x=0 to 2 PI radians */

  for (i = 0; i < NPTS; i++)
    {

      x = (double) i *XI;
      xvals[i] = x;
      points[i] = sin (x);
    }

  for (i = 0; i < NPTS; i++)
    printf ("%3d: sin(%2.6f) = %2.6f\n", i, xvals[i], points[i]);

  printf ("XI = %3.6f, PI2 = %3.6f, PI = %3.6f\n", XI, PI2, PI);

  exit (0);
}
