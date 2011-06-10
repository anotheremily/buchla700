/*
   ============================================================================
	basenam.c -- Extract base name from a path
	Version 4 -- 1988-07-13 -- D.N. Lynx Crowe

	Extracts the base name of a file from a path.

		char *
		basenam(s)
		char *s;

	Can be used with MSDOS, PCDOS, GEMDOS, or Unix(tm) with suitable
	defines for SWITCHAR and DRIVES, as follows:

	For Unix(tm):
		SWITCHAR	'/'
		DRIVES		FALSE

	For MSDOS or PCDOS:
		SWITCHAR	swchar()	function to get DOS SWITCHAR
		DRIVES		(c == ':')

	For GEMDOS:
		SWITCHAR	'\\'
		DRIVES		(c == ':')

	Define TESTER to get a test program and edit the array of test cases
	for the system you're compiling for.
   ============================================================================
*/

#define	FALSE		0

#define	SWITCHAR	'\\'	/* TOS definition */
#define	DRIVES		(c == ':')	/* TOS / MSDOS / PCDOS definition */

/*  */

/*
   ============================================================================
	basenam(s) -- extract the base name from the path at s
   ============================================================================
*/

char *
basenam (s)
     char *s;
{
  register char *cp = s;
  register char c;

  while ('\0' != *s)
    {

      c = *s++;

      if ((c == SWITCHAR) || DRIVES)
	cp = s;
    }

  return (cp);
}

/*  */

#ifdef	TESTER

#include "stdio.h"

char *t[] = {			/* test cases for GEMDOS */

  "simple.one",
  "c:more.doc",
  "c:\\more",
  "\\yet\\more.doc",
  "another.1\\yet.more\\complex\\",
  "and\\still.one\\more",
  NULL
};

main ()
{
  register int n = 0;
  register char *p;

  while (NULL != (p = t[n++]))
    printf ("%s returned %s\n", p, basenam (p));

  printf ("Done.\n");
  exit (0);
}

#endif
