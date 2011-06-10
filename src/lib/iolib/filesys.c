/*
   =============================================================================
	filesys.c -- file system support functions
	Version 44 -- 1987-11-15 -- D.N. Lynx Crowe

	These functions support a {MS,GEM}DOS|TOS-like file system.
	Only 12 bit FAT entries are supported.
	Subdirectories are not supported in this version.
   =============================================================================
*/

#define	DEBUGIT		0

#define	_FS_DEF_		/* to avoid unnecessary externals */

#include "stddefs.h"
#include "biosdefs.h"
#include "errno.h"
#include "memory.h"
#include "io.h"
#include "fcntl.h"

#define	FAT_EOF		0x0FF8

#define	IO_ERR		-1
#define	IO_OK		0
#define	IO_EOF		1

#define	DE_NULL		((struct dirent *)0L)

extern unsigned micons ();
extern long miconl ();
extern int _ptcl12 ();
extern char *FilName (), *FilExt ();

#if DEBUGIT
short fsdebug;
extern int mdump ();
#endif

struct bpb *_thebpb;		/* BIOS parameter block pointer */
struct dirent _thedir[MAXDIRNT];	/* directory */
struct dirent *_dptr;		/* internal directory pointer */

unsigned _thefat[MAXFAT * WDPSEC];	/* file allocation table */

int _fatin;			/* FAT has been read */
int _dirin;			/* directory has been read */
int _bpbin;			/* BPB has been read */
int _fatmod;			/* FAT modified flag */
int _dirmod;			/* directory modified flag */

long _berrno;			/* BIOS error number */

/* 
*/


/*
   =============================================================================
	_cl2lsn(bpp, clnum) -- return logical sector number for cluster 'clnum'
	using BPB pointed to by 'bpp'
   =============================================================================
*/

unsigned
_cl2lsn (bpp, clnum)
     struct bpb *bpp;
     unsigned clnum;
{
  return (bpp->datrec + (bpp->clsiz * (clnum - 2)));
}

/*
   =============================================================================
	_gtcl12(fat, cl) -- return 12 bit cluster entry 'cl' from FAT pointed
	to by 'fat'
   =============================================================================
*/

unsigned
_gtcl12 (fat, cl)
     register char *fat;
     unsigned cl;
{
  register unsigned cla, clt;

  cla = cl + (cl >> 1);
  clt = ((unsigned) 0xFF00 & (fat[cla + 1] << 8))
    | ((unsigned) 0x00FF & fat[cla]);

  if (cl & 1)
    clt >>= 4;

  clt &= (unsigned) 0x0FFF;
  return (clt);
}

/* 
*/

/*
   =============================================================================
	_getfat(bufad, bpp, nfat) -- get FAT 'nfat' into buffer
	pointed to by 'bufad' using BPB pointed to by 'bpp'
   =============================================================================
*/

unsigned
_getfat (bufad, bpp, nfat)
     unsigned *bufad, nfat;
     register struct bpb *bpp;
{
  unsigned fatsec;

  fatsec = nfat ? bpp->fatrec : (bpp->fatrec - bpp->fsiz);
  return (BIOS (B_RDWR, 0, (char *) bufad, bpp->fsiz, fatsec, 0));
}

/* 
*/

/*
   =============================================================================
	_rdfat(bufad, bpp) -- read FAT into buffer pointed to by 'bufad'
	using BPB pointed to by 'bpp'
   =============================================================================
*/

int
_rdfat (bufad, bpp)
     unsigned *bufad;
     struct bpb *bpp;
{
  if (_getfat (bufad, bpp, 0))
    {

      if (_getfat (bufad, bpp, 1))
	return (IO_ERR);	/* couldn't read either FAT */
      else
	return (1);		/* secondary FAT read OK */

    }
  else
    return (0);			/* main FAT read OK */
}

/*
   =============================================================================
	_rdroot(buf, bpp) -- read root directory into buffer
	pointed to by 'buf' using BPB pointed to by 'bpp'
   =============================================================================
*/

int
_rdroot (buf, bpp)
     unsigned *buf;
     register struct bpb *bpp;
{
  return (BIOS (B_RDWR, 0, (char *) buf, bpp->rdlen,
		(bpp->fatrec + bpp->fsiz), 0));
}

