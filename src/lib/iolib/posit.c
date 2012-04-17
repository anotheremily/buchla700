/*
   =============================================================================
	posit.c -- position a file to a specific relative sector
	Version 4 -- 1987-06-29 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "biosdefs.h"
#include "io.h"
#include "errno.h"
#include "stddefs.h"

extern int _seek ();

/*
   =============================================================================
	posit(fd, pos) -- position file 'fd' at sector 'pos'.
   =============================================================================
*/

int
posit (fd, pos)
     int fd;
     unsigned pos;
{
  register struct fcb *fp;

  if ((fd < 0) || (fd > MAXCHAN))
    {

      errno = EBADF;
      return (FAILURE);
    }

  fp = chantab[fd].c_arg;

  if ((chantab[fd].c_seek == 0) || ((pos << FILESHFT) > fp->curlen))
    {

      errno = EINVAL;
      fp->modefl |= FC_ERR;
      return (FAILURE);
    }

  fp->curlsn = pos;
  fp->offset = 0;

  if (_seek (fp) < 0)
    {

      errno = EIO;
      fp->modefl |= FC_ERR;
      return (FAILURE);
    }

  return (SUCCESS);
}
