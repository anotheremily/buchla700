/*
   =============================================================================
	clusmap.c -- various file structure utilities
	Version 9 -- 1987-10-29 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "biosdefs.h"
#include "errno.h"
#include "io.h"
#include "stdio.h"
#include "stddefs.h"

extern	int	micons(), _gtcl12();
extern	long	miconl();

extern	struct	bpb	*_thebpb;
extern	short	_thefat[];

extern	int	_filecl(), _noper();

/* 
*/

static char	*mfname[] = {	/* FCB flag names */

	"?D0",	/* D0  - 0001 */
	"?D1",	/* D1  - 0002 */
	"?D2",	/* D2  - 0004 */
	"?D3",	/* D3  - 0008 */

	"BF ",	/* D4  - 0010 */
	"NB ",	/* D5  - 0020 */
	"TR ",	/* D6  - 0040 */
	"EX ",	/* D7  - 0080 */

	"RD ",	/* D8  - 0100 */
	"WR ",	/* D9  - 0200 */
	"AP ",	/* D10 - 0400 */
	"CR ",	/* D11 - 0800 */

	"OPN",	/* D12 - 1000 */
	"ERR",	/* D13 - 2000 */
	"BAD",	/* D14 - 4000 */
	"EOF"	/* D15 - 8000 */
};

/* 
*/

static	char	*dfname[] = {

	"RDONLY",	/* D0 - 01 */
	"HIDDEN",	/* D1 - 02 */
	"SYSTEM",	/* D2 - 04 */
	"VOLUME",	/* D3 - 08 */
	"SUBDIR",	/* D4 - 10 */
	"ARCHIV",	/* D5 - 20 */
	"??D6??",	/* D6 - 40 */
	"??D7??"	/* D7 - 80 */
};

static	char	*ffname[] = {

	"BUSY  ",	/* D0 - 01 */
	"ALLBUF", 	/* D1 - 02 */
	"DIRTY ", 	/* D2 - 04 */
	"EOF   ",	/* D3 - 08 */
	"IOERR ",	/* D4 - 10 */
	"??D5??",	/* D5 - 20 */
	"??D6??",	/* D6 - 40 */
	"??D7??"	/* D7 - 80 */
};

/* 
*/

static int
waitcr()
{
	char	c;

	BIOS(B_PUTC, CON_DEV, '\007');

	while ('\r' NE (c = (0x7F & BIOS(B_GETC, CON_DEV))))
		if (c EQ '\007')
			xtrap15();
}

/* 
*/

/*
   =============================================================================
	ClusMap(fcp) -- print a map of the clusters for the file associated with
	the FCB pointed to by 'fcp'.  Nothing is printed if the file isn't open.
	Returns 0 if a map was printed, -1 otherwise.
   =============================================================================
*/

int
ClusMap(fcp)
struct fcb *fcp;
{
	int	clus, nc;
	long	alsize, bused, bunused;

	if (!(fcp->modefl & FC_OPN)) {

		errno = EINVAL;
		return(FAILURE);
	}

	nc = 0;
	clus = micons(fcp->de.bclust);

	if (clus) {

		printf("Allocated cluster chain for [%-8.8s].[%-3.3s]:\n%6d",
			fcp->de.fname, fcp->de.fext, clus);
		nc = 1;	

		while (clus < 0xFF0) {

			clus = _gtcl12(_thefat, clus);

			if (clus < 0xFF0) {

				nc++;

				if (0 EQ (nc-1) % 10)
					printf("\n");

				printf("%6d", clus);
			}
		}
	}

	alsize = nc * _thebpb->clsizb;
	bused = fcp->curlen;
	bunused = alsize - bused;

	printf("\nFAT cluster count=%d, asects=%ld\n", nc, fcp->asects);
	printf("%ld bytes allocated, %ld bytes used", alsize, bused);

	if (alsize GE bused)
		printf(", %ld bytes unused", bunused);

	printf("\n");

	if (bused GT alsize)
		printf("ERROR:  directory file size exceeds FAT allocation\n");

