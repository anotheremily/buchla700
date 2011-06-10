/*
   =============================================================================
	secdump.c -- sector dump for the Atari ST
	Version 1 -- 1988-01-27 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	PROGID	"secdump"

#include "stddefs.h"
#include "osbind.h"
#include "stdio.h"
#include "ctype.h"

#define PERLINE 	16
#define	SECLEN		512

short spp;
short secnum;
short sec_bgn;
short sec_end;
short drive;

char ebuf[128];
char secbuf[SECLEN];

/* 
*/

/*
   =============================================================================
	pipc() -- print if printable characters
   =============================================================================
*/

static
pipc (chars, length)
     char chars[];
     int length;
{
  int i;

  for (i = 0; i < length; i++)
    if (isascii (0x00FF & chars[i]) AND (isprint (0x00FF & chars[i])))
      printf ("%c", chars[i]);
    else
      printf (".");
}

/* 
*/

/*
   =============================================================================
	mdump() -- dump a memory area in hexadecimal
   =============================================================================
*/

mdump (begin, end, start)
     char *begin, *end;
     long start;
{
  long i, ii;
  int j, jj, k;
  char c, chars[PERLINE];

  i = 0L;
  ii = start;
  j = 0;

  if (begin GT end)
    return;

  while (begin LE end)
    {

      c = *begin++;

      if (!(i % PERLINE))
	{

	  if (i)
	    {

	      j = 0;
	      printf ("  ");
	      pipc (chars, PERLINE);
	    }

	  printf ("\n%08lx:", ii);
	}

      ii++;
      i++;

      printf (" %02.2x", (c & 0x00FF));
      chars[j++] = c;
    }

  if (k = (i % PERLINE))
    {

      k = PERLINE - k;

      for (jj = 0; jj < (3 * k); ++jj)
	printf (" ");
    }

  printf ("  ");
  pipc (chars, PERLINE);
  printf ("\n");
}

/* 
*/

/*
   =============================================================================
	errmsg() -- print an error message on stderr
   =============================================================================
*/

errmsg (s)
     char *s;
{
  fprintf (stderr, "%s:  ERROR - %s\n", PROGID, s);
}

/*
   =============================================================================
	usage() -- print a usage message on stdout
   =============================================================================
*/

usage ()
{
  printf ("usage:  %s drive first [last]\n", PROGID);
}

/* 
*/

/*
   =============================================================================
	getsnum() -- get a sector number into secnum
   =============================================================================
*/

short
getsnum (from)
     char *from;
{
  short c;
  long n;

  n = 0;

  while (c = *from++)
    {

      if (isascii (c))
	{

	  if (isdigit (c))
	    {

	      n = (n * 10) + (c - '0');

	    }
	  else
	    {

	      return (FAILURE);
	    }

	}
      else
	{

	  return (FAILURE);
	}
    }

  if (n < 32767L)
    {

      secnum = (short) n;
      return (SUCCESS);

    }

  return (FAILURE);
}

/* 
*/

/*
   =============================================================================
	getdrv() -- get and check a drive specifier
   =============================================================================
*/

short
getdrv (from)
     char *from;
{
  short c;
  long dmap;

  dmap = Drvmap ();

  c = *from & 0x00FF;

  if (isascii (c))
    if (isalpha (c))
      {

	drive = _toupper (c) - 'A';

	if (dmap & (1L << drive))
	  return (SUCCESS);
      }

  return (FAILURE);
}

/* 
*/

/*
   =============================================================================
	prtsec() -- print a sector in hex and ASCII
   =============================================================================
*/

prtsec (sn)
     short sn;
{
  if (Rwabs (2, secbuf, 1, sn, drive))
    {

      sprintf (ebuf, "Sector %d is unreadable", sn);
      errmsg (ebuf);
      exit (1);
    }

  if (NOT spp)
    {

      spp = TRUE;
      printf ("\f");
    }

  printf ("\n\nDrive %c: -- Sector %d\n\n", drive + 'A', sn);
  mdump (secbuf, &secbuf[SECLEN - 1], 0L);
}

/* 
*/

/*
   =============================================================================
	main() -- main driver for secdump.c
   =============================================================================
*/

main (argc, argv)
     int argc;
     char *argv[];
{
  short isec;

  /* decode the command line */

  switch (argc)
    {

    case 1:			/* no arguments -- just give usage message */

      usage ();
      exit (0);

    case 3:			/* 2 arguments -- expect drive and first sector */

      if (getdrv (argv[1]))
	{

	  errmsg ("Invalid drive specifier");
	  exit (1);
	}

      if (getsnum (argv[2]))
	{

	  errmsg ("Invalid first sector");
	  exit (1);
	}

      sec_bgn = secnum;
      sec_end = sec_bgn;
      break;

    case 4:			/* 3 arguments -- expect drive and sector range */

      if (getdrv (argv[1]))
	{

	  errmsg ("Invalid drive specifier");
	  exit (1);
	}

      if (getsnum (argv[2]))
	{

	  errmsg ("Invalid first sector");
	  exit (1);
	}

      sec_bgn = secnum;

      if (getsnum (argv[3]))
	{

	  errmsg ("Invalid last sector");
	  exit (1);
	}

      sec_end = secnum;

      if (sec_end < sec_bgn)
	{

	  errmsg ("Last sector preceeds first sector");
	  exit (1);
	}

      break;

    default:			/* wrong argument count -- give an error message */

      errmsg ("Wrong number of arguments");
      usage ();
      exit (1);
    }

  spp = FALSE;

  /* get the sectors, and print them */

  for (isec = sec_bgn; isec LE sec_end; isec++)
    prtsec (isec);

  exit (0);
}
