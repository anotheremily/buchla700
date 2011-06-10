/*
   =============================================================================
	dec2fr.c -- Buchla 700 fractional 2's complement conversion functions
	Version 10 -- 1987-12-06 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	TESTER	0

#include "stddefs.h"

#define	SFACT	1000L		/* scale factor */

static long d2f[][10] = {	/* decimal to fraction table */

  {0x00000000L, 0x000019A0L, 0x00003336L, 0x00004CD0L, 0x00006668L,
   0x00008000L, 0x000099A0L, 0x0000B336L, 0x0000CCCEL, 0x0000E668L},

  {0x00000000L, 0x00000290L, 0x00000520L, 0x000007B0L, 0x00000A40L,
   0x00000CD0L, 0x00000F60L, 0x000011F0L, 0x00001480L, 0x00001710L}
};

static long f2d[] = {		/* fraction to decimal table */

  50000L, 25000L, 12500L, 6250L,
  3125L, 1562L, 781L, 390L,
  195L, 97L, 48L, 24L,
  12L, 6L, 3L, 1L
};

/* 
*/

/*
   =============================================================================
	dec2fr() -- convert an ASCII decimal string to fractional binary

	The input string is 4 characters in the range 100- to 100+ inclusive.
	The result is a 2's complement binary fractional value.
   =============================================================================
*/

short
dec2fr (s)
     register char *s;
{
  register short i;

  if (s[0] EQ '1')
    {

      if (s[1] EQ '0')
	{

	  if (s[2] EQ '0')
	    {

	      if (s[3] EQ '-')
		return (0x8000);	/* -100 */
	      else
		return (0x7FFF);	/* +100 */

	    }
	  else
	    {

	      return (0xFFFF);	/* ERROR */
	    }

	}
      else
	{

	  return (0xFFFF);	/* ERROR */

	}

/* 
*/

    }
  else if (s[0] EQ '0')
    {

      i = (d2f[0][s[1] - '0'] + d2f[1][s[2] - '0']) >> 1;

      if (s[3] EQ '-')
	{

	  if (i)
	    return (~i);	/* negative number */
	  else
	    return (0);		/* zero is always 0 */

	}
      else
	{

	  return (i);		/* positive number */
	}
    }

  return (0xFFFF);		/* ERROR */
}

/* 
*/

/*
   =============================================================================
	fr2dec() -- convert a 2's complement fraction to decimal ASCII notation

	The input is a 2's complement binary fractional value.
	The result string is 4 characters in the range 100- to 100+ inclusive.
   =============================================================================
*/

char *
fr2dec (v, s)
     register short v;
     register char *s;
{
  register long acc, sc;
  register short i, p;

  if (v EQ 0x7FFF)
    {				/* +100 */

      sprintf (s, "100+");
      return (s);
    }

  if (v EQ 0x8000)
    {				/* -100 */

      sprintf (s, "100-");
      return (s);
    }

  if (v & 0x8000)
    {

      v = ~v;			/* negative number */
      p = '-';

    }
  else
    {

      p = '+';			/* positive number */
    }

  acc = 0;

  for (i = 0; i LT 15; i++)
    if (v & (1 << (14 - i)))
      acc += f2d[i];

  sc = SFACT;
  acc /= sc;
  sprintf (s, "%03d%c", (short) acc, p);
  return (s);
}

/* 
*/

#if TESTER

#include "stdio.h"

extern int memcmp ();

char t[8], v[8];

/*
   =============================================================================
	test program for fractional two's complement conversion functions
   =============================================================================
*/

main ()
{
  register short i;
  register short u;

  for (i = 0; i < 101; i++)
    {

      if (i EQ 50)
	printf ("\f");

      sprintf (t, "%03d+", i);
      u = dec2fr (t);

      if (u EQ 0xFFFF)
	{

	  printf ("[%s] = ERROR  = ??????    ", t);

	}
      else
	{

	  fr2dec (u, v);
	  printf ("[%s] = 0x%04x = [%s]", t, u, v);
	}

      printf (" %s    ", memcmp (t, v, 4) ? "<--" : "   ");

      sprintf (t, "%03d-", i);
      u = dec2fr (t);

      if (u EQ 0xFFFF)
	{

	  printf ("    [%s] = ERROR  = ??????\n", t);

	}
      else
	{

	  fr2dec (u, v);
	  printf ("    [%s] = 0x%04x = [%s]", t, u, v);
	}

      printf (" %s\n", memcmp (t, v, 4) ? "<--" : "");

    }
}

#endif
