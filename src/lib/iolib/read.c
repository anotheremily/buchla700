/*
   =============================================================================
	read.c -- file read functions for the C I/O Library
	Version 9 -- 1987-10-28 -- D.N. Lynx Crowe

	int
	read(fd, buff, len)
	char *buff;

		Reads 'len' bytes from file 'fd' into 'buff'.
		Returns FAILURE (-1) for errors, number of bytes read
		if successful.

	int
	_getsec(fp, buf, len)
	register struct fcb *fp;
	char *buf;
	unsigned len;

		Gets 'len' bytes into 'buf' from file 'fp'.
		Returns FAILURE (-1) for errors, SUCCESS (0) if successful.

	int
	_filerd(fp, buffer, len)
	register struct fcb *fp;
	char *buffer;
	unsigned len;

		Reads 'len' bytes into 'buffer' from 'fp'.
		Returns FAILURE (-1) for errors, number of bytes read
		if successful.

   =============================================================================
*/

#define	DEBUGIT		0

#include "biosdefs.h"
#include "io.h"
#include "errno.h"
#include "fcntl.h"
#include "stddefs.h"

#if	DEBUGIT
extern	short	fsdebug;
#endif

extern	int	_badfd(), _conin(), _seek();

extern	char	*memcpy();

int	_filerd();

static int (*t_read[])() = {

	_badfd,		/* 0 - invalid type */
	_filerd,	/* 1 - disk file read */
	_conin		/* 2 - console read */
};

/* 
*/

/*
   =============================================================================
	read(fd, buff, len) -- Reads 'len' bytes from file 'fd' into 'buff'.
	Returns FAILURE (-1) for errors, number of bytes read if successful.
   =============================================================================
*/

int
read(fd, buff, len)
int fd;
char *buff;
unsigned len;
{
	register struct channel *chp;

	if (fd < 0 OR fd > MAXCHAN) {	/* check fd range */

		errno = EBADF;	/* bad fd */
		return(FAILURE);
	}

	chp = &chantab[fd];	/* point at the channel table */
	return((*t_read[chp->c_read])(chp->c_arg, buff, len));	/* do the read */
}

/* 
*/

/*
   =============================================================================
	_getsec(fp, buf, len) -- Gets 'len' bytes into 'buf' from file 'fp'.
	Returns FAILURE (-1) for errors, SUCCESS (0) if successful.
   =============================================================================
*/

int
_getsec(fp, buf, len)
register struct fcb *fp;
char *buf;
unsigned len;
{
	if ((errno = ReadRN(fp, Wrkbuf)) NE 0)	/* get current sector */
		return(FAILURE);

	memcpy(buf, Wrkbuf + fp->offset, len);	/* move what we need */

	if ((fp->offset = (fp->offset + len) & (BPSEC - 1)) EQ 0) {

		++fp->curlsn;			/* advance the sector number */

		if (_seek(fp) < 0)		/* seek to the next sector */
			return(FAILURE);
	}

	return(SUCCESS);			/* return:  all bytes read */
}

/* 
*/

/*
   =============================================================================
	_filerd(fp, buffer, len) -- Reads 'len' bytes into 'buffer' from 'fp'.
	Returns FAILURE (-1) for errors, number of bytes read if successful.
   =============================================================================
*/

int
_filerd(fp, buffer, len)
register struct fcb *fp;
char *buffer;
unsigned len;
{
	register unsigned l;
	register unsigned j, k;
	register long curpos, newpos;

	l = 0;
	curpos = fp->offset + (fp->curlsn << FILESHFT);
	newpos = curpos + len;

#if	DEBUGIT
	if (fsdebug)
		printf("_filerd():  len=%u, curpos=%ld, newpos=%ld, curlen=%ld\n",
			len, curpos, newpos, fp->curlen);
#endif

	if (newpos GT fp->curlen) {

		len = fp->curlen - curpos;

#if	DEBUGIT
	if (fsdebug)
		printf("_filerd():  len adjusted to %u\n", len);
#endif
	}

	if (fp->offset) {	/* see if we start in the middle of a sector */

		if ((l = BPSEC - fp->offset) > len)	/* see what we need */
			l = len;

		if (_getsec(fp, buffer, l))	/* read what we can */
			return(len);	/* return if ERROR */
	}

	if (k = (len - l) / BPSEC)		/* see what we still need */
			if ((j = blkrd(fp, buffer + l, k)) NE 0)
			return((k - j) * BPSEC + l);	/* return bytes read */

	l += k * BPSEC;			/* adjust l by what we just read */

	if (l < len)	/* see if we still need a partial sector */
		if (_getsec(fp, buffer + l, len - l))	/* read partial sector */
			return(l);		/* return if ERROR or EOF */

	return(len);				/* return - got the whole thing */
}
