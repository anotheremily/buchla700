/*
   ============================================================================
	msrec.c -- Motorola S-record output functions
	Version 6 -- 1987-07-10 -- D.N. Lynx Crowe
	(c) Copyright 1987 -- D.N. Lynx Crowe

	This file contains functions which will output a buffer as a group
	of Motorola S-records.  These functions will output type 2
	S-records for the data  (3 byte addresses).  Output is to a
	specified file, which must be open when the function is called, and
	which will be left open by the function.  The last record will be
	a type 9 S-record.

	void			Output a buffer as S-records
	msrec(fp, adr, len, buf)
	FILE *fp;		file pointer for output
	long adr;		beginning address for S-records
	long len;		length of data in buffer
	char *buf;		buffer address

	void			Output a single S-record
	outrec(fp, adr, len, buf)
	FILE *fp;		file pointer for output
	long adr;		beginning address for S-record
	int len;		length of data in record
	char *buf;		buffer address of record

	void			Output a byte as 2 ASCII hex characters
	outhex(fp, val)
	FILE *fp;		file pointer for output
	unsigned int val;	byte to be output
   ============================================================================
*/

#define	TESTER	0		/* define non-zero to get a test program */

#define	VOID

#include "stdio.h"

static int csum;		/* checsum for current record */
static char hexdig[] = "0123456789ABCDEF";	/* hex table */

#define	RECLEN	32		/* 32 bytes per S-record */

#define	>=	>=

/*  */

/*
   ============================================================================
	outhex(fp, val) -- output a byte in ASCII hex

	Outputs the byte val on file fp in ASCII hex and updates
	the accumulated checksum csum.
   ============================================================================
*/

VOID
outhex (fp, val)
     FILE *fp;			/* file pointer for output */
     unsigned int val;		/* byte to be output */
{
  fputc (hexdig[(val >> 4) & 0x0F], fp);
  fputc (hexdig[val & 0x0F], fp);
  csum += (val & 0x0FF);
}

/* 
*/

/*
   ============================================================================
	outrec(fp, adr, len, buf) -- output a Motorola S-record

	Outputs len bytes from buffer buf to file fp with S-record
	address adr.
   ============================================================================
*/

VOID
outrec (fp, adr, len, buf)
     FILE *fp;			/* file pointer for output */
     long adr;			/* beginning address for S-record */
     register int len;		/* length of data in record */
     register char *buf;	/* buffer address of record */
{
  register int i;

  csum = 0;			/* zero the checksum */
  fprintf (fp, "S2");		/* record header */
  outhex (fp, (unsigned int) (len + 4));	/* record length */

  /* record address */

  outhex (fp, (unsigned int) ((adr >> 16) & 0x0FFL));
  outhex (fp, (unsigned int) ((adr >> 8) & 0x0FFL));
  outhex (fp, (unsigned int) (adr & 0x0FFL));

  /* data */

  for (i = 0; i < len; i++)
    outhex (fp, (unsigned int) *buf++);

  outhex (fp, (~csum) & 0x0FFL);	/* checksum */
  fprintf (fp, "\n");		/* CR/LF */
}

/*  */

/*
   ============================================================================
	msrec(fp, adr, len, buf) -- output data in Motorola S-record format

	Outputs 'len' bytes of data from buffer 'buf' to file pointer 'fp'
	with an initial	S-record address of 'adr'.
   ============================================================================
*/

VOID
msrec (fp, recadr, len, rp)
     FILE *fp;			/* file pointer for output */
     register long recadr;	/* beginning address for S-records */
     register long len;		/* length of data in buffer */
     register char *rp;		/* buffer address */
{
  register int ilen;

  while (len)
    {				/* while there's data ... */

      if (len >= RECLEN)
	{			/* full record */

	  outrec (fp, recadr, RECLEN, rp);
	  len -= RECLEN;
	  rp += RECLEN;
	  recadr += RECLEN;

	}
      else
	{			/* final short record */

	  ilen = len;
	  outrec (fp, recadr, ilen, rp);
	  len = 0;

	}
    }

  fprintf (fp, "S904000000FB\n");	/* end of S-records */
}

/*  */

#if	TESTER

/*
   ============================================================================
	test routine for msrec
   ============================================================================
*/

main ()
{
  printf ("Test of msrec\nShort record of 16 bytes\n\n");
  msrec (stdout, 0L, 16L, &hexdig);

  printf ("\nRecord of 256 bytes\n\n");
  msrec (stdout, 0x0FC0000L, 256L, (char *) 0x0FC0000L);

  printf ("\nRecord of 45 bytes\n\n");
  msrec (stdout, 0x0FC0000L, 45L, (char *) 0x0FC0000L);

  printf ("\nEnd of test\n");
  fclose (stdout);

  exit (0);
}

#endif