/* 
*/

/*
   =============================================================================
	_nsic(fcp, bpp, fp) -- get the next sector in the file controlled
	by the FCB at 'fcp', using the BPB pointed to by 'bpp' and
	the FAT pointed to by 'fp'.  Returns 0 if successful, or -1 on error,
	or 1 on EOF;
   =============================================================================
*/

int
_nsic (fcp, bpp, fp)
     register struct fcb *fcp;
     struct bpb *bpp;
     char *fp;
{
  register unsigned tfe;

  /* check the FCB flags */

  if (!(fcp->modefl & FC_OPN))
    {

      errno = EINVAL;		/* not open */
      return (IO_ERR);
    }

  if (fcp->modefl & FC_BAD)
    {

      errno = EIO;		/* previous I/O error */
      return (IO_ERR);
    }

  if (fcp->de.bclust EQ 0)
    {				/* no sectors allocated  (empty file) */

      fcp->modefl |= FC_EOF;
      return (IO_EOF);
    }
/* 
*/
  /* update clsec and curlsn and see if we need a new cluster */

  ++fcp->curlsn;

  if (++fcp->clsec GE bpp->clsiz)
    {

      /* new cluster needed -- follow the chain */

      fcp->clsec = 0;
      tfe = _gtcl12 (fp, fcp->curcls);

      if (tfe < 0x0FF0)
	{			/* see if we've got a good cluster */


	  fcp->curcls = tfe;	/* good cluster, update fcb */
	  fcp->curdsn = _cl2lsn (bpp, tfe);

#if DEBUGIT
	  if (fsdebug)
	    printf ("_nsic():  NXT curlsn=%ld, curcls=%u, curdsn=%ld\n",
		    fcp->curlsn, fcp->curcls, fcp->curdsn);
#endif

	  return (IO_OK);
	}

      if (tfe < 0x0FF8)
	{			/* see if it's a bad cluster */

	  fcp->modefl |= (FC_BAD | FC_EOF);	/* bad one */

#if DEBUGIT
	  if (fsdebug)
	    printf ("_nsic():  IO_ERR - curlsn=%ld, curcls=%u, curdsn=%ld\n",
		    fcp->curlsn, fcp->curcls, fcp->curdsn);
#endif

	  return (IO_ERR);
	}

      fcp->modefl |= FC_EOF;	/* EOF hit -- set EOF in FCB */
      return (IO_EOF);

/* 
*/

    }
  else
    {

      ++fcp->curdsn;		/* advance to next sector in current cluster */

#if DEBUGIT
      if (fsdebug)
	printf ("_nsic():  CUR curlsn=%ld, curcls=%u, curdsn=%ld\n",
		fcp->curlsn, fcp->curcls, fcp->curdsn);
#endif

      return (IO_OK);
    }
}

/* 
*/

/*
   =============================================================================
	_dsrch(de) -- search for the directory entry pointed to by 'de'
	in the current directory
   =============================================================================
*/

struct dirent *
_dsrch (de)
     register struct dirent *de;
{
  unsigned i, dl;
  register struct dirent *dp;

  dp = _thedir;
  dl = _thebpb->rdlen * (_thebpb->recsiz / DENTSIZE);

  for (i = 0; i < dl; i++)
    {				/* check each entry */

      /* see if we've found it */

      if (0 EQ memcmpu (de->fname, dp->fname, 11))
	return (dp);

      /* check for end of used entries */

      if ('\0' EQ dp->fname[0])
	return (DE_NULL);

      dp++;			/* point at next entry to check */
    }

  return (DE_NULL);
}

/* 
*/

/*
   =============================================================================
	_dsnew() -- search for an available directory entry in the current
	directory.
   =============================================================================
*/

struct dirent *
_dsnew ()
{
  unsigned i, dl;
  register struct dirent *dp;

  dp = _thedir;
  dl = _thebpb->rdlen * (_thebpb->recsiz / DENTSIZE);

  for (i = 0; i < dl; i++)
    {				/* check each entry */

      /* check for a deleted entry */

      if (0x00E5 EQ (0x00FF & dp->fname[0]))
	return (dp);

      /* check for end of used entries */

      if ('\0' EQ dp->fname[0])
	return (dp);

      dp++;			/* point at next entry to check */
    }

  return (DE_NULL);		/* couldn't find an available entry */
}

