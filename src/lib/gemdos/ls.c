/*
   =============================================================================
	ls.c - a Unix-like directory listing program for GEMDOS
	Version 1 -- 1988-05-18
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"
#include "malloc.h"
#include "osbind.h"

#undef	ANSI			/* if ansi display */
#define	GLASS			/* if glass tty */

#define QS		'\\'	/* filename separator (character) */
#define DQS		"\\"	/* filename separator (string) */

#define ID		TRUE	/* always identify directory if TRUE */
#define ALL		TRUE	/* show hidden files by default if TRUE */
#define LONG		TRUE	/* long listing by default if TRUE */
#define COLM		FALSE	/* 1-column listing by default if TRUE */
#define RSORT		FALSE	/* reverse sort by default if TRUE */
#define TSORT		FALSE	/* time sort by default if TRUE */
#define DU		FALSE	/* include disk use by default if TRUE */

#define NAMESIZ		13	/* 12 character name + NULL */
#define ONECS		512	/* cluster size on one-sided floppy */
#define TWOCS		1024	/* cluster size on two-sided floppy */
#define HARDCS		4096	/* cluster size on hard disk */
#define SCRSIZ		22	/* scrolling size of display screen */

/* 
*/

struct dta
{				/* DOS Disk Transfer Address table */

  char reserved[21];		/* used in "find next" operation */
  char attr;			/* file attribute byte */
  int ftime;			/* time of last modification */
  int fdate;			/* date of last modification */
  long fsize;			/* file size in bytes */
  char fname[NAMESIZ];		/* filename and extension */
};

struct outbuf
{				/* output buffer -- array of file structs */

  unsigned oattr;
  unsigned odate;
  unsigned otime;
  long osize;
  char oname[NAMESIZ + 1];

} *obuf;

/* 
*/

char spath[80];			/* holds current pathname string */

int allf = ALL;			/* include hidden & system files */
int ll = LONG;			/* long listing */
int colm = COLM;		/* 1-column format */
int rev = RSORT;		/* reverse sort */
int tsrt = TSORT;		/* timesort the listing */
int usage = DU;			/* print disk usage */
int recd;			/* recursive descent requested */
int sizonly;			/* only print sizes */
int np;				/* number of groups printed */
int nargs;			/* number of non-option arguments */
int clsize = 0;			/* size of a cluster, in bytes */
int clmask;			/* clsize-1 for rounding & chopping */
int drive;			/* code number for drive requested */
int tsc;			/* 1 if output is to console screen */

long left;			/* unused space left on disk */
long total;			/* total of sizes encountered */

/* 
*/

