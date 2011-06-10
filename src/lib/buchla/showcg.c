/*
   =============================================================================
	showcg.c -- show the contents of a VSDD character generator table
	Version 10 -- 1988-01-26 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

#define	NPIXELS	8

#define	NCHARS	0xE0

#include "cg2.c"

int bitmask[] = {

  0x0001, 0x0002, 0x0004, 0x0008,
  0x0010, 0x0020, 0x0040, 0x0080,
  0x0100, 0x0200, 0x0400, 0x0800,
  0x1000, 0x2000, 0x4000, 0x8000
};

/*  */

/*
   =============================================================================
	bitrev(bitsin, nbits) -- reverse the rightmost nbits of bitsin.

	Any bits to the left of the reversed bits in the result will be zeros.
   =============================================================================
*/

int
bitrev (bitsin, nbits)
     register int bitsin, nbits;
{
  register int n, m;

  n = 0;

  for (m = 0; m < nbits; m++)
    if (bitsin & bitmask[m])
      n |= bitmask[nbits - 1 - m];

  return (n);
}

/*  */

/*
   =============================================================================
	pbits(row, bits) -- print the bits in a row of a character
   =============================================================================
*/

pbits (row, bits)
     register int row, bits;
{
  register int j, k;

  k = bits;

  for (j = 0; j < NPIXELS; j++)
    {

      if (bits & 1)
	putchar ('*');
      else
	putchar (' ');

      bits = bits >> 1;
    }

  bits = bitrev (k, NPIXELS);

  printf (" %1x %02x  ", row, bits);
}

/*  */

main ()
{
  register int ch, irow, ich;

  for (ch = 0; ch < NCHARS; ch += 4)
    {

      printf ("\n\n\t");

      for (ich = 0; ich < 4; ich++)
	printf ("Ch %03d/%02x/%03o  ", ch + ich, ch + ich, ch + ich);

      printf ("\n\t");

      for (ich = 0; ich < 4; ich++)
	printf ("76543210   DA  ");

      printf ("\n");

      for (irow = cg_rows - 1; irow GE 0; irow--)
	{

	  printf ("\t");
	  pbits (irow, cgtable[irow][ch]);
	  pbits (irow, cgtable[irow][ch + 1]);
	  pbits (irow, cgtable[irow][ch + 2]);
	  pbits (irow, cgtable[irow][ch + 3]);
	  printf ("\n");
	}

      if ((ch & 0x0F) EQ 12)
	printf ("\f");
    }
}