/* 
*/

/*
   =============================================================================
	_deadio(fcp, err) -- mark the FCB pointed to by 'fcp' as 'in error'
   =============================================================================
*/

_deadio (fcp, err)
     register struct fcb *fcp;
     int err;
{
  fcp->clsec = 0;
  fcp->curcls = 0;
  fcp->curlsn = 0L;
  fcp->curdsn = 0L;
  fcp->curlen = 0L;
  fcp->modefl &= ~FC_OPN;
  fcp->modefl |= FC_ERR;
  errno = err;
}

/* 
*/

/*
   =============================================================================
	_seek(fcp) -- seek to logical sector number in FCB pointed to by 'fcp'.
	Returns:  -1 = error; 0 = not at EOF; 1 = soft EOF; 2 = hard EOF.
   =============================================================================
*/

int
_seek (fcp)
     register struct fcb *fcp;
{
  register unsigned acls, rcls, nc;
  unsigned sic, spc;

  if (!(fcp->modefl & FC_OPN))
    {				/* file must be open */

      errno = EINVAL;		/* invalid request */
      return (-1);		/* return with an error */
    }

  if (fcp->curlsn GT fcp->asects)
    {				/* error if seek past EOF */

#if	DEBUGIT
      if (fsdebug)
	printf ("_seek():  ERROR - seek past EOF - curlsn=%ld, asects=%ld\n",
		fcp->curlsn, fcp->asects);
#endif

      errno = EINVAL;		/* invalid request */
      return (-1);		/* return with an error */
    }

  spc = _thebpb->clsiz;		/* get sectors per cluster */
  rcls = fcp->curlsn / spc;	/* calculate relative cluster */
  sic = fcp->curlsn - (rcls * spc);	/* calculate sector in cluster */
  acls = micons (fcp->de.bclust);	/* get first cluster */
  fcp->modefl &= ~FC_EOF;	/* turn off the EOF flag */

/* 
*/

#if DEBUGIT
  if (fsdebug)
    {
      printf
	("_seek():  ENTRY curlsn=%ld, acls=%u, sic=%d, curdsn=%ld, rcls=%u, asects=%ld\n",
	 fcp->curlsn, acls, sic, fcp->curdsn, rcls, fcp->asects);
    }
#endif

  if (fcp->curlsn EQ fcp->asects)
    {				/* see if we seeked to EOF */

      fcp->modefl |= FC_EOF;	/* flag the EOF condition */

#if DEBUGIT
      if (fsdebug)
	printf ("_seek():  EOF curlsn=%ld, acls=%u, sic=%d, curdsn=%ld\n",
		fcp->curlsn, acls, sic, fcp->curdsn);
#endif

      if (acls EQ 0)		/* see if anything allocated */
	return (2);		/* hard EOF if nothing there */

      if (sic EQ 0)		/* see if first in cluster */
	return (2);		/* hard EOF if so */
    }

/* 
*/
  for (nc = 0; nc < rcls; nc++)
    {				/* follow the FAT chain */

      acls = _gtcl12 (_thefat, acls);	/* next cluster pointer */

      if (acls LT 0x0FF0)	/* OK if it's a good cluster */
	continue;

      fcp->modefl |= FC_ERR;	/* EOF or bad cluster hit */
      errno = EINVAL;		/* mark it as an invalid seek */

      if (acls LT 0x0FF8)
	{			/* bad cluster ? */

	  errno = EIO;		/* mark it as an I/O error */
	  fcp->modefl |= FC_BAD;	/* mark FCB:  bad cluster hit */
	}

      return (IO_ERR);		/* ERROR:  seek error */
    }

  fcp->curcls = acls;		/* current cluster */
  fcp->clsec = sic;		/* sector in cluster */

  fcp->curdsn = _cl2lsn (_thebpb, acls) + sic;	/* disk sector number */

#if DEBUGIT
  if (fsdebug)
    printf ("_seek():  OK curlsn=%ld, acls=%u, sic=%d, curdsn=%ld\n",
	    fcp->curlsn, acls, sic, fcp->curdsn);
#endif

  if (fcp->curlsn EQ fcp->asects)	/* see if we're at EOF */
    return (1);			/* soft EOF */

  return (0);			/* not EOF */
}

