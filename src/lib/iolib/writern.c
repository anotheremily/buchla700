/*
   =============================================================================
	writern.c -- write a random sector onto a file
	Version 6 -- 1987-12-15 -- D.N. Lynx Crowe

	int
	WriteRN(fcp, buf)
	struct fcb *fcp;
	char *buf;

		Writes a sector onto file 'fcp' from 'buf'.  Seeks as needed.
		Returns SUCCESS (0) if OK,  FAILURE (-1) for errors.
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "biosdefs.h"
#include "errno.h"
#include "errdefs.h"
#include "fspars.h"

extern long _berrno;
extern int _seek ();

#if DEBUGIT
extern short fsdebug;
#endif

#if	TBUFFER
extern long _secwr ();		/* update buffer function */
#endif

/* 
*/

/*
   =============================================================================
	WriteRN(fcp, buf) -- Writes a sector onto file 'fcp' from 'buf'.
	Seeks as needed.  Returns SUCCESS (0) if OK,  FAILURE (-1) for errors.
   =============================================================================
*/

int
WriteRN (fcp, buf)
     struct fcb *fcp;
     char *buf;
{
  int sv;			/* seek return code */
  long brc;			/* bios return code */

  if (sv = _seek (fcp))
    {				/* try to find the sector we want */

      if (sv < 0)
	{			/* seek error ? */

#if DEBUGIT
	  if (fsdebug)
	    printf
	      ("WriteRN():  _seek FAILED (%d) - curlsn=%ld, curdsn=%ld\n", sv,
	       fcp->curlsn, fcp->curdsn);
#endif

	  errno = EIO;		/* I/O error or seek past EOF */
	  return (FAILURE);

	}
      else if (sv EQ 2)
	{			/* at hard EOF ? */

	  if (_alcnew (fcp))
	    {			/* allocate a new cluster */

	      errno = EIO;
	      return (FAILURE);
	    }
#if DEBUGIT
	  if (fsdebug)
	    printf ("WriteRN():  cluster allocated - curcls=%d, clsec=%d\n",
		    fcp->curcls, fcp->clsec);
#endif

	}
    }

#if DEBUGIT
  if (fsdebug)
    printf ("WriteRN():  curlsn=%ld, curdsn=%ld\n", fcp->curlsn, fcp->curdsn);
#endif

  /* write the sector */

  if (brc = BIOS (B_RDWR, 1, buf, 1, (short) fcp->curdsn, 0))
    {

#if DEBUGIT
      if (fsdebug)
	printf ("WriteRN():  B_RDWR FAILED - brc=%ld\n", brc);
#endif

      _berrno = brc;		/* log the error */
      errno = EIO;		/* ... as an I/O error */
      return (FAILURE);		/* return:  ERROR */
    }

#if	TBUFFER
  _secwr (buf, (short) fcp->curdsn);
#endif

  return (SUCCESS);		/* return:  SUCCESS */
}
