/*
   =============================================================================
	readrn.c -- read a random sector from a file
	Version 8 -- 1987-12-15 -- D.N. Lynx Crowe

	int
	ReadRN(fcp, buf)
	struct fcb *fcp;
	char *buf;

		Reads a sector from file 'fcp' into 'buf'.  Seeks as needed.
		Returns 0 if OK, -1 for errors, 1 for EOF (no data returned).
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "biosdefs.h"
#include "errno.h"
#include "errdefs.h"
#include "fspars.h"

#if	DEBUGIT
extern short fsdebug;
#endif

#if	TBUFFER
extern long _secrd ();
#endif

extern long _berrno;
extern int _seek ();

/* 
*/

/*
   =============================================================================
	ReadRN(fcp, buf) -- Reads a sector from file 'fcp' into 'buf'.
	Seeks as needed.  Returns SUCCESS (0) if OK,  FAILURE (-1) for errors.
   =============================================================================
*/

int
ReadRN (fcp, buf)
     struct fcb *fcp;
     char *buf;
{
  int sv;			/* seek return code */
  long brc;			/* bios return code */

  if (sv = _seek (fcp))		/* try to find the sector we want */
    if (sv < 0)
      {

	errno = EIO;		/* I/O error */
	return (-1);		/* return:  ERROR */

      }
    else
      {

	errno = EINVAL;		/* invalid argument */
	return (1);		/* return:  EOF */
      }

#if	DEBUGIT
  if (fsdebug)
    printf ("ReadRN():  curlsn=%ld, curdsn=%ld, offset=%u\n",
	    fcp->curlsn, fcp->curdsn, fcp->offset);
#endif

#if	TBUFFER
  if (brc = _secrd (buf, (short) fcp->curdsn))
    {
#else
  if (brc = BIOS (B_RDWR, 0, buf, 1, (short) fcp->curdsn, 0))
    {
#endif

      _berrno = brc;		/* log the error */
      errno = EIO;		/* ... as an I/O error */
      return (FAILURE);		/* return:  ERROR */
    }

  return (SUCCESS);		/* return:  SUCCESS */
}
