/*
   =============================================================================
	mem.c -- show some critical memory allocation values
	Version 1 -- 1988-02-01 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "stdio.h"
#include "osbind.h"

typedef unsigned word;

struct basepg
{

  char *lowtpa;			/* low TPA (basepage) address */
  char *hitpa;			/* high TPA (initial stack) address */
  word *tbase;			/* text segment base */
  long tlen;			/* text segment length */
  word *dbase;			/* data segment base */
  long dlen;			/* data segment length */
  word *bbase;			/* bss segment base */
  long blen;			/* bss segment length */
  char *dta;			/* initial DTA */
  struct basepg *parent;	/* pointer to parent's basepage */
  long resptr;			/* reserved pointer */
  char *envstr;			/* environment string pointer */
};

extern char *_break;
extern struct basepg *_base;

/* 
*/

main ()
{
  long savessp, memsize, tpasize, syssize;
  long *phystop, *membot, *memtop, *v_basad, *sysbase, *end_os;
  long pt, mb, mt, vb, sb, eo;

  phystop = (long *) 0x000042EL;
  membot = (long *) 0x0000432L;
  memtop = (long *) 0x0000436L;
  v_basad = (long *) 0x000044EL;
  sysbase = (long *) 0x00004F2L;
  end_os = (long *) 0x00004FAL;

  printf ("GEMDOS System memory allocation information:\n\n");

  savessp = Super (0L);		/* do this in supervisor mode */

  mb = *membot;			/* TPA start */
  mt = *memtop;			/* TPA end */
  pt = *phystop;		/* physical RAM top */
  sb = *sysbase;		/* system base */
  eo = *end_os;			/* OS RAM end */
  vb = *v_basad;		/* video RAM base */

  Super (savessp);		/* back to user mode */

  memsize = mt - mb;		/* size of the maximum possible TPA */
  tpasize = (long) _base->hitpa - (long) _base->lowtpa;	/* actual TPA */
  syssize = memsize - tpasize;

  printf ("   membot  = %08.8lx\n", mb);
  printf ("   memtop  = %08.8lx\n", mt);
  printf ("   memsize = %ld bytes\n\n", memsize);

  printf ("   TPA low = %08.8lx\n", _base->lowtpa);
  printf ("   TPA hi  = %08.8lx\n", _base->hitpa);
  printf ("   TPA len = %ld bytes\n\n", tpasize);

  printf ("   syssize = %ld bytes\n\n", syssize);

  printf ("   tbase   = %08.8lx\n", _base->tbase);
  printf ("   DTA     = %08.8lx\n", _base->dta);
  printf ("   parent  = %08.8lx\n\n", _base->parent);

  printf ("   phystop = %08.8lx\n", pt);
  printf ("   sysbase = %08.8lx\n\n", sb);

  printf ("   end_os  = %08.8lx\n", eo);
  printf ("   v_basad = %08.8lx\n", vb);
}
