/*
   =============================================================================
	vsddcalc.c -- calculate some VSDD values
	Version 8 -- 1989-01-25 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

#define	PSA	0		/* value for psa */
#define	HRS	1		/* value for hrs */

#define	VCK	12800000L	/* value for video clock (Hz) */

#define	HC0	3		/* width of HSYNC (GCLKs - 1) */
#define	HC1	5		/* AHZ start (GCLKs - 1) */
#define	HC2	37		/* AHZ stop (GCLKs - 1) */
#define	HC3	40		/* horizontal sweep (GCLKs - 1) */

#define	VC0	8		/* width of VSYNC (lines - 1) */
#define	VC1	10		/* AVZ start (lines - 1) */
#define	VC2	360		/* AVZ stop (lines - 1) */
#define	VC3	362		/* vertical sweep (lines - 1) */

/* 
*/

short hrs = HRS,		/* High Resolution Select */
  psa = PSA,			/* Pre-Scaler Active */
  ahp,				/* active horizontal period (GCLKs) */
  avl,				/* active vertical lines */
  gpix,				/* pixels per GCLK */
  hpix,				/* active pixels per line */
  hc0 = HC0 + 1,		/* width of HSYNC (GCLKs) */
  hc1 = HC1 + 1,		/* AHZ start (GCLKs) */
  hc2 = HC2 + 1,		/* AHZ stop (GCLKs) */
  hc3 = HC3 + 1,		/* horizontal sweep (GCLKs) */
  hc4,				/* 1/2 horizontal sweep (GCLKs) */
  hrate,			/* horizontal sweep rate (Hz) */
  ihp,				/* inactive horizontal period (GCLKs) */
  ivl,				/* inactive vertical lines */
  vc0 = VC0 + 1,		/* width of VSYNC (lines) */
  vc1 = VC1 + 1,		/* AVZ start (lines) */
  vc2 = VC2 + 1,		/* AVZ stop (lines) */
  vc3 = VC3 + 1,		/* vertical sweep (lines) */
  vrate;			/* vertical sweep rate (Hz) */

long gclk,			/* general clock (Hz) */
  gclkn,			/* general clock (nanoseconds) */
  vck = VCK,			/* video clock (Hz) */
  pixclk,			/* pixel clock (Hz) */
  vph,				/* sync gnerator clock (Hz) */
  ahz,				/* active horizontal zone time (nanoseconds) */
  avz,				/* active vertical zone time (nanoseconds) */
  hsweep,			/* horizontal sweep time (nanoseconds) */
  hsync,			/* horizontal sync time (nanoseconds) */
  vsweep,			/* vertical sweep time (nanoseconds) */
  vsync;			/* vertical sync time (nanoseconds) */

/* 
*/

main ()
{
  /* check horizontal constants */

  if (hc0 GE hc1)
    {

      printf ("ERROR:  hc0 {%d} must be < hc1 {%d}\n", hc0, hc1);
      exit (1);
    }

  hc4 = hc3 >> 1;

  if (hc1 GE hc4)
    {

      printf ("ERROR:  hc1 {%d} must be < (hc3 / 2) {%d}\n", hc1, hc4);
      exit (1);
    }

  if (hc4 GE hc2)
    {

      printf ("ERROR:  hc2 {%d} must be > (hc3 / 2) {%d}\n", hc2, hc4);
      exit (1);
    }

  if (hc2 GE hc3)
    {

      printf ("ERROR:  hc2 {%d} must be < hc3 {%d}\n", hc2, hc3);
      exit (1);
    }

  /* check vertical constants */

  if (vc0 GE vc1)
    {

      printf ("ERROR:  vc0 {%d} must be < vc1 {%d}\n", vc0, vc1);
      exit (1);
    }

  if (vc1 GE vc2)
    {

      printf ("ERROR:  vc1 {%d} must be < vc2 {%d}\n", vc1, vc2);
      exit (1);
    }

  if (vc2 GE vc3)
    {

      printf ("ERROR:  vc2 {%d} must be < vc3 {%d}\n", vc2, vc3);
      exit (1);
    }

/* 
*/
  /* caluclate pixel clock */

  if (hrs)
    pixclk = vck;
  else
    pixclk = vck >> 1;

  /* calculate internal clock */

  if (psa)
    vph = vck >> 2;
  else
    vph = vck >> 1;

  /* check internal clock limit */

  if (vph > 8000000L)
    {

      printf ("\nERROR:  Calculated vph {%ld} is > 8000000\n", vph);

      if (psa)
	printf ("   Try a lower value for vck {%ld}\n", vck);
      else
	printf ("   Try psa = 1\n");

      exit (1);
    }

  /* calculate general clock */

  gclk = vph >> 3;
  gclkn = (1000000000L / (gclk / 1000)) / 1000;

  gpix = pixclk / gclk;

  /* calculate horizontal timings */

  ahp = hc2 - hc1;
  ihp = hc3 - ahp;
  hsync = hc0 * gclkn;
  hsweep = hc3 * gclkn;
  ahz = ahp * gclkn;
  hpix = ahp * gpix;
  hrate = 1000000000L / hsweep;

  /* calculate vertical timings */

  avl = vc2 - vc1;
  ivl = vc3 - avl;
  vsync = vc0 * hsweep;
  vsweep = vc3 * hsweep;
  avz = avl * hsweep;
  vrate = 1000000000L / vsweep;

/* 
*/
  /* print the results */

  printf ("82716 VSDD Timing Calculations\n");
  printf ("------------------------------\n\n");

  printf ("   with HRS = %d and PSA = %d\n\n", hrs, psa);

  printf ("Clock values\n");
  printf ("------------\n\n");

  printf ("VCK    = %8ld Hz   Master clock\n", vck);
  printf ("PIXCLK = %8ld Hz   Pixel clock\n", pixclk);
  printf ("VPH    = %8ld Hz   Internal clock\n", vph);
  printf ("GCLK   = %8ld Hz   General clock  (%ld nanoseconds)\n\n",
	  gclk, gclkn);


  printf ("Horizontal values                 Vertical values\n");
  printf ("-----------------                 ---------------\n\n");

  printf ("HC0    = %8d GCLKs           VC0    = %8d lines   sync\n",
	  hc0, vc0);

  printf ("HC1    = %8d GCLKs           VC1    = %8d lines   start\n",
	  hc1, vc1);

  printf ("HC2    = %8d GCLKs           VC2    = %8d lines   stop\n",
	  hc2, vc2);

  printf ("HC3    = %8d GCLKs           VC3    = %8d lines   sweep\n\n",
	  hc3, vc3);

  printf ("AHP    = %8d GCLKs           AVL    = %8d lines   active\n",
	  ahp, avl);

  printf ("IHP    = %8d GCLKs           IVL    = %8d lines   inactive\n\n",
	  ihp, ivl);


  printf ("HSYNC  = %8ld nanoseconds     VSYNC  = %8ld nanoseconds\n",
	  hsync, vsync);

  printf ("HSWEEP = %8ld nanoseconds     VSWEEP = %8ld nanoseconds\n",
	  hsweep, vsweep);

  printf ("AHZ    = %8ld nanoseconds     AVZ    = %8ld nanoseconds\n\n",
	  ahz, avz);


  printf ("Miscellaneous values\n");
  printf ("--------------------\n\n");

  printf ("GPIX   = %8d pixels / GCLK   HPIX   = %8d pixels displayed\n",
	  gpix, hpix);

  printf ("HRATE  = %8d Hz              VRATE  = %8d Hz\n\n", hrate, vrate);

  exit (0);
}
