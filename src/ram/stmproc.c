/*
   =============================================================================
	stmproc.c -- MIDAS-VII Patch facility support functions
	Version 33 -- 1988-12-06 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGDP		0
#define	DEBUGEP		0
#define	DEBUGFD		0
#define	DEBUGFP		0
#define	DEBUGSR		0

#define	PATCHDEF			/* so patch.h gets it right */

#include "stddefs.h"
#include "graphdef.h"
#include "hwdefs.h"
#include "patch.h"
#include "vsdd.h"
#include "wordq.h"

#include "midas.h"
#include "ptdisp.h"

extern	short	ptegood, ptedfok, ptestok, ptedsok, ptedtok;

extern	short	ptedef, ptestm, ptespec, ptesuba, ptedat1;

extern	short	pteset;

extern	short	ptecpos, ptepred, ptesucc;

extern	struct patch	ptebuf;

extern	char	ptdebuf[];

#if	DEBUGDP
short	debugdp = 1;
#endif

#if	DEBUGEP
short	debugep = 0;
short	snapep  = 1;
#endif

#if	DEBUGFD
short	debugfd = 0;
#endif

#if	DEBUGFP
short	debugfp = 1;
#endif

#if	DEBUGSR
short	debugsr = 1;
short	snapsr  = 1;
#endif

#if	(DEBUGDP|DEBUGEP|DEBUGFD|DEBUGFP|DEBUGSR)
extern	short	debugsw;
#endif

unsigned short	dtfree;			/* defent free list index */
unsigned short	ptfree;			/* patch free list index */

unsigned short	dpepred;		/* defent predecessor index */
unsigned short	dpecpos;		/* current defent index */
unsigned short	dpesucc;		/* defent successor index */

char	ptdsbuf[50];			/* patch display build buffer */

char	defptr[NDEFSTMS];		/* definition index table */
char	stmptr[NDEFSTMS];		/* stimulus index table */

struct	defent	defents[RAWDEFS];	/* definition control table */

struct	patch	patches[MAXPATCH];	/* patch table */

struct	wordq	ptefifo;		/* patch trigger fifo header */

unsigned short	ptewrds[NPTEQELS];	/* patch trigger fifo entries */

char	dmatch[] = {			/* addat1 match tags */

	0, 0, 0, 0, 0, 0, 0, 0,		/*  0 ..  7 */
	0, 0, 0, 1, 0, 0, 0, 0,		/*  8 .. 15 */
	1, 1, 1, 1, 1, 1, 1, 1,		/* 16 .. 23 */
	1, 1, 1				/* 24 .. 26 */
};

/* 
*/

/*
   =============================================================================
	initpt() -- initialize patch table data structure
   =============================================================================
*/

initpt()
{
	register unsigned short i;

	/* initialize the trigger fifo */

	setwq(&ptefifo, ptewrds, NPTEQELS, NPTEQHI, NPTEQLO);

	/* clear DEF / STM index tables */

	memset(defptr, 0, sizeof defptr);
	memset(stmptr, 0, sizeof stmptr);

	/* setup patch free chain */

	memset(patches, 0, sizeof patches);

	for (i = 1; i < 255; i++)
		patches[i].nextstm = 1 + i;

	ptfree = 1;

	/* setup defent free chain */

	memset(defents, 0, sizeof defents);

	for (i = 1; i < 255; i++)
		defents[i].nextdef = 1 + i;

	dtfree = 1;
}

/* 
*/

/*
   =============================================================================
	pt_alc() -- allocate a patch table entry from the free list
   =============================================================================
*/

unsigned short
pt_alc()
{
	register unsigned short pe;

	if (0 NE (pe = ptfree))
		ptfree = patches[pe].nextstm;

	return(pe);
}

/*
   =============================================================================
	pt_del() -- return a patch table entry to the free list
   =============================================================================
*/

pt_del(pe)
register unsigned short pe;
{
	patches[pe].nextstm = ptfree;
	ptfree = pe;
}

/* 
*/

/*
   =============================================================================
	dt_alc() -- allocate a def table entry from the free list
   =============================================================================
*/

unsigned short
dt_alc()
{
	register unsigned short de;

	if (0 NE (de = dtfree))
		dtfree = defents[de].nextdef;

	return(de);
}

/*
   =============================================================================
	dt_del() -- return a def table entry to the free list
   =============================================================================
*/

dt_del(de)
register unsigned short de;
{
	defents[de].nextdef = dtfree;
	dtfree = de;
}

/* 
*/

