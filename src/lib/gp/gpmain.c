/*
   =============================================================================
	gpmain.c -- general purpose main program
	Version 1 -- 1987-10-14 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"

extern int gp_init ();		/* application initialization */

int (*gp_clin) (),		/* command line processor */
  (*gp_btch) (),		/* batch processor */
  (*gp_intr) (),		/* interactive processor */
  (*gp_exit) ();		/* exit processor */

/* 
*/

/*
   =============================================================================
	gp_null() -- null SUCCESS function
   =============================================================================
*/

int
gp_null ()
{
  return (SUCCESS);		/* return a SUCCESS response */
}

/*
   =============================================================================
	gp_fail() -- null FAILURE function
   =============================================================================
*/

int
gp_fail ()
{
  return (FAILURE);		/* return a FAILURE response */
}

/*
   =============================================================================
	gp_pass() -- null argument pass function
   =============================================================================
*/

int
gp_pass (arg)
     int arg;
{
  return (arg);			/* just pass along the input argument */
}

/* 
*/

/*
   =============================================================================
	gp_main() -- general purpose main program driver
   =============================================================================
*/

gp_main (argc, argv)
     int argc;
     char *argv[];
{
  register int rc;		/* return code */

  gp_clin = gp_null;		/* initialize pointers to functions */
  gp_btch = gp_null;
  gp_intr = gp_null;
  gp_exit = gp_pass;

  if (rc = gp_init (argc, argv))	/* application initialization */
    exit (rc);

  if ((*gp_clin) (argc, argv))	/* command line processing */
    rc = (*gp_btch) (argc, argv);	/* batch processing */
  else
    rc = (*gp_intr) (argc, argv);	/* interactive processing */

  rc = (*gp_exit) (rc);		/* exit processing */

  exit (rc);			/* exit back to system */
}
