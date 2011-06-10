/*
   =============================================================================
	getopt.c -- process the options on a Unix{tm} style command line
	Version 2 -- 1988-05-04 -- D.N. Lynx Crowe

	Adapted from public domain code by Henry Specner, et al,
	from Usenet "net.sources".

	extern int optind;	Index into nargv
	extern int opterr;	Error output flag
	extern int optopt;	Most recently parsed option letter
	extern char *optarg;	Pointer to option argument or NULL

	int
	getopt(nargc, nargv, optstr)
	int nargc;
	char *nargv[];
	char *optstr;

		Parses nargv[] according to optstr.  Returns the next
		option letter in argv that matches a letter in optstr.
		'optstr' is a string of recognized option letters;  if a
		letter is followed by a colon, the option is expected
		to have an argument that may or may not be separated
		from it by whitespace.  'optarg' is set to point to the
		start of the option argument on return from getopt.
		Assumes argv[0] is the program name.  Outputs error
		messages to stderr if 'opterr' is non-zero.  'optopt' is
		the most recently parsed option letter.

		Note:  this code is not ROM-able due to the use of an
		initialized static variable,  but this is probably not
		too important, as it's designed to be used as a command
		line processor.

	int
	getarg(nargc, nargv)
	int nargc;
	char *nargv[];

		Returns the index of the next argument, or EOF if the
		argument string is exhausted.  Used to get the file
		arguments after getopt() is finished.  Updates optarg
		to point at the argument.  Increments optind to point
		at the next argument.
   =============================================================================
*/

/* 
*/

#include "stdio.h"
#include "stddefs.h"

#define	ARGCH		(int)':'
#define	BADCH		(int)'?'
#define	EMSG		""
#define	ENDARGS		"--"

int opterr;			/* set to non-zero to get error message output */
int optind;			/* index of current option string in parent argv */
int optopt;			/* current option letter */

char *optarg;			/* pointer to the option argument */

/* 
*/

int
getarg (nargc, nargv)
     int nargc;
     char *nargv[];
{
  if (optind EQ 0)		/* in case getopt() wasn't called */
    optind++;

  if (optind GE nargc)
    return (EOF);		/* out of arguments */

  optarg = nargv[optind];	/* set optarg to point at the argument */
  return (optind++);		/* return the argument index */
}

static char *
index (s, c)
     register char *s;
     register int c;
{
  while (*s)
    if (c EQ * s)
      return (s);
    else
      s++;

  return (NULL);
}

/* 
*/

int
getopt (nargc, nargv, ostr)
     int nargc;
     char *nargv[], *ostr;
{
  static char *place = EMSG;	/* private scan pointer */
  register char *oli;

  if (optind EQ 0)		/* make sure optind starts out non-zero */
    ++optind;

  if (!*place)
    {				/* update the scan pointer */

      if ((optind GE nargc)
	  OR (*(place = nargv[optind]) NE '-') OR (!*++place))
	return (EOF);

      if (*place EQ '-')
	{			/* found "--" */

	  ++optind;
	  return (EOF);
	}
    }

  if (((optopt = (int) *place++) EQ ARGCH) OR (!(oli = index (ostr, optopt))))
    {				/* option letter OK ? */

      if (!*place)
	++optind;

      if (opterr)
	{

	  fputs (*nargv, stderr);
	  fputs (": unknown argument \042", stderr);
	  fputc (optopt, stderr);
	  fputs ("\042\n", stderr);
	}

      return (BADCH);
    }
/* 
*/
  if (*++oli NE ARGCH)
    {				/* check for required argument */

      optarg = NULL;		/* no argument needed */

      if (!*place)
	++optind;

    }
  else
    {				/* we need an arguement */

      if (*place)
	optarg = place;
      else if (nargc LE++ optind)
	{

	  place = EMSG;

	  if (opterr)
	    {

	      fputs (*nargv, stderr);
	      fputs (": option \042", stderr);
	      fputc (optopt, stderr);
	      fputs ("\042 requires an argument.\n", stderr);
	    }

	  return (BADCH);

	}
      else
	optarg = nargv[optind];

      place = EMSG;
      ++optind;
    }

  return (optopt);
}
