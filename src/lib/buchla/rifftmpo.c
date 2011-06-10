/*
   =============================================================================
	rifftmpo.c -- calculate the Thunder riff tempo multiplier table
	Version 1 -- 1989-01-25 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

#define	RTFILE	"rifftmpo.dat"

double rate[19][2] = {

  {8.0, 1.0},
  {5.0, 1.0},
  {4.0, 1.0},
  {3.0, 1.0},
  {5.0, 2.0},
  {2.0, 1.0},
  {5.0, 3.0},
  {3.0, 2.0},
  {4.0, 3.0},
  {1.0, 1.0},
  {3.0, 4.0},
  {2.0, 3.0},
  {3.0, 5.0},
  {1.0, 2.0},
  {2.0, 5.0},
  {1.0, 3.0},
  {1.0, 4.0},
  {1.0, 5.0},
  {1.0, 8.0}
};

/* 
*/

main ()
{
  register int i, ri;
  double rt;

  FILE *fp;

  if ((FILE *) NULL EQ (fp = fopen (RTFILE, "w")))
    {

      printf ("ERROR:  Unable to open \"%s\" for output\n", RTFILE);
      exit (1);
    }

  for (i = 0; i < 19; i++)
    {

      rt = rate[i][0] / rate[i][1];
      ri = (int) (rt * 1000.0);

      fprintf (fp, "\tDATA	>%04.4x\t; %d/%d  %6.4f  %4d  %2d\n",
	       0xFFFF & ri, (int) rate[i][0], (int) rate[i][1],
	       rt, 0xFFFF & ri, i);
    }

  fclose (fp);
  exit (0);
}
