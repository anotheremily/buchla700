/*
   =============================================================================
	barbadj.c -- MIDAS-VII -- GLC bar graph drivers
	Version 8 -- 1988-10-27 -- D.N. Lynx Crowe

	BarBadj(bar, val)
	short bar, val;

		Adjusts a bottom-zero bar graph, 'bar',
		to read 'val'.

	BarBset(bar, val)
	short bar, val;

		Sets a bottom-zero bar graph, 'bar',
		to an intial value, 'val'.

	BarCadj(bar, val)
	short bar, val;

		Adjusts a centered-zero bar graph, 'bar',
		to read 'val'.

	BarCset(bar, val)
	short bar, val;

		Sets a centered-zero bar graph, 'bar',
		to an intial value, 'val'.
   =============================================================================
*/

#include "stddefs.h"
#include "hwdefs.h"
#include "glcfns.h"
#include "glcdefs.h"

extern short BarBcur[];
extern short BarCcur[];

/* left-most bar columns */

short BarCols[14] = { 2, 8, 14, 20, 26, 32, 38, 44, 50, 56, 62, 68, 74, 80 };

/* bar dot data */

short BarDots[3] = { 0x1C, 0xFC, 0xE0 };

#include "glcbars.h"		/* bar graph driver constant definitions */

/* 
*/

/*
   =============================================================================
	BarBadj() -- adjust a bottom-zero bar graph to a new value
   =============================================================================
*/

BarBadj (bar, val)
     short bar, val;
{
  register short bardot, barpos, curdif;
  register unsigned baradr;
  short barcol, bardif, curbar, i, newbar;

  newbar = BarBLn[val];		/* look up the new bar position */
  curbar = BarBcur[bar];	/* get the current bar position */
  bardif = newbar - curbar;	/* calculate how far to move the bar */

  if (0 EQ bardif)		/* done if bar doesn't need to be moved */
    return;

  GLCcurs (G_ON);		/* turn on GLC cursor to enable writing */
  barcol = BarCols[bar];	/* find leftmost column of bar */

  if (bardif > 0)
    {				/* increasing value */

      /* calculate initial GLC RAM write address */

      baradr = barcol + (85 * (63 - (curbar + 1))) + G_PLANE2;

      LCD_WC = G_CRSMUP;	/* set cursor motion "up" */

      for (i = 0; i < 3; i++)
	{			/* for each bar column ... */

	  curdif = bardif;	/* set difference counter */
	  bardot = BarDots[i];	/* get the column dot value */

	  LCD_WC = G_CRSWR;	/* set cursor address */
	  LCD_WD = baradr & 0xFF;
	  LCD_WD = (baradr >> 8) & 0xFF;

	  ++baradr;		/* update GLC start address */

	  LCD_WC = G_MWRITE;	/* setup to write */

	  while (curdif--)	/* write new dots */
	    LCD_WD = bardot;
	}
/* 
*/
    }
  else
    {				/* decreasing value */

      /* calculate initial GLC RAM write address */

      baradr = barcol + (85 * (63 - curbar)) + G_PLANE2;

      LCD_WC = G_CRSMDN;	/* set cursor motion "down" */

      for (i = 0; i < 3; i++)
	{			/* for each bar column ... */

	  curdif = -bardif;	/* set difference counter */

	  LCD_WC = G_CRSWR;	/* set cursor address */
	  LCD_WD = baradr & 0xFF;
	  LCD_WD = (baradr >> 8) & 0xFF;

	  ++baradr;		/* update GLC start address */

	  LCD_WC = G_MWRITE;	/* setup to write */

	  while (curdif--)	/* erase old dots */
	    LCD_WD = 0x00;
	}
    }

  LCD_WC = G_CRSMRT;		/* set cursor motion = "right" */
  GLCcurs (G_OFF);		/* turn off the cursor */

  BarBcur[bar] = newbar;	/* update current bar position */
}

/* 
*/

/*
   =============================================================================
	BarBset() -- set a bottom-zero bar graph to an initial value
   =============================================================================
*/

BarBset (bar, val)
     short bar, val;
{
  register short bardot, barpos, newbar;
  register unsigned baradr;
  short barcol, i;

  newbar = BarBLn[val];		/* look up the new bar position */
  barcol = BarCols[bar];	/* find leftmost column of bar */

  GLCcurs (G_ON);		/* turn on GLC cursor to enable writing */

  /* calculate initial GLC RAM write address */

  baradr = barcol + (85 * (63 - BBase)) + G_PLANE2;

  LCD_WC = G_CRSMUP;		/* set cursor motion = "up" */

  for (i = 0; i < 3; i++)
    {				/* for each bar column ... */

      bardot = BarDots[i];	/* get the column dot value */
      barpos = BBase;		/* get base of bar */

      LCD_WC = G_CRSWR;		/* set cursor address */
      LCD_WD = baradr & 0xFF;
      LCD_WD = (baradr >> 8) & 0xFF;

      ++baradr;			/* update GLC start address */

      LCD_WC = G_MWRITE;	/* setup to write */

      while (barpos++ LE newbar)	/* write new dots */
	LCD_WD = bardot;

      while (barpos++ < BTop)	/* erase old dots */
	LCD_WD = 0x00;
    }

  LCD_WC = G_CRSMRT;		/* set cursor motion = "right" */
  GLCcurs (G_OFF);		/* turn off the cursor */

  BarBcur[bar] = newbar;	/* update current bar position */
}

