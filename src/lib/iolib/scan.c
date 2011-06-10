/*
   =============================================================================
	scan.c -- input scan conversion for the portable C I/O Library
	Version 3 -- 1989-01-16 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"
#include "ctype.h"

static int maxwide;
static int (*gsub) ();

extern char *index ();

static char *scnstr;
static char quit;

/* 
*/

static long
getnum (list, values, base)
     char *list;
     char *values;
     int base;
{
  register long val;
  register char *cp;
  int c;
  int sign;

  if (maxwide LE 0)
    return (0L);

  val = sign = 0;

  if ((c = (*gsub) (0)) EQ '-')
    {

      sign = 1;
      --maxwide;

    }
  else if (c EQ '+')
    --maxwide;
  else
    (*gsub) (1);

  while (maxwide--)
    {

      if ((cp = index (list, (*gsub) (0))) EQ NULL)
	{

	  (*gsub) (1);
	  break;
	}

      val *= base;
      val += values[(long) cp - (long) list];
    }

  if (sign)
    val = -val;

  return (val);
}

/* 
*/

static
skipblk ()
{
  while (isspace ((*gsub) (0)))
    ;

  if ((*gsub) (1) EQ EOF)
    return (EOF);

  return (0);
}

static
sgetc (what)
{
  if (what EQ 0)
    {

      if (*scnstr)
	return (*scnstr++ & 0x00FF);

      quit = TRUE;

    }
  else
    {

      if (!quit)
	return (*--scnstr & 0x00FF);
    }

  return (EOF);
}

/* 
*/

scanfmt (getsub, fmt, args)
     int (*getsub) ();
     register char *fmt;
     int **args;
{

#ifdef FLOAT
  double getflt (), d;
#endif

  long lv;
  int c, count, dontdo, lflag, base;
  char *cp;
  char tlist[130];

  static char list[] = "ABCDEFabcdef9876543210";

  static char vals[] = {

    10, 11, 12, 13, 14, 15, 10, 11, 12, 13, 14, 15, 9, 8, 7, 6, 5, 4, 3, 2, 1,
    0
  };

/* 
*/
  count = 0;
  gsub = getsub;

  while (c = *fmt++)
    {

      if (c EQ '%')
	{

	  lflag = dontdo = FALSE;
	  maxwide = 127;

	  if (*fmt EQ '*')
	    {
	      ++fmt;
	      dontdo = 1;
	    }

	  if (isdigit (*fmt))
	    {

	      maxwide = 0;

	      do
		{

		  maxwide = maxwide * 10 + *fmt - '0';

		}
	      while (isdigit (*++fmt));
	    }

	  if (*fmt EQ 'l')
	    {

	      lflag = TRUE;
	      ++fmt;
	    }

/* 
*/
	  switch (*fmt++)
	    {

	    case '%':
	      c = '%';
	      goto matchit;

	    case 'h':		/* specify short (for compatibility) */
	      lflag = FALSE;
	      goto decimal;

	    case 'D':
	      lflag = TRUE;

	    case 'd':
	    decimal:
	      c = 12;
	      base = 10;
	      goto getval;

	    case 'X':
	      lflag = TRUE;

	    case 'x':
	      c = 0;
	      base = 16;
	      goto getval;

	    case 'O':
	      lflag = TRUE;

	    case 'o':
	      c = 14;
	      base = 8;
	    getval:
	      if (skipblk ())
		goto ateof;

	      lv = getnum (&list[c], &vals[c], base);

	      if (!dontdo)
		{

		  if (lflag)
		    *(long *) (*args++) = lv;
		  else
		    **args++ = lv;
		  ++count;
		}

	      break;
/* 
*/

#ifdef FLOAT
	    case 'E':
	    case 'F':
	      lflag = TRUE;

	    case 'e':
	    case 'f':
	      if (skipblk ())
		goto ateof;

	      d = getflt (tlist);

	      if (!dontdo)
		{

		  if (lflag)
		    *(double *) (*args++) = d;
		  else
		    *(float *) (*args++) = d;
		  ++count;
		}

	      break;
#endif

/* 
*/

	    case '[':
	      lflag = FALSE;

	      if (*fmt EQ '^' || *fmt EQ '~')
		{

		  ++fmt;
		  lflag = TRUE;
		}

	      for (cp = tlist; (c = *fmt++) != ']';)
		*cp++ = c;

	      *cp = 0;
	      goto string;

	    case 's':
	      lflag = TRUE;
	      tlist[0] = ' ';
	      tlist[1] = '\t';
	      tlist[2] = '\n';
	      tlist[3] = 0;
	    string:
	      if (skipblk ())
		goto ateof;

	      if (!dontdo)
		cp = *args++;

	      while (maxwide--)
		{

		  if ((c = (*gsub) (0)) EQ EOF)
		    break;

		  if (lflag ?
		      (index (tlist, c) NE 0) : (index (tlist, c) EQ 0))
		    {

		      (*gsub) (1);	/* unget last character */
		      break;
		    }

		  if (!dontdo)
		    *cp++ = c;
		}

	      if (!dontdo)
		{

		  *cp = 0;
		  ++count;
		}

	      break;
/* 
*/
	    case 'c':
	      if ((c = (*gsub) (0)) EQ EOF)
		goto ateof;

	      if (!dontdo)
		{

		  *(char *) (*args++) = c;
		  ++count;
		}

	      break;
	    }

	}
      else if (isspace (c))
	{

	  if (skipblk ())
	    {
	    ateof:
	      if (count EQ 0)
		return (EOF);

	      return (count);
	    }

	}
      else
	{

	matchit:
	  if (skipblk ())
	    goto ateof;

	  if ((*gsub) (0) != c)
	    return (count);
	}
    }

  return (count);
}

/* 
*/

#ifdef FLOAT

double
getflt (buffer)
     char *buffer;
{
  register int c;
  char decpt, sign, exp;
  register char *cp;
  double atof ();

  cp = buffer;
  sign = exp = decpt = 0;

  while (maxwide--)
    {

      c = (*gsub) (0);

      if (!sign AND (c EQ '-' OR c EQ '+'))
	sign = 1;
      else if (!decpt AND c EQ '.')
	decpt = 1;
      else if (!exp AND (c EQ 'e' OR c EQ 'E'))
	{

	  sign = 0;
	  exp = decpt = 1;

	}
      else if (!isdigit (c))
	{

	  (*gsub) (1);
	  break;
	}

      *cp++ = c;
    }

  *cp = 0;
  return (atof (buffer));
}

#endif

/* 
*/

sscanf (string, fmt, args)
     char *string, *fmt;
     int *args;
{
  scnstr = string;
  quit = FALSE;
  return (scanfmt (sgetc, fmt, &args));
}
