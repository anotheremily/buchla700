/*
   =============================================================================
	ptdisp.h -- MIDAS-VII Patch editor display definitions
	Version 10 -- 1988-11-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	OB08LOC		0		/* offset to headings */
#define	OB09LOC		1024		/* offset to patches */
#define	OB10LOC		8192		/* offset to sequence status */
#define	OB11LOC		4096		/* offset to menu */

#define	PDFL_08		(CHR3)		/* heading object flags */
#define	PDFL_09		(CHR3)		/* patch entry object flags  */
#define	PDFL_10		(BIT3)		/* sequence status object flags */
#define	PDFL_11		(CHR3)		/* menu object flags */

#define	PTBATR		0x00E2		/* colors for border */
#define	PTHATR		0x0012		/* colors for headings */

#define	PTPATR		0x0012		/* colors for patches */
#define	PTEATR		0x0012		/* colors for patch entry line */
#define	PTDATR		0x0062		/* colors for patch data entry */

#define	PTMATR		0x0092		/* colors for menu - normal */
#define	PTIATR		0x0012		/* colors for menu - highlighted */

#define	PDSEQFG		1		/* sequence status foreground color */
#define	PDSEQBG		2		/* sequence status background color */
#define	PDSEQRN		6		/* sequence status run color */
#define	PDPTRFG		6		/* patch pointer color */
#define	PDBORFG		14		/* border foreground color */

#define	PDSEQLEN	16		/* width of the sequence display */
#define	PDPATLEN	48		/* width of the patch display */
