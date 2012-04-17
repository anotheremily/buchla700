/*
   ============================================================================
	mothex.c -- convert Alcyon object file to Motorola hex S-records
	Version 18 -- 1989-07-18 -- D.N. Lynx Crowe
	(c) Copyright 1987, 1988, 1989 -- D.N. Lynx Crowe
   ============================================================================
*/

#include "stdio.h"
#include "stddefs.h"
#include "ctype.h"
#include "portab.h"
#include "objdefs.h"

#define	MOT_ALL		"mot"	/* extension for un-split files */
#define	MOT_EVN		"mhi"	/* extension for even (high) byte files */
#define	MOT_ODD		"mlo"	/* extension for odd (low) byte files */

#define	CHUNK		16384	/* input buffer read size */

#define	SWITCHAR(c)	((c) == '\\')	/* TOS definition */
#define	DRIVES(c)	((c) == ':')	/* TOS / MSDOS / PCDOS definition */

extern char *sbrk ();

int textsw,			/* user specified text origin switch */
  bytesw;			/* byte split switch */

FILE *curfp,			/* input file pointer */
 *motfp;			/* output file pointer */

struct EXFILE fhdr;		/* input file header */

long textorg,			/* text origin from input file */
  dataorg,			/* data origin from input file */
  bssorg,			/* bss origin from input file */
  textval;			/* text origin for output */

char *buffer,			/* input buffer pointer */
 *obuf,				/* output buffer pointer */
  fnbuf[64];			/* output file name buffer */

/* 
*/

/*
   ============================================================================
	usage() -- print the usage message on stderr
   ============================================================================
*/

usage ()
{
  fprintf (stderr,
	   "\nmothex:  Alcyon .abs to Motorola .mot file converter\n\n");

  fprintf (stderr, "usage:    mothex [-b] [-t addr]\n\n");

  fprintf (stderr, "   -b        byte split  (produces .mhi, .mlo files\n");
  fprintf (stderr, "   -t addr   set text origin\n");
}

/* 
*/

/*
   ============================================================================
	base_nm(s1, s2) -- copy the base name from 's2' into 's1'
   ============================================================================
*/

char *
base_nm (s1, s2)
     char *s1;
     register char *s2;
{
  register char *cp, *s3;
  register char c;

  cp = s2;
  s3 = s1;

  while ('\0' != * s2)
    {

      c = *s2++;

      if (SWITCHAR (c) || DRIVES (c))
	cp = s2;
    }

  while (*s3++ = *cp++);

  *s3++ = '\0';

  return (s1);
}

/* 
*/

/*
   ============================================================================
	root_nm(s1, s2) -- copy the root name from file name 's2'
		to string 's1'
   ============================================================================
*/

char *
root_nm (s1, s2)
     register char *s1, *s2;
{
  register int c;
  register char *s4;
  char *s3;
  char buf[33];

  s3 = s1;
  s4 = base_nm (buf, s2);

  while (c = 0x00FF & *s4++)
    {

      if (c == '.')
	break;

      *s1++ = c;
    }

  *s1++ = '\0';
  return (s3);
}

/* 
*/

/*
   ============================================================================
	out_nm(s1, s2, s3) -- create an output file name in 's1' from
		the  input string 's2' and extension string 's3'

	s1	output string pointer
	s2	input string pointer
	s3	extension string pointer
   ============================================================================
*/

char *
out_nm (s1, s2, s3)
     register char *s1;
     char *s2, *s3;
{
  root_nm (s1, s2);
  strcat (s1, ".");
  strcat (s1, s3);
  return (s1);
}

/*
   ============================================================================
	atolx(p) -- convert an ASCII hex string to a long
   ============================================================================
*/

long
atolx (p)
     register char **p;
{
  register long n;
  register int c;

  n = 0L;
  --*p;

  while (isxdigit (c = *++*p))
    n = (n << 4) + tonumber (c);

  --*p;
  return (n);
}

/* 
*/

/*
   ============================================================================
	findopt -- process options and find first file name
   ============================================================================
*/

findopt (argc, argv)
     int argc;
     char *argv[];
{
  char **eargv;
  int eargc, c;

  eargv = argv;
  eargc = 0;
  textval = 0L;
  textsw = FALSE;
  bytesw = FALSE;

  while (--argc > 0)
    {

      switch (c = **++argv)
	{

	case '-':

	  if ((c = *++*argv) == '\0')
	    break;

	  do
	    {
	      switch (c)
		{

		case 'b':	/* byte split the file */

		  bytesw = TRUE;
		  continue;

		case 't':	/* set text origin */

		  textsw = TRUE;

		  if (--argc > 0)
		    {

		      ++argv;
		      textval = atolx (argv);
		    }

		  continue;
/* 
*/
		default:

		  fprintf (stderr, "mothex:  Invalid option [%c]\n", c);
		  usage ();
		  exit (2);
		}

	    }
	  while ((c = *++*argv) != '\0');

	  continue;
	}

      *eargv++ = *argv;
      ++eargc;
    }

  return (eargc);
}

/* 
*/

/*
   ============================================================================
	lread(buff, len, fp) -- read 'len' bytes into 'buff' from file 'fp'
   ============================================================================
*/

int
lread (buff, len, fp)
     char *buff;
     long len;
     FILE *fp;
{
  int ilen;

  while (len > 0)
    {

      if (len >= (long) CHUNK)
	{

	  if (1 != fread (buff, CHUNK, 1, fp))
	    return (EOF);

	  buff += (long) CHUNK;
	  len -= (long) CHUNK;

	}
      else
	{

	  ilen = len;

	  if (1 != fread (buff, ilen, 1, fp))
	    return (EOF);

	  len = 0L;
	}
    }

  return (0);
}

