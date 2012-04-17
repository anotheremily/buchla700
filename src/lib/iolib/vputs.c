/*
   =============================================================================
	vputs.c -- video display string put functions
	Version 2 -- 1987-03-30 -- D.N. Lynx Crowe
	(c) Copyright 1987 -- D.N. Lynx Crowe

	These functions drive the video character write function vputc()
	which puts characters into a 82716 video RAM character object.

	Full attribute format is assumed.
   =============================================================================
*/

#include <stddefs.h>

typedef unsigned int uint;

extern int vputc ();

/*
   =============================================================================
	vputs(sbase, row, col, str, attrib)

	Write string str to video RAM object pointed to by sbase
	at (row,col) with attrib used for all characters.
   =============================================================================
*/

vputs (sbase, row, col, str, attrib)
     uint *sbase, row, col, attrib;
     char *str;
{
  uint c;

  while (c = *str++)
    {

      vputc (sbase, row, col, c, attrib);

      if (++col >= 64)
	{

	  col = 0;

	  if (++row >= 25)
	    row = 0;
	}
    }
}

/*  */

/*
   =============================================================================
	vputsa(sbase, row, col, str, attrib)

	Write string str in video RAM pointed to by sbase starting
	at (row, col) using attributes from the words pointed to by attrib.
   =============================================================================
*/

vputsa (sbase, row, col, str, attrib)
     uint *sbase, row, col, *attrib;
     char *str;
{
  uint c;

  while (c = *str++)
    {

      vputc (sbase, row, col, c, *attrib++);

      if (++col >= 64)
	{

	  col = 0;

	  if (++row >= 25)
	    row = 0;
	}
    }
}
