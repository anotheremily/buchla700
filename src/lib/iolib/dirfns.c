/*
   =============================================================================
	dirfns.c -- miscellaneous directory functions
	Version 4 -- 1987-06-04 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "biosdefs.h"

extern int sprintf ();

static char atrcons[] = "ADVSHR";

/*
   =============================================================================
	atrstr(atr, s) -- convert attributes to character string
   =============================================================================
*/

char *
atrstr (atr, s)
     register short atr;
     register char s[];
{
  register short i, j;

  i = 0x20;

  for (j = 0; j < 6; j++)
    {

      if (atr & i)
	s[j] = atrcons[j];
      else
	s[j] = '-';

      i >>= 1;
    }

  s[j] = '\0';
  return (s);
}

/*  */

static char *mnames[] = {

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

/*
   =============================================================================
	mname - convert month number to month name
   =============================================================================
*/

char *
mname (n)
     short n;
{
  return ((n < 1 || n > 12) ? mnames[0] : mnames[n]);
}

/*  */

/*
   =============================================================================
	dtunpk(din, tin, s, fmt) -- unpack the date and time from DOS format
	into "yyyy-mm-dd hh:mm", "yyyy mmm dd hh:mm", or "mmm dd yyyy hh:mm"
	format.
   =============================================================================
*/

char *
dtunpk (din, tin, s, fmt)
     short din, tin, fmt;
     char *s;
{
  register short ftm, fdt;

  ftm = ((tin << 8) & 0xFF00) | ((tin >> 8) & 0x00FF);
  fdt = ((din << 8) & 0xFF00) | ((din >> 8) & 0x00FF);

  switch (fmt)
    {

    case 0:			/* yyyy-mm-dd hh:mm format */

      sprintf (s, "%04d-%02d-%02d %02d:%02d",
	       1980 + ((fdt >> 9) & 0x7F),
	       (fdt >> 5) & 0xF,
	       fdt & 0x1F, (ftm >> 11) & 0x1F, (ftm >> 5) & 0x3F);

      s[16] = '\0';
      break;

    case 1:			/* yyyy mmm dd hh:mm format */
    default:

      sprintf (s, "%04d %s %-2d %02d:%02d",
	       1980 + ((fdt >> 9) & 0x7F),
	       mname ((fdt >> 5) & 0xF),
	       fdt & 0x1F, (ftm >> 11) & 0x1F, (ftm >> 5) & 0x3F);

      s[17] = '\0';
      break;

/*  */

    case 2:			/* mmm dd yyyy hh:mm format */

      sprintf (s, "%s %2d %04d %02d:%02d",
	       mname ((fdt >> 5) & 0xF),
	       fdt & 0x1F,
	       1980 + ((fdt >> 9) & 0x7F),
	       (ftm >> 11) & 0x1F, (ftm >> 5) & 0x3F);

      s[17] = '\0';
      break;
    }

  return (s);
}
