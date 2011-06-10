/*
   =============================================================================
	fseek.c -- perform a seek on a stream file
	Version 4 -- 1987-10-28 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "biosdefs.h"
#include "io.h"
#include "stddefs.h"

/*
   =============================================================================
	fseek(fp, pos, mode) -- position file 'fp' at 'pos' in mode 'mode'.
   =============================================================================
*/

fseek (fp, pos, mode)
     register FILE *fp;
     long pos;
     int mode;
{
  register int i, lr;
  long curpos, lseek ();

  if (fp->_flags & _DIRTY)
    {

      if (flush_ (fp, -1))
	return (EOF);

    }
  else
    {

      if (mode EQ 1 AND fp->_bp)
	pos -= (long) fp->_bend - (long) fp->_bp;
    }

  fp->_bp = fp->_bend = NULL;
  fp->_flags &= ~_EOF;

  lr = lseek (fp->_unit, pos, mode);

  if (chantab[fp->_unit].c_arg->modefl & FC_EOF)
    fp->_flags |= _EOF;

  if (lr < 0)
    return (EOF);

  return (0);
}