/* 
*/

/*
   =============================================================================
	_ftrnc(dp) -- truncate file pointed to by directory entry 'dp'
   =============================================================================
*/

int
_ftrnc (dp)
     struct dirent *dp;
{
  register unsigned acls, ncls;

#if	DEBUGIT
  register int lim;

  lim = _thebpb->numcl;
#endif

  acls = micons (dp->bclust);	/* get first cluster number */

  /* zap entries until EOF or bad cluster */

#if	DEBUGIT
  while (lim-- AND (acls LT 0x0FF0))
    {
#else
  while (acls LT 0x0FF0)
    {
#endif

      ncls = _gtcl12 (_thefat, acls);	/* get next cluster number */
      _ptcl12 (_thefat, acls, 0);	/* zap the current one */

#if	DEBUGIT
      if (fsdebug)
	printf ("_ftrnc():  zapped %d\n", acls);
#endif

      acls = ncls;		/* point at the next one */
    }

/* 
*/

  dp->flen = 0L;		/* file length = 0 */
  dp->bclust = 0;		/* no sectors allocated */
  _fatmod = TRUE;		/* FAT got changed */
  _dirmod = TRUE;		/* directory got changed */

#if	DEBUGIT
  if (lim LE 0)
    {

      errno = EIO;
      printf ("_ftrnc():  ERROR - lim went to %d\n", lim);
      mdump (_thefat, (char *) _thefat + 255, _thefat);
      xtrap15 ();
      return (FAILURE);
    }
#endif

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	_newcls() -- allocate a new cluster
   =============================================================================
*/

int
_newcls ()
{
  register int tc, i;

  tc = _thebpb->numcl;

  for (i = 2; i < tc; i++)
    if (0 EQ _gtcl12 (_thefat, i))
      return (i);

  return (0);			/* ERROR:  no empty clusters left */
}

/* 
*/

/*
   =============================================================================
	_alcnew() -- allocate a new cluster to extend a file
   =============================================================================
*/

int
_alcnew (fcp)
     register struct fcb *fcp;
{
  register int ac, nc, pc;

#if	DEBUGIT
  register int lim;
#endif

  if (nc = _newcls ())
    {				/* get a new cluster */

#if	DEBUGIT
      lim = (fcp->asects / _thebpb->clsiz) + 1;	/* limit search */
#endif

      _ptcl12 (_thefat, nc, FAT_EOF);	/* mark new cluster as EOF */
      pc = micons (fcp->de.bclust);	/* get first cluster */

#if	DEBUGIT
      if (pc EQ 0)
	{

	  printf ("_alcnew():  ERROR - pc EQ 0, bclust=$%04.4X, nc=%d\n",
		  fcp->de.bclust, nc);
	  xtrap15 ();
	}
#endif

      /* find end of allocation chain */

#if	DEBUGIT
      while (lim-- AND ((ac = _gtcl12 (_thefat, pc)) < 0x0FF0))
	{
#else
      while ((ac = _gtcl12 (_thefat, pc)) < 0x0FF0)
	{
#endif

	  pc = ac;
	}

/* 
*/

#if	DEBUGIT
      if (lim LE 0)
	{

	  printf ("_alcnew():  ERROR - lim ran out, nc=%d\n", nc);
	  mdump (_thefat, (char *) _thefat + 63, _thefat);
	  xtrap15 ();
	  return (FAILURE);
	}
#endif

      _ptcl12 (_thefat, pc, nc);	/* add cluster to chain */
      fcp->asects += _thebpb->clsiz;	/* update allocated sector count */
      fcp->curcls = nc;		/* current cluster = new cluster */
      fcp->clsec = 0;		/* current sector in cluster = 0 */

      fcp->curdsn = _cl2lsn (_thebpb, nc);	/* current disk sector */

#if DEBUGIT
      if (fsdebug)
	{

	  printf ("alcnew():  allocated cluster %d (%04.4x), ", nc, nc);
	  printf ("previous = %d (%04.04x), asects =%ld\n",
		  pc, pc, fcp->asects);
	}
#endif

      return (SUCCESS);		/* return:  file extended */

    }
  else
    {

#if DEBUGIT
      if (fsdebug)
	printf ("_alcnew():  _newcls() FAILED\n");
#endif

      return (FAILURE);		/* return:  no clusters left */
    }
}

/* 
*/

/*
   =============================================================================
	_fmake(fcp) -- make a new file from the FCB pointed to by 'fcp'
   =============================================================================
*/

int
_fmake (fcp)
     struct fcb *fcp;
{
  register struct dirent *dp;

  if (DE_NULL EQ (dp = _dsnew ()))	/* find a dirent */
    return (FAILURE);

  _dptr = dp;			/* set the directory pointer */
  memset (dp, 0, DENTSIZE);	/* clear the dirent */
  memcpy (dp->fname, fcp->de.fname, 12);	/* setup file name, attribute */
  dp->atrib |= F_ARCHIV;	/* set the archive bit */
  _dirmod = TRUE;		/* directory got changed */
  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	_opnvol() -- open a volume
   =============================================================================
*/

int
_opnvol ()
{
  register long drc;

  /* check for media change if we already have a BPB */

  if (_bpbin)
    {

      drc = BIOS (B_MCHG, 0);

#if DEBUGIT
      if (fsdebug)
	{

	  printf ("_opnvol():  found _bpbin FALSE\n");
	  printf ("_opnvol():  disk change test returned %ld\n", drc);
	}
#endif

      if (drc)
	_bpbin = FALSE;
    }
  else
    {

#if DEBUGIT
      if (fsdebug)
	printf ("_opnvol():  found _bpbin FALSE\n");
#endif

    }

  if (!_bpbin)
    {				/* access the BPB */

      _fatin = FALSE;		/* invalidate the FAT and directory */
      _dirin = FALSE;

      if ((struct bpb *) 0L EQ (_thebpb = BIOS (B_GBPB, 0)))
	{

	  errno = EIO;		/* couldn't read the BPB */
	  return (FAILURE);
	}

      _bpbin = TRUE;
    }

  if (!_fatin)
    {				/* access the FAT */

      _dirin = FALSE;

      if (_rdfat (_thefat, _thebpb) < 0)
	{

	  errno = EIO;		/* couldn't read either FAT */
	  return (FAILURE);
	}

      _fatin = TRUE;
      _fatmod = FALSE;
    }
/* 
*/
  if (!_dirin)
    {				/* access the root directory */

      if (_rdroot (_thedir, _thebpb))
	{

	  errno = EIO;		/* couldn't read the directory */
	  return (FAILURE);
	}

      _dirin = TRUE;
      _dirmod = FALSE;
    }

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	_opfcb(fcp) -- open the FCB pointed to by 'fcp'.  Returns 0 if
	opened successfully, -1 if an error occurred, with the error code
	in errno.
   =============================================================================
*/

int
_opfcb (fcp)
     register struct fcb *fcp;
{
  int rc;

  /* check for an already open FCB */

  if (fcp->modefl & FC_OPN)
    {

      errno = EINVAL;
      return (IO_ERR);
    }

  if (_opnvol ())
    {				/* open the volume */

      _deadio (fcp, errno);
      return (IO_ERR);
    }

  /* search the directory for the file */

  if (DE_NULL EQ (_dptr = _dsrch (&fcp->de)))
    {

      /* file doesn't exist -- see if we should create it */

      if (!(fcp->modefl & FC_CR))
	{

	  _deadio (fcp, ENOENT);	/* file nonexistant, not creating one */
	  return (IO_ERR);
	}

      /* create the file */

      if (rc = _fmake (fcp))
	{

	  _deadio (fcp, EIO);	/* couldn't create the file */
	  return (IO_ERR);
	}

/* 
*/

    }
  else
    {

      /* file exists -- check for writes to a read-only file */

      if ((fcp->de.atrib & F_RDONLY) AND
	  (fcp->modefl & (FC_AP | FC_WR | FC_TR)))
	{

	  /* can't append, write or truncate RO files */

	  _deadio (fcp, EACCES);
	  return (IO_ERR);
	}

      /* check for append and not write */

      if ((fcp->modefl & FC_AP) AND ! (fcp->modefl & FC_WR))
	{

	  /* can't append if not open to write */

	  _deadio (fcp, EACCES);
	  return (IO_ERR);
	}

      /* check for create and exclusive with extant file */

      if ((fcp->modefl & FC_CR) AND (fcp->modefl & FC_EX))
	{

	  /* file exists and open was create-exclusive */

	  _deadio (fcp, EEXIST);
	  return (IO_ERR);
	}

/* 
*/

      /* truncate the file if requested */

      if (fcp->modefl & FC_TR)
	{

	  /* see if we can write */

	  if (fcp->modefl & FC_WR)
	    {

	      /* truncate the file */

	      if (_ftrnc (_dptr))
		{

		  _deadio (fcp, EIO);	/* truncation failed */
		  return (IO_ERR);
		}

	    }
	  else
	    {

	      /* we can't write, so we can't truncate */

	      _deadio (fcp, EACCES);
	      return (IO_ERR);
	    }
	}
    }

  /* setup the FCB fields */

  memcpy (&fcp->de, _dptr, DENTSIZE);
  fcp->clsec = 0;
  fcp->curcls = micons (fcp->de.bclust);
  fcp->offset = 0;
  fcp->curlsn = 0L;
  fcp->curdsn = fcp->de.bclust ? _cl2lsn (_thebpb, fcp->curcls) : 0;
  fcp->curlen = miconl (fcp->de.flen);

  fcp->asects = ((fcp->curlen / _thebpb->clsizb)
		 + ((fcp->curlen & (_thebpb->clsizb - 1)) ? 1 : 0))
    * _thebpb->clsiz;

  fcp->modefl &= ~(FC_EOF | FC_BAD | FC_ERR);
  fcp->modefl |= FC_OPN;

#if DEBUGIT
  if (fsdebug)
    SnapFCB (fcp);
#endif

  return (IO_OK);
}

/* 
*/

/*
   =============================================================================
	_inifcb(fcp, name, ext, mode) -- initialize FCB pointed to by 'fcp'
	for file named 'name'.'ext' in mode 'mode'.
	Returns SUCCESS (0) if ok, FAILURE (-1) if not.
   =============================================================================
*/

int
_inifcb (fcp, name, ext, mode)
     register struct fcb *fcp;
     char *name, *ext;
     int mode;
{
  int fl;
  register int i;
  register char *s1, *s2, c;

  /* clear the FCB */

  memset ((char *) fcp, 0, sizeof (struct fcb));

  /* check for valid flags */

  if (mode &
      ~(O_WRONLY | O_RDWR | O_NDELAY | O_APPEND | O_CREAT | O_TRUNC | O_EXCL |
	O_RAW))
    return (FAILURE);

/* 
*/
  /* check the file name for length */

  fl = TRUE;
  s1 = name;

  for (i = 0; i < 9; i++)
    if (*s1++ EQ '\0')
      {

	fl = FALSE;
	break;
      }

  if (fl)
    return (FAILURE);

  /* check the file name extension for length */

  fl = TRUE;
  s1 = ext;

  for (i = 0; i < 4; i++)
    if (*s1++ EQ '\0')
      {

	fl = FALSE;
	break;
      }

  if (fl)
    return (FAILURE);

/* 
*/
  /* clear the file name and extension in the FCB to blanks */

  memset (fcp->de.fname, ' ', 11);

  /* move the file name into the FCB */

  s1 = fcp->de.fname;
  s2 = name;

  for (i = 0; i < 8; i++)
    {

      c = *s2++;

      if ((c EQ '\0') OR (c EQ ' '))
	break;

      *s1++ = c;
    }

  /* move the file name extension into the FCB */

  s1 = fcp->de.fext;
  s2 = ext;

  for (i = 0; i < 3; i++)
    {

      c = *s2++;

      if ((c EQ '\0') OR (c EQ ' '))
	break;

      *s1++ = c;
    }

/* 
*/

  /* set the flags in the FCB and exit */

  if (mode & O_RDONLY)
    fcp->modefl |= FC_RD;

  if (mode & O_WRONLY)
    fcp->modefl |= FC_WR;

  if (mode & O_RDWR)
    fcp->modefl |= FC_RW;

  if (mode & O_NDELAY)
    fcp->modefl |= FC_NB;

  if (mode & O_APPEND)
    fcp->modefl |= FC_AP;

  if (mode & O_CREAT)
    fcp->modefl |= FC_CR;

  if (mode & O_TRUNC)
    fcp->modefl |= FC_TR;

  if (mode & O_EXCL)
    fcp->modefl |= FC_EX;

  if (mode & O_RAW)
    fcp->modefl |= FC_BF;

  return (SUCCESS);
}

/* 
*/

/*
   =============================================================================
	ClsFile() -- Closes the file controlled by the FCB pointed to by 'fcp'
	at the bios level.
   =============================================================================
*/

int
ClsFile (fcp)
     register struct fcb *fcp;
{
  register struct dirent *dp;

#if DEBUGIT
  if (fsdebug)
    {

      printf ("ClsFile($%08lX):  closing file\n", fcp);
      SnapFCB (fcp);
    }
#endif

  if (fcp->modefl & FC_OPN)
    {				/* is it open ? */

      if (fcp->modefl & (FC_CR | FC_AP | FC_WR))
	{			/* writing ? */

	  if (dp = _dsrch (&fcp->de))
	    {			/* locate the dirent */

	      /* update the directory entry if it changed */

	      if (memcmp (dp, &fcp->de, DENTSIZE))
		{

		  memcpy (dp, &fcp->de, DENTSIZE);
		  _dirmod = TRUE;
#if DEBUGIT
		  if (fsdebug)
		    {

		      printf ("ClsFile():  updated the directory.  FAT:\n");
		      mdump (_thefat, (char *) _thefat + 127, _thefat);
		    }
#endif

		}

/* 
*/

	    }
	  else
	    {

#if DEBUGIT
	      if (fsdebug)
		printf
		  ("ClsFile():  ERROR - can't find the directory entry\n");
#endif

	      errno = EIO;	/* can't find the entry */
	      return (FAILURE);
	    }
	}

#if DEBUGIT
      if (fsdebug)
	SnapFCB (fcp);
#endif

      return (SUCCESS);

    }
  else
    {				/* not open */

#if DEBUGIT
      if (fsdebug)
	printf ("ClsFile():  ERROR - file not open\n");
#endif

      errno = EINVAL;
      return (FAILURE);
    }
}

/* 
*/

/*
   =============================================================================
	_clsvol() -- close the volume
   =============================================================================
*/

_clsvol ()
{
  if (_fatmod)
    {

      _clsfat ();

#if	DEBUGIT
      if (fsdebug)
	printf ("_clsvol():  modified FAT written\n");
#endif
    }

  if (_dirmod)
    {

      _clsdir ();

#if	DEBUGIT
      if (fsdebug)
	printf ("_clsvol():  modified directory written\n");
#endif
    }

  _bpbin = FALSE;
  _fatin = FALSE;
  _dirin = FALSE;
}

/*
   =============================================================================
	fcbinit() -- initialize an FCB
   =============================================================================
*/

short
fcbinit (name, fcp)
     register char *name;
     register struct fcb *fcp;
{
  char tmpname[9], tmpext[4];

  return (_inifcb (fcp, FilName (name, tmpname), FilExt (name, tmpext), 0));
}

/* 
*/

/*
   =============================================================================
	DelFile() -- Deletes the file named by fcb pointed to by 'fcp'.
   =============================================================================
*/

int
DelFile (fcp)
     struct fcb *fcp;
{
#if	DEBUGIT
  if (fsdebug)
    printf ("DelFile($%08.8lX):  deleting [%-.8s].[%-.3s]\n",
	    fcp, fcp->de.fname, fcp->de.fext);
#endif

  if (_opnvol ())		/* open the volume */
    return (FAILURE);

  if (DE_NULL EQ (_dptr = _dsrch (&fcp->de)))
    {				/* find the file */

      errno = ENOENT;		/* can't find the file */
      return (FAILURE);
    }

  if (_dptr->atrib & F_RDONLY)
    {

      errno = EACCES;		/* can't delete an R/O file */
      return (FAILURE);
    }

  if (_ftrnc (_dptr))		/* truncate the file to free its space  */
    return (FAILURE);

  _dptr->fname[0] = 0xE5;	/* mark directory entry as free */

#if	DEBUGIT
  if (fsdebug)
    printf ("DelFile($%08.8lX):  file [%-.8s].[%-.3s] DELETED\n",
	    fcp, fcp->de.fname, fcp->de.fext);
#endif

  return (SUCCESS);
}