main (argc, argv)
     int argc;
     char *argv[];
{
  char *s;
  int c;
  int nt;

  nt = 0;
  c = 0;

  /* process input options */

  while (--argc > 0 AND (*++argv)[0] EQ '-')
    {

      for (s = argv[0] + 1; *s NE '\0'; s++)
	{

	  switch (0x007F & *s)
	    {

	    case 'a':		/* -a: list all files */

	      allf = NOT allf;
	      break;

	    case 'c':		/* -c: 1-column listing requested */

	      colm = NOT colm;
	      break;

	    case 'l':		/* -l: long listing requested */

	      ll = NOT ll;
	      break;

	    case 'r':		/* -r: reverse sort direction */

	      rev = NOT rev;
	      break;

	    case 's':		/* -s: print sizes only */

	      sizonly = TRUE;

	      if (*(s + 1) EQ '1')
		{

		  clsize = ONECS;	/* diskuse for 1-sided floppy */
		  s++;
		  nt++;

		}
	      else if (*(s + 1) EQ '2')
		{

		  clsize = TWOCS;	/* or 2-sided */
		  s++;
		  nt++;
		}

	      break;

	    case 't':		/* -t: time sort requested */

	      tsrt = NOT tsrt;
	      break;

	    case 'u':		/* -u: print disk usage */

	      usage = NOT usage;
	      break;

	    case 'R':		/* -R: recursively list subdirs */

	      recd = 1;
	      break;

	    default:

	      fprintf (stderr, "unknown arg %c\n", *s);
	      exit (1);
	    }
	}
    }

/* 
*/

  nargs = argc;
  tsc = toscreen ();		/* find out if output is to console screen */
  obuf = (struct outbuf *) malloc (sizeof (*obuf));	/* point to free memory */

  if (argc EQ 0)
    {

      argc++;
      curdrv (spath);		/* default to current drive */

    }
  else
    strcpy (spath, *argv);

  FOREVER
  {				/* cycle through args present */

    if (spath[1] EQ ':' AND spath[2] EQ '\0')	/* if drive only */
      getpath (spath);		/* get path */

    if (usage OR sizonly OR ll)
      c = getsize (spath);	/* get use data only if needed */

    if (c EQ 0)
      search (spath);		/* go do the hard work */

    if (--argc > 0)
      {

	strcpy (spath, *++argv);

      }
    else
      {

	if (usage OR sizonly)
	  {

	    if (np > 1)
	      {

		fprintf (stdout, "-------\n%7ld bytes total", total);

		if (!nt)
		  fputs ("; ", stdout);
	      }

	    if (!nt)
	      fprintf (stdout, "%7ld bytes left on drive %c\n",
		       left, drive + 'a');
	  }

	return;
      }
  }
}

/* 
*/

int
getsize (path)			/* get file cluster size */
     char *path;
{
  if (clsize EQ 0)
    {				/* if size not already set */

      if ((clsize = getcl (path)) EQ 0)
	{			/* get cluster size for drive */

	  fprintf (stderr, "Invalid drive: %c\n", *path);
	  return (1);
	}
    }

  clmask = clsize - 1;
  return (0);
}

/* 
*/

/* toscreen - find out if output is to console screen */

int
toscreen ()
{
  struct
  {

    unsigned int ax, bx, cx, dx, si, di, ds, es;
  } r;

  r.ax = 0x4400;
  r.bx = 1;
  sysint (0x21, &r, &r);
  return (r.dx & 1);
}

/* 
*/

/* search - search 'path' for filename or directory */

search (path)
     char *path;
{
  struct dta dta;		/* DOS file data table */
  extern struct outbuf;		/* array of file structs */
  extern int nargs;		/* number of files or directories */
  int path_len;			/* length of initial path */
  int z;			/* char counter */
  int k = 0;			/* counts number of entries found */
  char work[80];		/* working path string */
  int comp ();			/* string, time comparison routine */
  int mask = 0x0010;		/* attribute mask */
  long bytes = 0;		/* count of disk usage this directory */

  if (allf)
    mask = 0x001F;

  strcpy (work, path);
  path_len = strlen (work);	/* save original path length */

  if (!find_first (work, &dta, 0) OR work[path_len - 1] EQ QS)
    {

      if (work[path_len - 1] NE QS)
	{

	  strcat (work, DQS);	/* if path is to a directory */
	  path_len++;
	}

      strcat (work, "*.*");	/* list everything in it */
    }

/* 
*/

  if (find_first (work, &dta, mask))
    {

      do
	{

	  if (dta.attr & 0x08)	/* ignore volume label */
	    continue;

	  if (dta.fname[0] EQ '.' AND ! allf)	/* unless -a option */
	    continue;		/* ignore "." and ".." */

	  obuf[k].oattr = dta.attr;	/* stash this entry */
	  obuf[k].otime = dta.ftime;
	  obuf[k].odate = dta.fdate;
	  obuf[k].osize = dta.fsize;
	  strcpy (obuf[k].oname, dta.fname);

	  if (usage OR sizonly)
	    {

	      if ((dta.attr & 0x10) AND dta.fname[0] NE '.')
		{

		  bytes += clsize;	/* sum up disk usage */

		}
	      else if (dta.fsize)
		{

		  obuf[k].osize = ((dta.fsize + clmask) & (long) (~clmask));
		  bytes += obuf[k].osize;
		}
	    }

	  k++;

	}
      while (find_next (&dta));

    }
  else
    {

      work[path_len - 1] = NULL;
      fprintf (stderr, "Can't find a file or directory named \"%s\"\n", work);
      return;
    }

/* 
*/
  work[path_len] = NULL;	/* restore directory pathname */

  if (np++ AND ! sizonly)
    fputc (endlin (), stdout);	/* separate listing blocks */

  if (usage OR sizonly)
    {

      total += bytes;		/* total bytes to date */
      fprintf (stdout, "%7ld  ", bytes);
    }

  if (recd OR nargs > 1 OR usage OR sizonly OR ID)
    {

      fprintf (stdout, "%s", work);	/* identify the block */
      fputc (endlin (), stdout);
    }

  if (!sizonly)
    {

      qsort (obuf, k, sizeof (obuf[0]), comp);	/* sort the entries */

      if (ll)
	longlist (k);		/* and print them */
      else
	shortlist (k);
    }

  if (!recd)
    return;			/* quit if not -R */

  strcat (work, "*.*");

  if (find_first (work, &dta, mask))	/* else find all sub-dirs */
    do
      {

	if (dta.attr & 0x10 AND dta.fname[0] NE '.')
	  {

	    work[path_len] = 0;	/* discard old name */

	    for (z = 0; dta.fname[z] NE NULL; z++)
	      dta.fname[z] = tolower (dta.fname[z]);

	    strcat (work, dta.fname);	/* install a new one */
	    strcat (work, DQS);
	    search (work);	/* and recurse */
	  }

      }
    while (find_next (&dta));

  return;
}

