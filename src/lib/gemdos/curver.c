/*
   =============================================================================
	curver.c -- read and print a GEMDOS version message object file
	Version 1 -- 1988-11-03  -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe -- All rights reserved

	Compiled and linked with the GEMDOS Alcyon C developer's package.

	NOTE:  Because we use the Alcyon GEMDOS object file format,
	which is specific to the Atari,  this code is NON-PORTABLE.

	The idea, however, is adaptable to a wide variety of systems
	and object file formats.
   =============================================================================
*/

#include "stdio.h"		/* Atari GEMDOS standard I/O definitions */
#include "stddefs.h"		/* some usefull standard C definitions */
#include "objdefs.h"		/* GEMDOS object file definitions */

#define	VERFILE		"verdate.o"	/* version message file */

extern int errno;		/* system error code */

FILE *fp;			/* VERFILE file pointer */

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
	read and print an Alcyon/GEMDOS format version message object file
   =============================================================================
*/

main ()
{
  short len, rc;

  len = sizeof verbuf;

  /* first, read the version message object file */

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

  /* close the file, print the version message, and exit */

  fclose (fp);
  printf ("Current version:  %s\n", verbuf.ver);
  exit (0);
}
