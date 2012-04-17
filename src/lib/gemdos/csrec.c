/*
   ============================================================================
	csrec.c -- Motorola S record format checker
	Version 3 -- 1987-07-10 -- D.N. Lynx Crowe
   ============================================================================
*/

#define	CHECKER		0	/* define non-zero for main */

#if	CHECKER

#define	MAXBUFLN	2048	/* big enough for the longest expected record */

#include "stdio.h"

char iobuf[MAXBUFLN + 1];
char *fileid;

#endif

#include <stddefs.h>
#include <ctype.h>

#define	SREC9	"04000000FB"

static char ahex[] = "0123456789abcdefABCDEF";

/*  */

/*
   ============================================================================
	memcmpu -- compare two memory areas while ignoring case
   ============================================================================
*/

int
memcmpu (s1, s2, n)
     register char *s1, *s2;
     int n;
{
  register int c1, c2;

  while (n)
    {

      c1 = 0377 & *s1++;
      c2 = 0377 & *s2++;

      if (isascii (c1) && islower (c1))
	c1 = _toupper (c1);

      if (isascii (c2) && islower (c2))
	c2 = _toupper (c2);

      if (c1 < c2)
	return (-1);

      if (c1 > c2)
	return (1);

      n--;
    }

  return (0);
}

/*  */

/*
   ============================================================================
	xdtoi -- convert hex ASCII to an int digit
   ============================================================================
*/

static int
xdtoi (c)
     register int c;
{
  register int i;
  register char *ap = &ahex[0];

  for (i = 0; i < 22; i++)
    if (c == * ap++)
      if (i > 15)
	return (i - 6);
      else
	return (i);

  return (-1);
}

/*  */

/*
   ============================================================================
	csrec -- check a Motorola S record for errors
   ============================================================================
*/

int
csrec (line)
     register char *line;
{
  register int c, len, i;

  int csum, val;

  if ('S' != (c = *line++))
    return (-1);

  switch (c = *line++)
    {

    case '2':

      csum = 0;

      if (isxdigit (c = *line++))
	len = xdtoi (c);
      else
	return (-1);

      if (isxdigit (c = *line++))
	len = (len << 4) + xdtoi (c);
      else
	return (-1);

      csum += (len & 0377);
      len -= 4;

      for (i = 0; i < 3; i++)
	{

	  if (isxdigit (c = *line++))
	    val = xdtoi (c);
	  else
	    return (-1);

	  if (isxdigit (c = *line++))
	    val = (val << 4) + xdtoi (c);
	  else
	    return (-1);

	  csum += (val & 0377);
	}

      for (i = 0; i < len; i++)
	{

	  if (isxdigit (c = *line++))
	    val = xdtoi (c);
	  else
	    return (-1);

	  if (isxdigit (c = *line++))
	    val = (val << 4) + xdtoi (c);
	  else
	    return (-1);

	  csum += (val & 0377);
	}

      csum = 0377 & ~csum;

      if (isxdigit (c = *line++))
	val = xdtoi (c);
      else
	return (-1);

      if (isxdigit (c = *line++))
	val = (val << 4) + xdtoi (c);
      else
	return (-1);

      if (csum != (val & 0377))
	return (-1);

      return (1);

    case '9':

      if (memcmpu (line, SREC9, 10) == 0)
	return (0);
      else
	return (-1);

    default:

      return (-1);
    }

  return (-1);
}

/*  */

#if	CHECKER

main (argc, argv)
     int argc;
     char **argv;
{
  FILE *sfile;
  int rc;
  long recnum;

  if (argc < 2)
    {

      printf ("csrec:  No file specified on input line.\n");
      printf ("csrec:  Using stdin for input.\n");
      sfile = stdin;
      fileid = "stdin";

    }
  else
    {

      ++argv;
      fileid = *argv;

      if (NULL == (sfile = fopena (fileid, "r")))
	{

	  printf ("csrec:  Unable to open [%s]\n", fileid);
	  printf ("csrec:  Processing aborted.\n");
	  exit (1);
	}
    }

  recnum = 0L;

  do
    {

      recnum++;
      memset (iobuf, ' ', MAXBUFLN + 1);
      rc = fgets (iobuf, MAXBUFLN, sfile);

      if (rc == NULL)
	{

	  printf ("csrec:  Could not read record %ld on [%s]\n",
		  recnum, fileid);

	  if (sfile != stdin)
	    fclose (sfile);

	  printf ("csrec:  Processing aborted.\n");
	  exit (1);
	}

      if ((rc = csrec (iobuf)) < 0)
	{

	  printf ("csrec:  Record %ld on [%s] is in error\n", recnum, fileid);

	  if (sfile != stdin)
	    fclose (sfile);

	  printf ("csrec:  Processing aborted.\n");
	  exit (1);
	}

    }
  while (rc);

  if (sfile != stdin)
    fclose (sfile);

  printf ("csrec:  S-Record file [%s] is valid\n", fileid);
  printf ("csrec:  Processing complete.\n");
  exit (0);
}

#endif
