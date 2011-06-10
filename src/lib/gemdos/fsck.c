/*
   =============================================================================
	fsck.c -- Atari ST file system checker
	Written by:  D.N. Lynx Crowe

	See VERMSG (below) for version information.

	This version checks files in the root directory on floppy drive A only.
   =============================================================================
*/

#define	PROGID	"fsck"
#define	VERMSG	"1.8 -- 1988-03-25"

#include "stddefs.h"
#include "stdio.h"
#include "osbind.h"
#include "ctype.h"
#include "strings.h"
#include "bpb.h"
#include "dirent.h"
#include "bootsec.h"

#define	MAXSECLN	512	/* maximum sector length */

#define PERLINE 	16	/* bytes per line for mdump */
#define	MAXCLINE	10	/* number of clusters per line */
#define	FNLEN		13	/* file name buffer length */

#define	MAXFATSZ	9	/* maximum FAT size */
#define	MAXRDSZ		14	/* maximum root directory size */

#define	MAXFATLN	(MAXSECLN * MAXFATSZ)
#define	MAXRDLN		(MAXSECLN * MAXRDSZ)

#define	MAXFATNM	(MAXFATLN / 2)

/* 
*/
char fat1[MAXFATLN];		/* FAT 1 image */
char fat2[MAXFATLN];		/* FAT 2 image */
char rdir[MAXRDLN];		/* root directory image */

short bsec[MAXSECLN / 2];	/* boot sector image */

short drive;			/* drive number */
short rdsize;			/* number of root directory entries */
short cltype;			/* FAT entry type - 0 = 12, 1 = 16 */

long fatlen;			/* length of FAT in bytes */

BOOL errflag;			/* error flag */
BOOL quiet;			/* quiet flag */
BOOL verbose;			/* verbose flag */
BOOL errdump;			/* dump on errors flag */
BOOL clslist;			/* list clusters */
BOOL bscdump;			/* dump boot sector */
BOOL dirlist;			/* list directory entries */

short fatmap[MAXFATNM];		/* FAT check map */

unsigned fat1loc;		/* starting sector of FAT 1 */
unsigned fat2loc;		/* starting sector of FAT 2 */
unsigned rdloc;			/* starting sector of root directory */
unsigned datloc;		/* starting sector of data */
unsigned eofclus;		/* cluster value limit for EOF */
unsigned badclus;		/* cluster value limit for errors */

struct BPB bpb;			/* copy of BPB for the drive */

/* 
*/

/*
   =============================================================================
	pipc() -- print if printable characters
   =============================================================================
*/

static
pipc (chars, length)
     char chars[];
     int length;
{
  int i;

  for (i = 0; i < length; i++)
    if (isascii (0x00FF & chars[i]) AND (isprint (0x00FF & chars[i])))
      printf ("%c", chars[i]);
    else
      printf (".");
}

/* 
*/

/*
   =============================================================================
	mdump() -- dump a memory area in hexadecimal
   =============================================================================
*/

mdump (begin, end, start)
     char *begin, *end;
     long start;
{
  long i, ii;
  int j, jj, k;
  char c, chars[PERLINE];

  i = 0L;
  ii = start;
  j = 0;

  if (begin GT end)
    return;

  while (begin LE end)
    {

      c = *begin++;

      if (!(i % PERLINE))
	{

	  if (i)
	    {

	      j = 0;
	      printf ("  ");
	      pipc (chars, PERLINE);
	    }

	  printf ("\n%08.8lx:", ii);
	}

      ii++;
      i++;
      printf (" %02.2x", (c & 0x00FF));
      chars[j++] = c;
    }

  if (k = (i % PERLINE))
    {

      k = PERLINE - k;

      for (jj = 0; jj < (3 * k); ++jj)
	printf (" ");
    }

  printf ("  ");
  pipc (chars, PERLINE);
  printf ("\n");
}

/* 
*/

/*
   =============================================================================
	getiwd() -- get an Intel byte reversed word
   =============================================================================
*/

unsigned
getiwd (wp)
     register char wp[];
{
  register unsigned val;

  val = ((wp[1] & 0x00FF) << 8) | (wp[0] & 0x00FF);
  return (val);
}

/*
   =============================================================================
	trim() -- trim trailing blanks from a string
   =============================================================================
*/

char *
trim (s)
     register char *s;
{
  register char *sp;

  if (*s EQ '\0')		/* handle empty string */
    return (s);

  sp = s;

  while (*sp)			/* find end of string */
    ++sp;

  while (' ' EQ * --sp)
    {

      *sp = '\0';

      if (sp EQ s)
	break;
    }

  return (s);
}

