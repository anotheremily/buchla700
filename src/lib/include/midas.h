/*
   =============================================================================
	midas.h -- MIDAS 700 global definitions
	Version 38 -- 1989-12-19 -- D.N. Lynx Crowe

	Uses definitions from:  "graphdef.h", "vsdd.h" 
   =============================================================================
*/

typedef	short	(*PFS)();	/* pointer to a function returning a short */

#define	SM_SCALE(x)	(((x) * 252) & 0x7FE0)
#define	ART_VAL(x)	(((((x)-5) > 100) ? 100 : (((x)-5) < 0 ? 0 : ((x)-5)))*320)

#define	CWORD(x)	((short)((x)|((x)<<4)|((x)<<8)|((x)<<12)))

#define	CTOX(C)		((C) << 3)	/* column to x pixel value */
#define	RTOY(R)		((R) * 14)	/* row to y pixel value */
#define	XTOC(X)		((X) >> 3)	/* x pixel value to column */
#define	YTOR(Y)		((Y) / 14)	/* y pixel value to row */

#define	AMP_TIME	200		/* amplitude pot interpolate time */

#define	GCPRI	15			/* graphic cursor priority */
#define	GCURS	0			/* graphic cursor object number */

#define	BIT2	(V_RES2)		/* graphics - 2 bit pixels */
#define	BIT3	(V_RES3)		/* graphics - 4 bit pixels */
#define	CHR2	(0)			/* characters - no attributes */
#define	CHR3	(V_FAD | V_HCR)		/* characters - full attributes */

#define	OBFL_00	(BIT3 | V_TDE)		/* cursor - arrow */

#define	LIBRFL		(V_RES3)	/* librarian display object flags */

#define	TTCURS		2		/* typewriter cursor object */
#define	TTCPRI		14		/* typewriter cursor priority */
#define	TTCCFL		(V_RES3 | V_OBL | V_TDE)

#define	XYCENTRE	50		/* center of range of cursor pots */

#define	SMYRATE		4		/* smooth scroll vertical rate */

#define	HCWVAL		30		/* graphics cursor x wait time */
#define	VCWVAL		30		/* graphics cursor y wait time */
#define	THCWVAL		60		/* text cursor h wait time */
#define	TVCWVAL		120		/* text cursor v wait time */
#define	CURHOLD		300		/* cursor motion startup delay */

#define	CXMAX		XMAX		/* cursor x maximum */
#define	CYMAX		YMAX		/* cursor y maximum */

#define	FN_NULL		0L		/* null for address entries */

/* 
*/

#define	NASGS		100	/* number of assignments in memory */
#define	NASGLIB		99	/* number of assignments on disk */

#define	NTUNS		10	/* number of tunings in memory */
#define	NTUNSLIB	9	/* number of tunings on disk */

#define	NINST		41	/* number of instruments in memory */
#define	NINORC		20	/* number of instruments on disk */
#define	NIPNTS		128	/* maximum number of points in the instrument */
#define	MAXIDLN		16	/* maximum length of function ID strings */
#define	NFINST		13	/* number of functions in an instrument */
#define	NUMCFG		12	/* number of oscillator configurations */

#define	NUMWAVS		20	/* number of waveshapes in memory or on disk*/
#define	NUMWIDS		8	/* number of waveshape cursor widths */
#define	NUMHARM		32	/* number of waveshape harmonics */
#define	NUMWPNT		254	/* number of waveshape points stored */
#define	NUMWPCAL	256	/* number of waveshape points calculated */

#define	VID_EI		0	/* IPL to enable video */
#define	VID_DI		1	/* IPL to disable video */
#define	FPU_EI		1	/* IPL to enable FPU */
#define	FPU_DI		2	/* IPL to disable FPU */
#define	TIM_DI		4	/* IPL to disable timer */

#define	NTRIGS		6144	/* number of trigger related entries */
#define	NCTRLS		6144	/* number of controller related entries */
#define	NPFENTS		336	/* number of pendant function list entries */
#define	NLSTENTS	48	/* number of last note entry table entries */

#define	NMPORTS		2	/* number of MIDI ports */

#define	MASENSCT	500	/* active sensing timeout count */

#define	MIDISUS		64	/* MIDI damper (sustain) controller number */
#define	MIDIHLD		66	/* MIDI sustenuto (hold) controller number */

#define	MSW_ON		0x3F	/* MIDI switch on threshold */
#define	MSW_OFF		0x40	/* MIDI switch off threshold */

#define	M_KSTATE	0x01	/* MIDI key status */
#define	M_LCLSUS	0x04	/* MIDI local sustain status */
#define	M_CHNSUS	0x08	/* MIDI channel sustain status */
#define	M_LCLHLD	0x40	/* MIDI local hold status */
#define	M_CHNHLD	0x80	/* MIDI channel hold status */
#define	MKEYHELD	(M_CHNHLD | M_LCLHLD)	/* key held */

#define	PITCHMIN	320		/* C0 in 1/2 cents (160 cents) */
#define	PITCHMAX	21920		/* C9 in 1/2 cents (10960 cents) */

#define	GTAG1		0x0100
#define	GTAG2		0x0200
#define	GTAG3		0x0400
#define	GTAG4		0x0800
#define	GTAG5		0x1000
#define	GTAG6		0x2000
#define	GTAG7		0x4000
#define	GTAG8		0x8000

#define	CTAG1		0x0100
#define	CTAG2		0x0200
#define	CTAG3		0x0400
#define	CTAG4		0x0800
#define	CTAG5		0x1000
#define	CTAG6		0x2000
#define	CTAG7		0x4000
#define	CTAG8		0x8000
