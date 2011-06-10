#include "stdio.h"
#include "dir.h"

extern int alphasort ();

char pattern[80];

int
select (dirptr)
     struct direct *dirptr;
{
  return (fmatch (dirptr->d_name, pattern));
}

main (argc, argv)
     int argc;
     char *argv[];
{
  struct direct **namelist;
  int num, ii;
  char *ptr, *strrchr ();

  if (argc < 2)
    {

      argv[1] = ".";
      strcpy (pattern, "*.*");

    }
  else if ((ptr = strrchr (argv[1], '/')) == NULL)
    {

      strcpy (pattern, argv[1]);
      argv[1] = ".";

    }
  else
    {

      strcpy (pattern, ptr + 1);
      *ptr = '\0';
    }

  if ((num = scandir (argv[1], &namelist, select, alphasort)) <= 0)
    {

      fprintf (stderr, "dirtest2: ");
      perror (argv[1]);
      exit (1);
    }

  if (num == 0)
    {

      printf ("%s: Not found\n", pattern);
      exit (1);
    }

  for (ii = 0; ii < num; ii++)
    {

      printf ("%s, size = %ld\n", namelist[ii]->d_name, namelist[ii]->d_size);
    }

  freedir (namelist, num);
}