/* 
*/

/*
   =============================================================================
	fileid() -- get a file identifier from a directory entry
   =============================================================================
*/

char *
fileid (dir, buf)
     struct Dirent *dir;
     char *buf;
{
  memset (buf, 0, 13);
  strncat (buf, dir->name, 8);
  trim (buf);

  if (strncmp (dir->ext, "   ", 3))
    {

      strcat (buf, ".");
      strncat (buf, dir->ext, 3);
      trim (buf);
    }

  return (buf);
}


/* 
*/

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
   =============================================================================
	getcl() -- get a FAT entry
   =============================================================================
*/

unsigned
getcl (fp, ncl, ft)
     char *fp;
     unsigned ncl;
     short ft;
{
  if (ft)
    return (getiwd (fp[ncl << 1]));
  else
    return (_gtcl12 (fp, ncl));
}

/* 
*/

/*
   =============================================================================
	ckdir() -- check a directory entry
   =============================================================================
*/

BOOL
ckdir (fat, dir, ent)
     char *fat;
     struct Dirent *dir;
     short ent;
{
  unsigned dirclus;
  unsigned curclus;
  short cln;
  BOOL ok;
  char fn1[FNLEN];
  char fn2[FNLEN];

  if ((dir[ent].name[0] & 0x00FF) EQ 0x00E5)
    return (FALSE);

  if ((dir[ent].name[0] & 0x00FF) EQ 0)
    return (TRUE);

  if (verbose OR clslist OR dirlist)
    {

      printf ("  Checking entry %d:  \"%s\"", ent, fileid (&dir[ent], fn1));

      if (dir[ent].atrib & F_VOLUME)
	{

	  printf (" -- Volume label\n");
	  return (FALSE);

	}
      else
	{

	  printf (" -- Flags:  ");

	  printf ("%c", dir[ent].atrib & F_ARCHIV ? 'A' : '.');
	  printf ("%c", dir[ent].atrib & F_SUBDIR ? 'D' : '.');
	  printf ("%c", dir[ent].atrib & F_SYSTEM ? 'S' : '.');
	  printf ("%c", dir[ent].atrib & F_HIDDEN ? 'H' : '.');
	  printf ("%c", dir[ent].atrib & F_RDONLY ? 'R' : '.');

	  printf ("\n");
	}
    }
/* 
*/
  dirclus = getiwd (dir[ent].clus);

  if (fatmap[dirclus] NE - 1)
    {

      printf ("ERROR:  Overlapping allocations --\n");
      printf ("  %s overlaps %s at cluster %u\n\n",
	      fileid (&dir[ent], fn1),
	      fileid (&dir[fatmap[dirclus]], fn2), dirclus);

      errflag = TRUE;
      return (FALSE);
    }

  if (clslist)
    printf ("     allocated clusters:\n      ");

  cln = 0;
  ok = TRUE;

  while (ok)
    {

      fatmap[dirclus] = ent;

      if (clslist)
	{

	  if (cln EQ MAXCLINE)
	    {

	      printf ("\n      ");
	      cln = 0;
	    }

	  ++cln;
	  printf (" %5u", dirclus);
	}

      curclus = getcl (fat, dirclus, cltype);
/* 
*/
      if (curclus LT 2)
	{

	  printf ("ERROR:  Bad FAT entry ($%x) at %u ($%x)",
		  curclus, dirclus, dirclus);

	  printf (" for file \"%s\"\n", fileid (&dir[ent], fn1));
	  errflag = TRUE;
	  ok = FALSE;

	}
      else if (curclus LT badclus)
	{

	  if (fatmap[curclus] NE - 1)
	    {

	      printf ("ERROR:  Overlapping allocations --\n");
	      printf ("  \"%s\" overlaps \"%s\" at cluster %u\n\n",
		      fileid (&dir[ent], fn1),
		      fileid (&dir[fatmap[dirclus]], fn2), dirclus);

	      errflag = TRUE;
	      return (FALSE);
	    }

	  dirclus = curclus;

	}
      else if (curclus LT eofclus)
	{

	  printf ("ERROR:  Bad FAT entry ($%x) at %u ($%x)",
		  curclus, dirclus, dirclus);

	  printf (" for file \"%s\"\n", fileid (&dir[ent], fn1));
	  errflag = TRUE;
	  ok = FALSE;

	}
      else
	{

	  if (clslist AND cln)
	    printf ("\n");

	  ok = FALSE;
	}
    }

  return (FALSE);
}

