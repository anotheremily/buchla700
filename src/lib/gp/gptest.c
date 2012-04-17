/*
   =============================================================================
	gptest.c -- test gp_main and friends
	Version 1 -- 1987-10-14 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "stdio.h"

#include "gp_main.h"

/*
   =============================================================================
	tp_clin() -- gp_clin test stub
   =============================================================================
*/

int
tp_clin (argc, argv)
     int argc;
     char *argv[];
{
  register int i;

  printf ("tp_clin(%d, $%08lx) executed\n", argc, argv);

  for (i = 0; i < argc; i++)
    printf ("   argv[%d] = \042%s\042\n", i, argv[i]);

  return (argc > 1);
}

/*
   =============================================================================
	tp_btch() -- gp_btch test stub
   =============================================================================
*/

int
tp_btch (argc, argv)
     int argc;
     char *argv[];
{
  printf ("tp_btch(%d, $%08lx) executed\n", argc, argv);
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	tp_intr() -- gp_intr test stub
   =============================================================================
*/

int
tp_intr (argc, argv)
     int argc;
     char *argv[];
{
  printf ("tp_intr(%d, $%08lx) executed\n", argc, argv);
  return (SUCCESS);
}

/*
   =============================================================================
	tp_exit() -- gp_exit test stub
   =============================================================================
*/

int
tp_exit (arg)
     int arg;
{
  printf ("tp_exit(%d) executed\n", arg);
  return (arg);
}

/* 
*/

/*
   =============================================================================
	gp_init() -- initialize the application for gp_main()
   =============================================================================
*/

int
gp_init (argc, argv)
     int argc;
     char *argv[];
{
  gp_clin = tp_clin;
  gp_btch = tp_btch;
  gp_intr = tp_intr;
  gp_exit = tp_exit;

  printf ("gp_init(%d, $%08lx):  Application initialized.\n", argc, argv);

  return (SUCCESS);
}

/*
   =============================================================================
	main driver for the test
   =============================================================================
*/

main (argc, argv)
     int argc;
     char *argv[];
{
  printf ("main(%d, $%08lx) executed.\n", argc, argv);

  gp_main (argc, argv);		/* run gp_main */

  printf ("main():  ERROR -- gp_main() returned.\n");

  exit (1);			/* it should never return */
}