/* 
*/

/* find_first - find first file in chosen directory */

find_first (path, dta, mask)
     char *path;
     struct dta *dta;
     int mask;
{
  struct
  {

    int ax, bx, cx;
    char *dx;
    int si, di, ds, es;
  } r;
  extern int _showds ();

  r.ax = 0x1A00;		/* DOS interrupt 1A */
  r.dx = (char *) dta;
  r.ds = _showds ();
  sysint (0x21, &r, &r);	/* sets data transfer address */

  r.ax = 0x4E00;		/* DOS interrupt 4E */
  r.cx = mask;
  r.dx = path;
  r.ds = _showds ();
  return (!(sysint (0x21, &r, &r) & 1));	/* fills the structure */
}

/* 
*/

/* find_next - find the next file in the same directory */

find_next (dta)
     struct dta *dta;
{
  struct
  {

    int ax, bx, cx;
    char *dx;
    int si, di, ds, es;
  } r;
  extern int _showds ();

  r.ax = 0x1A00;
  r.dx = (char *) dta;
  r.ds = _showds ();
  sysint (0x21, &r, &r);	/* set dta */

  r.ax = 0x4F00;
  return (!(sysint (0x21, &r, &r) & 1));	/* fill the table */
}

/* 
*/

/* curdrv - get current default drive */

curdrv (sp)
     char *sp;
{
  struct
  {

    int ax, bx, cx;
    char *dx, *si, *di, *ds, *es;
  } r;

  r.ax = 0x1900;		/* DOS interrupt 19 */
  sysint (0x21, &r, &r);	/* gets current drive number */
  *sp++ = r.ax + 'a';		/* convert to symbolic drive name */
  *sp++ = ':';
  return;
}

/* 
*/

/* getpath - get path to directory on indicated drive */

getpath (sp)
     char *sp;
{
  struct
  {

    int ax, bx, cx, dx;
    char *si;
    int di, ds, es;
  } r;
  extern int _showds ();

  strcat (sp, DQS);		/* append root file symbol to drive name */

  r.ax = 0x4700;		/* DOS interrupt 47 gets path string */
  r.dx = *sp - '`';		/* convert drive name to index */
  r.ds = _showds ();
  r.si = sp + 3;		/* paste string after root symbol */
  sysint (0x21, &r, &r);
  return;
}

