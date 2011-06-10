/*
   =============================================================================
	conin.c -- read from the console
	Version 6 -- 1987-06-30 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	_FS_DEF_		/* to avoid unnecessary externals */

#include "biosdefs.h"
#include "io.h"
#include "errno.h"
#include "fcntl.h"
#include "stddefs.h"

extern char *memcpy ();
extern int readbuf (), writeln ();

char _ConBuf[258];		/* console input buffer */
int _CBused;

int
_conin (x, buff, len)
     char *buff;
{
  int nbp;
  register int l;

  if (_ConBuf[1] EQ 0)
    {

      _ConBuf[0] = 255;
      _ConBuf[1] = _ConBuf[2] = 0;

      readbuf (CON_DEV, _ConBuf);
      writeln (CON_DEV, "\r\n");

      if (_ConBuf[2] EQ 0x1a)
	{

	  _ConBuf[1] = 0;
	  return (0);
	}

      nbp = ++_ConBuf[1];
      _ConBuf[nbp++ + 1] = '\r';
      _ConBuf[nbp + 1] = '\n';
      _CBused = 2;
    }

  if ((l = _ConBuf[1]) > len)
    l = len;

  memcpy (buff, (_ConBuf + _CBused), l);
  _CBused += l;
  _ConBuf[1] -= l;
  return (l);
}
