/*
   =============================================================================
	filname.c -- file name and extension extractors
	Version 3 -- 1987-07-09 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	TESTER	0		/* define non-zero for test routine */

#include <stddefs.h>

#define	MAX_NAME	8	/* maximum length of a file name */
#define	MAX_EXT		3	/* maximum length of an extension */

/*
   =============================================================================
	FilName(s, p) -- Return a pointer to a string containing the file name
	found in the string at 's'.  Copy the file name into string 'p'.
   =============================================================================
*/

char *
FilName (s, p)
     register char *s, *p;
{
  register char *tp;
  register int i;

  tp = p;

  for (i = 0; i < MAX_NAME; i++)
    {				/* scan the name */

      if (*s)
	{			/* ... until we hit a '\0' */

	  if (*s EQ '.')	/* ... or a '.' */
	    break;

	  *p++ = *s++;		/* ... copying as we go */

	}
      else
	{			/* stop at '\0' */

	  *p = '\0';		/* terminate the string */
	  return (tp);		/* return a pointer to it */
	}
    }

  *p = '\0';			/* copied MAX_NAME bytes - that's all folks */
  return (tp);
}

/* 
*/

/*
   =============================================================================
	FilExt(s, p) -- Returns a pointer to a copy of the extension of
	the file name in 's'.  The file name is copied into string 'p' and
	a pointer to 'p' is returned.
   =============================================================================
*/

char *
FilExt (s, p)
     register char *s, *p;
{
  register char c, *tp;
  register int i;

  tp = p;

  while (c = *s)
    {				/* scan the string */

      if (c EQ '.')
	{			/* ... until we hit the dot */

	  ++s;			/* point past the dot */

	  for (i = 0; i < MAX_EXT; i++)
	    {			/* scan the extension ... */

	      if (*s)
		{		/* ... until we hit a '\0' */

		  *p++ = *s++;	/* ... copying as we go */

		}
	      else
		{		/* stop at '\0' */

		  *p = '\0';	/* terminate the string */
		  return (tp);	/* return a pointer to it */
		}
	    }

	  *p = '\0';		/* copied MAX_EXT bytes - that's all folks */
	  return (tp);		/* return a pointer to the result */

	}
      else
	{

	  ++s;			/* advance the pointer */
	}
    }

  *p = '\0';			/* terminate the string */
  return (tp);			/* return a pointer to the result */
}

/* 
*/

#if	TESTER

char *fn[] = {			/* test cases */

  "FILE.NAM",
  "FILE",
  "FILE.",
  ".NAM",
  ".",
  "",
  "fartoolonganame.longextension",
  "fartoolonganame",
  "fartoolonganame.",
  ".longextension"
};

#define	NCASES	((sizeof fn) / (sizeof (char *)))

char temp1[MAX_NAME + 1], temp2[MAX_EXT + 1];

fnt (s)				/* test both functions and print the result */
     char *s;
{
  printf ("[%s] gave [%s] [%s]\r\n",
	  s, FilName (s, temp1), FilExt (s, temp2));
}

main ()
{
  int i;

  /* hand the functions each of the test cases */

  for (i = 0; i < NCASES; i++)
    fnt (fn[i]);

  exit (0);
}

#endif
