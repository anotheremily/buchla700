/*
   ============================================================================
	croot.c -- Root for Buchla 700 C programs
	Version 8 -- 1987-06-29 -- D.N. Lynx Crowe

	This version can be setup for I/O redirection or not, depending on the
	value of REDIRECT.  If redirection is supported, so are command line
	arguments, which must be passed to Croot() at 'cp' by the startup code.

	Normally, this won't be selected, as this is a dedicated application
	that doesn't expect command line arguments or I/O redirection.
   ============================================================================
*/

#define	ROOTMSG	"{Buchla 700 Croot - Version 8 - 1987-06-29}"

#define	_FS_DEF_		/* to avoid unnecessary externals */

#include "biosdefs.h"
#include "errno.h"
#include "fcntl.h"
#include "io.h"
#include "stddefs.h"

#define	REDIRECT	0	/* non-zero for command line stuff */

#define	MAXARGS		30	/* maximum number of command line arguments */

extern int open (), creat ();

extern int InitFS ();
extern int _fd_cls ();
extern int xtrap15 ();

int (*_clsall) ();

/* 
*/

static int Argc;
static char *Argv[MAXARGS];

/* 
*/

/*
   ============================================================================
	exit(code) -- return control to the BIOS
   ============================================================================
*/

exit (code)
{
  (*_clsall) ();		/* close all open files */
  xtrap15 (code);		/* return to the BIOS */
}


#if	REDIRECT

/*
   ============================================================================
	_eredir(name) -- output I/O redirection error message to stderr
   ============================================================================
*/

static
_eredir (name)
     char *name;
{
  char buff[200];

  strcpy (buff, "Can't open file for redirection: ");
  strcat (buff, name);
  strcat (buff, "\n");
  write (2, buff, strlen (buff));
  exit (EINVAL);
}

#endif

/* 
*/

/*
   ============================================================================
	Croot(cp) -- C root module for the Buchla 700
   ============================================================================
*/

Croot (cp)
     register char *cp;
{
  register char *fname;
  register int k;

  Argv[0] = ROOTMSG;
  Argc = 1;

  _clsall = _fd_cls;
  InitFS ();

#if	REDIRECT

  while (Argc < MAXARGS)
    {				/* handle command line arguments */

      while (*cp == ' ' || * cp == '\t')	/* skip whitespace */
	++cp;

      if (*cp == 0)		/* check for end of line */
	break;

      if (*cp == '>')
	{			/* > - redirect output */

	  k = 1;		/* stdout */
	  goto redir;

	}
      else if (*cp == '<')
	{			/* < - redirect input */

	  k = 0;		/* stdin */
	redir:
	  while (*++cp == ' ' || * cp == '\t')	/* skip whitespace */
	    ;

	  fname = cp;		/* pointer to start of name */

	  while (*++cp)		/* skip to whitespace */
	    if (*cp == ' ' || * cp == '\t')
	      {

		*cp++ = 0;
		break;
	      }

	  close (k);		/* close old assignment */

	  if (k)
	    k = creat (fname, 0666);	/* stdout */
	  else
	    k = open (fname, O_RDONLY);	/* stdin */

	  if (k == - 1)
	    _eredir (fname);

	}
      else
	{			/* collect a command line argument */

	  Argv[Argc++] = cp;

	  while (*++cp)		/* find end of argument */
	    if (*cp == ' ' || * cp == '\t')
	      {

		*cp++ = 0;
		break;
	      }
	}
    }

#endif

  main (Argc, Argv);		/* call application */
  exit (0);			/* exit in case the application didn't */
}