/*
   =============================================================================
	cprdpe() -- compare patch buffer to def table entry

	Returns:
		-1 = buffer <  table entry
		 0 = buffer EQ table entry
		+1 = buffer >  table entry
   =============================================================================
*/

short
cprdpe(np)
unsigned short np;
{
	register unsigned short ca, cb, ct;
	register struct defent *pp;

	pp = &defents[np];

	if ((cb = ptestm) < (ct = pp->stm))
		return(-1);

	if (cb > ct)
		return(1);

	if ((ca = (PE_SPEC & ptespec)) < (ct = (PE_SPEC & pp->adspec)))
		return(-1);

	if (ca > ct)
		return(1);

	if ((cb = ptesuba) < (ct = pp->adsuba))
		return(-1);

	if (cb > ct)
		return(1);
/* 
*/
	/* check for extended destinations -- they need further testing */

	if (dmatch[ca]) {		/* check destination type */

		if ((cb = ptedat1) < (ct = pp->addat1))
			return(-1);

		if (cb > ct)
			return(1);
		else
			return(0);

	} else {

		return(0);
	}
}

/* 
*/

/*
   =============================================================================
	finddpe() -- find def table entry

	Returns:	+1 = new def -- no DEF entry yet
			 0 = old def -- entry found in DEF chain
			-1 = new def -- entry not in DEF chain

	Sets:		dpepred		predecessor index,  or 0 if none exists
			dpecpos		def table index,    or 0 if none exists
			dpesucc		successor index,    or 0 if none exists
   =============================================================================
*/

short
finddpe()
{
	register unsigned short c, idef;

	dpepred = 0;		/* initialize dpepred = 0  (no predecessor) */
	dpecpos = 0;		/* initialize dpecpos = 0  (no current defent */
	dpesucc = 0;		/* initialize dpesucc = 0  (no successor) */

#if	DEBUGFD
	if (debugsw AND debugfd) {

		printf("finddpe():  entry\n");

		printf("  ptb  $%04.4X $%04.4X   $%04.4X $%04.4X   $%04.4X $%04.4X\n",
			ptebuf.defnum, ptebuf.stmnum, ptebuf.paspec,
			ptebuf.pasuba, ptebuf.padat1, ptebuf.padat2);
	}
#endif

	/* try to find the DEF chain index for the defent in defptr[] */

	if (0 EQ (idef = defptr[TRG_MASK & ptebuf.defnum])) {

#if	DEBUGFD
	if (debugsw AND debugfd) {

		printf("finddpe():   1 -- NO DEF -- ");
		printf("dpe pred: %3d  cpos: %3d  succ: %3d\n",
			dpepred, dpecpos, dpesucc);
	}
#endif

		return(1);	/* +1 = new defent -- no DEF entry yet */
	}

/* 
*/

	while (idef) {		/* search the DEF chain */

		/* compare ptebuf to defents[idef] */

		if (1 NE (c = cprdpe(idef))) {

			if (c EQ 0) {	/* if we find the defent ... */

				dpecpos = idef;	/* ... point at it */
				dpesucc = defents[idef].nextdef;

			}

#if	DEBUGFD
	if (debugsw AND debugfd) {

		printf("finddpe():  %d --  %s -- ", c,
			c ? "NOT FOUND (>)" : "FOUND");

		printf("dpe pred: %3d  cpos: %3d  succ: %3d\n",
			dpepred, dpecpos, dpesucc);
	}
#endif
			return(c);	/* return search result */
		}

		dpepred = idef;	/* point at next entry in DEF chain */
		idef   = defents[idef].nextdef;
	}

#if	DEBUGFD
	if (debugsw AND debugfd) {

		printf("finddpe(): -1 -- NOT FOUND (<) -- ");
		printf("dpe pred: %3d  cpos: %3d  succ: %3d\n",
			dpepred, dpecpos, dpesucc);
	}
#endif

	return(-1);	/* -1 = new defent -- entry not in DEF chain */
}

/* 
*/

/*
   =============================================================================
	cprpte() -- compare patch buffer to patch table entry

	Returns:
		-1 = buffer <  table entry
		 0 = buffer EQ table entry
		+1 = buffer >  table entry
   =============================================================================
*/

