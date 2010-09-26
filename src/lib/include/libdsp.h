/*
   =============================================================================
	libdsp.h -- librarian common parameter header file
	Version 18 -- 1988-11-28 -- D.N. Lynx Crowe

	Uses definitions from:  "graphdef.h", "midas.h", "vsdd.h"
   =============================================================================
*/

#define	LIBROBJ		8		/* librarian display object number */
#define	LIBRPRI		8		/* librarian display object priority */

#define	LCURX		32		/* initial cursor x location */
#define	LCURY		316		/* initial cursor y location */

#define	TTCURC		1		/* typewriter cursor color */
#define	TTBACK		10		/* typewriter background color */

#define	LBORD		11		/* librarian display border color */
#define	LUNDRLN		12		/* librarian title underline color */
#define	LD_DELC		14		/* librarian file delete color */
#define	HILORC		6		/* librarian autoload hilite color */

#define	LD_EMCF		14		/* error message foreground color */
#define	LD_EMCB		9		/* error message background color */
#define	LD_SELC		13		/* file selected foreground color */

#define	LCFBX00		12		/* foreground color */
#define	LCFBX01		12		/* foreground color */
#define	LCFBX02		12		/* foreground color */
#define	LCFBX03		12		/* foreground color */
#define	LCFBX04		12		/* foreground color */
#define	LCFBX05		12		/* foreground color */
#define	LCFBX06		12		/* foreground color */
#define	LCFBX07		12		/* foreground color */
#define	LCFBX08		12		/* foreground color */
#define	LCFBX09		12		/* foreground color */
#define	LCFBX10		12		/* foreground color */

#define	LCBBX00		2		/* background color */
#define	LCBBX01		2		/* background color */
#define	LCBBX02		3		/* background color */
#define	LCBBX03		4		/* background color */
#define	LCBBX04		5		/* background color */
#define	LCBBX05		6		/* background color */
#define	LCBBX06		7		/* background color */
#define	LCBBX07		7		/* background color */
#define	LCBBX08		7		/* background color */
#define	LCBBX09		8		/* background color */
#define	LCBBX10		9		/* background color */

/* 
*/

#define	FCMAX		20		/* number of file catalog entries */

#define	CATNAME		"M7FILES.CAT"	/* catalog file name */

/* file types */

#define	FT_ASG		1		/* Assignments */
#define	FT_ORL		2		/* Orchestra (old Lo Orch) */
#define	FT_ORH		3		/* Orchestra (old Hi Orch) */
#define	FT_SCR		4		/* Score */
#define	FT_TUN		5		/* Tunings */
#define	FT_WAV		6		/* Waveshapes */
#define	FT_ORC		7		/* Orchestra */
#define	FT_PAT		8		/* Patches */
#define	FT_SEQ		9		/* Sequences */

#define	NFTYPES		9		/* number of file types */

/* load types */

#define	LT_ASG		0		/* Assignment */
#define	LT_ORL		1		/* Orchestra - Lo */
#define	LT_ORH		2		/* Orchestra - Hi */
#define	LT_SCR		3		/* Score */
#define	LT_TUN		4		/* Tuning */
#define	LT_WAV		5		/* Waveshape */
#define	LT_PAT		6		/* Patches */
#define	LT_SEQ		7		/* Sequences */

#define	NLTYPES		8		/* number of load types */

/* 
*/

struct	scndx {		/* score index */

	long	sclen;		/* score length */
	char	scfnm[16];	/* score name */
};

struct	fcat {		/* file catalog entry */

	char	fcsize[3];	/* allocated size in clusters */
	char	fcp0;		/* parameter 0 - Autoload flag */
	char	fcname[8];	/* directory name */
	char	fcp1;		/* parameter 1 - -reserved- */
	char	fcextn[3];	/* directory extension  (file type) */
	char	fcp2;		/* parameter 2 - -reserved- */
	char	fccmnt[37];	/* comment field */
	char	fceol[2];	/* end of line  (CR, LF) */
};

struct	mlibhdr	{	/* MIDAS library file header */

	char	l_csum[8];	/* checksum in HEX ASCII, of all but l_csum[] */
	char	l_name[8];	/* file name */
	char	l_type[3];	/* file type */
	char	l_cmnt[37];	/* comment */
};

#define	LH_LEN		(sizeof (struct mlibhdr))

#define	OR_LEN1		((long)&idefs[0].idhwvaf[0]-(long)&idefs[0].idhflag)
#define	OR_LEN2		(2 * (NUMWPNT + NUMHARM))

#ifndef	M7CAT
extern	struct	fcat	filecat[];
#endif
