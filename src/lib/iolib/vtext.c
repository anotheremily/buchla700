/*
   =============================================================================
	vtext.c -- output a text string to the VSDD in 1 byte / character mode
	Version 3 -- 1987-06-02 -- D.N. Lynx Crowe

	Assumes VSDD is pointing at bank 0.
   =============================================================================
*/

static int msk[] = { 0xFF00, 0x00FF };

vtext (obj, nc, row, col, ip)
     register unsigned *obj, nc, row, col;
     register char *ip;
{
  register unsigned *op;

  while (*ip)
    {

      op = obj + ((nc >> 1) * row) + (col >> 1);

      *op = (*op & (unsigned) msk[col & 1]) |
	((*ip++ & 0x00FF) << ((col & 1) ? 8 : 0));

      col++;
    }
}
