/*
   ============================================================================
	fsinit.c -- Initialize file system
	Version 9 -- 1988-01-31 -- D.N. Lynx Crowe
   ============================================================================
*/

#define	_FS_DEF_	/* define so that stdio.h and io.h get things right */

#include "biosdefs.h"
#include "errno.h"
#include "io.h"
#include "stdio.h"
#include "stddefs.h"

extern	int	_bpbin, _dirin, _fatin, _dirmod, _fatmod;

int	_badfd(), _noper();

char	*Stdbufs;			/* buffer chain pointer */

char	Wrkbuf[BPSEC];			/* sector work buffer */

long	Stdbuf[MAXDFILE][BUFSIZL];	/* standard buffers */

FILE Cbuffs[NSTREAMS];			/* stream file control table */

struct fcb _fcbtab[MAXDFILE];		/* fcb table */

struct channel  chantab[MAXCHAN];	/* channel table:  relates fd's to devices */

#if	TBUFFER

/* WARNING:  this ONLY works for 512 byte sectors, 9 sectors per track */

short	_b_tbuf[9][256];	/* the track buffer */

short	_b_trak;		/* current track */
short	_b_side;		/* current side */
short	_b_sect;		/* current sector */
short	_b_tsec;		/* current base sector of current track */

#endif

/* 
*/

/*
   ============================================================================
	_badfd() -- set "bad fd" error
   ============================================================================
*/

int
_badfd()
{
	errno = EBADF;		/* set bad fd code */
	return(FAILURE);	/* return with an error indication */
}

/*
   ============================================================================
	_noper() -- null return with no error condition
   ============================================================================
*/

int
_noper()
{
	return(SUCCESS);	/* return with a non-error indication */
}

/* 
*/

/*
   ============================================================================
	InitCH() -- Initialize chantab structure entry
   ============================================================================
*/

InitCH(cp, rdi, wri, ioi, ski, cfp, charg)
register struct channel *cp;
char rdi, wri, ioi, ski;
int (*cfp)();
io_arg charg;
{
	cp->c_read  = rdi;
	cp->c_write = wri;
	cp->c_ioctl = ioi;
	cp->c_seek  = ski;
	cp->c_close = cfp;
	cp->c_arg   = charg;
}

/*
   ============================================================================
	Init_CB() -- Initialize Cbuff structure entry
   ============================================================================
*/

Init_CB(fp, flags, unit, bufad, bufsize)
register FILE *fp;
char unit, flags;
long *bufad;
int bufsize;
{
	fp->_bp     = (char *)0L;
	fp->_bend   = (char *)0L;
	fp->_buff   = (char *)bufad;
	fp->_flags  = flags;
	fp->_unit   = unit;
	fp->_bytbuf = 0;
	fp->_buflen = bufsize;
};

/* 
*/

/*
   ============================================================================
	InitFS() -- Initialize file system
   ============================================================================
*/

InitFS()
{
	register int i;

	memset(_fcbtab, 0, sizeof _fcbtab);		/* clear fcb table */
	memsetw(Stdbuf, 0, sizeof Stdbuf / 2);		/* clear buffers */

	Init_CB(stdin,  _BUSY, 0, (char *)0L, BUFSIZ);	/* stdin */
	Init_CB(stdout, _BUSY, 1, (char *)0L, 1);	/* stdout */ 
	Init_CB(stderr, _BUSY, 2, (char *)0L, 1);	/* stderr */

	for (i = 3; i < NSTREAMS; i++)
		Init_CB(&Cbuffs[i], 0, 0, (char *)0L, 0);

	Stdbuf[0][0] = 0L;		/* initialize the buffer list */

	for (i = 1; i < MAXDFILE; i++)
		Stdbuf[i][0] = (long)Stdbuf[i-1];

	Stdbufs = Stdbuf[MAXDFILE-1];

	InitCH(&chantab[0], 2, 0, 1, 0, _noper, (io_arg)CON_DEV );	/*  0 - stdin  */
	InitCH(&chantab[1], 0, 2, 1, 0, _noper, (io_arg)CON_DEV );	/*  1 - stdout */
	InitCH(&chantab[2], 0, 2, 1, 0, _noper, (io_arg)CON_DEV );	/*  2 - stderr */

	for (i = 3; i < MAXCHAN; i++)	/*  3..MAXCHAN-1 - not preassigned */
		InitCH(&chantab[i], 0, 0, 0, 0, _badfd, (io_arg)0L );

	_bpbin  = FALSE;	/* BPB isn't in memory */
	_dirin  = FALSE;	/* directory isn't in memory */
	_fatin  = FALSE;	/* FAT isn't in memory */
	_fatmod = FALSE;	/* FAT hasn't been changed */
	_dirmod = FALSE;	/* directory hasn't been changed */

#if	TBUFFER
	_b_trak = -1;		/* no track in the buffer */
	_b_side = -1;		/* ... */
#endif
}
