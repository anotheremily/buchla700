/*
   ============================================================================
	lxmain.c -- _main() for the Atari
	Version 4 -- 1987-02-10 -- D.N. Lynx Crowe
   ============================================================================
*/

/* #define	LOWERIT		define for GEMDOS case hacking (Yech!!) */

#ifdef	LOWERIT
#include "ctype.h"
#endif

extern char __tname[];

_main (cmdline, cmdlen)
     char cmdline[];
     int cmdlen;
{

#ifdef	LOWERIT
  register char c;
  register char *bp = cmdline;
#endif

  _chinit ();

  open (__tname, 0);
  open (__tname, 1);
  open (__tname, 1);

  cmdline[cmdlen] = '\0';

#ifdef	LOWERIT
  while (c = *bp)
    {

      if (isupper (c))
	*bp = c + 0x20;

      ++bp;
    }
#endif

  __main (cmdline, cmdlen);
}
