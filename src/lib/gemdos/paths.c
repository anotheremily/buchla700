/*
   =============================================================================
	paths.c -- show the directory paths on multiple drives
	Version 3 -- 1989-01-17 -- D.N. Lynx Crowe
	(c) Copyright 1989 -- D.N. Lynx Crowe

	Defaults are setup to show the hard disks and any installed RAM disks.

	The installation drive mask is set so that drive B: will always be
	skipped since we don't have one on this system.
   =============================================================================
*/

#include "ctype.h"
#include "osbind.h"
#include "stddefs.h"

#define	FIRST		'C'	/* default 1st drive (upper case)  */
#define	NDRIVES		24	/* default number of drives */

#define	DMASK		0x03FFFFFD	/* installation drive mask */

char buf[256];			/* directory path buffer */


/*
   =============================================================================
	usage() -- print the usage message
   =============================================================================
*/

usage ()
{
  Cconws ("\r\nusage:  PATHS [first [last]]\r\n\r\n");

  Cconws
    ("    Displays the current directory path for each installed drive\r\n");
  Cconws
    ("    in the range \"first\" through \"last\".  System defaults are:\r\n");
  Cconws ("        \"first\" = ");
  Cconout (FIRST);
  Cconws ("    \"last\" = ");
  Cconout (FIRST + NDRIVES - 1);
  Cconws ("\r\n\r\n");

  Cconws
    ("    Where:  \"first\" and \"last\" are drive letters A..Z, and\r\n");
  Cconws ("    if specified, \"last\" must be greater than \"first\".\r\n");
  Cconws
    ("    If only \"first\" is specified, \"last\" defaults to \"first\"\r\n");
}

/* 
*/

/*
   =============================================================================
	show the directory paths on multiple drives
   =============================================================================
*/

main (argc, argv)
     int argc;
     char *argv[];
{
  register short c, drive, last;
  register long dmap;
  short first;

  if (argc > 1)
    {				/* process first optional argument */

      c = *argv[1];

      if (isascii (c) && isalpha (c))
	{			/* check argument */

	  first = _toupper (c) - 'A';
	  last = first;

	}
      else
	{

	  Cconws ("ERROR:  Invalid first argument - \"");
	  Cconws (argv[1]);
	  Cconws ("\".\r\n");
	  usage ();
	  Pterm (1);
	}

    }
  else
    {				/* default for no arguments */

      first = FIRST - 'A';
      last = first + NDRIVES - 1;
    }

/* 
*/

  if (argc > 2)
    {				/* process second optional argument */

      c = *argv[2];

      if (isascii (c) && isalpha (c))
	{			/* check argument */

	  last = _toupper (c) - 'A';

	  if (last < first)
	    {			/* check drive order */

	      Cconws ("ERROR:  Arguments out of order.\r\n");
	      usage ();
	      Pterm (1);
	    }

	}
      else
	{

	  Cconws ("ERROR:  Invalid 2nd argument - \"");
	  Cconws (argv[2]);
	  Cconws ("\".\r\n");
	  usage ();
	  Pterm (1);
	}
    }

/* 
*/
  if (argc > 3)
    {				/* check for too many arguments */

      Cconws ("ERROR:  Invalid number of arguments.\r\n");
      usage ();
      Pterm (1);
    }

  dmap = Drvmap () & DMASK;	/* get map of installed drives */

  /* for each drive in the range first to last ... */

  for (drive = first; drive < (last + 1); drive++)
    {

      /* ... if it's installed ... */

      if (dmap & (0x0000001L << drive))
	{

	  /* ... print the current directory path */

	  Dgetpath (buf, 1 + drive);

	  if ('\0' == buf[0])
	    {			/* clean up root path */

	      buf[0] = '\\';
	      buf[1] = '\0';
	    }

	  Cconout (drive + 'A');
	  Cconout (':');
	  Cconws (buf);
	  Cconws ("\r\n");
	}
    }

  Pterm (0);
}