/* 
*/

/* getcl - get cluster size & space left on requested drive */

int
getcl (pp)
     char *pp;
{
  struct
  {

    int ax, bx, cx, dx, si, di, ds, es;
  } r;
  int cs;
  extern long left;
  extern int drive;

  if (*(pp + 1) EQ ':')		/* use specified drive if any */
    r.ax = *pp - 'a';
  else
    {

      r.ax = 0x1900;		/* else get code for default drive */
      sysint (0x21, &r, &r);
    }

  drive = r.ax & 0x7F;

  if (!usage AND ! sizonly AND drive EQ 2)
    return (HARDCS);
  else
    {

      r.dx = drive + 1;		/* 0 = default, 1 = a, etc */
      r.ax = 0x3600;
      sysint (0x21, &r, &r);	/* DOS interrupt hex 36 */

      if (r.ax EQ 0xFFFF)	/* gets free disk space */
	return (0);		/* and other goodies */
      else
	{
	  cs = r.ax * r.cx;	/* r.ax = sectors/cluster */
	  left = (long) cs *r.bx;	/* r.bx = # unused clusters */
	  return (cs);		/* r.cx = bytes/sector */
	}			/* r.dx = drive capacity (clusters) */
    }
}

/* 
*/

/* comp - compare size of two entries */

int
comp (a, b)
     struct outbuf *a, *b;
{
  int y;

  if (tsrt)
    {

      if (a->odate NE b->odate)	/* if dates differ */
	y = (a->odate < b->odate) ? -1 : 1;	/* that settles it */
      else
	y = (a->otime < b->otime) ? -1 : 1;	/* else compare times */

      return ((rev) ? y : -y);

    }
  else
    {

      y = strcmp (a->oname, b->oname);	/* name comparison */
      return ((rev) ? -y : y);
    }
}

/* 
*/

/* shortlist - print a list of names in 5 columns */

shortlist (k)
     int k;			/* total number to print */
{
  int i, m, n;

  if (colm)
    n = k;			/* set for 1-column listing */
  else
    n = (k + 4) / 5;		/* or 5-column */

  for (i = 0; i < n; i++)
    {

      for (m = 0; (i + m) < k; m += n)
	{

	  if (obuf[i + m].oattr & 0x10)
	    strcat (obuf[i + m].oname, DQS);	/* mark directories */

	  putname (i + m);	/* print the name */
	  fputs ("   ", stdout);
	}

      fputc (endlin (), stdout);
    }

  return;
}

/* 
*/

/* putname - convert name to lower case and print */

putname (i)
     int i;
{
  int c, j = 0;

  while ((c = tolower (obuf[i].oname[j])) NE 0)
    {

      fputc (c, stdout);
      j++;
    }

  while (j++ < NAMESIZ - 1)	/* pad to columnarize */
    fputc (' ', stdout);
}

/* 
*/

/* endlin - end a line and watch for screen overflow */

static int lc = 0;		/* line counter */

endlin (fp)
     FILE *fp;
{
  extern int tsc;		/* true if output is to screen */
  int c;

  if (tsc AND++ lc GE SCRSIZ)
    {				/* pause if output is to console screen */
      /* and we've shown a screenful */

#ifdef ANSI
      fputs ("\n\033[30;43m--More--", fp);
      c = ci ();
      fputs ("\033[32;40m\b\b\b\b\b\b\b\b        \b\b\b\b\b\b\b", fp);
#endif

#ifdef GLASS
      fputs ("\n--More--", fp);
      c = ci ();
      fputs ("\b\b\b\b\b\b\b\b        \b\b\b\b\b\b\b", fp);
#endif

      switch (c)
	{

	case '\r':		/* <RETURN> - show 1 more line */

	  lc = SCRSIZ - 1;
	  break;

	case 'q':		/* quit with "q" or "ctrl-C" */
	case '\003':

	  exit (0);

	default:

	  lc = 0;		/* else show another screenful */
	  break;
	}

      return ('\b');

    }
  else
    return ('\n');
}

