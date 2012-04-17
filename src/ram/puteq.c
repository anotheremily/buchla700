/*
   =============================================================================
	puteq.c -- output functions for the LMC835 == chip on the Buchla 700
	Version 3 -- 1987-12-10 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	TESTER		0

#if	TESTER

#include "ascii.h"
#include "biosdefs.h"
#include "ctype.h"
#include "rawio.h"

#define	MAXLINE		4

#endif

#include "hwdefs.h"
#include "stddefs.h"

#define	EQ_STB		0x04
#define	EQ_DAT		0x08
#define	EQ_CLK		0x10

#define	EQ_IDL		(EQ_STB | EQ_CLK)
#define	EQ_MASK		(EQ_STB | EQ_CLK | EQ_DAT)

#define	EQ_A6DB		0x20
#define	EQ_B6DB		0x10

#define	EQ_6DB		(EQ_A6DB | EQ_B6DB)

#define	EQ_ADD		0x40

#define	PSG_ADDR	0
#define	PSG_READ	0
#define	PSG_WRIT	2

#define	PSG_IOEN	7
#define	PSG_IDLE	0xBF

#define	PSG_PRTB	15

/* 
*/

#if	TESTER
char cmdline[32];
#endif

char eqgaint[] = {

  0x00,				/*  0 db */
  0x20,				/*  1 db */
  0x10,				/*  2 db */
  0x08,				/*  3 db */
  0x04,				/*  4 db */
  0x02,				/*  5 db */
  0x12,				/*  6 db */
  0x2A,				/*  7 db */
  0x16,				/*  8 db */
  0x01,				/*  9 db */
  0x29,				/* 10 db */
  0x2D,				/* 11 db */
  0x2F				/* 12 db */
};

/* 
*/

puteq (byte)
     register char byte;
{
  register short i;
  register char *psg;
  register char eqdata;

  psg = &io_tone;

  *(psg + PSG_ADDR) = PSG_IOEN;	/* setup PSG I/O controls */
  *(psg + PSG_WRIT) = PSG_IDLE;

  *(psg + PSG_ADDR) = PSG_PRTB;	/* setup == control lines */
  eqdata = EQ_IDL | (*(psg + PSG_READ) & ~EQ_MASK);

  for (i = 0; i < 8; i++)
    {				/* send out 8 bits */

      if (byte & 1)		/* setup data line from LSB */
	eqdata |= EQ_DAT;	/* "1" */
      else
	eqdata &= ~EQ_DAT;	/* "0" */

      eqdata &= ~EQ_CLK;	/* set clock low */

      *(psg + PSG_ADDR) = PSG_PRTB;
      *(psg + PSG_WRIT) = eqdata;

      eqdata |= EQ_CLK;		/* set clock high */

      *(psg + PSG_ADDR) = PSG_PRTB;
      *(psg + PSG_WRIT) = eqdata;

      byte >>= 1;		/* shift next bit into LSB */
    }

  eqdata &= ~EQ_STB;		/* set strobe low */

  *(psg + PSG_ADDR) = PSG_PRTB;
  *(psg + PSG_WRIT) = eqdata;

  eqdata |= EQ_STB;		/* set strobe high */

  *(psg + PSG_ADDR) = PSG_PRTB;
  *(psg + PSG_WRIT) = eqdata;
}

/* 
*/

sendeq (band, gain)
     char band, gain;
{
  puteq (band);
  puteq (gain);
}

char
gain2eq (gain)
     short gain;
{
  register char eqdat;

  if (gain > 0)
    eqdat = eqgaint[gain] | EQ_ADD;
  else
    eqdat = eqgaint[-gain];

  return (eqdat);
}

/* 
*/

#if	TESTER

extern int xtrap15 ();

char ahex[] = "0123456789abcdefABCDEF";

/*
   ============================================================================
	xdtoi -- convert hex ASCII to an int digit
   ============================================================================
*/

int
xdtoi (c)
     register int c;
{
  register int i;
  register char *ap = &ahex[0];

  for (i = 0; i < 22; i++)
    if (c == * ap++)
      if (i > 15)
	return (i - 6);
      else
	return (i);

  return (-1);
}

/* 
*/

main ()
{
  short rc, c, j;
  register long temp;
  char gain, band;
  register char *aptr;

  printf ("\n\nBuchla 700 == chip test -- Enter data in hex\n\n");

  do
    {

      printf ("Band = ");

      rc = getln (CON_DEV, MAXLINE, cmdline);

      if (rc == A_CR)
	{

	  printf ("\n");

	  temp = 0L;
	  aptr = cmdline;

	  if (A_CR == (*aptr & 0x00FF))
	    {

	      xtrap15 ();
	      continue;
	    }

	  if (CTL ('G') == (*aptr & 0x00FF))
	    {

	      while (0 == BIOS (B_RDAV, CON_DEV))
		sendeq (band, gain);

	      BIOS (B_GETC, CON_DEV);
	      continue;
	    }

	  while (isxdigit (c = *aptr++))
	    temp = (temp << 4) + xdtoi (c);

	  if (temp > 255)
	    {

	      printf ("\nInput must be < 100\n\n");
	      continue;
	    }

	  band = (char) (temp & 0x000000FFL);

	}
      else
	{

	  printf ("Huh ?\n\n");
	  continue;
	}
/* 
*/
      printf ("Gain = ");

      rc = getln (CON_DEV, MAXLINE, cmdline);

      if (rc == A_CR)
	{

	  printf ("\n");

	  temp = 0L;
	  aptr = cmdline;

	  if (A_CR == (*aptr & 0x00FF))
	    {

	      xtrap15 ();
	      continue;
	    }

	  while (isxdigit (c = *aptr++))
	    temp = (temp << 4) + xdtoi (c);

	  if (temp > 255)
	    {

	      printf ("\nInput must be < 100\n\n");
	      continue;
	    }

	  gain = (char) (temp & 0x000000FFL);

	}
      else
	{

	  printf ("Huh ?\n\n");
	  continue;
	}

      sendeq (band, gain);
      printf ("\n");

    }
  while (1);
}

#endif
