/*
   =============================================================================
	tmults. -- calculate a table of time multipliers for the Buchla 700
	Version 5 -- 1987-09-24 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

#define	OUT_FILE	"tmultab.tmp"
#define	PRN_FILE	"tmultab.prn"

#define	SHIFTCON	32768.0

float j, m;
int i, k[101], n;
FILE *fp_out, *fp_prn;

main ()
{
  if (NULL EQ (fp_out = fopen (OUT_FILE, "wa")))
    {

      printf ("Unable to open [%s] for output\n", OUT_FILE);
      exit (1);
    }

  if (NULL EQ (fp_prn = fopen (PRN_FILE, "wa")))
    {

      printf ("Unable to open [%s] for output\n", PRN_FILE);
      exit (1);
    }

  printf ("Creating time multiplier table on [%s]\n", OUT_FILE);
  printf ("   and documentation on [%s]\n\n", PRN_FILE);

  fprintf (fp_prn, "Time Multipliers\n\n");
  fprintf (fp_prn, "Ndx Mult 1/Mult Value\n");

  for (i = 0; i LE 100; i++)
    {

      j = 0.5 + (float) i *0.01;
      m = 1.0 / j;

      if (m EQ 2.0)		/* limit to 'almost 2.0' to avoid overflow */
	m = 1.99999;

      n = m * SHIFTCON;
      k[i] = n;

      fprintf (fp_prn, "%3d %4.2f %6.4f 0x%04x\n", i, j, m, n);
    }

  for (i = 0; i LE 100; i++)
    {

      if (0 EQ (i % 5))
	fprintf (fp_out, "\n\t");

      fprintf (fp_out, "0x%04x, ", k[i]);
    }

  fprintf (fp_out, "\n");
  fflush (fp_out);
  fclose (fp_out);
  fflush (fp_prn);
  fclose (fp_prn);
  printf ("Time multipler table output complete.\n");
  exit (0);
}
