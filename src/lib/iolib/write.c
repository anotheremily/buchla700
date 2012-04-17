/*
   ============================================================================
	write.c -- write() and friends
	Version 17 -- 1987-10-27 -- D.N. Lynx Crowe
   ============================================================================
*/

#define	DEBUGIT		0

#include "biosdefs.h"
#include "io.h"
#include "errno.h"
#include "stddefs.h"

int _filewr ();

extern int _badfd (), _conwr (), blkwr ();
extern int ReadRN (), WriteRN ();
extern int _newcls (), micons (), _ptcl12 ();

extern long miconl ();

extern char *memset ();

extern short _fatmod;

extern unsigned _thefat[];

extern struct bpb *_thebpb;

#if DEBUGIT
extern short fsdebug;
#endif

static int (*wr_tab[]) () =
{				/* write routine dispatch table */

  _badfd,			/* 0 - invalid entry */
    _filewr,			/* 1 - disk file */
    _conwr			/* 2 - console device */
};

/* 
*/

/*
   ============================================================================
	_putsec(fp, buf, len) -- write 'len' bytes from 'buf' on file 'fp'
   ============================================================================
*/

int
_putsec (fp, buf, len)
     register struct fcb *fp;
     char *buf;
     unsigned len;
{
#if DEBUGIT
  if (fsdebug)
    printf ("_putsec($%08lX, $%08lx, %d):  initial curlsn=%ld\n",
	    fp, buf, len, fp->curlsn);
#endif

  if ((errno = ReadRN (fp, Wrkbuf)) == 1)
    {				/* try to read sector */

#if DEBUGIT
      if (fsdebug)
	printf ("_putsec():  ReadRN saw EOF at curlsn=%ld, asects=%ld\n",
		fp->curlsn, fp->asects);
#endif

      errno = 0;		/* we're at EOF */
      memset (Wrkbuf, 0x1A, BPSEC);	/* pad end of sector */

    }
  else if (errno)
    return (FAILURE);

  memcpy (Wrkbuf + fp->offset, buf, len);	/* move in the new data */

  if ((errno = WriteRN (fp, Wrkbuf)) != 0)
    {				/* write the sector */

#if DEBUGIT
      if (fsdebug)
	printf ("_putsec():  WriteRN() FAILED (%d) - curlsn=%ld\n",
		errno, fp->curlsn);
#endif

      return (FAILURE);
    }

  if ((fp->offset = (fp->offset + len) & (BPSEC - 1)) == 0)
    {

      ++fp->curlsn;		/* update file position */

      if (_seek (fp) < 0)
	{

#if DEBUGIT
	  if (fsdebug)
	    printf ("_putsec():  _seek() failed - curlsn=%ld, asects=%ld\n",
		    fp->curlsn, fp->asects);
#endif

	  return (FAILURE);
	}
    }

#if DEBUGIT
  if (fsdebug)
    printf ("_putsec():  final curlsn=%ld, offset=%d, len=%d\n",
	    fp->curlsn, fp->offset, len);
#endif

  return (SUCCESS);
}

/* 
*/

/*
   ============================================================================
	_filewr(fp, buffer, len) -- write 'len' bytes on file 'fp'
	from 'buffer'.
   ============================================================================
*/

int
_filewr (fp, buffer, len)
     register struct fcb *fp;
     register char *buffer;
     register unsigned len;
{
  register unsigned j, k, l;
  int clustr;
  register long curpos;

  curpos = fp->offset + (fp->curlsn << FILESHFT);	/* get position */

  if (fp->de.bclust == 0)
    {				/* see if we need to allocate */

#if DEBUGIT
      if (fsdebug)
	if (curpos)
	  printf ("_filewr():  ERROR - bclust == 0 and curpos (%ld) != 0\n",
		  curpos);
#endif

      if (0 == (clustr = _newcls ()))
	{			/* allocate a cluster */

	  errno = EIO;
	  return (len);
	}

      fp->de.bclust = micons (clustr);	/* update FCB */
      _ptcl12 (_thefat, clustr, 0x0FF8);	/* update FAT */
      _fatmod = TRUE;
      fp->curdsn = _cl2lsn (_thebpb, clustr);	/* set curdsn */
      fp->curcls = clustr;
      fp->clsec = 0;
      fp->asects = _thebpb->clsiz;
#if DEBUGIT
      if (fsdebug)
	{

	  printf ("_filewr():  allocated initial cluster=%d, asects=%ld\n",
		  clustr, fp->asects);
	  SnapFCB (fp);
	}
#endif
    }

  l = 0;			/* zero the length-written counter */

#if DEBUGIT
  if (fsdebug)
    printf ("_filewr():  init pos=%ld, len=%u, curcls=%u, offset=%u\n",
	    curpos, len, fp->curcls, fp->offset);
#endif

  if (fp->offset)
    {				/* see if we have a partial sector to fill */

      if ((l = (BPSEC - fp->offset)) > len)
	l = len;

      if (_putsec (fp, buffer, l))	/* fill up the sector */
	return (-1);
    }

  if (k = (len - l) / BPSEC)
    {				/* write out any full sectors */

      if ((j = blkwr (fp, buffer + l, k)) != 0)
	{

	  l += (k - j) * BPSEC;	/* update amount written */

	  if ((curpos + l) > fp->curlen)	/* udpate file length */
	    fp->de.flen = miconl (fp->curlen = curpos + l);

#if DEBUGIT
	  if (fsdebug)
	    printf
	      ("_filewr():  ERROR - curlen=%ld, curlsn=%ld, curdsn=%ld\n",
	       fp->curlen, fp->curlsn, fp->curdsn);
#endif

	  return (l);
	}

      l += k * BPSEC;		/* update amount written */
    }
/* 
*/
  if (l < len)
    {				/* write out partial sector at end */

      if (_putsec (fp, buffer + l, len - l))
	{

	  if ((curpos + l) > fp->curlen)	/* update file length */
	    fp->de.flen = miconl (fp->curlen = curpos + l);

#if DEBUGIT
	  if (fsdebug)
	    printf
	      ("_filewr():  ERROR - curlen=%ld, curlsn=%ld, curdsn=%ld\n",
	       fp->curlen, fp->curlsn, fp->curdsn);
#endif

	  return (l);
	}
    }

  if ((curpos + len) > fp->curlen)	/* update file length */
    fp->de.flen = miconl (fp->curlen = curpos + len);

#if DEBUGIT
  if (fsdebug)
    printf
      ("_filewr():  final curlen=%ld, flen=$%08lX, curlsn=%ld, curdsn=%ld\n",
       fp->curlen, fp->de.flen, fp->curlsn, fp->curdsn);
#endif

  return (len);
}

/* 
*/

/*
   ============================================================================
	write(fd, buff, len) -- write 'len' bytes from 'buff' on file 'fd'
   ============================================================================
*/

int
write (fd, buff, len)
     int fd;
     char *buff;
     unsigned len;
{
  register struct channel *chp;

  if ((fd < 0) || (fd > MAXCHAN))
    {

      errno = EBADF;
      return (-1);
    }

  chp = &chantab[fd];

  return ((*wr_tab[chp->c_write]) (chp->c_arg, buff, len));
}
