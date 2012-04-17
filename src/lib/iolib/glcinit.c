/*
   =============================================================================
	glcinit.c -- LCD drivers for the Epson GLC controller chip
	Version 5 -- 1988-08-03 -- D.N. Lynx Crowe
	(c) Copyright 1987, 1988 -- D.N. Lynx Crowe

	GLCinit()

		Initializes the GLC.

	unsigned
	GLCcrc(row, col)
	unsigned row, col;

		Positions the GLC cursor at ('row', 'col') preparatory
		to writing text.  Returns the calculated cursor address.

	unsigned
	GLCcxy(x, y)
	unsigned x, y;

		Positions the GLC cursor at ('x', 'y') preparatory to
		writing graphics.  Returns a bit mask for the pixel.
		Leaves the cursor address in lcdcurs.
		Limits:  0 <= x <= 511,  0 <= y <= 63.

	GLCwrts(s)
	char *s;

		Writes the character string pointed to by 's' at the
		current cursor position on the LCD display.
		Cursor must start and end on the same line.
		No error checks are done.

	GLCtext(row, col, s)
	unsigned row, col;
	char *s;

		Sets the GLC cursor to ('row', 'col'), then writes the
		character string pointed to by 's'.
		Cursor must start and end on the same line.
		No error checks are done.

	GLCdisp(dsp, crs, blk1, blk2, blk3)
	short dsp, crs, blk1, blk2, blk3;

		Sets the overall display, cursor and block status values.

	GLCcurs(crs)
	short crs;

		Turns the cursor on or off.
   =============================================================================
*/

/* 
*/

#include "stddefs.h"
#include "hwdefs.h"
#include "glcdefs.h"

unsigned lcdbase;		/* LCD graphics base address */
unsigned lcdbit;		/* LCD graphics pixel bit mask */
unsigned lcdcol;		/* LCD text column */
unsigned lcdctl1;		/* LCD display control -- command */
unsigned lcdctl2;		/* LCD display control -- data */
unsigned lcdcurs;		/* LCD graphics pixel byte address */
unsigned lcdrow;		/* LCD text row */
unsigned lcdx;			/* LCD graphics x */
unsigned lcdy;			/* LCD graphics y */

/* GLC initialization values */

char glc_is1[] = { 0x12, 0x05, 0x07, 0x54, 0x58, 0x3F, 0x55, 0x00 };
char glc_is2[] = { 0x00, 0x00, 0x3F, 0x00, 0x20, 0x3F, 0x00, 0x00 };

/* 
*/

/*
   =============================================================================
	GLCdisp(dsp, crs, blk1, blk2, blk3) -- set GLC display status
	Sets the overall display, cursor and block status values.
   =============================================================================
*/

GLCdisp (dsp, crs, blk1, blk2, blk3)
     short dsp, crs, blk1, blk2, blk3;
{
  register short val;

  val = ((blk3 & 3) << 6) | ((blk2 & 3) << 4) | ((blk1 & 3) << 2) | (crs & 3);

  lcdctl1 = G_DSPCTL | (dsp & 1);
  lcdctl2 = val;

  LCD_WC = lcdctl1;
  LCD_WD = lcdctl2;

}

/*
   =============================================================================
	GLCcurs() -- turns the cursor on or off
   =============================================================================
*/

GLCcurs (crs)
     short crs;
{
  lcdctl2 = (crs & 3) | (lcdctl2 & ~3);

  LCD_WC = lcdctl1;
  LCD_WD = lcdctl2;
}

/* 
*/

/*
   =============================================================================
	GLCinit() -- initialize GLC
	Initializes the GLC.
   =============================================================================
*/

