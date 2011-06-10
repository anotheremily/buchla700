/*
   =============================================================================
	xport.h -- a "portability" header package
	Version 2 -- 1987-06-15 -- D.N. Lynx Crowe

	This can generate a set of definitions compatible with portab.h if
	ALCYON is defined.
   =============================================================================
*/

/* only define one of the following symbols:  ALCYON, LATTICE */

#define	ALCYON	1
#define	LATTICE	0

/* ************ */

#if	ALCYON
#define	UCHARA	1
#define	m68k	0
#define UCHARS	1		/* non-zero if char is unsigned */
#define	ULONGS	0		/* non-zero if unsigned longs exist */
#define	INT16	1		/* non-zero if int is 16 bits */
#define	VOIDS	0		/* non-zero if we have voids */
#endif

#if	LATTICE
#define UCHARS	0		/* non-zero if char is unsigned */
#define	ULONGS	1		/* non-zero if unsigned longs exist */
#define	INT16	0		/* non-zero if int is 16 bits */
#define	VOIDS	1		/* non-zero if we have voids */
#endif

#if	VOIDS
#define	VOID	void		/* void function return */
#else
#define	VOID	int		/* void function return */
#endif

#define	BYTE	char		/* signed byte */
#define BOOLEAN	short		/* 2 valued (true/false) */

#if	INT16
#define	WORD	int		/* signed word (16 bits) */
#define	UWORD	unsigned int	/* unsigned word */
#else
#define	WORD	short		/* signed word (16 bits) */
#define	UWORD	unsigned short	/* unsigned word */
#endif

#define	LONG	long		/* signed long (32 bits) */

#if	ULONGS
#define	ULONG	unsigned long	/* Unsigned long */
#else
#define	ULONG	long		/* Unsigned long */
#endif

#if	UCHARS
#define UBYTE	char		/* Unsigned byte */
#else
#define	UBYTE	unsigned char	/* Unsigned byte */
#endif

#define	REG	register	/* register variable */
#define	LOCAL	auto		/* Local var on 68000 */
#define	EXTERN	extern		/* External variable */
#define	MLOCAL	static		/* Local to module */
#define	GLOBAL	/**/		/* Global variable */
#define	DEFAULT	int		/* Default size */

#define	FAILURE	(-1)		/* Function failure return val */
#define SUCCESS	(0)		/* Function success return val */

#define	YES	1		/* Function affirmative response */
#define	NO	0		/* Function negative response */

#define	FOREVER	for(;;)		/* Infinite loop declaration */

#define	NULL	0		/* Null pointer value */
#define NULLPTR (char *) 0	/* Null character pointer */

#define	EOF	(-1)		/* EOF Value */

#define	TRUE	(1)		/* Function TRUE  value */
#define	FALSE	(0)		/* Function FALSE value */
