/*
   =============================================================================
	blkrd.c -- read a block of 0..32767 sectors
	Version 10 -- 1988-01-08 -- D.N. Lynx Crowe

	int
	blkrd(fcp, buf, ns)
	struct fcb *fcp;
	char *buf;
	int ns;

		Reads 'ns' sectors from file 'fcp' into 'buf'.
		Returns the number of unread sectors, or 0 if all were read.

	long
	_secrd(buf, rec)
	register char *buf;
	register short rec;

		Reads a logical sector via the track buffer.
		Functionally equivalent to the BIOS B_RDWR read function
		with the addition of transparent write-thru track buffering.
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "biosdefs.h"
#include "errno.h"
#include "errdefs.h"
#include "fspars.h"

/* 
*/

#if	DEBUGIT
extern short fsdebug;		/* file system debug switch */
#endif

extern int _nsic ();		/* next sector function */

extern long _berrno;		/* last file system bios error number */

extern short _thefat[];		/* current file allocation table */

extern struct bpb *_thebpb;	/* current bios parameter block */

#if	TBUFFER

/* WARNING:  this ONLY works for 512 byte sectors, 9 sectors per track */

extern short _b_tbuf[9][256];	/* the track buffer */

extern short _b_trak;		/* current track */
extern short _b_side;		/* current side */
extern short _b_sect;		/* current sector */
extern short _b_tsec;		/* current base sector of current track */

#endif

/* 
*/

#if	TBUFFER

/*
   =============================================================================
	_secrd(buf, rec) -- read a logical sector via the track buffer
   =============================================================================
*/

long
_secrd (buf, rec)
     register char *buf;
     register short rec;
{
  register short track, side, sector;
  long brc;

  if (_thebpb->dspt NE 9)	/* make sure we can do this */
    return (ERR07);

  if (_thebpb->recsiz NE 512)
    return (ERR07);

  track = rec / _thebpb->dspc;	/* desired track */
  _b_tsec = track * _thebpb->dspc;	/* base sector of track */
  sector = rec - _b_tsec;	/* logical sector in cylinder */

  if (sector GE _thebpb->dspt)
    {				/* adjust sector and side */

      sector -= _thebpb->dspt;	/* sector now in track */
      side = 1;
      _b_tsec += _thebpb->dspt;

    }
  else
    {

      side = 0;
    }

#if	DEBUGIT
  if (fsdebug)
    printf
      ("_secrd($%08.8LX, %d):  track=%d, side=%d, sector=%d, _b_tsec=%d\n",
       buf, rec, track, side, sector, _b_tsec);
#endif

  if ((track NE _b_trak) OR (side NE _b_side))
    {				/* track in buffer ? */

      if (brc = BIOS (B_RDWR, 0, &_b_tbuf, _thebpb->dspt, _b_tsec, 0))
	{

	  _b_trak = -1;
	  _b_side = -1;
	  return (brc);
	}

      _b_trak = track;
      _b_side = side;
    }

  memcpy (buf, (char *) _b_tbuf[sector], 512);
  return (0L);
}

#endif

/* 
*/

/*
   =============================================================================
	blkrd(fcp, buf, ns) -- read 'ns' sectors from file 'fcp' into 'buf'.
	Returns the number of unread sectors, or 0 if all were read.
   =============================================================================
*/

int
blkrd (fcp, buf, ns)
     register struct fcb *fcp;
     register char *buf;
     register int ns;
{
  register long brc;		/* bios return code */
  register int rb;		/* _nsic return code */

  if (ns < 0)			/* can't read a negative number of sectors */
    return (ns);

  while (ns--)
    {				/* read a sector at a time */

#if	DEBUGIT
      if (fsdebug)
	printf
	  ("_blkrd():  ns=%d, buf=$%08.8lX, curlsn=%ld, curdsn=%ld, offset=%u\n",
	   ns, buf, fcp->curlsn, fcp->curdsn, fcp->offset);
#endif

#if	TBUFFER
      if (brc = _secrd (buf, (short) fcp->curdsn))
	{
#else
      if (brc = BIOS (B_RDWR, 0, buf, 1, (short) fcp->curdsn, 0))
	{
#endif

	  _berrno = brc;	/* log the error */
	  errno = EIO;
	  return (ns);		/* return unread sector count */
	}

      if (rb = _nsic (fcp, _thebpb, _thefat))
	{			/* find next sector */

	  if (rb EQ - 1)	/* see if we had an error */
	    errno = EIO;	/* set error number */

	  return (ns);		/* return unread sector count */
	}

      buf += _thebpb->recsiz;	/* advance buffer pointer */
    }

  return (0);			/* return -- all sectors read */
}
