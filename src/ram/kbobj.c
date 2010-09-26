/*
   =============================================================================
	kbobj.c -- keyboard object display data
	Version 6 -- 1988-02-02 -- D.N. Lynx Crowe

	Defines the data for the Keyboard object.  Includes the "new data"
	label.

	Each group of keys is made up of groups of patterns KA..KD,
	organized in 4 types of groups, 0..3, in 4 ways, W..Z.
   =============================================================================
*/

#define	K_BLK	0	/* black keys */
#define	K_WHT	1	/* white keys */
#define	K_GRY	2	/* lines between keys */

#define	LFC	((K_WHT << 12) | (K_WHT << 8) | (K_WHT << 4) | K_WHT)
#define	LFB	((K_GRY << 12) | (K_GRY << 8) | (K_GRY << 4) | K_GRY)

#define	LM(c)	((c & LFC) | ((~c) & LFB))

#define	KA	((K_GRY << 12) | (K_GRY << 8) | (K_GRY << 4) | K_GRY)
#define	KB	((K_WHT << 12) | (K_WHT << 8) | (K_WHT << 4) | K_WHT)
#define	KC	((K_GRY << 12) | (K_WHT << 8) | (K_WHT << 4) | K_WHT)
#define	KD	((K_BLK << 12) | (K_BLK << 8) | (K_BLK << 4) | K_BLK)

int	kbobj[] = {

	KA, KA, KA, KA,		/* 0 */		/* W */		/* C7 */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KB, KB, KB, KC,

/*  */

	KA, KA, KA, KA,		/* 1 */		/* X */		/* B */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 1 */				/* A7 */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* G6 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* F */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

	KA, KA, KA, KA,		/* 1 */		/* Y */		/* E */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 0 */				/* D */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* C6 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

/*  */

	KA, KA, KA, KA,		/* 1 */		/* X */		/* B */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 1 */				/* A6 */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* G5 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* F */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

	KA, KA, KA, KA,		/* 1 */		/* Y */		/* E */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 0 */				/* D */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* C5 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

/*  */

	KA, KA, KA, KA,		/* 1 */		/* X */		/* B */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 1 */				/* A5 */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* G4 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* F */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

	KA, KA, KA, KA,		/* 1 */		/* Y */		/* E */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 0 */				/* D */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* C4 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

/*  */

	KA, KA, KA, KA,		/* 1 */		/* X */		/* B */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 1 */				/* A4 */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* G3 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* F */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

	KA, KA, KA, KA,		/* 1 */		/* Y */		/* E */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 0 */				/* D */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* C3 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

/*  */

	KA, KA, KA, KA,		/* 1 */		/* X */		/* B */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 1 */				/* A3 */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* G2 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* F */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

	KA, KA, KA, KA,		/* 1 */		/* Y */		/* E */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 0 */				/* D */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* C2 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

/*  */

	KA, KA, KA, KA,		/* 1 */		/* X */		/* B */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 1 */				/* A2 */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* G1 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* F */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

	KA, KA, KA, KA,		/* 1 */		/* Y */		/* E */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 0 */				/* D */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* C1 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

/*  */

	KA, KA, KA, KA,		/* 1 */		/* X */		/* B */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 1 */				/* A1 */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* G0 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* F */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

	KA, KA, KA, KA,		/* 1 */		/* Y */		/* E */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 0 */				/* D */
	KD, KD, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* C0 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

/*  */

	KA, KA, KA, KA,		/* 1 */		/* Z */		/* B */
	KB, KB, KB, KC,
	KB, KB, KB, KC,
	KD, KD, KB, KC,

	KD, KD, KA, KA,		/* 2 */				/* A0 */
	KD, KD, KB, KC,
	KB, KB ,KB, KC,
	KB, KB, KB, KC,

	KA, KA, KA, KA,		/* 3 */		/* Bottom border */
	KD, KD, KD, KD,

	/* New Data label */

	LM(0x0000), LM(0x0000), LM(0x0000), LM(0x0000),
	LM(0x00F0), LM(0xFF0F), LM(0x0F0F), LM(0x0F00),
	LM(0x0FF0), LM(0x0F0F), LM(0x0F00), LM(0x0F00),
	LM(0xF0F0), LM(0xFF0F), LM(0x0F00), LM(0x0F0F),
	LM(0x00F0), LM(0x0F0F), LM(0xFF00), LM(0x0FF0),
	LM(0x00F0), LM(0xFF0F), LM(0x0F0F), LM(0x0F00),
	LM(0x0000), LM(0x0000), LM(0x0000), LM(0x0000),
	LM(0x0000), LM(0x0000), LM(0x0000), LM(0x0000),
	LM(0x0FF0), LM(0xFF00), LM(0xFFF0), LM(0x0FF0),
	LM(0xF0F0), LM(0x00F0), LM(0x0F0F), LM(0xF00F),
	LM(0xF0F0), LM(0xFFF0), LM(0x0F0F), LM(0xFFFF),
	LM(0xF0F0), LM(0x00F0), LM(0x0F0F), LM(0xF00F),
	LM(0x0FF0), LM(0x00F0), LM(0x0F0F), LM(0xF00F),
	LM(0x0000), LM(0x0000), LM(0x0000), LM(0x0000)
};
