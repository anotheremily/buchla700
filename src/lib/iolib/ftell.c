/*
   =============================================================================
	ftell.c -- return current file position
	Version 7 -- 1987-10-28 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "biosdefs.h"
#include "stdio.h"
#include "io.h"
#include "stddefs.h"

#if	DEBUGIT
extern	short	fsdebug;
#endif

extern	int	_filecl();

/*
   =============================================================================
	ftell(fp) -- return the current position of file 'fp'.
   =============================================================================
*/

long
ftell(fp)
register FILE *fp;
{
	register struct fcb *fcp;
	register struct channel *chp;
	register long dpos, pos, diff;

	if (fp EQ (FILE *)0L) {		/* see if we point at a FILE */

#if	DEBUGIT
	if (fsdebug)
		printf("ftell($%08.8lX):  ERROR - null FILE pointer\n", fp);
#endif

		return(0L);
	}

	if (!fp->_flags & _BUSY) {	/* see if it's open */

#if	DEBUGIT
	if (fsdebug)
		printf("ftell($%08.8lX):  ERROR - FILE not open\n", fp);
#endif

		return(0L);
	}

	chp = &chantab[fp->_unit];	/* point at the channel */

	if (chp->c_close NE _filecl) {	/* see if it's seekable */

#if	DEBUGIT
	if (fsdebug)
		printf("ftell($%08.8lX):  ERROR - FILE device not seekable\n",
			fp);
#endif

		return(0L);
	}

	fcp = chp->c_arg;		/* point at the FCB */

	dpos = fcp->offset + (fcp->curlsn << FILESHFT);

	if (fp->_flags & _DIRTY)	/* adjust for the buffering */
		pos = dpos + (diff = ((long)fp->_bp - (long)fp->_buff));
	else if (fp->_bp)
		pos = dpos - (diff = ((long)fp->_bend - (long)fp->_bp));
	else
		pos = dpos;

#if	DEBUGIT
	if (fsdebug) {

		printf("ftell($%08.8lX):  flags=$%04.4X, buff=$%08.8lX, bp=$%08.8lX, bend=$%08.8lX\n",
			fp, fp->_flags, fp->_buff, fp->_bp, fp->_bend);
	printf("ftell($%08.8lX):  fcp=$%08.8lX, pos=%ld, dpos=%ld, diff=%ld\n",
			fp, fcp, pos, dpos, diff);
		printf("ftell($%08.8lX):  curlsn=%ld, offset=%u\n",
			fp, fcp->curlsn, fcp->offset);

		if ((fp->_flags & _DIRTY) AND (fp->_bp EQ NULL))
			printf("ftell($%08.8lX):  ERROR - file is DIRTY and bp is NULL\n",
				fp);
	}
#endif

	return(pos);
}
