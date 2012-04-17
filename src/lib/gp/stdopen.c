/*
   =============================================================================
	stdopen.c -- standard C file openers
	Version 1 -- 1988-11-07 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	OPEN_AB			/* define to compile stdopna(), stdopnb() */

#include "stdio.h"		/* standard I/O definitions */
#include "stddefs.h"		/* standard C utility definitions */

extern char *pgmname;		/* program name string pointer */

extern short (*errterm) ();	/* program error termination exit function */

extern FILE *(*opnfail) ();	/* file open error function */

/*
   =============================================================================
	stdopen() -- standard C file opener -- system standard files
   =============================================================================
*/

FILE *
stdopen (name, mode)
     char *name, *mode;
{
  FILE *fp;

  if ((FILE *) NULL == (fp = fopen (name, mode)))
    {

      if (opnfail)
	if ((FILE *) NULL != (fp = (*opnfail) (name, mode)))
	  return (fp);

      fprintf (stderr, "%s: ERROR - Unable to open \"%s\" in mode \"%s\".\n",
	       name, mode);

      if (errterm)
	(*errterm) ();

      fprintf (stderr, "%s: Program terminated due to I/O error.\n");
      exit (1);
    }

  return (fp);
}

/* 
*/

#ifdef	OPEN_AB

/*
   =============================================================================
	stdopna() -- standard C file opener -- ASCII files
   =============================================================================
*/

FILE *
stdopna (name, mode)
     char *name, *mode;
{
  FILE *fp;

  if ((FILE *) NULL == (fp = fopena (name, mode)))
    {

      if (opnfail)
	if ((FILE *) NULL != (fp = (*opnfail) (name, mode)))
	  return (fp);

      fprintf (stderr, "%s: ERROR - Unable to open \"%s\" in mode \"%s\".\n",
	       name, mode);

      if (errterm)
	(*errterm) ();

      fprintf (stderr, "%s: Program terminated due to I/O error.\n");
      exit (1);
    }

  return (fp);
}

#endif

/* 
*/

#ifdef	OPEN_AB

/*
   =============================================================================
	stdopnb() -- standard C file opener -- binary files
   =============================================================================
*/

FILE *
stdopnb (name, mode)
     char *name, *mode;
{
  FILE *fp;

  if ((FILE *) NULL == (fp = fopenb (name, mode)))
    {

      if (opnfail)
	if ((FILE *) NULL != (fp = (*opnfail) (name, mode)))
	  return (fp);

      fprintf (stderr, "%s: ERROR - Unable to open \"%s\" in mode \"%s\".\n",
	       name, mode);

      if (errterm)
	(*errterm) ();

      fprintf (stderr, "%s: Program terminated due to I/O error.\n");
      exit (1);
    }

  return (fp);
}

#endif
