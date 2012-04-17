/*
   =============================================================================
	testcio.c -- test how the C I/O functions handle writes and seeks
	Version 7 -- 1987-10-19 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	LATTICEC	1	/* non-zero for Lattice C */

#include "stdio.h"
#include "stddefs.h"
#include "errno.h"

#define	THEFILE		"testcio.tmp"	/* file name */

#if	LATTICEC
#define	fopenb(x,y)	fopen(x,y)
#define	THEMODE		"wb+"	/* file open mode */
#define	MAGIC		0x0000FEDC	/* our MAGIC number */
#else
#define	MAGIC		0xFEDC	/* our MAGIC number */
#define	THEMODE		"w+"	/* file open mode */
#endif

#define	AMOUNT		2400	/* a little bigger than a cluster */

extern int errno;		/* the most recent system error code */

FILE *fp1;			/* our file pointer */

/* 
*/

int
getw (stream)
     register FILE *stream;
{
  int temp;
  register char *t;

  temp = 0;

#if	LATTICEC
  t = (char *) &temp + 2;
#else
  t = (char *) &temp;
#endif

  *t++ = getc (stream);

  if (ferror (stream))
    return (EOF);

  *t = getc (stream);

  return (temp & 0xFFFF);
}

#if	LATTICEC
void
#endif
putw (w, stream)
     register unsigned w;
     FILE *stream;
{
  putc (((w >> 8) & 0xFF), stream);

  if (ferror (stream))
    return;

  putc ((w & 0xFF), stream);
}

/* 
*/

