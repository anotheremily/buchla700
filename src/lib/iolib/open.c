/*
   =============================================================================
	open.c -- open a file for the Buchla 700 C I/O library functions
	Version 11 -- 1988-01-31 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "biosdefs.h"
#include "errno.h"
#include "fcntl.h"
#include "io.h"
#include "stddefs.h"

extern	int	_badfd(), _noper();
extern	int	_inifcb(), _opfcb();

extern	char	*FilName(), *FilExt();

int	_fileop(), _filecl();

static struct device condev  = { 2, 2, 1, 0, _noper  };
static struct device filedev = { 1, 1, 0, 1, _fileop };

extern	struct fcb _fcbtab[];

/*
   =============================================================================
	device table:  contains names and pointers to device control structures
   =============================================================================
*/

static struct devtabl devtabl[] = {

	{ "con:", &condev,  2 },	/* console device */
	{ "CON:", &condev,  2 },

	{      0, &filedev, 0 }		/* this MUST be the last entry */
};

/* 
*/

/*
   =============================================================================
	open(name, flag, mode) -- Opens file 'name' with flags 'flag'
	and access mode 'mode'.  File will be ASCII unless opened with O_RAW.
	Returns a file descriptor (small positive integer) if successful, or
	FAILURE (-1) if an error occurred.
   =============================================================================
*/

int
open(name, flag, mode)
char *name;
int flag, mode;
{
	register struct devtabl	*dp;
	register struct channel	*chp;
	register struct device	*dev;
	int	fd, mdmask;

	/* search for a free channel */

	for (chp = chantab, fd = 0 ; fd < MAXCHAN ; ++chp, ++fd)
		if (chp->c_close EQ _badfd)
			goto fndchan;

	errno = EMFILE;		/* no channels available */
	return(FAILURE);

/* 
*/

fndchan:	/* found a channel to use */

	for (dp = devtabl; dp->d_name; ++dp)	 /* search for the device */
		if (strcmp(dp->d_name, name) EQ 0)
			break;

	dev = dp->d_dev;
	mdmask = (flag & 3) + 1;

	if (mdmask & 1) {	/* see if device is readable */

		if ((chp->c_read = dev->d_read) EQ 0) {

			errno = EACCES;		/* can't read */
			return(FAILURE);
		}
	}

	if (mdmask & 2) {	/* see if device is writeable */

		if ((chp->c_write = dev->d_write) EQ 0) {

			errno = EACCES;		/* can't write */
			return(FAILURE);
		}
	}

	/* setup the channel table entries */

	chp->c_arg   = dp->d_arg;
	chp->c_ioctl = dev->d_ioctl;
	chp->c_seek  = dev->d_seek;
	chp->c_close = _noper;

	if ((*dev->d_open)(name, flag, mode, chp, dp) < 0) {	/* open */

		chp->c_close = _badfd;	/* couldn't open for some reason */
		return(FAILURE);
	}

	return(fd);
}

/* 
*/

/*
   =============================================================================
	opena(name, flag, mode) -- Opens ASCII file 'name' with flags 'flag'
	and access mode 'mode'.  Newline translation will be done.
	Returns a file descriptor (small positive integer) if successful, or
	FAILURE (-1) if an error occurred.
   =============================================================================
*/

int
opena(name, flag, mode)
char *name;
int flag, mode;
{
	return(open(name, flag, mode));
}

/*
   =============================================================================
	openb(name, flag, mode) -- Opens binary file 'name' with flags 'flag'
	and access mode 'mode'.  No newline translation is done.
	Returns a file descriptor (small positive integer) if successful, or
	FAILURE (-1) if an error occurred.
   =============================================================================
*/

int
openb(name, flag, mode)
char *name;
int flag, mode;
{
	return(open(name, flag|O_RAW, mode));
}

/* 
*/

/*
   =============================================================================
	creat(name, mode) -- Creates file 'name' with access mode 'mode'.
	The created file is initially open for writing only.  The file
	will be ASCII unless mode contains O_RAW.
	Returns a file descriptor (small positive integer) if successful, or
	FAILURE (-1) if an error occurred.
   =============================================================================
*/

int
creat(name, mode)
char *name;
int mode;
{
	return(open(name, O_WRONLY|O_TRUNC|O_CREAT, mode));
}

/* 
*/

/*
   =============================================================================
	creata(name, mode) -- Creates ASCII file 'name' with access mode 'mode'.
	The created file is initially open for writing only.
	Files created with creata() do newline translations.
	Returns a file descriptor (small positive integer) if successful, or
	FAILURE (-1) if an error occurred.
   =============================================================================
*/

int
creata(name, mode)
char *name;
int mode;
{
	return(open(name, O_WRONLY|O_TRUNC|O_CREAT, mode));
}

/*
   =============================================================================
	creatb(name, mode) -- create binary file 'name' with access mode 'mode'.
	The created file is initially open for writing only.
	Files created with creatb don't do newline translations.
	Returns a file descriptor (small positive integer) if successful, or
	FAILURE (-1) if an error occurred.
   =============================================================================
*/

int
creatb(name, mode)
char *name;
int mode;
{
	return(creat(name, O_WRONLY|O_TRUNC|O_CREAT|O_RAW, mode));
}

/* 
*/

/*
   =============================================================================
	_fileop(name, flag, mode, chp, dp) -- Opens disk file 'name' with
	flags 'flag' in mode 'mode' with channel pointer 'chp' and
	device pointer 'dp'.  Returns SUCCESS (0) or FAILURE (-1).
   =============================================================================
*/

int
_fileop(name, flag, mode, chp, dp)
char *name;
int flag, mode;
struct channel *chp;
struct devtabl *dp;
{
	register struct fcb *fp;
	char	tmpname[9], tmpext[4];

	/* search for an available fcb entry */

	for (fp = _fcbtab; fp < (_fcbtab + MAXDFILE); ++fp)
		if (fp->modefl EQ 0)
			goto havefcb;

	errno = ENFILE;		/* no fcb space available for file */
	return (FAILURE);

havefcb:

	/* setup the initial fcb */

	if (_inifcb(fp, FilName(name, tmpname), FilExt(name, tmpext), flag)) {

		errno = EINVAL;		/* bad file name or flags */
		return(FAILURE);
	}

	if (_opfcb(fp))			/* open the file */
		return(FAILURE);

	chp->c_arg   = fp;		/* allocate the channel */
	chp->c_close = _filecl;

	return(SUCCESS);
}

