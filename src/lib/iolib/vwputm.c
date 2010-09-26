/*
   =============================================================================
	vwputm.c -- put a menu in a 2-bit per pixel graphics window
	Version 1 -- 1987-04-13 -- D.N. Lynx Crowe

	vwputm(obase, nw, fg, bg, row, col, ml)
	unsigned int *obase;
	int nw, fg, bg;
	char *ml[];

		Writes the menu described by the list 'ml' in the
		'nw' character wide bitmap 'obase', using 'fg' for
		the foreground color and 'bg' for the background.
		The menu is at ('row','col') in the bitmap.
   =============================================================================
*/

#include <vsdd.h>

extern	int	vwputs();

/*
   =============================================================================
	vwputm(obase, nw, fg, bg, row, col, ml) -- output a menu in a bitmap
   =============================================================================
*/

vwputm(obase, nw, fg, bg, row, col, ml)
unsigned int *obase;
int nw, fg, bg, row, col;
register char *ml[];
{
	while (*ml) {

		vwputs(obase, nw, fg, bg, row++, col, *ml++);
	}
}
	