/* 
*/

/*
   =============================================================================
	main() -- main driver function for fsck.c
   =============================================================================
*/

main (argc, argv)
     short argc;
     char *argv[];
{
  char *bpp;
  char *ap;
  short c;
  long rc;
  register long i;
  register short j;
  register struct BootSec *bsp;

  /* setup miscellaneous variables and defaults */

  drive = 0;			/* drive A */
  cltype = 0;			/* 12 bit FAT entries */
  eofclus = 0x0FF8;		/* EOF cluster limit */
  badclus = 0x0FF0;		/* bad cluster limit */

  quiet = FALSE;		/* log all activity */
  verbose = FALSE;		/* log details */
  errdump = FALSE;		/* dump on errors */
  clslist = FALSE;		/* list cluster allocations */
  bscdump = FALSE;		/* dump boot sector */
  dirlist = FALSE;		/* list directory entries */
/* 
*/
  /* parse command line */

  if (argc > 1)
    {

      ap = argv[1];

      while (c = 0x00FF & *ap++)
	{

	  c = tolower (c);

	  switch (c)
	    {

	    case 'a':		/* set options b, c, d, e, v */

	      bscdump = TRUE;	/* b option */
	      clslist = TRUE;	/* c option */
	      dirlist = TRUE;	/* d option */
	      errdump = TRUE;	/* e option */
	      verbose = TRUE;	/* v option */
	      break;

	    case 'b':		/* dump boot sector */

	      bscdump = TRUE;
	      break;

	    case 'c':		/* list clusters */

	      clslist = TRUE;
	      break;

	    case 'd':		/* list directory entries */

	      dirlist = TRUE;
	      break;

	    case 'e':		/* dump on errors */

	      errdump = TRUE;
	      break;
/* 
*/
	    case 'q':		/* quiet mode */

	      quiet = TRUE;
	      break;

	    case 'v':		/* verbose mode */

	      verbose = TRUE;
	      break;

	    default:		/* invalid option */

	      printf ("%s:  ERROR - invalid option \"%c\" [$%02.2x]\n\n",
		      PROGID, c, c);

	      printf ("usage:  %s [abcdeqv]\n", PROGID);
	      printf ("    a  set options b, c, d, e, v\n");
	      printf ("    b  dump boot sector\n");
	      printf ("    c  list cluster allocations\n");
	      printf ("    d  list directory entries\n");
	      printf ("    e  dump FATS and DIR on errors\n");
	      printf ("    q  quiet mode\n");
	      printf ("    v  verbose mode\n");
	      exit (1);
	    }
	}
    }
/* 
*/
  errflag = FALSE;		/* reset the error flag */

  /* identify the program */

  if (NOT quiet)
    printf ("\n%s for the Atari ST -- Version %s\n\n", PROGID, VERMSG);

  if (verbose)
    printf ("Reading boot sector ...\n");

  if (rc = Floprd (bsec, 0L, drive, 1, 0, 0, 1))
    {

      printf ("ERROR -- Unable to read boot sector\n");
      exit (1);
    }

  if (verbose)
    printf ("Boot sector read OK\n");

  bsp = (struct BootSec *) bsec;

  if (bscdump)
    {

      printf ("\nBoot sector contents --\n\n");

      printf ("  Branch word       = $%02.2x $%02.2x\n",
	      bsp->branch[0], bsp->branch[1]);

      printf ("  OEM area          =");

      for (j = 0; j < 6; j++)
	printf (" $%02.2x", bsp->oem[j]);

      printf ("\n  Volume S/N        =");

      for (j = 0; j < 3; j++)
	printf (" $%02.2x", bsp->vsn[j]);

      printf ("\n  Bytes / Sector    = %u\n", getiwd (bsp->bps));
      printf ("  Sectors / Cluster = %u\n", bsp->spc);
      printf ("  Reserved sectors  = %u\n", getiwd (bsp->res));
      printf ("  Number of FATS    = %u\n", bsp->nfats);
      printf ("  Directory entries = %u\n", getiwd (bsp->ndirs));
      printf ("  Total Sectors     = %u\n", getiwd (bsp->nsects));
      printf ("  Media byte        = $%02.2x\n", bsp->media);
      printf ("  Sectors / FAT     = %u\n", getiwd (bsp->spf));
      printf ("  Sectors / Track   = %u\n", getiwd (bsp->spt));
      printf ("  Sides             = %u\n", getiwd (bsp->nsides));
      printf ("  Hidden files      = %u\n", getiwd (bsp->nhid));
      printf ("  Checksum          = $%04.4x\n", getiwd (bsp->cksum));

      printf ("\nBoot record data area --\n\n");
      mdump (bsp->boot, &bsp->boot[479], 0L);
      printf ("\n");
    }
/* 
*/
  /* get the BPB */

  if (bpp = (char *) Getbpb (drive))
    {

      memcpy (&bpb, bpp, sizeof (struct BPB));

    }
  else
    {

      printf ("ERROR:  Unable to get BPB for drive");
      exit (1);
    }

  /* determine where things live, etc. */

  fat2loc = bpb.fatrec;
  fat1loc = bpb.fatrec - bpb.fsiz;
  rdloc = bpb.fatrec + bpb.fsiz;
  datloc = bpb.fatrec + bpb.fsiz + bpb.rdlen;
  rdsize = (bpb.recsiz * bpb.rdlen) / sizeof (struct Dirent);
  fatlen = bpb.fsiz * bpb.recsiz;

  /* check BPB values for consistency */

  if (verbose)
    printf ("Checking BPB for consistency ...\n");

  if (datloc NE bpb.datrec)
    {

      printf ("ERROR:  Data locations inconsistent:\n");
      printf ("   BPB value:  %u\n", bpb.datrec);
      printf ("   Calculated: %u\n", datloc);
      exit (1);
    }
/* 
*/
  /* read FAT 1 */

  if (verbose)
    printf ("Reading FAT 1 ...\n");

  if (rc = Rwabs (0, fat1, bpb.fsiz, fat1loc, drive))
    {

      printf ("ERROR:  Unable to read FAT 1 -- error code = %ld\n", rc);

      exit (1);
    }

  if (verbose)
    printf ("FAT 1 read OK\n");

  /* read FAT 2 */

  if (verbose)
    printf ("Reading FAT 2 ...\n");

  if (rc = Rwabs (0, fat2, bpb.fsiz, fat2loc, drive))
    {

      printf ("ERROR:  Unable to read FAT 2 -- error code = %ld\n", rc);

      exit (1);
    }

  if (verbose)
    printf ("FAT 2 read OK\n");

  /* read the root directory */

  if (verbose)
    printf ("Reading root directory ...\n");

  if (rc = Rwabs (0, rdir, bpb.rdlen, rdloc, drive))
    {

      printf ("ERROR:  Unable to read root directory -- error code = %ld\n",
	      rc);

      exit (1);
    }

  if (verbose)
    printf ("Root directory read OK\n");
/* 
*/
  /* compare the FATs */

  if (verbose)
    printf ("Comparing FATs ...\n");

  for (i = 0; i < fatlen; i++)
    {

      if (fat1[i] NE fat2[i])
	{

	  printf ("ERROR:  FATs differ at offset %lu ($%lx)\n\n", i, i);

	  if (errdump)
	    {

	      printf ("FAT 1:\n\n");
	      mdump (fat1, fat1 + (long) (fatlen - 1), 0L);

	      printf ("\fFAT 2:\n\n");
	      mdump (fat2, fat2 + (long) (fatlen - 1), 0L);

	      printf ("\f");

	    }
	  else
	    {

	      printf ("WARNING -- Primary file system damaged.\n");
	    }

	  exit (1);
	}
    }

  if (verbose)
    printf ("FATs are consistent\n");

  /* check the root directory files */

  if (verbose | clslist)
    printf ("Checking root directory ...\n");

  memsetw (fatmap, -1, MAXFATNM);

  for (j = 0; j < rdsize; j++)
    if (ckdir (fat1, rdir, j))
      break;
/* 
*/
  if (errflag)
    if (errdump)
      {

	printf ("\fDirectory dump:\n\n");
	mdump (rdir,
	       rdir + (long) (sizeof (struct Dirent) * rdsize) - 1L, 0L);

	printf ("\fFAT dump:\n\n");
	mdump (fat1, fat1 + (long) (fatlen - 1), 0L);

	printf ("\n");

      }
    else
      {

	printf ("\nWARNING -- Primary file system damaged.\n");
      }

  if (NOT quiet)
    printf ("\nPrimary file system check complete on drive %c.\n\n",
	    drive + 'A');

  exit (errflag ? 1 : 0);
}
