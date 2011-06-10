/*
   =============================================================================
	vmput.c -- video menu put functions
	Version 3 -- 1987-03-30 -- D.N. Lynx Crowe
	(c) Copyright 1987 -- D.N. Lynx Crowe

	vmput(sbase, row, col, ms, ma)
	uint *sbase, rwo, col, ma;
	char *ms[];

		Copies lines from ms, with attribute ma, to sbase at (row,col).

	vmputa(sbase, row, col, ms, ma)
	uint *sbase, row, col, *ma;
	char *ms[];

		Copies lines from ms, with attributes from ma, to sbase at (row,col).
   =============================================================================
*/

typedef unsigned int uint;

extern int vputc ();

/*
   =============================================================================
	vmput(sbase, row, col, ms, ma) -- put a menu item in a screen image.
	Copies lines from ms, with attribute ma, to sbase at (row,col).
   =============================================================================
*/

vmput (sbase, row, col, ms, ma)
     uint *sbase, row, col, ma;
     char *ms[];
{
  register uint c, tc, tr;
  char *cp;

  tr = row;

  while (cp = *ms++)
    {

      tc = col;

      while (c = *cp++)
	vputc (sbase, tr, tc++, c, ma);

      tr++;
    }
}

/*  */

/*
   =============================================================================
	vmputa(sbase, row, col, ms, ma) -- put a menu item in a screen image.
	Copies lines from ms, with attributes from ma, to sbase at (row,col).
   =============================================================================
*/

vmputa (sbase, row, col, ms, ma)
     uint *sbase, row, col, *ma[];
     char *ms[];
{
  register uint c, tc, tr;
  uint *tm;
  char *cp;

  tr = row;

  while (cp = *ms++)
    {

      tc = col;
      tm = *ma++;

      while (c = *cp++)
	vputc (sbase, tr, tc++, c, *tm++);

      tr++;
    }
}
