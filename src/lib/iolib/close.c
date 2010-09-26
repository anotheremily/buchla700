/*
   =============================================================================
	close.c -- close a file for the Buchla 700 C I/O Library
	Version 9 -- 1987-11-13 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "biosdefs.h"
#include "errno.h"
#include "fcntl.h"
#include "io.h"
#include "stddefs.h"

extern	int	_badfd(), ClsFile(), _clsvol();

extern	int	_fatmod, _dirmod;
extern	struct	bpb	*_thebpb;
extern	struct	dirent	_thedir[];
extern	unsigned	_thefat[];

/* 
*/

/*
   =============================================================================
	_clsfat() -- write out the modified FAT
   =============================================================================
*/

_clsfat()
{
	/* write the primary FAT to disk */

	BIOS(B_RDWR, 1, _thefat, _thebpb->fsiz,
		_thebpb->fatrec, 0);

	/* write the secondary FAT to disk */

	BIOS(B_RDWR, 1, _thefat, _thebpb->fsiz,
		(_thebpb->fatrec - _thebpb->fsiz), 0);

	_fatmod = FALSE;	/* FAT on disk now matches memory */
}

/*
   =============================================================================
	_clsdir() -- write out the modified directory
   =============================================================================
*/

_clsdir()
{
	/* write the directory to disk */

	BIOS(B_RDWR, 1, _thedir, _thebpb->rdlen,
		(_thebpb->fatrec + _thebpb->fsiz), 0);

	_dirmod = FALSE;	/* directory on disk now matches memory */
}

/* 
*/

/*
   =============================================================================
	close(fd) -- close file 'fd'
   =============================================================================
*/

int
close(fd)
int fd;
{
	register struct channel *chp;
	register int rc;

	if ((fd < 0) OR (fd > MAXCHAN)) {

		errno = EBADF;
		return(FAILURE);
	}

	chp = &chantab[fd];			/* point at the channel */

	rc  = (*chp->c_close)(chp->c_arg);	/* close the FCB */

	chp->c_read  = 0;			/* release the channel */
	chp->c_write = 0;
	chp->c_ioctl = 0;
	chp->c_seek  = 0;
	chp->c_close = _badfd;

	if (_fatmod)
		_clsfat();			/* write modified FAT */

	if (_dirmod)
		_clsdir();			/* write modified directory */

	return(rc);				/* return result of close */
}

/* 
*/

/*
   =============================================================================
	_filecl(fp) -- close file 'fp' at the BDOS level
   =============================================================================
*/

int
_filecl(fp)
register struct fcb *fp;
{
	register int rc;

	rc = ClsFile(fp);		/* close the FCB */
	fp->modefl = 0;		/* mark the FILE closed */
	return(rc);
}

/*
   =============================================================================
	_fd_cls() -- close all open files
   =============================================================================
*/

_fd_cls()
{
	register int fd;

	for (fd = 0; fd < MAXCHAN; ++fd)
		if (chantab[fd].c_close NE _badfd)
			close(fd);

	_clsvol();			/* write modified directory adn FAT */
}