/* 
*/

/*
   =============================================================================
	BarCadj() -- adjust a centered-zero bar graph to a new value
   =============================================================================
*/

BarCadj (bar, val)
     short bar, val;
{
  register short bardot, barpos, newbar;
  register unsigned baradr;
  short barcol, bardif, curbar, i;

  newbar = BarCLn[val + BOffset];	/* look up the new bar position */
  curbar = BarCcur[bar];	/* get the current bar position */
  bardif = newbar - curbar;	/* calculate how far to move the bar */

  if (0 EQ bardif)		/* done if bar doesn't need to be moved */
    return;

  GLCcurs (G_ON);		/* turn on GLC cursor to enable writing */

  barcol = BarCols[bar];	/* find leftmost column of bar */

  /* calculate initial GLC RAM write address */

  baradr = barcol + (85 * (63 - curbar)) + G_PLANE2;

/* 
*/
  if (newbar > curbar)
    {				/* increasing value */

      LCD_WC = G_CRSMUP;	/* set cursor motion "up" */

      for (i = 0; i < 3; i++)
	{			/* for each bar column ... */

	  bardot = BarDots[i];	/* get the column dot value */
	  barpos = curbar;	/* set current vert. position */

	  LCD_WC = G_CRSWR;	/* set cursor address */
	  LCD_WD = baradr & 0xFF;
	  LCD_WD = (baradr >> 8) & 0xFF;

	  LCD_WC = G_MWRITE;	/* setup to write */

	  while (barpos NE newbar)	/* write bar on LCD */
	    if (barpos++ < BCenter)
	      LCD_WD = 0x00;	/* dots off */
	    else
	      LCD_WD = bardot;	/* dots on */

	  ++baradr;		/* update GLC start address */
	}
/* 
*/
    }
  else
    {				/* decreasing value */

      LCD_WC = G_CRSMDN;	/* set cursor motion "down" */

      for (i = 0; i < 3; i++)
	{			/* for each bar column ... */

	  bardot = BarDots[i];	/* get column dot value */
	  barpos = curbar;	/* set current bar location */

	  LCD_WC = G_CRSWR;	/* set cursor address */
	  LCD_WD = baradr & 0xFF;
	  LCD_WD = (baradr >> 8) & 0xFF;

	  LCD_WC = G_MWRITE;	/* setup to write */

	  while (barpos NE newbar)	/* write bar to LCD */
	    if (barpos-- > BCenter)
	      LCD_WD = 0x00;	/* dots off */
	    else
	      LCD_WD = bardot;	/* dots on */

	  ++baradr;		/* update GLC start address */
	}
    }

  LCD_WC = G_CRSMRT;		/* set cursor motion = "right" */
  GLCcurs (G_OFF);		/* turn off the cursor */

  BarCcur[bar] = newbar;	/* update current bar position */
}

/* 
*/

/*
   =============================================================================
	BarCset() -- set a centered-zero bar graph to an initial value
   =============================================================================
*/

BarCset (bar, val)
     short bar, val;
{
  register short bardot, barpos, barloc1, barloc2;
  register unsigned baradr;
  short barcol, i, newbar;

  GLCcurs (G_ON);		/* turn on GLC cursor to enable writing */

  newbar = BarCLn[val + BOffset];	/* look up the new bar position */
  barcol = BarCols[bar];	/* find leftmost column of bar */

  /* calculate initial GLC RAM write address */

  baradr = barcol + (85 * (63 - BBase)) + G_PLANE2;

  if (newbar < BCenter)
    {				/* target below center */

      barloc1 = newbar;		/* off limit */
      barloc2 = BCenter;	/* on limit */

    }
  else
    {				/* target at or above center */

      barloc1 = BCenter;	/* off limit */
      barloc2 = newbar;		/* on limit */
    }

  LCD_WC = G_CRSMUP;		/* set cursor motion "up" */

/* 
*/
  for (i = 0; i < 3; i++)
    {				/* for each bar column ... */

      bardot = BarDots[i];	/* get the column dot value */
      barpos = BBase;		/* set current vert. position */

      LCD_WC = G_CRSWR;		/* set cursor address */
      LCD_WD = baradr & 0xFF;
      LCD_WD = (baradr >> 8) & 0xFF;

      LCD_WC = G_MWRITE;	/* setup to write */

      while (barpos < barloc1)
	{			/* write "off" dots */

	  LCD_WD = 0x00;
	  barpos++;
	}

      while (barpos LE barloc2)
	{			/* write "on" dots */

	  LCD_WD = bardot;
	  barpos++;
	}

      while (barpos LE BTop)
	{			/* write "off" dots */

	  LCD_WD = 0x00;
	  barpos++;
	}

      ++baradr;			/* update GLC start address */
    }

  LCD_WC = G_CRSMRT;		/* set cursor motion = "right" */
  GLCcurs (G_OFF);		/* turn off the cursor */

  BarCcur[bar] = newbar;	/* update current bar position */
}