GLCinit ()
{
  register int i;
  register long ic;
  register char *gp;

  lcdbase = G_PLANE2;		/* set defaults for graphics variables */
  lcdx = 0;
  lcdy = 0;
  lcdbit = 0x01;

  lcdrow = 0;			/* set default for text variables */
  lcdcol = 0;

  lcdctl1 = G_DSPCTL;
  lcdctl2 = 0;

  LCD_WC = G_INIT;		/* initialize the GLC */
  gp = &glc_is1[0];

  for (i = 0; i < 8; i++)
    LCD_WD = *gp++;

  LCD_WC = G_SETSAD;		/* setup scroll registers */
  gp = &glc_is2[0];

  for (i = 0; i < 8; i++)
    LCD_WD = *gp++;

  LCD_WC = G_HSCRL;		/* clear the horizontal scroll counter */
  LCD_WD = 0;

  LCD_WC = G_OVRLAY;		/* setup the display mode */
  LCD_WD = 0x08;

  GLCdisp (G_OFF, G_B2, G_ON, G_ON, G_OFF);

/* 
*/
  LCD_WC = G_CRSWR;		/* set cursor at (0,0) in G_PLANE1 */
  LCD_WD = G_PLANE1 & 0xFF;
  LCD_WD = (G_PLANE1 >> 8) & 0xFF;

  LCD_WC = G_CRSMRT;		/* set cursor motion forward */

  LCD_WC = G_MWRITE;		/* write zeros to GLC RAM */

  for (ic = 0; ic < 65536L; ic++)
    LCD_WD = 0;

  LCD_WC = G_CRSWR;		/* set cursor to (0,0) in G_PLANE1 */
  LCD_WD = G_PLANE1 & 0xFF;
  LCD_WD = (G_PLANE1 >> 8) & 0xFF;

  LCD_WC = G_CRSFRM;		/* setup a blinking underline cursor */
  LCD_WD = 0x04;
  LCD_WD = 0x06;

  /* enable display */

  GLCdisp (G_ON, G_B2, G_ON, G_ON, G_OFF);
}

/* 
*/

/*
   =============================================================================
	GLCcrc(row, col) -- position GLC text cursor
	Positions the GLC cursor at ('row', 'col') preparatory
	to writing text.  Returns calculated cursor address.
   =============================================================================
*/

unsigned
GLCcrc (row, col)
     unsigned row, col;
{
  unsigned curad;

  curad = col + (row * 85);	/* calculate cursor location */

  LCD_WC = G_CRSWR;		/* send cursor address to GLC */
  LCD_WD = curad & 0xFF;
  LCD_WD = (curad >> 8) & 0xFF;

  lcdrow = row;			/* set text cursor variables */
  lcdcol = col;

  return (curad);		/* return calculated cursor address */
}

/* 
*/

/*
   =============================================================================
	GLCcxy(x, y) -- position GLC graphics cursor
	Positions the GLC cursor at ('x', 'y') preparatory to
	writing graphics.  Returns a bit mask for the pixel.
	Leaves cursor address in lcdcurs.
	Limits:  0 <= x <= 511,  0 <= y <= 63.
   =============================================================================
*/

unsigned
GLCcxy (x, y)
     register unsigned x, y;
{
  register unsigned curad, xby6;

  /* calculate cursor address */

  xby6 = x % 6;
  curad = lcdbase + (85 * (63 - y)) + (x / 6) + (xby6 >> 3);
  lcdcurs = curad;

  /* send cursor address to GLC */

  LCD_WC = G_CRSWR;
  LCD_WD = curad & 0xFF;
  LCD_WD = (curad >> 8) & 0xFF;

  /* set graphics variables */

  lcdx = x;
  lcdy = y;

  /* calculate bit mask */

  lcdbit = 0x01 << (xby6 & 0x07);

  return (lcdbit);
}

/* 
*/

/*
   =============================================================================
	GLCwrts(s) -- write text string to GLC
	Writes the character string pointed to by 's' at the
	current cursor position on the LCD display.
	Cursor must start and end on the same line.
	No error checks are done.
   =============================================================================
*/

GLCwrts (s)
     register char *s;
{
  LCD_WC = G_CRSMRT;		/* set cursor motion =  right */

  LCD_WC = G_MWRITE;		/* set to write data */

  while (*s)
    {				/* write string to GLC */

      LCD_WD = *s++;
      lcdcol++;			/* keep column variable up to date */
    }
}

/* 
*/

/*
   =============================================================================
	GLCtext(row, col, s) -- position GLC cursor and write text
	Sets the GLC cursor to ('row', 'col'), then writes the
	character string pointed to by 's'.
	Cursor must start and end on the same line.
	No error checks are done.
   =============================================================================
*/

GLCtext (row, col, s)
     register unsigned row, col;
     register char *s;
{
  register unsigned curad;

  curad = col + (row * 85);	/* calculate cursor address */

  LCD_WC = G_CRSWR;		/* send cursor address to GLC */
  LCD_WD = curad & 0xFF;
  LCD_WD = (curad >> 8) & 0xFF;

  lcdrow = row;			/* set GLC text cursor variables */
  lcdcol = col;

  LCD_WC = G_CRSMRT;		/* set cursor motion = right */

  LCD_WC = G_MWRITE;		/* set to write data */

  while (*s)
    {				/* write string to GLC */

      LCD_WD = *s++;
      lcdcol++;			/* keep cursor column up to date */
    }
}
