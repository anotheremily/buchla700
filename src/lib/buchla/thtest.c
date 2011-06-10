/*
   =============================================================================
	thtest.c -- send, capture, and display MIDI data for Thunder checkout
	See VERMSG below for version and date.

	Environment:  Atari 1040ST, GEMDOS, Alcyon C.
   =============================================================================
*/

#define	VERMSG	"thtest -- Version 4.01 -- 1990-04-25 -- D.N. Lynx Crowe"

#include "stdio.h"
#include "osbind.h"
#include "ctype.h"

#include "stddefs.h"

#define	void	int		/* 'cause we ain't got no steenking voids */

#define	MBUFSZ		(unsigned short)(31*1024)	/* MIDI buffer size */

#define	DEFUNIT		0	/* default Thunder unit number */
#define	DEFCHAN		0	/* default Thunder base channel */

#define	MMCODE1		0x007F	/* Buchla MIDI manufacturer ID byte 1 */
#define	MMCODE2		0x007F	/* Buchla MIDI manufacturer ID byte 2 */

#define	PRODUCT		0x0001	/* Buchla product code for Thunder */

#define	SNAPFILE	"THTEST.TMP"	/* snap file name */
#define	FILE_1		"THTEST.FFF"	/* config file name */
#define	FILE_2		"THTEST.GGG"	/* library file name */

#define	MAXNOL		16	/* max number of data bytes on a line */

#define	FORMAT1		"\n%5u  %02.02x"
#define	FORMAT2		"  %02.02x"
#define	FORMAT3		"\n%5u"

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

void PrMIDI ();			/* forward reference */
void cleanbf ();		/* forward reference */

int SetMBuf ();			/* forward reference */
int midi_in ();			/* forward reference */
int m_stat ();			/* forward reference */

FILE *ifp;			/* input file pointer */
FILE *ofp;			/* output file pointer */
FILE *sfp;			/* snap file pointer */

char *newbuf;			/* expanded MIDI buffer pointer */
char *oldbuf;			/* old MIDI buffer pointer */

char sxhead[32];		/* SysEx message header buffer */

unsigned short indx;		/* MIDI input byte number */
unsigned short thsum;		/* Thunder SysEx checksum */
unsigned short Addr;		/* address input temporary */
unsigned short Chan;		/* channel number input temporary */
unsigned short Ctrl;		/* controller number input temporary */
unsigned short Data;		/* data input temporary */
unsigned short Prog;		/* program input temporary */
unsigned short Unit;		/* unit number input temporary */
unsigned short Valu;		/* value input temporary */

short feseen;			/* MIDI active sensing seen */
short mstate;			/* output monitor state */
short nol;			/* number of MIDI data bytes on the line */
short oldbsz;			/* old MIDI buffer size */
short oldbhi;			/* old MIDI buffer high water mark */
short oldblo;			/* old MIDI buffer low water mark */
short thunit;			/* Thunder unit number */
short thchan;			/* Thunder base channel */

struct iorec *m_buff;		/* MIDI iorec pointer */

char *helpms[] = {

  "\n",
  "Escape  exit this program\n",
  "space   pause output\n",
  "/       clear buffer and screen\n",
  "b       set base channel\n",
  "c       request current Thunder config\n",
  "e       examine a word in the current Thunder config\n",
  "f       write THTEST.FFF to MIDI\n",
  "g       write THTEST.GGG to MIDI\n",
  "h       display this help message\n",
  "l       request Thunder library\n",
  "m       toggle output monitor state\n",
  "p       send program change\n",
  "s       set a word in the current Thunder config\n",
  "u       set Thunder unit number\n",
  "v       send control change\n",
  "w       write MIDI input buffer to snap file\n",
  "\n",
  (char *) NULL
};

/* 
*/

/*
   =============================================================================
	SendB() -- send a byte to the MIDI port and update the checksum
   =============================================================================
*/

void
SendB (byte)
     unsigned short byte;
{
  Bconout (3, 0x00FF & byte);
  thsum = 0x00FF & (thsum + byte);

  if (mstate)
    printf ("<%02.2x> ", 0x00FF & byte);
}

/*
   =============================================================================
	SendM() -- send a byte string to the MIDI port -- Checksum NOT updated.
   =============================================================================
*/

void
SendM (nb, adr)
     unsigned short nb;
     char *adr;
{
  short c;

  while (nb-- > 0)
    {

      Bconout (3, c = 0x00FF & *adr++);

      if (mstate)
	printf ("<%02.2x> ", c);
    }
}

/*
   =============================================================================
	SendW() -- send a 16 bit word out to MIDI and update the checksum
   =============================================================================
*/

void
SendW (wrd)
     unsigned short wrd;
{
  SendB (0x000F & (wrd >> 12));	/* send MS bits */
  SendB (0x003F & (wrd >> 6));	/* send middle bits */
  SendB (0x003F & wrd);		/* send LS bits */
}

/* 
*/

/*
   =============================================================================
	ThSysEx() -- send a Thunder SysEx message
   =============================================================================
*/