short
cprpte(np)
unsigned short np;
{
	register unsigned short ca, cb, ct;
	register struct patch *pb, *pp;

	pb = &ptebuf;
	pp = &patches[np];

	if ((cb = pb->stmnum) < (ct = pp->stmnum))
		return(-1);

	if (cb > ct)
		return(1);

	if ((cb = pb->defnum) < (ct = pp->defnum))
		return(-1);

	if (cb > ct)
		return(1);

	if ((ca = (PE_SPEC & pb->paspec)) < (ct = (PE_SPEC & pp->paspec)))
		return(-1);

	if (ca > ct)
		return(1);

	if ((cb = pb->pasuba) < (ct = pp->pasuba))
		return(-1);

	if (cb > ct)
		return(1);
/* 
*/
	/* check for extended destinations -- they need further testing */

	if (dmatch[ca]) {		/* check destination type */

		if ((cb = pb->padat1) < (ct = pp->padat1))
			return(-1);

		if (cb > ct)
			return(1);
		else
			return(0);

	} else {

		return(0);
	}
}

/* 
*/

/*
   =============================================================================
	findpte() -- find patch table entry

	Returns:	+1 = new patch -- no STM entry yet
			 0 = old patch -- entry found in STM chain
			-1 = new patch -- entry not in STM chain

	Sets:		ptepred		predecessor index,  or 0 if none exists
			ptecpos		patch table index,  or 0 if none exists
			ptesucc		successor index,    or 0 if none exists
   =============================================================================
*/

short
findpte()
{
	register unsigned short c, istim;

	ptepred = 0;		/* initialize ptepred = 0  (no predecessor) */
	ptecpos = 0;		/* initialize ptecpos = 0  (no current patch */
	ptesucc = 0;		/* initialize ptesucc = 0  (no successor) */

#if	DEBUGFP
	if (debugsw AND debugfp) {

		printf("findpte():  entry\n");

		printf("  ptb  $%04.4X $%04.4X   $%04.4X $%04.4X   $%04.4X $%04.4X\n",
			ptebuf.defnum, ptebuf.stmnum, ptebuf.paspec,
			ptebuf.pasuba, ptebuf.padat1, ptebuf.padat2);
	}
#endif

	/* try to find the STM chain index for the patch in stmptr[] */

	if (0 EQ (istim = stmptr[TRG_MASK & ptebuf.stmnum])) {

#if	DEBUGFP
	if (debugsw AND debugfp) {

		printf("findpte():   1 -- NO STM -- ");
		printf("pte pred: %3d  cpos: %3d  succ: %3d\n",
			ptepred, ptecpos, ptesucc);
	}
#endif

		return(1);	/* +1 = new patch -- no STM entry yet */
	}

/* 
*/

	while (istim) {		/* search the STM chain */

		/* compare ptebuf to patches[istim] */

		if (1 NE (c = cprpte(istim))) {

			if (c EQ 0) {	/* if we find the patch ... */

				ptecpos = istim;	/* ... point at it */
				ptesucc = patches[istim].nextstm;

			}

#if	DEBUGFP
	if (debugsw AND debugfp) {

		printf("findpte():  %d --  %s -- ", c,
			c ? "NOT FOUND (>)" : "FOUND");

		printf("pte pred: %3d  cpos: %3d  succ: %3d\n",
			ptepred, ptecpos, ptesucc);
	}
#endif
			return(c);	/* return search result */
		}

		ptepred = istim;	/* point at next entry in STM chain */
		istim   = patches[istim].nextstm;
	}

#if	DEBUGFP
	if (debugsw AND debugfp) {

		printf("findpte(): -1 -- NOT FOUND (<) -- ");
		printf("pte pred: %3d  cpos: %3d  succ: %3d\n",
			ptepred, ptecpos, ptesucc);
	}
#endif

	return(-1);	/* -1 = new patch -- entry not in STM chain */
}

/* 
*/

/*
   =============================================================================
	entrpte() -- enter or update a patch table entry
   =============================================================================
*/