	if (fcp->asects NE nc)
		printf("ERROR:  FAT cluster count (%d) NE FCB cluster count (%ld)\n",
			nc, fcp->asects);
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	FCBmode(fcp) -- print FCB mode flags
   =============================================================================
*/

struct fcb *
FCBmode(fcp)
register struct fcb *fcp;
{
	register unsigned short mf;
	register short i;

	printf("  flags:  ");
	mf = 0x0001;

	for (i = 0; i < 16; i++) {

		if (fcp->modefl & mf)
			printf("%s ", mfname[i]);

		mf <<= 1;
	}

	printf("\n  atrib:  ");

	mf = 0x01;

	for (i = 0; i < 8; i++) {

		if (fcp->de.atrib & mf)
			printf("%s ", dfname[i]);

		mf <<= 1;
	}

	printf("\n");
	return(fcp);
}


/* 
*/

/*
   =============================================================================
	SnapFCB(fcp) -- print contents of an FCB pointed to by 'fcp'
   =============================================================================
*/

struct fcb *
SnapFCB(fcp)
register struct fcb *fcp;
{
	printf("\nFCB at 0x%08lx:  [%-8.8s].[%-3.3s]\n",
		fcp, fcp->de.fname, fcp->de.fext);
	FCBmode(fcp);

	printf("  atrib      0x%04x",	fcp->de.atrib);
	printf("  modefl     0x%04x\n",	fcp->modefl);

	printf("  crtime     0x%04x",	micons(fcp->de.crtime));
	printf("  crdate     0x%04x\n",	micons(fcp->de.crdate));

	printf("  asects   %8ld",	fcp->asects);
	printf("  flen     %8ld",	miconl(fcp->de.flen));
	printf("  curlen   %8ld\n",	fcp->curlen);

	printf("  bclust   %8d",	micons(fcp->de.bclust));
	printf("  curcls   %8d",	fcp->curcls);
	printf("  clsec    %8d\n",	fcp->clsec);

	printf("  curlsn   %8ld",	fcp->curlsn);
	printf("  curdsn   %8ld",	fcp->curdsn);
	printf("  offset   %8d\n",	fcp->offset);

	printf("\n");
	return(fcp);
}

/* 
*/

/*
   =============================================================================
	MapFAT() -- print the first 'ncl' cluster entries from 'fat'
   =============================================================================
*/

MapFAT(fat, ncl, stops)
register char *fat;
short ncl, stops;
{
	register int i;

	printf("\nCluster dump of FAT at 0x%08.8lx  (%d entries):\n",
		fat, ncl);
	printf("   0: .... .... ");

	for (i = 2; i < ncl; i++) {

		if ((i % 10) EQ 0)
			printf("\n%4.4d: ", i);

		printf("%4.4d ", _gtcl12(fat, i));

		/* stop every 10 lines if requested */

		if (stops AND (((i / 10) % 10) EQ 0))
			waitcr();
	}

	printf("\n");
}

/* 
*/

/*
   =============================================================================
	FILEfl() -- print FILE flags
   =============================================================================
*/

FILE *
FILEfl(fp)
FILE *fp;
{
	register unsigned short mf;
	register short i;

	printf("  _flags:  ");
	mf = 0x0001;

	for (i = 0; i < 8; i++) {

		if (fp->_flags & mf)
			printf("%s ", ffname[i]);

		mf <<= 1;
	}

	printf("\n");
	return(fp);
}

/* 
*/

/*
   =============================================================================
	FILEpr(fp) -- print contents of a FILE structure pointed to by 'fp'
   =============================================================================
*/

FILEpr(fp)
FILE *fp;
{
	int	(*arg)(), ft;
	char	*ds, *fsn, *fse;
	struct	fcb	*fcp;

	if (fp EQ (FILE *)0L) {

		printf("FILEpr():  ERROR - argument was NULL\n");
		return;
	}


	printf("\nFILE at $%08.8lX", fp);

	arg = chantab[fp->_unit].c_close;
	ft = 0;

	if (arg EQ _noper) {

		ds = (struct device *)chantab[fp->_unit].c_arg->d_name;
		printf(" is a device:  [%s]\n", ds);

	} else if (arg EQ _filecl) {

		ft = 1;
		fcp = (struct fcb *)chantab[fp->_unit].c_arg;
		fsn = fcp->de.fname;
		fse = fcp->de.fext;
		printf(" is a disk file:  [%8.8s].[%3.3s],  fcb at $%08.8lX\n",
			fsn, fse, fcp);

	} else {

		printf(" is of UNKNOWN type: c_close=$%08.8lX, c_arg=$%08.8lX\n",
			arg, chantab[fp->_unit].c_arg);
	}

	printf("  _buff=$%08.8lX, _bp=$%08.8lX, _bend=$%08.8lX, _buflen=%u\n",
		fp->_buff, fp->_bp, fp->_bend, fp->_buflen);
	printf("  _flags=$%04.4X, _unit=%d, _bytbuf=$%02.2X\n",
		fp->_flags, fp->_unit, fp->_bytbuf);

	FILEfl(fp);

	if (ft)
		SnapFCB(fcp);
	else
		printf("\n");
}

/* 
*/

/*
   =============================================================================
	fd2fcb() -- convert a unit number to a pointer to a fcb
   =============================================================================
*/

struct fcb *
fd2fcb(fd)
short fd;
{
	if ((fd < 0) OR (fd > MAXCHAN))
		return((struct fcb *)NULL);

	return(chantab[fd].c_arg);
}

/*
   =============================================================================
	fp2fcb() -- convert a FILE pointer to a pointer to a fcb
   =============================================================================
*/

struct fcb *
fp2fcb(fp)
FILE *fp;
{
	if (fp EQ (FILE *)NULL)
		return((struct fcb *)NULL);

	return(chantab[fp->_unit].c_arg);
}
