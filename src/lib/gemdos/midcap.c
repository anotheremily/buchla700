/*
   =============================================================================
	midcap.c -- capture and display MIDI data
	See VERMSG below for version and date.
   =============================================================================
*/

#define	VERMSG	"midcap -- Version 1.06 -- 1989-12-12 -- D.N. Lynx Crowe"

#include "stdio.h"
#include "osbind.h"
#include "stddefs.h"

#define	void	int

#define	MBUFSZ		(unsigned short)(31*1024)

#define	SNAPFILE	"MIDCAP.DAT"	/* snap file name */
#define	MAXNOL		16	/* max number of data bytes on a line */

#define	FORMAT1		"\n%5u  %02.02x  "
#define	FORMAT2		"%02.02x  "
#define	FORMAT3		"\n%5u  "

struct iorec
{				/* structure for MIDI buffer description */

  char *ibuf;			/* base address of buffer */
  short ibufsz;			/* buffer size in bytes */
  short ibufhd;			/* head index */
  short ibuftl;			/* tail index */
  short ibuflo;			/* low water mark index */
  short ibufhi;			/* high water mark index */
};

extern char *malloc ();

void PrMIDI (), cleanbf ();

int SetMBuf (), midi_in (), m_stat ();

FILE *ofp;			/* output file pointer */
FILE *sfp;			/* snap file pointer */

char *newbuf;			/* expanded MIDI buffer pointer */
char *oldbuf;			/* old MIDI buffer pointer */

unsigned short indx;		/* MIDI input byte number */

short feseen;			/* MIDI active sensing seen */
short nol;			/* number of MIDI data bytes on the line */
short oldbsz;			/* old MIDI buffer size */
short oldbhi;			/* old MIDI buffer high water mark */
short oldblo;			/* old MIDI buffer low water mark */

struct iorec *m_buff;		/* MIDI iorec pointer */

/* 
*/

/*
   =============================================================================
	SetMBuf() -- set up MIDI buffer
   =============================================================================
*/

int
SetMBuf ()
{
  unsigned short size;

  size = MBUFSZ;		/* MIDI buffer */

  m_buff = (struct iorec *) Iorec (2);	/* pointer to buffer descriptor */

  oldbuf = m_buff->ibuf;
  oldbsz = m_buff->ibufsz;
  oldbhi = m_buff->ibufhi;
  oldblo = m_buff->ibuflo;

  if ((char *) NULL == (newbuf = (char *) malloc (size)))
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

  indx = 0;			/* reset the byte index */

  return (SUCCESS);
}

/*
   =============================================================================
	midi_in() -- get MIDI byte
   =============================================================================
*/

int
midi_in ()
{
  return ((int) Bconin (3) & 0x00FF);
}


/*
   =============================================================================
	PrMIDI() -- print a MIDI data byte
   =============================================================================
*/

void
PrMIDI (M_Byte)
     unsigned int M_Byte;
{
  if ((0x00FF & M_Byte) == 0x00FE)
    {

      if (! feseen)
	{

	  printf ("\nActive sense is active\n");

	  if ((FILE *) NULL != ofp)
	    fprintf (ofp, "\nActive sense is active\n");
	}

      nol = 0;
      feseen = TRUE;

    }
  else
    {

      ++indx;

      if (0x0080 & M_Byte)
	{			/* new status byte */

	  printf (FORMAT1, indx, M_Byte);
	  nol = 0;

	  if ((FILE *) NULL != ofp)
	    fprintf (ofp, FORMAT1, indx, M_Byte);

	}
      else
	{			/* data byte */

	  if (++nol > MAXNOL)
	    {

	      printf (FORMAT3, indx);
	      nol = 1;

	      if ((FILE *) NULL != ofp)
		fprintf (ofp, FORMAT3, indx);
	    }

	  printf (FORMAT2, M_Byte);

	  if ((FILE *) NULL != ofp)
	    fprintf (ofp, FORMAT2, M_Byte);
	}
    }

  fflush (stdout);

  if ((FILE *) NULL != ofp)
    fflush (ofp);
}


/*
   =============================================================================
	m_stat() -- check midi status
   =============================================================================
*/

int
m_stat ()
{
  return ((int) Bconstat (3) ? TRUE : FALSE);
}


/*
   =============================================================================
	cleanbf() -- clean out MIDI buffer
   =============================================================================
*/

void
cleanbf ()
{
  int mstat;

  printf ("Clearing MIDI input buffer ...\n");
  feseen = FALSE;

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

  indx = 0;
}

/* 
*/

/*
   =============================================================================
	midcap -- capture and display MIDI data
   =============================================================================
*/

main (argc, argv)
     int argc;
     char *argv[];
{
  int ch, runtag;

  ofp = (FILE *) NULL;

  printf ("\033E%s\n\n", VERMSG);
  printf ("Hit ESC to quit,  / to clear buffer,  space to pause output.\n\n");

  if (SetMBuf ())		/* move MIDI buffer & increase its size */
    exit (2);

  printf ("%u byte MIDI buffer allocated at 0x%08.8lx\n",
	  MBUFSZ, (long) newbuf);

  cleanbf ();			/* clear out MIDI buffer */

  if (argc == 2)
    {

      if ((FILE *) NULL == (ofp = fopen (argv[1], "w")))
	{

	  printf ("ERROR -- Unable to open \"%s\" for output.\n", argv[1]);

	  exit (2);

	}
      else
	{

	  printf ("Outputting to file \"%s\".\n", argv[1]);
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

	    case ' ':		/* space = pause */

	      printf ("PAUSED");
	      Bconin (2);
	      printf ("\b\b\b\b\b\b      \b\b\b\b\b\b");

	      break;

	    case '/':		/* / = clear buffer and screen */

	      cleanbf ();
	      printf ("\033E");
	      printf ("Ready for MIDI data.\n");

	      if ((FILE *) NULL != ofp)
		{

		  fprintf (ofp, "\n\nMIDI buffer flushed.\n\n");
		  fflush (ofp);
		}

	      break;

	    case 'w':		/* w = write to SNAPFILE */

	      if ((FILE *) NULL == (sfp = fopenb (SNAPFILE, "w")))
		{

		  printf ("ERROR -- Unable to open \"%s\" for output.\n",
			  SNAPFILE);

		  exit (2);

		}
	      else
		{

		  printf ("\n\nOutputting to file \"%s\".\n", SNAPFILE);
		}

	      fwrite (newbuf, indx, 1, sfp);

	      fflush (sfp);
	      fclose (sfp);

	      printf ("\nFile written and closed.\n\n");
	      break;
	    }
	}

      if (m_stat ())
	PrMIDI (midi_in ());

      if ((FILE *) NULL != ofp)
	fflush (ofp);
    }

  if ((FILE *) NULL != ofp)
    {

      fprintf (ofp, "\n");
      fflush (ofp);
      fclose (ofp);
    }

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