void
ThSysEx (mt, mp1, mp2)
     unsigned short mt, mp1, mp2;
{
  /* setup the SysEx header */

  sxhead[0] = 0x00F0;
  sxhead[1] = 0x0000;
  sxhead[2] = MMCODE1;
  sxhead[3] = MMCODE2;
  sxhead[4] = PRODUCT;
  sxhead[5] = thunit;
  sxhead[6] = mt;
  thsum = mt & 0x00FF;

  switch (mt)
    {

    case 0:			/* write word */
    case 3:			/* send word */
    case 4:			/* send current config */
    case 5:			/* send library */

      SendM (7, sxhead);	/* send the header */

      if ((0 EQ mt) OR (3 EQ mt))	/* optional Address */
	SendW (Addr);

      if (0 EQ mt)		/* optional Data */
	SendW (Data);

      Bconout (3, -thsum & 0x007F);	/* checksum */
      Bconout (3, 0x00F7);	/* EOX */

      if (mstate)
	printf ("<%02.2x> <F7>", -thsum & 0x007F);

      break;

    case 1:			/* write current config */
    case 2:			/* write library */
    default:

      break;
    }

  if (mstate)
    printf ("\n");
}

/* 
*/

/*
   =============================================================================
	GetUInt() -- get an unsigned integer parameter from the console
   =============================================================================
*/

short
GetUInt (msg, parm)
     char *msg;
     unsigned short *parm;
{
  register long iv;
  register int ten;
  register unsigned short ch;

  iv = 0L;
  ten = 10;

  if ((char *) NULL NE msg)
    printf ("%s", msg);

  while (0x000D NE (ch = 0x00FF & Bconin (2)))
    {

      if (isdigit (ch))
	{			/* see if it's a digit */

	  Bconout (2, ch);	/* echo the character */

	  iv = (iv * ten) + (ch - '0');

	  if (iv > 65535L)
	    {			/* check for overflow */

	      Bconout (2, '\r');	/* output CR/LF */
	      Bconout (2, '\n');

	      return (FAILURE);
	    }

	}
      else
	{			/* wasn't a digit */

	  Bconout (2, '\r');	/* output CR/LF */
	  Bconout (2, '\n');

	  return (FAILURE);
	}
    }

  Bconout (2, '\r');		/* output CR/LF */
  Bconout (2, '\n');

  if (iv > 65535L)		/* check for overflow */
    return (FAILURE);

  *parm = (unsigned short) iv;	/* update the parameter */

  return (SUCCESS);
}

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
  if ((0x00FF & M_Byte) EQ 0x00FE)
    {

      if (NOT feseen)
	{

	  printf ("\nActive sense is active\n");

	  if ((FILE *) NULL NE ofp)
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

	  if ((FILE *) NULL NE ofp)
	    fprintf (ofp, FORMAT1, indx, M_Byte);

	}
      else
	{			/* data byte */

	  if (++nol > MAXNOL)
	    {

	      printf (FORMAT3, indx);
	      nol = 1;

	      if ((FILE *) NULL NE ofp)
		fprintf (ofp, FORMAT3, indx);
	    }

	  printf (FORMAT2, M_Byte);

	  if ((FILE *) NULL NE ofp)
	    fprintf (ofp, FORMAT2, M_Byte);
	}
    }

  fflush (stdout);

  if ((FILE *) NULL NE ofp)
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
	thtest -- send, capture, and display MIDI data for Thunder checkout
   =============================================================================
*/