entrpte()
{
	register short c;
	register unsigned short np, stim;

	ptegood = ptedfok AND ptestok AND ptedsok AND ptedtok;

	if (ptegood) {

		buf2pte();

		c = findpte();

		if (c EQ 0) {		/* old patch -- just update it */

			memcpyw(&patches[ptecpos].defnum, &ptebuf.defnum, 6);

			patches[ptecpos].paspec |= PE_TBIT;	/* define it */

#if	DEBUGEP
	if (debugsw AND debugep) {

		if (snapep)
			SnapPTV("entrpte");

		printf("entrpte():  UPDATED\n");
	}
#endif

			return;
		}
/* 
*/
		/* allocate a patch entry and fill it in */

		if (0 EQ (ptecpos = pt_alc())) {

#if	DEBUGEP
	if (debugsw AND debugep)
		printf("entrpte():  patch table FULL\n");
#endif
			return;		/* no patch entries left */
		}

		memcpyw(&patches[ptecpos].defnum, &ptebuf.defnum, 6);
		patches[ptecpos].paspec |= PE_TBIT;	/* define it */
		stim = TRG_MASK & ptebuf.stmnum;

		if (c EQ 1) {		/* new patch -- no STM entry yet */

			ptepred = 0;
			stmptr[stim] = ptecpos;
		}
/* 
*/
		/* put patch in STM chain */

		if (ptepred) {		/* predecessor exits */

			ptesucc = patches[ptepred].nextstm;

			patches[ptecpos].nextstm = ptesucc;
			patches[ptecpos].prevstm = ptepred;

			patches[ptepred].nextstm = ptecpos;

			if (ptesucc)
				patches[ptesucc].prevstm = ptecpos;

		} else {		/* no predecessor */

			patches[ptecpos].prevstm = 0;

			if (c EQ -1) {

				ptesucc = stmptr[stim];

				patches[ptecpos].nextstm = ptesucc;

				patches[ptesucc].prevstm = ptecpos;

				stmptr[stim] = ptecpos;

			} else {

				patches[ptecpos].nextstm = 0;
			}
		}
/* 
*/
		/* update DEF table */

		if (0 EQ (c = finddpe())) {

#if	DEBUGEP
	if (debugsw AND debugep)
		printf("entrpte():  defent already exists\n");
#endif
			return;		/* defent already exists */
		}

		if (0 EQ (dpecpos = dt_alc())) {

#if	DEBUGEP
	if (debugsw AND debugep)
		printf("entrpte():  defent table FULL\n");
#endif
			return;		/* no defents left */
		}

		defents[dpecpos].nextdef = 0;
		defents[dpecpos].stm     = ptestm;
		defents[dpecpos].adspec  = ptespec;
		defents[dpecpos].adsuba  = ptesuba;
		defents[dpecpos].addat1  = ptedat1;

		np = TRG_MASK & ptebuf.defnum;

		if (c EQ 1) {

			dpepred = 0;
			defptr[np] = dpecpos;
		}

		if (dpepred) {

			dpesucc = defents[dpepred].nextdef;
			defents[dpecpos].nextdef = dpesucc;
			defents[dpepred].nextdef = dpecpos;

		} else {

			if (c EQ -1) {

				dpesucc = defptr[np];
				defents[dpecpos].nextdef = dpesucc;
				defptr[np] = dpecpos;

			} else {

				defents[dpecpos].nextdef = 0;
			}
		}

#if	DEBUGEP
	if (debugsw AND debugep) {

		if (snapep)
			SnapPTV("entrpte");

		printf("entrpte():  ENTERED\n");
	}
#endif
		return;
	}

#if	DEBUGEP
	if (debugsw AND debugep) {

		if (snapep)
			SnapPTV("entrpte");

		printf("entrpte():  INVALID\n");
	}
#endif
}

/* 
*/

/*
   =============================================================================
	find1st() -- find the first patch in the patch table
   =============================================================================
*/

short
find1st()
{
	register short cp, pp;

	for (cp = 0; cp < NDEFSTMS; cp++)
		if (0 NE (pp = ADR_MASK & stmptr[cp]))
			return(pp);

	return(0);
}

/*
   =============================================================================
	findnxt() -- find the next patch in the patch table
   =============================================================================
*/

short
findnxt(cp)
short cp;
{
	register short np, stim;

	if (0 NE (np = patches[cp].nextstm))
		return(np);

	stim = TRG_MASK & patches[cp].stmnum;

	while (++stim < NDEFSTMS)
		if (0 NE (np = ADR_MASK & stmptr[stim]))
			return(np);

	return(0);
}

/* 
*/

/*
   =============================================================================
	findprv() -- find the previous patch in the patch table
   =============================================================================
*/

short
findprv(cp)
short cp;
{
	register short np, pp, stim;

	if (0 NE (np = patches[cp].prevstm))	/* return prevstm if set */
		return(np);

	stim = TRG_MASK & patches[cp].stmnum;	/* extract the stimulus */

	while (--stim GE 0) {		/* back up one stimulus if we can */

		if (0 NE (np = ADR_MASK & stmptr[stim])) {	/* any there ? */

			/* find the end of the chain for the stimulus */

			while (pp = patches[np].nextstm)
				np = pp;

			return(np);
		}
	}

	return(0);	/* backed up to the start of the table */
}

