/*
   =============================================================================
	bitrev.c -- bit reverse function
	Version 1 -- 1987-03-24 -- D.N. Lynx Crowe
   =============================================================================
*/

static int bitmask[] = {

	0x0001, 0x0002, 0x0004, 0x0008,
	0x0010, 0x0020, 0x0040, 0x0080,
	0x0100, 0x0200, 0x0400, 0x0800,
	0x1000, 0x2000, 0x4000, 0x8000
};

/*
   =============================================================================
	bitrev(bitsin, nbits) -- reverses the rightmost nbits of bitsin.

	Any bits to the left of the reversed bits in the result will be zeros.
   =============================================================================
*/

int
bitrev(bitsin, nbits)
int bitsin, nbits;
{
	int	m, n;

	n = 0;

	for (m = 0; m < nbits; m++)
		if (bitsin & bitmask[m])
			n |= bitmask[nbits-1-m];

	return(n);
}

