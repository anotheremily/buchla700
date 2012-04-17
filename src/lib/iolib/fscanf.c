/*
   =============================================================================
	fscanf.c -- scan a stream file for input for the portable C I/O Library
	Version 4 -- 1989-01-16 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

static int scnlast;
static FILE *scnfp;

static int
gchar (what)
{
  if (what == 0)
    {

      if (feof (scnfp))
	scnlast = EOF;
      else
	scnlast = agetc (scnfp);
    }
  else
    scnlast = ungetc (scnlast, scnfp);

  return (scnlast);
}

int
scanf (fmt, args)
     char *fmt;
     int *args;
{
  scnfp = stdin;
  scnlast = 0;
  return (scanfmt (gchar, fmt, &args));
}

int
fscanf (fp, fmt, args)
     FILE *fp;
     char *fmt;
     int *args;
{
  scnfp = fp;
  scnlast = 0;
  return (scanfmt (gchar, fmt, &args));
}
