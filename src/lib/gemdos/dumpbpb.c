/*
   =============================================================================
	dumpbpb.c -- dump a standard GEMDOS BPB in readable form
	Version 2 -- 1988-09-23 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"

#define	TESTER	1		/* to get an Atari test program */

struct bpb
{				/* BIOS parameter block returned by GetBPB() */

  unsigned short recsiz;	/* physical sector size in bytes */
  unsigned short clsiz;		/* cluster size in sectors */
  unsigned short clsizb;	/* cluster size in bytes */
  unsigned short rdlen;		/* root directory length in sectors */
  unsigned short fsiz;		/* FAT size in sectors */
  unsigned short fatrec;	/* sector number of 1st sector of 2nd FAT */
  unsigned short datrec;	/* sector number of 1st data sector */
  unsigned short numcl;		/* number of data clusters on disk */
  unsigned short bflags;	/* flags */
};

/* 
*/

/*
   =============================================================================
	DumpBPB() -- dump a standard BPB in readable form for the Atari
   =============================================================================
*/

struct bpb *
DumpBPB (bpp)
     struct bpb *bpp;		/* pointer to the BPB */
{
  printf ("Sector length:            %u bytes\n", bpp->recsiz);
  printf ("Cluster size:             %u sectors\n", bpp->clsiz);
  printf ("                          %u bytes\n", bpp->clsizb);
  printf ("Root directory size:      %u sectors\n", bpp->rdlen);
  printf ("FAT size:                 %u sectors\n", bpp->fsiz);
  printf ("First sector of 2nd FAT:  %u\n", bpp->fatrec);
  printf ("First sector of data:     %u\n", bpp->datrec);
  printf ("Data area size:           %u clusters\n", bpp->numcl);
  printf ("FAT entry size:           %u bits\n",
	  bpp->bflags & 0x0001 ? 16 : 12);
  printf ("\n");

  return (bpp);
}

/* 
*/

#if	TESTER

extern long bios ();

#define	GetBPB(x)	(struct bpb *)bios(7,x)

/*
   =============================================================================
	simple test program for the Atari  (Beware:  no error checking is done)
   =============================================================================
*/

main (argc, argv)
     int argc;
     char *argv[];
{
  short d;
  char c;
  struct bpb *bp;

  while (--argc)
    {

      c = **++argv;

      if (c >= 'A' && c <= 'Z')
	d = c - 'A';
      else if (c >= 'a' && c <= 'z')
	d = c - 'a';
      else
	continue;

      if (bp = GetBPB (d))
	{

	  printf ("BPB for drive %c:\n\n", d + 'A');
	  DumpBPB (bp);
	}
    }

  exit (0);
}

#endif