void
main (argc, argv)
     int argc;
     char *argv[];
{
  unsigned short ch, runtag;
  char **s;

  ofp = (FILE *) NULL;
  thunit = DEFUNIT;
  thchan = DEFCHAN;

  printf ("\033E%s\n\n", VERMSG);
  printf ("Typing h gets you a list of commands\n\n");

  if (SetMBuf ())		/* move MIDI buffer & increase its size */
    exit (2);

  printf ("%u byte MIDI buffer allocated at 0x%08.8lx\n",
	  MBUFSZ, (long) newbuf);

  cleanbf ();			/* clear out MIDI buffer */

  if (argc EQ 2)
    {

      if ((FILE *) NULL EQ (ofp = fopen (argv[1], "w")))
	{

	  printf ("\nERROR -- Unable to open \"%s\" for output.\n", argv[1]);

	  exit (2);

	}
      else
	{

	  printf ("Writing log to file \"%s\".\n", argv[1]);
	}
    }

  printf ("\nReady.\n");
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
	      printf ("Ready.\n");

	      if ((FILE *) NULL NE ofp)
		{

		  fprintf (ofp, "\n\nMIDI buffer flushed.\n\n");
		  fflush (ofp);
		}

	      break;

	    case 'B':
	    case 'b':		/* b = set base channel */

	      if (SUCCESS EQ GetUInt ("\nChan: ", &Chan))
		{

		  if ((Chan > 0) AND (Chan < 17))
		    thchan = Chan - 1;
		  else
		    printf ("\nERROR:  out of range  (1..16)\n");
		}

	      printf ("\nThunder base channel = %u\n", 1 + thchan);

	      break;

	    case 'C':
	    case 'c':		/* c = request current Thunder config */

	      ThSysEx (4, 0, 0);
	      break;

	    case 'E':
	    case 'e':		/* e = examine a word in a Thunder config */

	      if (SUCCESS EQ GetUInt ("\nAddr: ", &Addr))
		ThSysEx (3, Addr, 0);

	      break;

	    case 'F':
	    case 'f':		/* f = write THTEST.FFF to MIDI */

	      if ((FILE *) NULL EQ (ifp = fopenb (FILE_1, "r")))
		{

		  printf ("\nERROR:  Unable to open \"%s\" for input\n",
			  FILE_1);

		  break;

		}
	      else
		{

		  while (0 EQ feof (ifp))
		    Bconout (3, 0x00FF & getc (ifp));

		  fclose (ifp);

		  printf ("\nFile \"%s\" written to MIDI\n", FILE_1);
		}

	      break;

	    case 'G':
	    case 'g':		/* g = write THTEST.GGG to MIDI */

	      if ((FILE *) NULL EQ (ifp = fopenb (FILE_2, "r")))
		{

		  printf ("\nERROR:  Unable to open \"%s\" for input\n",
			  FILE_2);

		  break;

		}
	      else
		{

		  while (0 EQ feof (ifp))
		    Bconout (3, 0x00FF & getc (ifp));

		  fclose (ifp);

		  printf ("\nFile \"%s\"  written to MIDI\n", FILE_2);
		}

	      break;

	    case 'H':
	    case 'h':		/* h = display on-line help */

	      s = helpms;

	      while (*s)
		printf ("%s", *s++);

	      break;

	    case 'L':
	    case 'l':		/* l = request Thunder library */

	      ThSysEx (5, 0, 0);
	      break;

	    case 'M':
	    case 'm':		/* m = toggle output monitor state */

	      mstate = NOT mstate;
	      break;

	    case 'P':
	    case 'p':		/* p = send program change */

	      if (SUCCESS EQ GetUInt ("\nProg: ", &Prog))
		{

		  if ((Prog GE 0) AND (Prog < 128))
		    {

		      Bconout (3, 0x00C0 | (thchan & 0x000F));
		      Bconout (3, 0x007F & Prog);

		    }
		  else
		    {

		      printf ("\nERROR:  out of range  (0..127)\n");
		    }
		}

	      break;

	    case 'S':
	    case 's':		/* s = set a word in a Thunder config */

	      if (SUCCESS EQ GetUInt ("\nAddr: ", &Addr))
		{

		  if (SUCCESS EQ GetUInt ("Data: ", &Data))
		    ThSysEx (0, Addr, Data);
		}

	      break;

	    case 'U':
	    case 'u':		/* u = set Thunder unit number */

	      if (SUCCESS EQ GetUInt ("\nUnit: ", &Unit))
		{

		  if ((Unit > 0) AND (Unit < 10))
		    thunit = Unit - 1;
		  else
		    printf ("\nERROR:  out of range  (1..9)\n");
		}

	      printf ("\nThunder unit number = %u\n", 1 + thunit);

	      break;

	    case 'v':
	    case 'V':		/* v = send control change */

	      if (SUCCESS EQ GetUInt ("\nCtrl: ", &Ctrl))
		{

		  if ((Ctrl GE 0) AND (Ctrl < 128))
		    {

		      if (SUCCESS EQ GetUInt ("\nValue: ", &Valu))
			{

			  if ((Valu GE 0) AND (Valu < 128))
			    {

			      Bconout (3, 0x00B0 | (thchan & 0x000F));
			      Bconout (3, 0x007F & Ctrl);
			      Bconout (3, 0x007F & Valu);

			    }
			  else
			    {

			      printf ("\nERROR:  out of range  (0..127)\n");
			    }
			}

		    }
		  else
		    {

		      printf ("\nERROR:  out of range  (0..127)\n");
		    }
		}

	      break;

	    case 'W':
	    case 'w':		/* w = write to SNAPFILE */

	      if ((FILE *) NULL EQ (sfp = fopenb (SNAPFILE, "w")))
		{

		  printf ("ERROR -- Unable to open \"%s\" for output.\n",
			  SNAPFILE);

		  exit (2);

		}
	      else
		{

		  printf ("\n\nWriting to file \"%s\".\n", SNAPFILE);
		}

	      fwrite (newbuf + 1, indx, 1, sfp);

	      fflush (sfp);
	      fclose (sfp);

	      printf ("\nFile written and closed.\n\n");
	      break;
	    }
	}

      if (m_stat ())
	PrMIDI (midi_in ());

      if ((FILE *) NULL NE ofp)
	fflush (ofp);
    }

  if ((FILE *) NULL NE ofp)
    {				/* close the log file if it's open */

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

  /* close up shop and exit */

  printf ("\n");
  fflush (stdout);
  exit (0);
}
