/*
   =============================================================================
	ss.c -- show sysem memory allocation values
	Version 1 -- 1988-06-02 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "stdio.h"
#include "osbind.h"

typedef	unsigned	word;

struct	basepg	{

	char	*lowtpa;		/* low TPA (basepage) address */
	char	*hitpa;			/* high TPA (initial stack) address */
	word	*tbase;			/* text segment base */
	long	tlen;			/* text segment length */
	word	*dbase;			/* data segment base */
	long	dlen;			/* data segment length */
	word	*bbase;			/* bss segment base */
	long	blen;			/* bss segment length */
	char	*dta;			/* initial DTA */
	struct	basepg	*parent;	/* pointer to parent's basepage */
	long	resptr;			/* reserved pointer */
	char	*envstr;		/* environment string pointer */
};

extern	struct	basepg	*_base;

/* 
*/

main()
{
	long	savessp, memsize, tpasize, syssize;
	long	*membot, *memtop;
	long	mb, mt;

	membot  = (long *)0x0000432L;
	memtop  = (long *)0x0000436L;

	printf("GEMDOS System memory allocation information:\n");

	savessp = Super(0L);	/* do this in supervisor mode */

	mb = *membot;		/* TPA start */
	mt = *memtop;		/* TPA end */

	Super(savessp);		/* back to user mode */

	memsize = mt - mb;	/* size of the maximum possible TPA */
	tpasize = (long)_base->hitpa - (long)_base->lowtpa;	/* actual TPA */
	syssize = memsize - tpasize;	/* system size */

	printf("syssize = %ld,  tpasize = %ld\n", syssize, tpasize);
}
