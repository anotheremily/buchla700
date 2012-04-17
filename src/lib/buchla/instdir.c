/*
   =============================================================================
	instdir.c -- read an instrument library and print its directory
	Version 2 -- 1988-08-03 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	M7CAT		1	/* so libdsp.h gets it right */

#include "stdio.h"
#include "stddefs.h"
#include "graphdef.h"
#include "vsdd.h"

#include "midas.h"
#include "instdsp.h"
#include "libdsp.h"

#define	LIBNAME		"m7slot00.orc"	/* library name */

extern int errno;

char libname[64];

struct instdef idefs[1];
struct mlibhdr ldhead;

/* 
*/

/*
   =============================================================================
	skipit() -- skip past data in a file,  with error checking
   =============================================================================
*/

short
skipit (fp, len)
     register FILE *fp;
     register long len;
{
  if (fseek (fp, len, 1))
    return (FAILURE);
  else
    return (SUCCESS);
}

/*
   =============================================================================
	die() -- close a file and terminate the program
   =============================================================================
*/

die (fp)
     FILE *fp;
{
  fclose (fp);
  printf ("Program terminated\n");
  exit (1);
}

/* 
*/

/*
   =============================================================================
	readit() -- read a file,  with error checking
   =============================================================================
*/

short
readit (fp, to, len)
     register FILE *fp;
     register char *to;
     register long len;
{
  register long count;
  register int c;

  for (count = 0; count < len; count++)
    {

      if (EOF == (c = getc (fp)))
	{

	  printf ("ERROR: Unexpected EOF -- errno = %d\n", errno);
	  fclose (fp);
	  return (FAILURE);

	}
      else
	{

	  *to++ = c;
	}
    }

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	print the directory of an instrument library
   =============================================================================
*/

main (argc, argv)
     int argc;
     char *argv[];
{
  register FILE *fp;
  register struct instdef *ip;
  register short i;

  strcpy (libname, LIBNAME);

  if (argc == 2)
    strcpy (libname, argv[1]);

  if ((FILE *) NULL == (fp = fopenb (libname, "r")))
    {

      printf ("Unable to open [%s]\n", libname);
      exit (1);
    }

  if (readit (fp, &ldhead, (long) LH_LEN))
    die (fp);

  printf ("Directory of Orchestra in [%s]\n", libname);
  printf ("N_  Name             Comments\n");
  printf
    ("--  ---------------- ---------------- ---------------- ----------------\n");

  for (i = 0; i < NINORC; i++)
    {

      ip = &idefs;

      if (readit (fp, ip, (long) OR_LEN1))
	die (fp);

      if (skipit (fp, (long) OR_LEN2))
	die (fp);

      if (skipit (fp, (long) OR_LEN2))
	die (fp);

      printf ("%2d: %-16.16s", i + 1, ip->idhname);
      printf (" %-16.16s", ip->idhcom1);
      printf (" %-16.16s", ip->idhcom2);
      printf (" %-16.16s\n", ip->idhcom3);
    }

  fclose (fp);
  exit (0);
}
