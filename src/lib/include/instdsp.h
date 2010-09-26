/*
   =============================================================================
	instdsp.h -- instrument editor definitions
	Version 37 -- 1988-08-30 -- D.N. Lynx Crowe

	Uses definitions from:  "graphdef.h", "midas.h", "vsdd.h"
   =============================================================================
*/

/* --------------- Miscellaneous Instrument display definitions ------------- */

#define	ICURX		256	/* initial cursor x location */
#define	ICURY		175	/* initial cursor y location */

#define	INSTOBJ		8		/* instrument display object number */
#define	INSTPRI		8		/* instrument display object priority */

#define	TCURS		0		/* text cursor object number */
#define	TCPRI		15		/* text cursor priority */

#define	INSTFL		(V_RES3)	/* instrument display object flags */
#define	TCURFL		(V_RES3)	/* text cursor display flags */

#define	CBORD		9		/* color of border */
#define	TGRID		2		/* color of level grid titles */
#define	LGRID		0		/* color of level grid */
#define	ID_LVLC		6		/* color of level background plot */
#define	ID_CLVL		2		/* color of level lines as reference */
#define	ID_CLIN		14		/* color of function lines */
#define	ID_CPNT		11		/* color of normal point */
#define	ID_NCUR		1		/* color of normal graphic cursor */
#define	ID_TCUR		1		/* color of text cursor */
#define	ID_CHGD		3		/* color of voice # for changed data */
#define	ID_INST		2		/* color of funcs started at def time */
#define	ID_ENTRY	10		/* color of new data being entered */
#define	ID_SELD		3		/* color of selected point and cursor */
#define	ID_ACTP		15		/* color of point with action or source */
#define	WSAFC		8		/* color of waveshape A final values */
#define	WSBFC		4		/* color of waveshape B final values */

/* 
*/

/* ------------- Foreground colors for instrument display windows ----------- */

#define	CFBX00	11		/* Freq 1 */
#define	CFBX01	11		/* Freq 2 */
#define	CFBX02	11		/* Freq 3 */
#define	CFBX03	11		/* Freq 4 */
#define	CFBX04	11		/* Filter / Resonance */
#define	CFBX05	11		/* Location */

#define	CFBX06	11		/* Index 1 */
#define	CFBX07	11		/* Index 2 */
#define	CFBX08	11		/* Index 3 */
#define	CFBX09	11		/* Index 4 */
#define	CFBX10	11		/* Index 5 */
#define	CFBX11	11		/* Index 6 */

#define	CFBX12	11		/* Level */

#define	CFBX13	11		/* Source Mult */
#define	CFBX14	11		/* Point */
#define	CFBX15	11		/* Time */
#define	CFBX16	11		/* Value */
#define	CFBX17	11		/* Action */

#define	CFBX18	11		/* Algorithm */
#define	CFBX19	11		/* Voice & Inst */
#define	CFBX20	11		/* Oscillators */
#define	CFBX21	11		/* Waveshapes */
#define	CFBX22	11		/* Variables */
#define	CFBX23	11		/* Name and comments */
/* 
*/

/* --------- Background colors for instrument display windows --------------- */

#define	CBBX00	5		/* Freq 1 */
#define	CBBX01	5		/* Freq 2 */
#define	CBBX02	5		/* Freq 3 */
#define	CBBX03	5		/* Freq 4 */
#define	CBBX04	5		/* Filter / Resonance */
#define	CBBX05	5		/* Location */

#define	CBBX06	7		/* Index 1 */
#define	CBBX07	7		/* Index 2 */
#define	CBBX08	7		/* Index 3 */
#define	CBBX09	7		/* Index 4 */
#define	CBBX10	7		/* Index 5 */
#define	CBBX11	7		/* Index 6 */

#define	CBBX12	ID_LVLC		/* Level */

#define	CBBX13	7		/* Source Mult */
#define	CBBX14	7		/* Point */
#define	CBBX15	7		/* Time */
#define	CBBX16	7		/* Value */
#define	CBBX17	7		/* Action */

#define	CBBX18	5		/* Algorithm */
#define	CBBX19	6		/* Voice & Inst */
#define	CBBX20	6		/* Oscillators */
#define	CBBX21	5		/* Waveshapes */
#define	CBBX22	7		/* Variables */
#define	CBBX23	6		/* Name and comments */

/* 
*/

/* -------------- pendant function list entry structure definition ---------- */

struct	pflent	{	/* pendant function list entry structure */

