/*
   =============================================================================
	curtab.c -- compute sine wave cursor table for waveshape editor
	Written by D.N. Lynx Crowe -- see VER below for version
   =============================================================================
*/

#define	VER	"Version 4 -- 1987-10-07"

#define	DEBUGIT		0

#include "stdio.h"
#include "limits.h"
#include "math.h"
#include "stddefs.h"

#define TWOPI	((double)(2.0 * PI))

#define TABLEN	256
#define	DTABLEN	((double)256.0)

#define	LO_LIM	64
#define	HI_LIM	192

#define	ADJUSTER	((double)32768.0)

#define	THE_FILE	"wdcurtab.dat"

char *fname1;

int t[TABLEN];
double k[TABLEN];
double m[TABLEN];

/* 
*/

/*
   =============================================================================
	curtab.c:  main -- compute sine wave cursor table for waveshape editor
   =============================================================================
*/

main ()
{
  register FILE *fp1;
  register int i, j;
  double q;

  printf ("Buchla 700 Waveshape Cursor Table Generator %s\n", VER);

  fname1 = THE_FILE;

  if ((fp1 = fopen (fname1, "wa")) EQ NULL)
    {

      printf ("curtab:  ERROR - couldn't open [%s]\n", fname1);
      exit ();
    }

  printf ("curtab:  Creating %d entry waveshape cursor table\n", TABLEN);

  q = TWOPI / DTABLEN;

  for (i = 0; i < TABLEN; i++)
    {

      k[i] = sin ((double) i * q);

      if (k[i] GE (double) 1.0)
	  k[i] = (double) 0.999999;

      m[i] = (k[i] + (double) 1.0) / (double) 2.0;
      t[i] = m[i] * ADJUSTER;

#if DEBUGIT
      if (!(i & 0x00000007))
	printf ("%4d %04x %04.4x %10.8f %11.8f\n",
		i, i, (t[i] & 0x0000FFFF), k[i], m[i]);
#endif
    }

  printf ("curtab:  Writing entries %d thru %d to file [%s]\n",
	  LO_LIM, (HI_LIM - 1), fname1);

  for (i = LO_LIM, j = 0; i < HI_LIM; i++, j++)
    fprintf (fp1, "\t0x%04.4x,\t/* %4d  %4d  %11.8f %11.8f */\n",
	     (t[i] & 0x0000FFFF), j, i, k[i], m[i]);

  fclose (fp1);

  printf ("curtab:  Output complete on [%s].\n", fname1);
}