/* 
*/

/*
   =============================================================================
	dpte() -- display a patch at a given line on the screen
   =============================================================================
*/

dpte(pe, row, atr)
register short pe, row, atr;
{
	register short i;

	memset(ptdsbuf, ' ', 50);

	if (pe) {

		dspdfst(&ptdsbuf[ 2], patches[pe].defnum);
		dspdfst(&ptdsbuf[15], patches[pe].stmnum);
		dspdest(&ptdsbuf[28], &patches[pe]);

		for (i = 0; i < 50; i++)
			if (ptdsbuf[i] EQ '\0')
				ptdsbuf[i] = ' ';
	
		ptdsbuf[48] = '\0';
	}

	UpdVid(row, 0, "\260 ", PTBATR);
	UpdVid(row, 1, &ptdsbuf[1], atr);
}

/* 
*/

/*
   =============================================================================
	dptw() -- display window around current patch at ptecpos
   =============================================================================
*/

dptw()
{
	register short cp, r, row, pp;

#if	DEBUGDP
	if (debugsw AND debugdp)
		printf("dptw():  ENTRY  ptecpos = %d\n", ptecpos);
#endif

	if (ptecpos) {

		/* search back from ptecpos for predecessors */

		row = 7;
		pp  = ptecpos;

		while (0 NE (cp = findprv(pp))) {

			pp = cp;

			if (--row EQ 0)
				break;
		}

#if	DEBUGDP
	if (debugsw AND debugdp)
		printf("dptw():  backed up to row = %d  pp = %d\n", row, pp);
#endif

		if (row) {	/* blank any unused lines  (rows 0..6) */

			for (r = 0; r < row; r++)
				dpte(0, r, PTPATR);
		}

		while (row < 7) {	/* display predecessors  (rows 0..6) */

			dpte(pp, row++, PTPATR);
			pp = findnxt(pp);
		}
/* 
*/
		/* display ptecpos at the center  (row 7) */

#if	DEBUGDP
	if (debugsw AND debugdp)
		printf("dptw():  row = %d  pp = %d  ptecpos = %d\n",
			row, pp, ptecpos);
#endif

		dpte(pp, row++, PTEATR);

		/* display forward from ptecpos  (rows 8..15) */

		while (0 NE (pp = findnxt(pp))) {

			dpte(pp, row++, PTPATR);

			if (row > 15)
				break;
		}

		/* blank any unused display lines  (rows 8..15) */
		while (row < 16)
			dpte(0, row++, PTPATR);
/* 
*/			
	} else {

		if (0 NE (ptecpos = find1st())) {

#if	DEBUGDP
	if (debugsw AND debugdp)
		printf("dptw():  found 1st at %d\n", ptecpos);
#endif

			/* clear lines above the center (rows 0..6) */

			for (row = 0; row < 7; ++row)
				dpte(0, row, PTPATR);

			/* display ptecpos at the center (row 7) */

			dpte(pp = ptecpos, row++, PTEATR);

			/* display forward from ptecpos (rows 8..15) */

			while (0 NE (pp = findnxt(pp))) {

				dpte(pp, row++, PTPATR);

				if (row > 15)
					break;
			}

			/* blank any unused display lines (rows 8..15) */

			while (row < 16)
				dpte(0, row++, PTPATR);
/* 
*/
		} else {

#if	DEBUGDP
	if (debugsw AND debugdp)
		printf("dptw():  no patches to display\n");
#endif

			/* clear the patch display */

			for (row = 0; row < 16; ++row)
				dpte(0, row, (row EQ 7) ? PTEATR : PTPATR);
		}
	}

	if (ptecpos) {

		memcpyw(&ptebuf.defnum, &patches[ptecpos].defnum, 6);
		pteset = TRUE;
		pte2buf();

#if	DEBUGDP
	if (debugsw AND debugdp)
		printf("dptw():  EXIT -- LOADED buffer,  ptecpos = %d\n",
			ptecpos);
#endif

	} else {

		pteset = FALSE;
		voidpb();

#if	DEBUGDP
	if (debugsw AND debugdp)
		printf("dptw():  EXIT -- VOIDED buffer,  ptecpos = %d\n",
			ptecpos);
#endif


	}
}

/* 
*/

/*
   =============================================================================
	srdspte() -- search for and display patch table entry
   =============================================================================
*/