	struct	pflent	*nextpf;	/* next entry pointer */
	unsigned	pftrig;		/* trigger number */
	unsigned	pfvpval;	/* voice / parameter number */
	long		d1reg;		/* D1 register contents */
	long		d2reg;		/* D2 register contents */
	long		d4reg;		/* D4 register contents */
	char		*a1reg;		/* A1 register contents */
	char		*a2reg;		/* A2 register contents */
	char		*a3reg;		/* A3 register contents */
};

/* 
*/

/* -------------- function point structure definitions ---------------------- */

struct	instpnt	{	/* function point definition */

	short	iptim;		/* time (packed) */
	short	ipval;		/* value */
	short	ipvmlt;		/* value multiplier */

	char	ipvsrc;		/* value source */
	char	ipact;		/* action code */

#define	AC_NULL		0	/* null action */
#define	AC_SUST		1	/* sustain (hold here while key is down) */
#define	AC_ENBL		2	/* enable  (hold here while key is up) */
#define	AC_JUMP		3	/* unconditional (continuous) jump */
#define	AC_LOOP		4	/* jump (loop) n times */
#define	AC_KYUP		5	/* jump if key up (enable jump) */
#define	AC_KYDN		6	/* jump if key down (sustain jump) */
#define	AC_UNDF		7	/* -- undefined - treated as AC_NULL -- */

	char	ippar1;		/* action parameter 1  (point) */
	char	ippar2;		/* action parameter 2  (count) */

	char	ippar3;		/* action parameter 3  (counter) */
	char	ippad;		/* padding for even boundary */
};

/* 
*/

/* ------------- function header structure definitions ---------------------- */

struct	idfnhdr	{	/* function header */

	short	idfpch;		/* pitch (used for freq1 only) */
	short	idfmlt;		/* overall value multiplier (-1 .. +1) */

	char	idfsrc;		/* overall value source */
	char	idfpif;		/* points in the function */

	char	idfpt1;		/* index of first point (0..NIPTS) */
	char	idftmd;		/* trigger mode and control bits */

#define	I_TM_KEY	0x01	/* trigger on key closure */
#define	I_ACTIVE	0x02	/* function active */
#define	I_NRATIO	0x04	/* not ratio mode         (matches hardware) */
#define	I_NVBITS	0x18	/* new value select bits  (matches hardware) */
#define	I_VNSUBN	0x08	/* new value sub-n bit    (matches hardware) */

	char	idfcpt;		/* index of current point (0..NIPTS) */
	char	idfprm;		/* parameter associated with this function */

	short	idftrg;		/* trigger that started the function */
};

/* 
*/

/* ---------- instrument definition structure definitions ------------------- */

struct	instdef	{	/* instrument definition */

	short	idhflag;		/* flags */

	char	idhname[MAXIDLN+1];	/* instrument name */
	char	idhcom1[MAXIDLN+1];	/* first line of comments */
	char	idhcom2[MAXIDLN+1];	/* second line of comments */
	char	idhcom3[MAXIDLN+1];	/* third line of comments */

	char	idhcfg;			/* configuration byte */
	char	idhplft;		/* points unused */

	char	idhwsa;			/* waveshape A library number */
	char	idhwsb;			/* waveshape B library number */

	char	idhos1c;		/* oscillator 1 controls */
	char	idhos2c;		/* oscillator 2 controls */
	char	idhos3c;		/* oscillator 3 controls */
	char	idhos4c;		/* oscillator 4 controls */

#define	OC_MOD		0x0003		/* oscillator mode mask */

#define	OC_INT		0x0000		/* oscillator in interval mode */
#define	OC_RAT		0x0001		/* oscillator in ratio mode */
#define	OC_FRQ		0x0002		/* oscillator in frequency mode */
#define	OC_PCH		0x0003		/* oscillator in pitch mode */

#define	OC_SYN		0x0004		/* oscillator in sync mode */

	short	idhos1v;		/* oscillator 1 value */
	short	idhos2v;		/* oscillator 2 value */
	short	idhos3v;		/* oscillator 3 value */
	short	idhos4v;		/* oscillator 4 value */

	struct	idfnhdr	idhfnc[NFINST];	/* function headers */

	struct	instpnt	idhpnt[NIPNTS];	/* point table */

	short	idhwvaf[NUMWPNT];	/* waveshape A final values */
	short	idhwvao[NUMWPNT];	/* waveshape A offset values */
	short	idhwvah[NUMHARM];	/* waveshape A harmonic values */

	short	idhwvbf[NUMWPNT];	/* waveshape B final values */
	short	idhwvbo[NUMWPNT];	/* waveshape B offset values */
	short	idhwvbh[NUMHARM];	/* waveshape B harmonic values */

};
