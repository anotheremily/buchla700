/*
   =============================================================================
	vclrs.c -- clear a VSDD text line to a specified value
	Version 2 -- 1987-04-15 -- D.N. Lynx Crowe
	(c) Copyright 1987 -- D.N. Lynx Crowe

	vclrs(obase, row, col, nc, ch, atr)
	unsigned int obase[];
	int row, col, nc, ch, atr;

		Clear 'nc' characters in the text object 'obase' to 'ch',
		using atrributes 'atr', starting at ('row','col');

		No error checking is done, so beware.
   =============================================================================
*/

extern	int	vputc();

/*
   =============================================================================
	vclrs(obase, row, col, nc, ch, atr) -- clear a line on the VSDD
   =============================================================================
*/

vclrs(obase, row, col, nc, ch, atr)
register unsigned int obase[];
int row, col;
register int nc;
int ch, atr;
{
	while (nc--)
		vputc(obase, row, col++, ch, atr);
}
