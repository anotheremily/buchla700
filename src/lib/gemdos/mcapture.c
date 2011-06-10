/*
   =============================================================================
	mcapture.c -- capture MIDI data to a file
	See VERMSG below for version and date.
   =============================================================================
*/

#define	VERMSG	"mcapture -- Version 1.1 -- 1989-07-28 -- D.N. Lynx Crowe"

#include "stdio.h"
#include "osbind.h"
#include "stddefs.h"

#define	void	int

#define	DFLTFILE	"mcapture.out"

#define	MIDI 2			/* device number */

#define	MAXNOL	2

#define	FORMAT1		"\n%5u  %02.2xH  %-16.16s"
#define	FORMAT2		"  %02.2xH %3u %-10.10s"
#define	FORMAT3		"\n%5u                       "

extern char *malloc ();

void cleanbf ();

int SetMBuf (), midi_in (), m_stat ();

struct iorec
{				/* structure for MIDI buffer description */

  char *ibuf;
  short ibufsz;
  short ibufhd;
  short ibuftl;
  short ibuflo;
  short ibufhi;
};

unsigned int index;		/* MIDI input byte number */

int nol;			/* number of MIDI data bytes on the line */

FILE *ofp;			/* output file pointer */

char *newbuf;
char *oldbuf;			/* old MIDI buffer pointer */
short oldbsz;
short oldbhi;
short oldblo;

struct iorec *m_buff;		/* MIDI iorec pointer */

/* 
*/

/*           FUNCTIONS          */

/* set up MIDI buffer */

int
SetMBuf ()
{
  unsigned short size;

  size = 16384;			/* 16K MIDI buffer */

  m_buff = (struct iorec *) Iorec (MIDI);	/* pointer to buffer descriptor */

  oldbuf = m_buff->ibuf;
  oldbsz = m_buff->ibufsz;
  oldbhi = m_buff->ibufhi;
  oldblo = m_buff->ibuflo;

  if ((char *) NULL EQ (newbuf = (char *) malloc (size)))
    {

      printf ("ERROR -- unable to allocate MIDI buffer.\n");
      return (FAILURE);
    }

  /* clear out the buffer */

  m_buff->ibufhd = 0;		/* reset the head index */
  m_buff->ibuftl = 0;		/* reset the tail index */

  /* we do this twice because we aren't disabling interrupts ... */

  m_buff->ibufhd = 0;		/* reset the head index */
  m_buff->ibuftl = 0;		/* reset the tail index */

  m_buff->ibuf = newbuf;	/* change address of buffer */
  m_buff->ibufsz = size;	/* change size of buffer */

  index = 0;			/* reset the byte index */

  return (SUCCESS);
}

/* get MIDI byte */

int
midi_in ()
{
  return ((int) Bconin (3) & 0x00ff);
}

/* check midi status */

int
m_stat ()
{
  return ((int) Bconstat (3) ? TRUE : FALSE);
}

/* clean out MIDI buffer */

void
cleanbf ()
{
  int mstat;

  printf ("Clearing MIDI input buffer ...\n");

  /* clear out the buffer by resetting the head and tail indices */

  m_buff->ibufhd = 0;		/* reset the head index */
  m_buff->ibuftl = 0;		/* reset the tail index */

  /* we do this twice because we aren't disabling interrupts ... */

  m_buff->ibufhd = 0;		/* reset the head index */
  m_buff->ibuftl = 0;		/* reset the tail index */

  /* make sure it's really drained */

  mstat = m_stat ();

  while (mstat)
    {

      midi_in ();
      mstat = m_stat ();
    }

  index = 0;
}

/* 
*/

/*            MAIN PROGRAM LOOP      */

main (argc, argv)
     int argc;
     char *argv[];
{
  int ch, runtag;

  ofp = (FILE *) NULL;

  printf ("\033E%s\n\n", VERMSG);
  printf ("Hit / to clear buffer, ESC to finish.\n\n");

  if (SetMBuf ())		/* move MIDI buffer & increase size */
    exit (2);

  cleanbf ();			/* clear out MIDI buffer */

  if (argc EQ 2)
    {

      if ((FILE *) NULL EQ (ofp = fopen (argv[1], "w")))
	{

	  printf ("ERROR -- Unable to open \"%s\" for output.\n", argv[1]);

	  exit (2);

	}
      else
	{

	  printf ("Program will output to file \"%s\".\n", argv[1]);
	}

    }
  else
    {

      if ((FILE *) NULL EQ (ofp = fopen (DFLTFILE, "w")))
	{

	  printf ("ERROR -- Unable to open \"%s\" for output.\n", DFLTFILE);

	  exit (2);

	}
      else
	{

	  printf ("Program will output to file \"%s\".\n", DFLTFILE);
	}
    }

  printf ("Ready for MIDI data.\n");

  runtag = TRUE;

  while (runtag)
    {

      if (Bconstat (2))
	{

	  ch = 0x00FF & Bconin (2);

	  switch (ch)
	    {

	    case '\033':	/* escape */

	      runtag = FALSE;
	      break;

	    case '/':		/* / = clear buffer and screen */

	      cleanbf ();
	      printf ("\033E");
	      printf ("Ready for MIDI data.\n");
	      break;
	    }
	}

      if (m_stat ())
	ProcMIDI (midi_in ());
    }

  fflush (ofp);
  fclose (ofp);

  /* clear out the buffer */

  m_buff->ibufhd = 0;		/* reset the head index */
  m_buff->ibuftl = 0;		/* reset the tail index */

  /* we do this twice because we aren't disabling interrupts ... */

  m_buff->ibufhd = 0;		/* reset the head index */
  m_buff->ibuftl = 0;		/* reset the tail index */

  m_buff->ibufsz = oldbsz;	/* restore the old buffer size */
  m_buff->ibuf = oldbuf;	/* restore the old buffer address */

  free (newbuf);		/* give back the big MIDI buffer */

  printf ("\n");
  fflush (stdout);
  exit (0);
}
