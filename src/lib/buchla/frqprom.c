/*
   =============================================================================
	frqprom.c -- generate the frequency / pitch PROM for the Buchla 700
	Written by:  D.N. Lynx Crowe -- see VER below for version
   =============================================================================
*/

#define	VER	"Version 12 - 1987-09-17"	/* version ID string */

#define	DEBUGIT		0

#include "stdio.h"
#include "math.h"
#include "stddefs.h"

#define	N	(double)192.0	/* number of steps in the oscillator */
#define	R	(double)524288.0	/* number of steps in the phase angle */
#define	C	(double)10066000.0	/* clock frequency */

#define	RNC	((R*N)/C)	/* frequency multiplier */

#if DEBUGIT
#define	FILEOUT		"FRQPROM.PRN"
#else
#define	FILEOUT		"FRQPROM.MOT"
#endif

#define	NOTIFY1	10		/* frquency notification interval */
#define	NOTIFY2	600		/* pitch notification interval */

#define	HILIMIT	(double)16383.5

extern int msrec ();

double p, q, s, t, inc, f;

unsigned short tab[32768];
unsigned char otab[65536];

int count, notify;

FILE *fp;

/* 
*/

main ()
{
  register long i;

  printf ("Buchla 700 Frequency / Pitch PROM Generator %s\n\n", VER);

  q = RNC;

  if (NULL EQ (fp = fopen (FILEOUT, "wa")))
    {				/* open output file */

      printf ("ERROR:  Unable to open [%s] for output.\n", FILEOUT);
      exit (1);
    }

  printf ("Output on [%s]\n", FILEOUT);
  printf ("Using:  R = %f, N = %f\n", R, N);
  printf ("        C = %f, (R*N)/C = %f\n\n", C, q);

#if DEBUGIT
  fprintf (fp, "Buchla 700 Frequency / Pitch PROM Generator %s\n\n", VER);
  fprintf (fp, "Using:  R = %f, N = %f\n", R, N);
  fprintf (fp, "        C = %f, (R*N)/C = %f\n\n", C, q);
  fprintf (fp, "Frequency portion of PROM\n");
#endif

  for (i = 0; i < 65536; i++)	/* preset table to saturation value */
    tab[i] = 65535;

  /* generate the frequency portion of the table from 0.0 to 15.95 Hz */
  /* using the formula:  inc = f * ((R*N)/C)  */

  printf ("Generating frequency portion of PROM file\n");
  notify = NOTIFY1;
  count = notify;

#if DEBUGIT
  for (f = (double) 0.0; f < (double) 16.0;
       f += ((double) 0.05 * (double) NOTIFY1))
    {
#else
  for (f = (double) 0.0; f < (double) 16.0; f += (double) 0.05)
    {
#endif
      inc = f * q;
      i = (long) floor (f * 20.0);
      tab[i] = floor (inc);

#if DEBUGIT
      fprintf (fp, "%5d 0x%04x %14.4f %14.4f\n", i, tab[i], f, inc);
#endif

      if (++count GE notify)
	{

	  count = 0;
	  printf (".");
	}
    }

  printf ("\n");

#if DEBUGIT
  fprintf (fp, "\n\nPitch portion of PROM\n");
#endif
/* 
*/
  /* generate the pitch portion of the table from 160.0 to HILIMIT cents */
  /* according to the formula:  inc = ((R*K*N)/C)*(2**(p/1200)) */

  printf ("Generating pitch portion of PROM file\n");
  notify = NOTIFY2;
  count = notify;

#if DEBUGIT
  for (p = (double) 160.0; p < HILIMIT;
       p += ((double) 0.5 * (double) NOTIFY2))
    {
#else
  for (p = (double) 160.0; p < HILIMIT; p += (double) 0.5)
    {
#endif

      s = p / (double) 1200.0;	/* calculate the increment */
      t = pow ((double) 2.0, s);
      inc = (double) 149.082 *t;

      if (inc GE 65536.0)	/* saturate */
	break;

      i = (long) floor ((double) 2.0 * p);	/* stuff the table */
      tab[i] = floor (inc);

#if DEBUGIT
      fprintf (fp, "%5d 0x%04x %14.4f %14.4f %14.4f %14.4f\n",
	       i, tab[i], p, s, t, inc);
#endif

      if (++count GE notify)
	{

	  count = 0;
	  printf (".");
	}
    }

  printf ("\n");

/* 
*/
  /* output the table */

#if !DEBUGIT

  printf ("Splitting table into LS and MS bytes for output\n");

  for (i = 0; i < 32768; i++)
    {				/* split into high and low bytes */

      otab[i] = tab[i] & 0x00FF;	/* LS byte in low half */
      otab[i + 32768L] = tab[i] >> 8;	/* MS byte in high half */
    }

  printf ("Writing Motorola S-Records to [%s]\n", FILEOUT);
  msrec (fp, 0L, 65536L, otab);	/* output the table as S-Records */

#endif

  fclose (fp);			/* close the file */
  printf ("Program complete.\n");	/* say we're done */
  exit (0);			/* go away */
}