srdspte()
{
	short	oldcpos, oldpred, oldsucc;
#if	DEBUGSR
	register short i;
	char dbuf[50];
#endif

	ptegood = ptedfok AND ptestok AND ptedsok AND ptedtok;

#if	DEBUGSR
	if (debugsw AND debugsr) {

		printf("srdspte():  ENTRY  pte good=%d dfok=%d stok=%d dsok=%d dtok=%d\n",
			ptegood, ptedfok, ptestok, ptedsok, ptedtok);

		memcpy(dbuf, ptdebuf, 48);

		for (i = 0; i < 48; i++)
			if (dbuf[i] EQ '\0')
				dbuf[i] = ' ';
			else if (dbuf[i] & 0x0080)
				dbuf[i] = '~';

		dbuf[48] = '\0';

		printf("  ptdebuf = \"%s\"\n", dbuf);
	}
#endif

	if (ptegood) {

		oldcpos = ptecpos;	/* save patch pointers */
		oldpred = ptepred;
		oldsucc = ptesucc;

		buf2pte();

		if (0 EQ findpte()) {

#if	DEBUGSR
	if (debugsw AND debugsr)
		printf("srdspte():  FOUND patch at ptecpos = %d\n", ptecpos);
#endif

			memcpyw(&ptebuf.defnum, &patches[ptecpos].defnum, 6);
			pteset = TRUE;
			pte2buf();
			dptw();

		} else {

			ptecpos = oldcpos;	/* restore patch pointers */
			ptepred = oldpred;
			ptesucc = oldsucc;

#if	DEBUGSR
	if (debugsw AND debugsr) {

		printf("srdspte():  patch not found\n");

		if (snapsr)
			SnapPTV("srdspte");

	}
#endif

		}
	}

#if	DEBUGSR
	if (debugsw AND debugsr)
		printf("srdspte():  EXIT -- ptecpos = %d\n", ptecpos);
#endif

}

/* 
*/

/*
   =============================================================================
	stmproc() -- process a trigger as a definer and a stimulus
   =============================================================================
*/

stmproc(trg)
register unsigned short trg;
{
	register struct defent	*nextdef;
	register struct patch	*nextpch;
	register unsigned short adspec, adsuba, np, stim;
	unsigned short addat1, adrtag;

	/* ***** DEFINER PROCESSING PHASE ***** */

	np      = ADR_MASK & defptr[TRG_MASK & trg];
	nextdef = np ? &defents[np] : (struct defent *)NULL;	/* point at DEF chain */

/* 
*/
	while ((struct defent *)NULL NE nextdef) {		/* process DEF chain */

		/* setup search criteria */

		adspec = nextdef->adspec;
		adsuba = nextdef->adsuba;
		addat1 = nextdef->addat1;
		stim   = nextdef->stm;
		adrtag = dmatch[adspec];

		/* point at the start of the STM chain */

		np      = ADR_MASK & stmptr[TRG_MASK & stim];
		nextpch = np ? &patches[np] : (struct patch *)NULL;

		while ((struct patch *)NULL NE nextpch) {	/* process STM chain */

			/* if this patch matches our search criteria ... */

			if ((stim   EQ  nextpch->stmnum) AND
			    (adspec EQ (nextpch->paspec & PE_SPEC)) AND
			    (adsuba EQ  nextpch->pasuba)) {

				if ((NOT adrtag) OR
				    (adrtag AND addat1 EQ nextpch->padat1)) {

					if (nextpch->defnum EQ trg)
						nextpch->paspec |= PE_TBIT;	/* define */
					else
						nextpch->paspec &= ~PE_TBIT;	/* undefine */
				}
			}

			/* point at the next patch in the STM chain */

			np      = nextpch->nextstm;
			nextpch = np ? &patches[np] : (struct patch *)NULL;
		}

		/* point at the next DEF entry */

		np      = nextdef->nextdef;
		nextdef = np ? &defents[np] : (struct defent *)NULL;
	}
/* 
*/

	/* ***** STIMULUS PROCESSING PHASE ***** */

	/* setup initial STM chain pointer */

	np = ADR_MASK & stmptr[TRG_MASK & trg];
	nextpch = np ? &patches[np] : (struct patch *)NULL;

	/* process the STM chain */

	while ((struct patch *)NULL NE nextpch) {	/* for each patch .. */

		if ((nextpch->paspec & PE_TBIT) AND	/* if it's defined ... */
		    (nextpch->stmnum EQ trg))		/* ... and stm matches */
			dopatch(nextpch);		/* ... do the patch */

		/* point at the next patch */

		np = nextpch->nextstm;
		nextpch = np ? &patches[np] : (struct patch *)NULL;
	}
}
