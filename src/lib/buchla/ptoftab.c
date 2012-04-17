/*
   =============================================================================
	ptoftab.c -- generate a pitch to frequency table
	Written by:  D.N. Lynx Crowe -- see VER below for version
   =============================================================================
*/

#define	VER	"Version 2 - 1987-12-19"	/* version ID string */

#define	DEBUGIT		0

#define	FILEOUT		"PTOFTAB.OUT"	/* output file name */
#define	MAXPIT		10960L	/* maximum pitch (cents) = C9 */

#include "stdio.h"
#include "math.h"
#include "stddefs.h"

double j;
double m;
double kj;
double kl;
double km;

FILE *fp;

/* 
*/

main ()
{
  register long i, k;
  register unsigned long n;

  printf ("Buchla 700 Pitch to Frequency Table Generator %s\n", VER);

  if (NULL == (fp = fopen (FILEOUT, "wa")))
    {				/* open output file */

      printf ("ERROR:  Unable to open [%s] for output.\n", FILEOUT);
      exit (1);
    }

  kj = (double) MAXPIT / (double) 1200.0;
  kl = pow ((double) 2.0, kj);
  km = ((double) 10.0 / kl);

#if	DEBUGIT
  printf ("Using km = %12.8f,  kl = %12.8f, kj = %12.8f\n", km, kl, kj);
#endif

  printf ("Writing output to [%s] ...\n", FILEOUT);
  fprintf (fp, "/* Pitch to Frequency */\n\nunsigned ptoftab[] = {\n\n");

  for (i = 0L; i < 256L; i++)
    {

      k = i << 6;
      j = (double) ((k <= MAXPIT) ? k : MAXPIT) / (double) 1200.0;
      m = km * pow ((double) 2.0, j) * (double) 2.0;
      n = (unsigned long) ((m > (double) 10.0 ? (double) 10.0 : m)
			   * (double) 100.0) << 5;

      fprintf (fp, "\t0x%04.4x%c\t/* %3d:  %10.5f %6d %10.5f */\n",
	       (0x0000FFFFL & n), (i < 255L ? ',' : ' '), i, m, k, j);

#if	DEBUGIT
      printf ("%3d:  m = %10.5f, n = $%04.4x, k = %6d, j = %10.5f\n",
	      i, m, (0x0000FFFFL & n), k, j);
#endif
    }

  fprintf (fp, "};\n");
  fclose (fp);			/* close the file */
  printf ("\nProgram complete.\n");	/* say we're done */
  exit (0);			/* go away */
}