/* 
*/

/* longlist - list everything about files in two columns */

struct llst
{				/* structure to hold file information */

  char *fattr;			/* file attribute pointer */
  long size;			/* file size */
  int day;			/* the day of creation */
  int mnum;			/* month number */
  int yr;
  int hh;			/* creation times */
  int mm;
  int ap;			/* am or pm */
} l;

/* 
*/

longlist (k)
     int k;			/* total number to list */
{

  int i, m, n, cdate;
  char *mon, *mname ();

  cdate = gcdate ();		/* get current date (in months) */

  if (colm)
    n = k;			/* set for 1 column listing */
  else
    n = (k + 1) / 2;		/* or for 2 column listing */

  for (i = 0; i < n; i++)
    {

      for (m = 0; (m + i) < k; m += n)
	{

	  fill (i + m, &l);	/* fill llst structure */
	  mon = mname (l.mnum);	/* conv month # to name */

	  fprintf (stdout, "%s%7ld  %2d %s ", l.fattr, l.size, l.day, mon);

	  if (cdate GE (l.yr * 12 + l.mnum) + 12)
	    fprintf (stdout, " %4d  ", l.yr);	/* print year if too old */
	  else
	    {
	      fprintf (stdout, "%2d:%02d%c ", l.hh, l.mm, l.ap);	/* else print time */
	    }

	  putname (i + m);

	  if (m + n < k)
	    fputs ("\272 ", stdout);	/* double bar separator */
	}

      fputc (endlin (), stdout);
    }

  return;
}

/* 
*/

/* fill - fill long list structure with file information */

fill (i, ll)
     int i;
     struct llst *ll;
{
  int j, k;
  static char fbuf[16][4] = {

    "--w",
    "---",
    "-hw",
    "-h-",
    "s-w",
    "s--",
    "shw",
    "sh-",
    "d-w",
    "d--",
    "dhw",
    "dh-",
    "d-w",
    "d--",
    "dhw",
    "dh-"
  };

/* 
*/

  if ((obuf[i].oattr & 0x10) AND obuf[i].oname[0] NE '.')
    {

      ll->size = clsize;
      j = 8;			/* if directory, use block size */

    }
  else
    {				/* and set dir attr offset */

      ll->size = obuf[i].osize;	/* else use file size */
      j = 0;			/* and file attr offset */
    }

  ll->fattr = fbuf[(obuf[i].oattr & 0x07) + j];	/* point to symbolic attr */
  ll->day = obuf[i].odate & 0x1F;
  ll->mnum = (obuf[i].odate >> 5) & 0x0F;
  ll->yr = (obuf[i].odate >> 9) + 1980;
  k = obuf[i].otime >> 5;	/* this is a mess */
  ll->mm = k & 0x3f;
  ll->ap = ((ll->hh = k >> 6) GE 12) ? 'p' : 'a';

  if (ll->hh > 12)
    ll->hh -= 12;

  if (ll->hh EQ 0)
    ll->hh = 12;

  return;
}


/* 
*/

/* gcdate - get current date (in months) for comparison */

int
gcdate ()
{
  struct
  {
    unsigned int ax, bx, cx, dx, si, di, ds, es;
  } r;

  r.ax = 0x2A00;
  sysint (0x21, &r, &r);
  return (r.cx * 12 + (r.dx >> 8));	/* yr * 12 + month */
}

/* 
*/

/* mname - convert month number to month name */

char *
mname (n)
     int n;
{
  static char *name[] = {

    "???",
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
  };

  return ((n < 1 OR n > 12) ? name[0] : name[n]);
}

/* 
*/

int
_showds ()
{
  struct
  {
    int cs, ss, ds, es;
  } r;

  segread (&r);
  return (r.ds);
}

int
ci ()
{
  return (bdos (7) & 0xFF);
}
