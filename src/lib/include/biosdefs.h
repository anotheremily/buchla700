/*
   ============================================================================
	biosdefs.h -- Buchla 700 BIOS and XBIOS definitions
	Version 22 -- 1988-04-11 -- D.N. Lynx Crowe
   ============================================================================
*/

extern	long	trap13();	/* BIOS linkage */
extern	long	trap14();	/* XBIOS linkage */

#define	BIOS	trap13
#define	XBIOS	trap14

/* BIOS functions */

/* function 0 unused (getmpb) */

#define	B_RDAV	1
#define	B_GETC	2
#define	B_PUTC	3
#define	B_RDWR	4
#define	B_SETV	5

/* function 6 unused (tickcal) */

#define	B_GBPB	7
#define	B_THRE	8
#define	B_MCHG	9
#define	B_DMAP	10

/* function 11 unused (shift) */

/* XBIOS functions */

#define	X_PIOREC	0
#define	X_SETPRT	1
#define	X_FLOPRD	2
#define	X_FLOPWR	3
#define	X_FORMAT	4
#define	X_VERIFY	5
#define	X_PRBOOT	6
#define	X_RANDOM	7
#define	X_ANALOG	8
#define	X_CLRAFI	9
#define	X_APICHK	10
#define	X_MTDEFS	11

/* device unit numbers */

#define	PRT_DEV	0
#define	AUX_DEV	1
#define	CON_DEV	2
#define	MC1_DEV	3
#define	MC2_DEV	4

/*  */
/*
   ============================================================================
	bpb -- BIOS parameter block
   ============================================================================
*/


struct bpb {			/* BIOS parameter block returned by B_GBPB */

	unsigned short	recsiz;		/* physical sector size in bytes */
	unsigned short	clsiz;		/* cluster size in sectors */
	unsigned short	clsizb;		/* cluster size in bytes */
	unsigned short	rdlen;		/* root directory length in sectors */
	unsigned short	fsiz;		/* FAT size in sectors */
	unsigned short	fatrec;		/* sector number of 1st sector of 2nd FAT */
	unsigned short	datrec;		/* sector number of 1st data sector */
	unsigned short	numcl;		/* number of data clusters on disk */
	unsigned short	bflags;		/* flags */
	unsigned short	ntracks;	/* number of tracks */
	unsigned short	nsides;		/* number of sides (heads) */
	unsigned short	dspc;		/* sectors per cylinder */
	unsigned short	dspt;		/* sectors per track */
	unsigned short	hidden;		/* number of hidden files */
	char	serno[3];		/* disk serial number */
	char	nada;			/* --- filler - nothing here --- */
};

#define	BPBSIZE	32		/* length of a BPB */

#define	B_FAT16	0x0001		/* flag for using 16 bit FAT entries */

/* 
*/

/*
   ============================================================================
	dirent -- Directory entry
   ============================================================================
*/

struct dirent {			/* Directory entry */

	char	fname[8];		/* file name */
	char	fext[3];		/* file extension */
	char	atrib;			/* attribute byte */
	char	unused[10];		/* unused space */
	unsigned short	crtime;		/* 'LLHH' creation time */
	unsigned short	crdate;		/* 'LLHH' creation date */
	unsigned short	bclust;		/* 'LLHH' starting cluster number */
	long	flen;			/* 'LLHH' file length */
};

#define	DENTSIZE	(sizeof (struct dirent))

#define	F_RDONLY	0x01	/* Read only file */
#define	F_HIDDEN	0x02	/* Hidden file */
#define	F_SYSTEM	0x04	/* System file */
#define	F_VOLUME	0x08	/* Volume label */
#define	F_SUBDIR	0x10	/* Sub-directory */
#define	F_ARCHIV	0x20	/* Archived */

/* 
*/

/*
   ============================================================================
	fcb -- File Control Block
   ============================================================================
*/

struct	fcb {			/* file control block */

	struct	dirent	de;	/* directory entry image for file */
	unsigned short	modefl;	/* fcb flags */
	unsigned short	clsec;	/* current sector in cluster */
	unsigned short	curcls;	/* current cluster */
	unsigned short	offset;	/* current offset into sector */
	long	curlsn;		/* current logical sector number in file */
	long	curdsn;		/* current logical sector number on disk */
	long	curlen;		/* current file length in bytes */
	long	asects;		/* allocated file length in sectors */
};

#define	FC_EOF	0x8000		/* end of file */
#define	FC_BAD	0x4000		/* bad FAT entry encountered */
#define	FC_ERR	0x2000		/* error encountered */
#define	FC_OPN	0x1000		/* file open */

#define	FC_CR	0x0800		/* create mode */
#define	FC_AP	0x0400		/* append mode */
#define	FC_WR	0x0200		/* write mode */
#define	FC_RD	0x0100		/* read mode */

#define	FC_EX	0x0080		/* exclusive mode */
#define	FC_TR	0x0040		/* truncate mode */
#define	FC_NB	0x0020		/* non-blocking mode */
#define	FC_BF	0x0010		/* binary file mode */

#define	FC_RW	(FC_RD | FC_WR)	/* read-write mode */

/*  */

/*
   ============================================================================
    BIOS and XBIOS function call formats
    ------------------------------------

	In the list below, .W and .L refer to the size of the parameter
	passed to the BIOS/XBIOS routine.

	The first parameter is always the function code, "code.W".

   BIOS calls are:
   ---------------

	BIOS(B_RDAV, unit.W);
	BIOS(B_GETC, unit.W);
	BIOS(B_PUTC, unit.W, c.W);
	BIOS(B_RDWR, rwflag.W, buf.L, count.W, recno.W, dev.W);
	BIOS(B_SETV, vecnum.W, vecadr.L);
	BIOS(B_GBPB, dev.W);
	BIOS(B_THRE, unit.W);
	BIOS(B_MCHG, unit.W);
	BIOS(B_DMAP);

   XBIOS calls are:
   ----------------

	XBIOS(X_PIOREC, unit.W)
	XBIOS(X_SETPRT, unit.W, mode.W, baud.W, cfr0.W, cr1.W)
	XBIOS(X_FLOPRD, buf.L, filler.L, dev.W, sector.W, track.W,
		side.W, count.W)
	XBIOS(X_FLOPWR, buf.L, filler.L, dev.W, sector.W, track.W,
		side.W, count.W)
	XBIOS(X_FORMAT, buf.L, filler.L, dev.W, spt.W, track.W, side.W,
		intrlv.W, magic.L, virgin.W)
	XBIOS(X_VERIFY, buf.L, filler.L, dev.W, sector.W, track.W,
		side.W, count.W)
	XBIOS(X_PRBOOT, buf.L, serial.L, type.W, execflag.W)
	XBIOS(X_RANDOM)
	XBIOS(X_ANALOG)
	XBIOS(X_CLRAFI)
   ============================================================================
*/
