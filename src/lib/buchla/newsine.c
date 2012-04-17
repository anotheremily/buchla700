/*
   ============================================================================
	newsine.c -- Buchla 700 sine PROM generator
	see VER for version message -- written by:  D.N. Lynx Crowe

	Creates two Motorola S-Record files for the Sine PROM:
		one for the MS bytes, and one for the LS bytes.
   ============================================================================
*/

#define	VER	"Version 10 -- 1987-12-19"

#include "stdio.h"
#include "limits.h"
#include "math.h"
#include "stddefs.h"

#define VOID	void

#define TWOPI	((double)2.0 * (double)PI)	/* 2 * PI */

#define	ISHIFT	((double)PI * (double)0.625)	/* phase shift */
#define	OSCALE	((double)30000.0)	/* output scale factor */

#define	BRLEN	15		/* address length */
#define TABLEN	32768		/* sine table length (integer) */
#define	DTABLEN	((double)32768.0)	/* sine table length (real) */

#define	LO_FILE	"SINELO.MOT"	/* LS byte file name */
#define	HI_FILE	"SINEHI.MOT"	/* MS byte file name */
#define	THE_LOG	"SINE.LOG"	/* log file name */

#define RECLEN	32		/* 32 bytes per S-record */
#define	BUFLEN	512		/* LS/MS buffer length */

char *fname1;			/* file name for LS bytes */
char *fname2;			/* file name for MS bytes */
char *fname3;			/* file name for log */

int t[TABLEN];			/* sine table */

char hi_buf[BUFLEN];		/* buffer for MS bytes */
char lo_buf[BUFLEN];		/* buffer for LS bytes */

int csum;			/* current record checksum */

unsigned bitmask[] = {		/* bit mask table for bitrev */

  0x0001, 0x0002, 0x0004, 0x0008,
  0x0010, 0x0020, 0x0040, 0x0080,
  0x0100, 0x0200, 0x0400, 0x0800,
  0x1000, 0x2000, 0x4000, 0x8000
};

char hexdig[] = "0123456789ABCDEF";	/* hex table */

/* 
*/

/*
   ============================================================================
	msdone() -- outputs the final S-Record
   ============================================================================
*/

VOID
msdone (fp)
     FILE *fp;
{
  fprintf (fp, "S904000000FB\n");	/* end of S-records */
}

/*
   ============================================================================
	outhex(fp,val) -- output a byte in ASCII hex

	Outputs the byte 'val' on file 'fp' in ASCII hex and updates
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
	outrec(fp,adr,len,buf) -- output a Motorola S-record

	Outputs len bytes from buffer buf to file fp with S-record
	address adr.
   ============================================================================
*/

