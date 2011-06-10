/*
   =============================================================================
	dir.c - directory routines similar to unix 4.2 BSD
	Version 1 -- 1988-12-08 -- D.N. Lynx Crowe

	Modified versions of Usenet public domain source.
	Original author unknown.
   =============================================================================
*/

#include "errno.h"
#include "dir.h"

extern int errno;
extern char *calloc ();

/* 
*/

/*
   =============================================================================
	opendir() - opens given directory and reads its contents into memory.
	Other functions use this data or the DIR structure to do their work.
   =============================================================================
*/

DIR *
opendir (dirname)
     char *dirname;
{
  struct direct **namelist;
  DIR *dirptr;

  dirptr = (DIR *) calloc (1, sizeof (DIR));

  if (dirptr == (DIR *) 0)
    {

      errno = ENOMEM;
      return ((DIR *) 0);
    }

  dirptr->d_magic = DMAGIC;
  dirptr->d_pos = 0;
  dirptr->d_length = scandir (dirname, &(dirptr->namelist),
			      (int (*)()) 0, (int (*)()) 0);

  if (dirptr->d_length < 0)
    {

      free ((char *) dirptr);
      return ((DIR *) 0);
    }

  return (dirptr);
}

/* 
*/

/*
   =============================================================================
	readdir - returns the next directory structure from the list and
	updates values in the DIR structure.
   =============================================================================
*/

struct direct *
readdir (dirptr)
     DIR *dirptr;
{
  if (dirptr->d_magic != DMAGIC)
    {

      errno = ENOTDIR;
      return ((struct direct *) 0);
    }

  if (dirptr->d_pos >= dirptr->d_length)
    {

      errno = ENFILE;
      return ((struct direct *) 0);
    }

  return (dirptr->namelist[dirptr->d_pos++]);
}

/* 
*/

/*
   =============================================================================
	telldir - return the current position of the directory.
   =============================================================================
*/

long
telldir (dirptr)
     DIR *dirptr;
{
  if (dirptr->d_magic != DMAGIC)
    {

      errno = ENOTDIR;
      return (-1L);
    }

  return ((long) dirptr->d_pos);
}

/* 
*/

/*
   =============================================================================
	seekdir - position the given DIR stream to position given.
   =============================================================================
*/

seekdir (dirptr, loc)
     DIR *dirptr;
     long loc;
{
  if (dirptr->d_magic != DMAGIC)
    {

      errno = ENOTDIR;
      return (-1);
    }

  if (loc > (long) dirptr->d_length)
    {

      errno = EINVAL;
      return (-1);
    }

  dirptr->d_pos = (int) loc;
  return (0);
}

/* 
*/

/*
   =============================================================================
	rewinddir - rewind given DIR to beginning
   =============================================================================
*/

rewinddir (dirptr)
     DIR *dirptr;
{
  if (dirptr->d_magic != DMAGIC)
    {

      errno = ENOTDIR;
      return (-1);
    }

  dirptr->d_pos = 0;
  return (0);
}

/* 
*/

/*
   =============================================================================
	closedir - close given directory. destroy given DIR struct so we
	know it is closed.
   =============================================================================
*/

closedir (dirptr)
     DIR *dirptr;
{
  if (dirptr->d_magic != DMAGIC)
    {

      errno = ENOTDIR;
      return (-1);
    }

  dirptr->d_magic = ~DMAGIC;	/* mess it up a little */
  freedir (dirptr->namelist);
  free (dirptr);
  return (0);
}
