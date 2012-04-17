/*
   =============================================================================
	newver.c -- read and update a GEMDOS version message object file
	Version 1 -- 1988-10-28  -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe -- All rights reserved

	Compiled and linked with the GEMDOS Alcyon C developer's package.

	NOTE:  Because we use the Alcyon GEMDOS object file format,
	which is specific to the Atari,  this code is NON-PORTABLE.

	The idea, however, is adaptable to a wide variety of systems
	and object file formats.

	Note also that the fopen() function in the Alcyon C library does not
	support the "r+" option,  making it necessary to close the file and
	then open it again before writing.  This is a serious shortcoming of
	the Alcyon C library,  as it makes updating files only possible by a
	series of questionable maneuvers, (e.g. opening, closing, and reopening
	the file, or using open(), then fdopen(), etc.).
   =============================================================================
*/

#include "stdio.h"		/* Atari GEMDOS standard I/O definitions */
#include "stddefs.h"		/* some usefull standard C definitions */
#include "objdefs.h"		/* GEMDOS object file definitions */

#define	VERFILE		"verdate.o"	/* version message file */

extern int errno;		/* system error code */

extern char *now ();		/* get formatted system date and time */

FILE *fp;			/* VERFILE file pointer */

			/* 000000000011111111112 */
			/* 012345678901234567890 */
char dtg[22];			/* yyyy-mm-dd  hh:mm:ss  zero terminated string */

			/* 000000000011 */
			/* 012345678901 */
char verstr[12];		/* yyyymmdd.vv   zero terminated string */

struct vb
{				/* VERFILE buffer (66 bytes) */

  struct EXFILE hdr;
  char ver[12];
  struct SYMBOL sym;
  char rem[12];

} verbuf;

/* 
*/

/*
   =============================================================================
	read and update an Alcyon/GEMDOS format version message object file
   =============================================================================
*/

main ()
{
  short len, rc, vn, vp1, vp2;

  len = sizeof verbuf;

  /* first, read the old version message object file */

  if ((FILE *) NULL == (fp = fopenb (VERFILE, "r")))
    {

      printf ("ERROR -- Unable to open \"%s\" for reading  (errno = %d)\n",
	      VERFILE, errno);

      exit (1);
    }

  rewind (fp);

  if (1 != (rc = fread (&verbuf, len, 1, fp)))
    {

      printf ("ERROR -- Unable to read \"%s\"  (rc = %d, errno = %d)\n",
	      VERFILE, rc, errno);

      if (ferror (fp))
	printf ("  File system ERROR.\n");
      else if (feof (fp))
	printf ("  Premature EOF.\n");
      else
	printf ("  Neither ERROR or EOF set -- very odd\n");

      fclose (fp);
      exit (1);
    }

  fclose (fp);
/* 
*/

  /* next, set the date and version */

  now (dtg);			/* set date and time */

  memcpy (&verstr[0], &dtg[0], 4);
  memcpy (&verstr[4], &dtg[5], 2);
  memcpy (&verstr[6], &dtg[8], 2);
  strcpy (&verstr[8], ".01");

  if (memcmp (verbuf.ver, verstr, 8))
    {				/* if date is different ... */

      strcpy (verbuf.ver, verstr);	/* ... it's today's 1st version */

    }
  else
    {				/* ... otherwise,  it's today's next version */

      vn = ((verbuf.ver[9] - '0') * 10) + (verbuf.ver[10] - '0') + 1;

      if (vn >= 100)
	{			/* too many versions today ? */

	  printf ("WARNING -- version number rolled over to 00.\n");
	  vn = 0;
	}

      vp1 = vn / 10;
      vp2 = vn - (vp1 * 10);

      verbuf.ver[9] = vp1 + '0';
      verbuf.ver[10] = vp2 + '0';
    }

/* 
*/
  /* finally, re-write the version message object file */

  if ((FILE *) NULL == (fp = fopenb (VERFILE, "w")))
    {

      printf ("ERROR -- Unable to open \"%s\" for writing  (errno = %d)\n",
	      VERFILE, errno);

      exit (1);
    }

  rewind (fp);

  if (1 != (rc = fwrite (&verbuf, len, 1, fp)))
    {

      printf ("ERROR -- Unable to write to \"%s\"  (rc = %d, errno = %d)\n",
	      VERFILE, rc, errno);

      if (ferror (fp))
	printf ("  File system ERROR.\n");
      else if (feof (fp))
	printf ("  Premature EOF.\n");
      else
	printf ("  Neither ERROR or EOF set -- very odd\n");

      fclose (fp);
      exit (1);
    }

  fclose (fp);
  printf ("Current version:  %s\n", verbuf.ver);
  exit (0);
}