VOID
outrec (fp, adr, len, buf)
     FILE *fp;			/* file pointer for output */
     long adr;			/* beginning address for S-record */
     int len;			/* length of data in record */
     char *buf;			/* buffer address of record */
{
  int i;

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

/* 
*/

/*
   ============================================================================
	msrec(fp,adr,len,buf) -- output data in Motorola S-record format

	Outputs len bytes of data from buffer buf to file fp with an initial
	S-record address of adr.
   ============================================================================
*/

VOID
msrec (fp, adr, len, buf)
     FILE *fp;			/* file pointer for output */
     long adr;			/* beginning address for S-records */
     long len;			/* length of data in buffer */
     char *buf;			/* buffer address */
{
  long recadr = adr;
  char *rp = buf;
  int j;
  long i = len;

  while (i)
    {				/* while there's data ... */

      if (i >= RECLEN)
	{			/* full record */

	  outrec (fp, recadr, RECLEN, rp);
	  i -= RECLEN;
	  rp += RECLEN;
	  recadr += RECLEN;

	}
      else
	{			/* final short record */

	  j = i;
	  outrec (fp, recadr, j, rp);
	  i = 0;

	}
    }
}

/* 
*/

/*
   =============================================================================
	bitrev(bitsin, nbits) -- reverses the rightmost nbits of bitsin.

	Any bits to the left of the reversed bits in the result will be zeros.
   =============================================================================
*/

unsigned
bitrev (bitsin, nbits)
     unsigned bitsin, nbits;
{
  unsigned m, n;

  n = 0;

  for (m = 0; m < nbits; m++)
    if (bitsin & bitmask[m])
      n |= bitmask[nbits - 1 - m];

  return (n);
}

/* 
*/

/*
   =============================================================================
	main function -- sine.c -- Buchla 700 sine PROM generator
   =============================================================================
*/

main ()
{
  FILE *fp1, *fp2, *fp3;
  register long i, j, nr;
  double k, q, ip, os;

  printf ("Buchla 700 Sine PROM Generator %s\n", VER);

  q = TWOPI / DTABLEN;		/* step size */
  ip = ISHIFT;			/* offset */
  os = OSCALE;			/* output scaling */

  fname1 = LO_FILE;		/* LS byte file */
  fname2 = HI_FILE;		/* MS byte file */
  fname3 = THE_LOG;		/* log file */

  if ((fp1 = fopen (fname1, "wa")) == NULL)
    {

      printf ("sine:  ERROR - couldn't open [%s]\n", fname1);
      exit (1);
    }

  if ((fp2 = fopen (fname2, "wa")) == NULL)
    {

      printf ("sine:  ERROR - couldn't open [%s]\n", fname2);
      fclose (fp1);
      exit (1);
    }

  if ((fp3 = fopen (fname3, "wa")) == NULL)
    {

      printf ("sine:  ERROR - couldn't open [%s]\n", fname3);
      fclose (fp1);
      fclose (fp2);
      exit (1);
    }

  printf ("  size = %ld, step = %f, offset = %f, scaling = %f\n",
	  (long) TABLEN, q, ip, os);

  fprintf (fp3, "Buchla 700 Sine PROM Generator %s\n", VER);
  fprintf (fp3, "  size = %ld, step = %f, offset = %f, scaling = %f\n\n",
	   (long) TABLEN, q, ip, os);
  fprintf (fp3, "Addr Rev  Data Value\n");

  printf ("  output on %s and %s, log on %s\n\n", fname1, fname2, fname3);

/* 
*/
  printf ("Beginning calculation phase.\n\n");

  printf ("Addr Rev  Data Value\n");

  for (i = 0; i < TABLEN; i++)
    {

      k = sin (((double) i * q) + ip);

      if (k >= 1.0)
	k = 0.9999999999;

      t[bitrev (i, BRLEN)] = k * os;

      if (!(i & 0x03FF))
	{

	  printf ("%04x %04x %04x %f\n", i, bitrev (i, BRLEN), t[i], k);

	  fprintf (fp3, "%04x %04x %04x %f\n", i, bitrev (i, BRLEN), t[i], k);
	}
    }
/* 
*/
  printf ("\nSine table created, starting output phase.\n");

  i = 0L;			/* initialize sine table index */
  nr = 0L;			/* initialize output address */

  while (i < (long) TABLEN)
    {

      for (j = 0; j < (long) BUFLEN; j++)
	{			/* split the words */

	  lo_buf[j] = t[i] & 0x00FF;
	  hi_buf[j] = (t[i] >> 8) & 0x00FF;
	  i++;
	}

      msrec (fp1, nr, (long) BUFLEN, lo_buf);	/* write the S-Records */
      msrec (fp2, nr, (long) BUFLEN, hi_buf);

      printf ("S-Record %5ld complete\n", nr);

      nr += (long) BUFLEN;
    }

  msdone (fp1);			/* write final S-Records */
  msdone (fp2);

  fflush (fp1);			/* flush the files */
  fflush (fp2);
  fflush (fp3);

  fclose (fp1);			/* close the files */
  fclose (fp2);
  fclose (fp3);

  exit (0);			/* exit back to the operating system */
}
