/*
   =============================================================================
	vspray4.c -- spray a pattern onto a graphics screen using tsplot4()
	Version 5 -- 1987-10-09 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "vsddsw.h"

static char cl[81];

/*
   =============================================================================
	vspray4(vobj, vwid, fg, ml, vb, pitch) -- spray a pattern on the screen

	Uses 'vobj' of width 'vwid' as the screen, 'fg' as the color, 'vb' as
	the bank, 'pitch' as the vertical pitch, and 'ml' as the pattern.

	If 'vb' EQ -1,  the bank is not set.  The color in 'fg' will be taken
	from the lower 4 bits.

	The list 'ml' points at pairs of counts and characters.  The last count
	is -1,  and the list terminates with a null pointer.
   =============================================================================
*/

/* 
*/

vspray4 (vobj, vwid, fg, ml, vb, pitch)
     unsigned *vobj;
     short vwid, fg, vb, pitch;
     char *ml[];
{
  register char *cp, *lp, c;
  register short j, k, row;

  row = 0;

  fg &= 0x000F;
  fg |= fg << 4;
  fg |= fg << 8;

  if (-1 NE vb)
    vbank (vb);

  while (NULL NE (cp = *ml++))
    {

      lp = cl;

      while (-1 NE (j = *cp++))
	{

	  j &= 0x00FF;
	  c = *cp++;

	  for (k = 0; k < j; k++)
	    *lp++ = c;
	}

      *lp = '\0';

      tsplot4 (vobj, vwid, fg, row++, 0, cl, pitch);
    }
}
