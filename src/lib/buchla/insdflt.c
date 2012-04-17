/*
   =============================================================================
	insdflt.c -- read the default isntrument and create a C data file
	Version 3 -- 1988-04-19 -- D.N. Lynx Crowe
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

#define	LIBNAME		"m7dflt.orc"	/* library name */
#define	THEINST		0	/* library offset:  0 = 1 */

#define	DFLTNAME	"dfltins.h"
#define	MAXPERL		8

extern int errno;

struct instdef idefs[NINORC];
struct mlibhdr ldhead;

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
	convert an instrument definition to a data block in a header file
   =============================================================================
*/

main ()
{
  register FILE *fp;
  register unsigned *wp;
  register struct instdef *ip;
  register short i, npl, count;

  if ((FILE *) NULL == (fp = fopenb (LIBNAME, "r")))
    {

      printf ("Unable to open [%s]\n", LIBNAME);
      exit (1);
    }

  printf ("Reading orchestra from [%s]\n", LIBNAME);

  printf ("   header\n");

  if (readit (fp, &ldhead, (long) LH_LEN))
    die (fp);

  for (i = 0; i < NINORC; i++)
    {

      ip = &idefs[i];

      printf ("   instrument %d\n", i + 1);

      if (readit (fp, ip, (long) OR_LEN1))
	die (fp);

      if (readit (fp, ip->idhwvao, (long) OR_LEN2))
	die (fp);

      if (readit (fp, ip->idhwvbo, (long) OR_LEN2))
	die (fp);

      /* unpack offsets (and eventually harmonics) into finals */

      memcpyw (ip->idhwvaf, ip->idhwvao, NUMWPNT);
      memcpyw (ip->idhwvbf, ip->idhwvbo, NUMWPNT);
    }

  fclose (fp);
/* 
*/
  if ((FILE *) NULL == (fp = fopena (DFLTNAME, "w")))
    {

      printf ("Unable to open [%s] for output\n", DFLTNAME);
      exit (1);
    }

  printf ("Writing default instrument (%d) to [%s]\n",
	  (THEINST + 1), DFLTNAME);

  count = sizeof (struct instdef) / 2;
  wp = (unsigned *) &idefs[THEINST];
  npl = 0;

  fprintf (fp, "short\tdfltins[] = {\t\t/* default instrument */\n\n\t");

  while (count--)
    {

      fprintf (fp, "0x%04.4x", *wp++);

      if (++npl == MAXPERL)
	{

	  if (count)
	    fprintf (fp, ",\n\t");

	  npl = 0;

	}
      else
	{

	  if (count)
	    fprintf (fp, ", ");
	}
    }

  fprintf (fp, "\n};\n");
  fclose (fp);
  printf ("Default instrument file written.\n");
  exit (0);
}
