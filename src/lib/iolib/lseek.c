/*
   =============================================================================
	lseek.c -- position a file to a specified byte location
	Version 7 -- 1987-10-28 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "io.h"
#include "biosdefs.h"
#include "errno.h"
#include "stddefs.h"

extern	int	_seek();

#if	DEBUGIT
extern	short	fsdebug;
#endif

long
lseek(fd, pos, how)
int fd;
register long pos;
int how;
{
	register struct fcb *fp;

	if ((fd < 0) OR (fd > MAXCHAN)) {

#if	DEBUGIT
	if (fsdebug)
		printf("lseek(%d):  bad file index\n", fd);
#endif

		errno = EBADF;		/* file number bad */
		return(-1L);
	}

	if (chantab[fd].c_seek EQ 0) {

#if	DEBUGIT
	if (fsdebug)
		printf("lseek(%d):  device not seekable\n", fd);
#endif

		errno = EINVAL;		/* device can't seek */
		return(-1L);
	}

	fp = chantab[fd].c_arg;		/* get fcb pointer from channel table */

	switch (how) {			/* dispatch off of seek type */

	case 2:				/* relative to end of file */
		pos += fp->curlen;
		break;

	case 1:				/* relative to current position */
		pos += fp->offset + (fp->curlsn << FILESHFT);

	case 0:				/* relative to start of file */
		break;

	default:
		errno = EINVAL;		/* invalid seek type */
		return(-1L);
	}
/* 
*/
	if (pos < 0) {				/* trap seeks before BOF */

		fp->offset = 0;
		fp->curlsn = 0;
		fp->modefl |= FC_ERR;

#if	DEBUGIT
	if (fsdebug)
		printf("lseek(%d):  seek (%ld) is before BOF\n", fd, pos);
#endif

		errno = EINVAL;
		return(-1L);

	} else if (pos > fp->curlen) {		/* trap seeks past EOF */

		fp->offset = fp->curlen & (BPSEC -1);
		fp->curlsn = fp->curlen >> FILESHFT;
		fp->modefl |= FC_ERR;

#if	DEBUGIT
	if (fsdebug)
		printf("lseek(%d):  seek (%ld) is after EOF (%ld)\n",
			fd, pos, fp->curlen);
#endif

		errno = EINVAL;
		return(-1L);
	}

	fp->offset = pos & ((long)BPSEC - 1);	/* calculate sector offset */
	fp->curlsn = pos >> FILESHFT;		/* calculate logical sector */

	if (_seek(fp) < 0) {			/* position to the physical sector */

		fp->modefl |= FC_ERR;		/* couldn't seek */
		errno = EIO;
		return(-1L);
	}

	if (pos EQ fp->curlen)
		fp->modefl |= FC_EOF;

#if	DEBUGIT
	if (fsdebug)
		printf("lseek():  pos=%d, curlsn=%ld, curdsn=%ld, offset=%u\n",
			pos, fp->curlsn, fp->curdsn, fp->offset);
#endif

	return(pos);				/* return current position */
}

