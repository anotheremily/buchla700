#include "stdio.h"
#include "ctype.h"
#include "osbind.h"

#define TRUE	1
#define FALSE	0

struct basepage
{

  char *ltpa;
  char *htpa;
  char *lcode;
  long codelen;
  char *ldata;
  long datalen;
  char *lbss;
  long bsslen;
  char *dta;
  char *parent;
  long rsvd;
  char *env;
};

extern struct basepage *_BasePag;

/* locate variable name in environment string */

static char *
findenv (s)
     char *s;
{
  char name[20];
  char *p;

  if (s)
    {

      strcpy (name, s);
      strcat (name, "=");
    }

  for (p = _BasePag->env; *p; p++)
    {

      if (s)
	{

	  if (strcmp (p, name) == 0)
	    break;
	}

      while (*p)
	p++;
    }

  return (p);
}

char *
getenv (name)
     char *name;
{
  char *p;

  p = findenv (name);

  if (*p)
    {

      while (*p)
	p++;

      return (++p);
    }

  return (NULL);
}
