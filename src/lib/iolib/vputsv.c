/*
   =============================================================================
	vputsv.c -- video display string put functions
	Version 1 -- 1988-10-06 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe

	These functions drive the video character write function vputc()
	which puts characters into a 82716 video RAM character object.

	Full attribute format is assumed.
   =============================================================================
*/

#include "stddefs.h"

typedef unsigned int uint;

extern int vputcv ();

/*
   =============================================================================
	vputsv(sbase, row, col, str, attrib, len)

	Write string str to video RAM object pointed to by sbase
	at (row,col) with attrib used for all characters. Line length is len.
   =============================================================================
*/

vputsv (sbase, row, col, str, attrib, len)
     uint *sbase, row, col;
     char *str;
     uint attrib, len;
{
  uint c;

  while (c = *str++)
    {

      vputcv (sbase, row, col, c, attrib, len);

      if (++col GE 64)
	{

	  col = 0;

	  if (++row GE 25)
	    row = 0;
	}
    }
}

/*  */

/*
   =============================================================================
	vputsav(sbase, row, col, str, attrib, len)

	Write string str in video RAM pointed to by sbase starting
	at (row, col) using attributes from the words pointed to by attrib.
	Line length is len.
   =============================================================================
*/

vputsav (sbase, row, col, str, attrib, len)
     uint *sbase, row, col;
     char *str;
     uint *attrib, len;
{
  uint c;

  while (c = *str++)
    {

      vputcv (sbase, row, col, c, *attrib++, len);

      if (++col GE 64)
	{

	  col = 0;

	  if (++row GE 25)
	    row = 0;
	}
    }
}
