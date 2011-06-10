/*
   =============================================================================
	mempage.c -- print a memory page layout sheet for the TMS320C25
	Version 1 -- 1988-12-29 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

extern int atoi ();

/*
   =============================================================================
	print a memory page layout sheet for the TMS320C25
   =============================================================================
*/

main (argc, argv)
     int argc;
     char *argv[];
{
  register short i, m, n;

  if (argc NE 2)
    m = 1;
  else
    m = atoi (argv[1]);

  for (n = 0; n < m; n++)
    {

      printf ("\n\n\n          TMS320C25 Memory Page _____\n\n\n");

      for (i = 0; i < 32; i++)
	printf
	  ("          %3d __________  %3d __________  %3d __________  %3d __________\n",
	   i, i + 32, i + 64, i + 96);

      printf ("\f");
    }

  exit (0);
}