/* 
*/

/*
   ============================================================================
	process(fn) -- output file 'fn' as Motorola hex S-records
   ============================================================================
*/

process (fn)
     char *fn;
{
  long bufneed, len_evn, len_odd;
  register long i;
  char *ofn;
  register char *p1, *p2;

  textorg = 0L;			/* initialize the origins to 0 */
  dataorg = 0L;
  bssorg = 0L;

  /* open the input file */

  if ((FILE *) NULL == (curfp = fopenb (fn, "r")))
    {

      fprintf (stderr, "mothex:  Unable to open \"%s\"\n", fn);
      exit (2);
    }

  /* read in the file header */

  fprintf (stderr, "mothex:  Reading \"%s\"\n", fn);

  if (1 != fread (&fhdr, sizeof fhdr, 1, curfp))
    {

      fprintf (stderr, "mothex:  Unable to read \"%s\"\n", fn);
      fclose (curfp);
      exit (2);
    }

  /* check the magic */

  if ((fhdr.F_Magic != F_R_C) && (fhdr.F_Magic != F_R_D))
    {

      fprintf (stderr, "mothex:  Bad magic [0x%04x] in \"%s\"",
	       fhdr.F_Magic, fn);

      fclose (curfp);
      exit (2);
    }
/* 
*/
  /* if it's a discontinuous file, read the origins */

  if (fhdr.F_Magic == F_R_D)
    {

      dataorg = getl (curfp);
      bssorg = getl (curfp);
    }

  bufneed = fhdr.F_Text + fhdr.F_Data;
  buffer = (char *) sbrk (0);

  if (-1 == brk ((char *) (buffer + bufneed)))
    {

      fprintf (stderr,
	       "mothex:  Unable to allocate %ld byte input buffer at $%lx\n",
	       bufneed, buffer);

      fprintf (stderr, "mothex:  F_Text = %ld   F_Data = %ld\n",
	       fhdr.F_Text, fhdr.F_Data);

      fclose (curfp);
      exit (2);
    }

  if (bytesw)
    {				/* allocate byte split buffer */

      obuf = sbrk (0);

      if (-1 == brk ((char *) (obuf + 1 + (bufneed >> 1))))
	{

	  fprintf (stderr, "mothex:  Unable to allocate bytesplit buffer\n");
	  fclose (curfp);
	  exit (2);
	}
    }

  if (0 != lread (buffer, bufneed, curfp))
    {

      fprintf (stderr, "mothex:  Unable to read \"%s\"\n", fn);
      fclose (curfp);
      exit (2);
    }

  fclose (curfp);

  if (textsw)
    textorg = textval;
  else
    textorg = fhdr.F_Res2;

  if ((fhdr.F_Magic == F_R_D) && (dataorg != 0L) && (fhdr.F_Data != 0L))
    {

      fprintf (stderr, "mothex:  ERROR - can't split text and data\n");
      exit (2);
    }
/* 
*/
  if (bytesw)
    {				/* process byte-split file */

      len_evn = (bufneed & 1L) + (bufneed >> 1);
      len_odd = bufneed >> 1;

      /* create the even byte output file */

      ofn = out_nm (fnbuf, fn, MOT_EVN);

      if ((FILE *) NULL == (motfp = fopena (ofn, "w")))
	{

	  fprintf (stderr, "mothex:  Unable to open \"%s\"\n", fn);
	  exit (2);
	}

      p1 = obuf;
      p2 = buffer;

      for (i = len_evn; i--;)
	{

	  *p1++ = *p2;
	  p2 += 2;
	}

      fprintf (stderr, "mothex:  Writing \"%s\"\n", ofn);
      msrec (motfp, textorg, len_evn, obuf);
      fclose (motfp);

      /* create the odd byte output file */

      p1 = obuf;
      p2 = buffer + 1;

      for (i = len_odd; i--;)
	{

	  *p1++ = *p2;
	  p2 += 2;
	}

      ofn = out_nm (fnbuf, fn, MOT_ODD);

      if ((FILE *) NULL == (motfp = fopena (ofn, "w")))
	{

	  fprintf (stderr, "mothex:  Unable to open \"%s\"\n", fn);
	  exit (2);
	}

      fprintf (stderr, "mothex:  Writing \"%s\"\n", ofn);
      msrec (motfp, textorg, len_odd, obuf);
      fclose (motfp);
/* 
*/
    }
  else
    {				/* process un-split file */

      /* create the output file */

      ofn = out_nm (fnbuf, fn, MOT_ALL);

      if ((FILE *) NULL == (motfp = fopena (ofn, "w")))
	{

	  fprintf (stderr, "mothex:  Unable to open \"%s\"\n", fn);
	  exit (2);
	}

      fprintf (stderr, "mothex:  Writing \"%s\"\n", ofn);
      msrec (motfp, textorg, bufneed, buffer);
      fclose (motfp);
    }

  brk (buffer);			/* restore program break for next file */
}

/* 
*/

/*
   ============================================================================
	main processing loop -- process arguments and files
   ============================================================================
*/

main (argc, argv)
     int argc;
     char *argv[];
{
  argc = findopt (argc, argv);	/* analyze the command line */

  while (argc-- > 0)		/* process the files */
    process (*argv++);

  exit (0);
}