main ()
{
  int i, j, errs;
  long where, was, after;

  /* open the file for write update */

  if (NULL == (fp1 = fopenb (THEFILE, THEMODE)))
    {

      printf ("ERROR:  unable to open [%s] in \042%s\042 mode.\n",
	      THEFILE, THEMODE);
      exit (1);
    }

  printf ("File [%s] open in \042%s\042 mode.\n", THEFILE, THEMODE);

  /* now write some data to it */

  for (i = 0; i < AMOUNT; i++)
    {

      putw (i, fp1);

      if (ferror (fp1))
	{

	  printf ("ERROR:  write %d failed.  errno = %d.\n", i, errno);
	  exit (1);
	}
    }

  fflush (fp1);			/* flush the buffer */

  printf ("%d words written to file.\n", AMOUNT);
  after = ftell (fp1);
  printf ("File position after writing = %ld.\n", after);

  /* seek to the beginning, and read the data back */

  where = 0L;

  if (fseek (fp1, where, 0))
    {

      printf ("ERROR:  could not seek to %ld.\n", where);
      exit (1);
    }

  was = ftell (fp1);
  printf ("File position after seek to %ld = %ld.\n", where, was);
  printf ("Reading back data.\n");
  errs = 0;

  for (i = 0; i < AMOUNT; i++)
    {

      if (feof (fp1))
	{

	  printf ("ERROR:  unexpected EOF at %d.\n", i);
	  exit (1);
	}

      j = getw (fp1);

      if (ferror (fp1))
	{

	  printf ("ERROR:  read error at %d.  errno=%d.\n", i, errno);
	  exit (1);
	}

      if (i != j)
	{

	  ++errs;
	  was = ftell (fp1);
	  printf ("ERROR:  at %ld, expected %d, read %d.\n", was, i, j);
	}
    }

  was = ftell (fp1);
  printf ("File written and read back.  File position = %ld.\n", was);
  printf ("Read back check complete with %d error%s\n",
	  errs, ((errs != 1) ? "s." : "."));

  if (feof (fp1))
    {

      printf ("Note:  EOF seen at end of read to EOF.\n");

      if (ferror (fp1))
	{

	  printf ("ERROR:  ferror() returned non-zero at EOF.  errno = %d\n",
		  errno);
	  exit (1);
	}
    }

  if (after == was)
    printf ("File positions at end of read and write match.\n");
  else
    {

      printf ("File positions at end of read and write differ.\n");
      printf ("Write ended at %ld, read ended at %ld.\n", after, was);
    }

  /* now get the word at AMOUNT and print it before we write over it */

  where = AMOUNT;

  if (fseek (fp1, where, 0))
    {

      printf ("ERROR:  could not seek to %ld.\n", where);
      exit (1);
    }

  if (feof (fp1))
    {

      printf ("ERROR:  unexpected EOF at %ld after fseek().\n", where);
      exit (1);
    }

  j = getw (fp1);

  if (ferror (fp1))
    {

      printf ("ERROR:  read error at %ld.  errno=%d.\n", where, errno);
      exit (1);
    }

  printf ("Random read executed at %ld.  Value read was %d\n", where, j);

  /* now write some data in the middle of the file */

  where = AMOUNT;		/* note that AMOUNT is words, where is bytes */

  if (fseek (fp1, where, 0))
    {

      printf ("ERROR:  could not seek to %ld.\n", where);
      exit (1);
    }

  i = MAGIC;

  putw (i, fp1);

  if (ferror (fp1))
    {

      printf ("ERROR:  write %d failed.  errno = %d.\n", i, errno);
      exit (1);
    }

  fflush (fp1);

  printf ("Random write of MAGIC executed to %ld.\n", where);

  /* seek back to what we just wrote */

  where = AMOUNT;

  if (fseek (fp1, where, 0))
    {

      printf ("ERROR:  could not seek to %ld.\n", where);
      exit (1);
    }

  if (feof (fp1))
    {

      printf ("ERROR:  unexpected EOF at %ld after fseek().\n", where);
      exit (1);
    }

  j = getw (fp1);

  if (ferror (fp1))
    {

      printf ("ERROR:  read error at %ld.  errno=%d.\n", where, errno);
      exit (1);
    }

  printf ("Random access read executed at %ld.\n", where);

  if (j != MAGIC)
    {

      printf ("ERROR:  j (%d) != MAGIC (%d).\n", j, MAGIC);
      exit (1);
    }

  if (feof (fp1))
    {

      printf ("EOF sensed after reading MAGIC at %ld.\n", where);
      exit (1);
    }

  j = getw (fp1);

  if (ferror (fp1))
    {

      printf ("ERROR:  read error after MAGIC.  errno=%d.\n", errno);
      exit (1);
    }

  printf ("Word after MAGIC = %d.\n", j);

  /* seek to the end of the file */

  where = after;

  if (fseek (fp1, where, 0))
    {

      printf ("ERROR:  could not seek to %ld.\n", where);
      exit (1);
    }

  if (feof (fp1))
    {

      printf ("Note:  EOF at %ld after fseek().\n", where);
    }

  /* write the magic word at EOF */

  i = MAGIC;

  putw (i, fp1);

  if (ferror (fp1))
    {

      printf ("ERROR:  write %d failed.  errno = %d.\n", i, errno);
      exit (1);
    }

  printf ("Random write of MAGIC executed to %ld.\n", where);

  was = ftell (fp1);
  printf ("File position after write at EOF = %ld\n", was);

  fflush (fp1);

  /* seek back to what we just wrote */

  where = after;

  if (fseek (fp1, where, 0))
    {

      printf ("ERROR:  could not seek to %ld.\n", where);
      exit (1);
    }

  if (feof (fp1))
    {

      printf ("ERROR:  unexpected EOF at %ld after fseek().\n", where);
      exit (1);
    }

  j = getw (fp1);

  if (ferror (fp1))
    {

      printf ("ERROR:  read error at %ld.  errno=%d.\n", where, errno);
      exit (1);
    }

  printf ("Random access read executed at %ld.\n", where);

  if (j != MAGIC)
    {

      printf ("ERROR:  j (%d) != MAGIC (%d).\n", j, MAGIC);
      exit (1);
    }

  if (feof (fp1))
    printf ("Note:  EOF sensed after reading MAGIC at %ld.\n", where);

  was = ftell (fp1);
  printf ("Final file position = %ld.\n", was);

  j = getw (fp1);

  if (!feof (fp1))
    {

      was = ftell (fp1);
      printf ("ERROR:  no EOF seen after read at EOF.  File at %ld.\n", was);
      exit (1);
    }

  printf ("EOF seen after read at EOF.\n");
  exit (0);
}
