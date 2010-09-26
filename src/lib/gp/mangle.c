/*
   =============================================================================
	mangle.c -- mangle a bit stream
	Version 2 -- 1987-08-28 -- D.N. Lynx Crowe
	(c) Copyright 1987 - D.N. Lynx Crowe
   =============================================================================
*/

/*
   =============================================================================
	mangle(bitmap, nb, ib) -- reorder the 'nb' least significant bits
	in 'ib' according to 'bitmap'.  Assumes that 'bitmap' is at least 'nb'
	words long, and that nb <= 32.  The 'bitmap' translation table contains
	an output word for each bit in the input word, with each 'bitmap' entry
	corresponding to the bit number matching its index.  For example,
	'bitmap[0]' contains the word which will be 'OR'ed into the output if
	the least significant bit of the input word is set, while 'bitmap[31]'
	corresponds to the most significant bit of the input word.
   =============================================================================
*/

long
mangle(bitmap, nb, ib)
register long *bitmap;		/* bit map table pointer */
register short nb;		/* number of least significant input bits */
register long ib;		/* input data (in nb least significant bits) */
{
	register long	bm;			/* scan mask */
	register long	rv;			/* result value */
	register short	bn;			/* bit number (bitmap index) */

	bm = 0x00000001L;			/* setup scan mask */
	rv = 0x00000000L;			/* clear the output word */

	for (bn = 0; bn < nb; bn++) {		/* scan across nb bits */

		if (ib & bm)			/* if the input bit is 1 */
			rv |= bitmap[bn];	/* 'OR' the bitmap into rv */

		bm <<= 1;			/* shift the scan mask left */
	}

	return(rv);				/* return rv as the result */
}
