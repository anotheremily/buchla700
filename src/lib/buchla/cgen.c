/*
   =============================================================================
	cgen.c -- Intel/Matra-Harris 82716 VSDD character generator translator
	Version 5 -- 1988-10-05 -- D.N. Lynx Crowe

	Reads lines of data from stdin containing the hex specifications
	of characters to be built for the VSDD character generator tables.

	The first line is a header containing the 1 to 7 character
	name of the cg array to be generated, and 2 decimal fields, the
	first of which gives the number of scan lines per character,
	in the range 1 to 16, and the second of which is the number
	of pixels per scan line, which must be 6, 8, 12 or 16.

	Each data line consists of 2 to 17 hex fields, the first of which is
	the hex character code, and the remainder of which are the hex codes
	for each scan line in the character, from bottom to top.

	The program reverses each pattern byte and stores it in the
	appropriate slice table.  When EOF is reached, the slice tables
	are output to stdout as a c source file.
   =============================================================================
*/

#define	PROGID	"cgen"

#include "stdio.h"
#include "stddefs.h"
#include "memory.h"
#include "strings.h"

extern char *now ();

int slice[16][256];		/* slice table */
int pixels;

int nscans, npixels;
int i, j, k, l;
int cnum;

char cgname[129];
char dtg[32];

int bitmask[] = {

  0x0001, 0x0002, 0x0004, 0x0008,
  0x0010, 0x0020, 0x0040, 0x0080,
  0x0100, 0x0200, 0x0400, 0x0800,
  0x1000, 0x2000, 0x4000, 0x8000
};

/*  */

/*
   =============================================================================
	bitrev(bitsin, nbits) -- reverses the rightmost nbits of bitsin.

	Any bits to the left of the reversed bits in the result will be zeros.
   =============================================================================
*/

int
bitrev (bitsin, nbits)
     int bitsin, nbits;
{
  int m, n;

  n = 0;

  for (m = 0; m < nbits; m++)
    if (bitsin & bitmask[m])
      n |= bitmask[nbits - 1 - m];

  return (n);
}

/*  */

main ()
{
  memset (cgname, 0, sizeof cgname);	/* clear the cg name array */
  memset (slice, 0, sizeof slice);	/* clear the slice array */

  /* read header data */

  if (3 NE scanf ("%s %d %d", &cgname, &nscans, &npixels))
    {

      printf ("\n%s:  Unable to read header data.\n", PROGID);
      exit (1);
    }

  if (strlen (cgname) GT 7)
    {

      printf ("%s:  Character generator name too long.\n", PROGID);
      exit (1);
    }

  if ((nscans GT 16) OR (nscans LE 0))
    {

      printf ("%s:  Number of scan lines must be > 0 and < 17.\n", PROGID);
      exit (1);
    }

  if ((npixels NE 6) AND (npixels NE 8) AND
      (npixels NE 12) AND (npixels NE 16))
    {

      printf ("%s:  Number of pixels must be 6, 8, 12, or 16.\n", PROGID);
      exit (1);
    }

/*  */

  /* process each character definition */

  while (TRUE)
    {

      if (EOF EQ scanf ("%x", &cnum))
	{			/* character number */

	  /* all characters are in slice table, output it */

	  printf ("/* VSDD character generator table %s */\n", cgname);

	  printf ("/* %dV by %dH */\n", nscans, npixels);

	  printf ("/* Generated:  %s */\n\n", now (dtg));

	  printf ("int\tcg_rows = %d;\n\n", nscans);

	  printf ("int\t%s[%d][256] = {\n\n", cgname, nscans);

	  for (i = 0; i < nscans - 1; i++)
	    {

	      l = 0;
	      printf ("\t{\t/* Scan line %d */\n", i);

	      for (j = 0; j < 31; j++)
		{

		  printf ("\t ");

		  for (k = 0; k < 8; k++)
		    {

		      printf ("0x%04x, ", slice[i][l]);
		      l++;
		    }

		  printf ("\n");
		}

	      printf ("\t ");

	      for (k = 0; k < 7; k++)
		{

		  printf ("0x%04x, ", slice[i][l]);
		  l++;
		}

	      printf ("0x%04x\n\t},\n\n", slice[i][l]);
	    }

/*  */

	  l = 0;
	  printf ("\t{\t/* Scan line %d */\n", i);

	  for (j = 0; j < 31; j++)
	    {

	      printf ("\t ");

	      for (k = 0; k < 8; k++)
		{

		  printf ("0x%04x, ", slice[i][l]);
		  l++;
		}

	      printf ("\n");
	    }

	  printf ("\t ");

	  for (k = 0; k < 7; k++)
	    {

	      printf ("0x%04x, ", slice[i][l]);
	      l++;
	    }

	  printf ("0x%04x\n\t}\n", slice[i][l]);
	  printf ("};\n");
	  exit (0);
	}

/*  */
      if ((cnum < 0) OR (cnum > 255))
	{

	  printf ("%s:  Character number must be > 0 and < FF.\n", PROGID);
	  exit (1);
	}

      for (i = 0; i < nscans; i++)
	{

	  if (1 NE scanf ("%x", &pixels))
	    {

	      printf ("\n%s:  Error reading slice data for %2x.\n",
		      PROGID, cnum);
	      exit (1);
	    }

	  slice[i][cnum] = bitrev (pixels, npixels);
	}
    }
}
