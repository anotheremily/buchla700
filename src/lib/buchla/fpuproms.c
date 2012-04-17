/*
   =============================================================================
	fpuproms.c -- Buchla 700 FPU Microcode PROM Utility
	Version (see VER below) -- D.N. Lynx Crowe

	This program takes an ASCII file giving the microcode addresses
	and data values, and creates 5 PROM files in Motorola S-Record format.

	The FPU700.DAT input file consists of 9 input fields, as follows:

		adr p1 p2 p2b p3 p3b p4 p4b p5

	where:

		adr	is the address in decimal for the data
		p1..p5	is the hex data for each prom

	PROM 2, PROM 3, and PROM 4 have separate data for each half, while the
	data from the first half is duplicated in the second half for
	PROM 1 and PROM 5.
   =============================================================================
*/

#define	VER	"4 - 1987-09-11"	/* version ID */

#include "stdio.h"
#include "stddefs.h"

extern short msrec ();
extern char *memset ();

#define	NFIELDS	9		/* number of input data fields */

#define	FN_IN	"FPU700.DAT"	/* input file name (stripped Wordstar file) */

#define	FN_P1	"FPU700P1.MOT"	/* output file name - PROM 1 */
#define	FN_P2	"FPU700P2.MOT"	/* output file name - PROM 2 */
#define	FN_P3	"FPU700P3.MOT"	/* output file name - PROM 3 */
#define	FN_P4	"FPU700P4.MOT"	/* output file name - PROM 4 */
#define	FN_P5	"FPU700P5.MOT"	/* output file name - PROM 5 */

FILE *fp;			/* stream pointer */

char prom[5][1024];		/* PROM images */

short padr, p1, p2, p2b, p3, p3b, p4, p4b, p5;	/* input variables */

/* 
*/

/*
   =============================================================================
	putfile(fn, n) -- output PROM file 'n' to file 'fn'
   =============================================================================
*/

putfile (fn, n)
     FILE *fn;
     short n;
{
  if (NULL == (fp = fopen (fn, "w")))
    {

      printf ("\nfpuproms:  ERROR - unable to open output file [%s]\n", fn);

      exit (1);
    }

  msrec (fp, 0L, 1024L, prom[n]);
  fclose (fp);
  printf ("fpuproms:  PROM %d file [%s] written\n", n + 1, fn);
}

/* 
*/

/*
   =============================================================================
	main function -- Buchla 700 FPU Microcode PROM Utility
   =============================================================================
*/

main ()
{
  register short i, rc;

  printf ("Buchla 700 FPU Microcode PROM Utility -- Version %s\n\n", VER);

  if (NULL == (fp = fopen (FN_IN, "r")))
    {

      printf ("fpuproms:  ERROR - Unable to open [%s] for input\n", FN_IN);

      exit (1);
    }

  padr = -1;
  memset (prom, 0xFF, sizeof prom);

/* 
*/
  /* read the PROM data */

  printf ("fpuproms:  Reading PROM data from [%s]\n", FN_IN);

  for (;;)
    {

      rc = fscanf (fp, "%d %x %x %x %x %x %x %x %x",
		   &padr, &p1, &p2, &p2b, &p3, &p3b, &p4, &p4b, &p5);

      if (rc == EOF)
	break;

      if (rc != NFIELDS)
	{

	  printf ("\nfpuproms:  ERROR - Invalid data near address %d\n",
		  padr);

	  exit (1);
	}

      if (padr < 0 || padr > 511)
	{

	  printf ("\nfpuproms:  ERROR - Address [%d] not in range 0..511\n",
		  padr);

	  exit (1);
	}

      prom[0][padr] = p1;	/* PROM 1 */
      prom[0][padr + 512] = p1;

      prom[1][padr] = p2;	/* PROM 2 */
      prom[1][padr + 512] = p2b;

      prom[2][padr] = p3;	/* PROM 3 */
      prom[2][padr + 512] = p3b;

      prom[3][padr] = p4;	/* PROM 4 */
      prom[3][padr + 512] = p4b;

      prom[4][padr] = p5;	/* PROM 5 */
      prom[4][padr + 512] = p5;
    }

/* 
*/

  fclose (fp);

  printf ("fpuproms:  End of input file reached\n");

  /* write out the Motorola S-Record PROM data files */

  printf ("\nfpuproms:  Writing Motorola S-Record PROM data files\n");

  putfile (FN_P1, 0);
  putfile (FN_P2, 1);
  putfile (FN_P3, 2);
  putfile (FN_P4, 3);
  putfile (FN_P5, 4);

  printf ("\nfpuproms:  PROM file conversion complete\n");
  exit (0);
}
