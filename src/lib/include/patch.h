/*
   =============================================================================
	patch.h -- MIDAS-VII Patch facility definitions
	Version 22 -- 1988-12-02 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	MAXPATCH	256		/* patch table size */
#define	RAWDEFS		256		/* def table size */

#define	NSLINES		1000		/* sequence table size */
#define	NSEQW		7		/* number of words in a seqent */

#define	SEQTIME		10		/* milliseconds per tick */

#define	DATAROW		9		/* data entry row */

#define	NDEFSTMS	5120		/* number of stms/defs */
#define	NPTEQELS	256		/* number of trigger fifo entries */

#define	NPTEQLO		(NPTEQELS >> 2)		/* trigger fifo lo water */
#define	NPTEQHI		(NPTEQELS - NPTEQLO)	/* trigger fifo hi water */

#define	NULL_DEF	0x1200		/* blank definer code */

#define	ADR_MASK	0x00FF		/* patch / defent table index mask */
#define	TRG_MASK	0x1FFF		/* trigger mask */
#define	PE_SPEC		0x00FF		/* destination mask */
#define	PE_TBIT		0x8000		/* DEF triggered bit */

/* 
*/

/* Patch structure definitions */

struct	patch	{	/* patch table entry (16 bytes per entry) */

	unsigned short	nextstm;	/* index of next entry in stm chain */
	unsigned short	prevstm;	/* index of previous entry in stm chain */
	unsigned short	defnum;		/* definer */
	unsigned short	stmnum;		/* stimulus */
	unsigned short	paspec;		/* destination type and flags */
	unsigned short	pasuba;		/* sub-address */
	unsigned short	padat1;		/* data word 1 */
	unsigned short	padat2;		/* data word 2 */
};

struct	defent	{	/* definition table entry -- 10 bytes per entry */

	unsigned short	nextdef;	/* index of next in def chain */
	unsigned short	stm;		/* stimulus */
	unsigned short	adspec;		/* destination type */
	unsigned short	adsuba;		/* sub-address */
	unsigned short	addat1;		/* data word 1 */
};


/* Sequence structure definitions */

struct	seqent	{	/* sequence table entry -- 14 bytes per entry */

	unsigned short	seqtime;	/* time */
	unsigned short	seqact1;	/* action 1 */
	unsigned short	seqdat1;	/* action 1 data */
	unsigned short	seqact2;	/* action 2 */
	unsigned short	seqdat2;	/* action 2 data */
	unsigned short	seqact3;	/* action 3 */
	unsigned short	seqdat3;	/* action 3 data */
};

/* 
*/

/* Patch table references */

#ifndef	PATCHDEF
extern	char		stmptr[];	/* stimulus pointer table */
extern	char		defptr[];	/* definition pointer table */

extern	struct patch	patches[];	/* patch table */

extern	struct defent	defents[];	/* definition control table */
#endif


/* Sequence table references */

#ifndef	SEQDEFS
extern	struct seqent	seqtab[];		/* sequence table */

extern	unsigned short	seqflag[16];		/* sequence flags */
extern	unsigned short	seqline[16];		/* sequence line */
extern	unsigned short	seqstim[16];		/* sequence stimulus */
extern	unsigned short	seqtime[16];		/* sequence timers */
extern	unsigned short	sregval[16];		/* register values */
extern	unsigned short	trstate[16];		/* trigger states */

#endif

/* 
*/

/* Patch destination types */

#define	PA_KEY		1
#define	PA_TRG		2
#define	PA_PLS		3
#define	PA_LED		4
#define	PA_SLIN		5
#define	PA_SCTL		6
#define	PA_TUNE		7
#define	PA_RSET		8
#define	PA_RADD		9
#define	PA_INST		10
#define	PA_OSC		11
#define	PA_WAVA		12
#define	PA_WAVB		13
#define	PA_CNFG		14
#define	PA_LEVL		15
#define	PA_INDX		16
#define	PA_FREQ		17
#define	PA_FILT		18
#define	PA_FILQ		19
#define	PA_LOCN		20
#define	PA_DYNM		21
#define	PA_AUX		22
#define	PA_RATE		23
#define	PA_INTN		24
#define	PA_DPTH		25
#define	PA_VOUT		26

/* Patch sub-address types */

#define	PSA_SRC		0
#define	PSA_MLT		1
#define	PSA_TIM		2
#define	PSA_VAL		3
#define	PSA_FNC		4

/* Patch oscillator data types */

#define	PSO_INT		0
#define	PSO_RAT		1
#define	PSO_FRQ		2
#define	PSO_PCH		3

/* 
*/

/* Sequence control flags */

#define	SQF_RUN		0x8000		/* RUN state */
#define	SQF_CLK		0x4000		/* CLK state */

/* Sequence action word masks */

#define	SQ_MACT	0x00FF		/* ACT -- action mask */
#define	SQ_MOBJ	0xFF00		/* ACT -- object mask */

/* Sequence action types */

#define	SQ_NULL	0x0000		/* NULL action */

#define	SQ_CKEY	0x0001		/* Key closure */
#define	SQ_RKEY	0x0002		/* Key release */
#define	SQ_TKEY	0x0003		/* Key transient */
#define	SQ_IKEY	0x0004		/* If key active */

#define	SQ_STRG	0x0005		/* Trigger on */
#define	SQ_CTRG	0x0006		/* Trigger off */
#define	SQ_TTRG	0x0007		/* Trigger toggle */
#define	SQ_ITRG	0x0008		/* If trigger active */

#define	SQ_SREG	0x0009		/* Set register */
#define	SQ_IREQ	0x000A		/* If register = */
#define	SQ_IRLT	0x000B		/* If register < */
#define	SQ_IRGT	0x000C		/* If register > */

#define	SQ_ISTM	0x000D		/* If stimulus active */
#define	SQ_JUMP	0x000E		/* Jump to sequence line */
#define	SQ_STOP	0x000F		/* Stop sequence */

#define	SQ_AREG	0x0010		/* Increment register */

/* Sequence data word masks */

#define	SQ_MFLG	0xF000		/* DAT -- flag mask */
#define	SQ_MTYP	0x0F00		/* DAT -- data type mask */
#define	SQ_MVAL	0x00FF		/* DAT -- data value mask */

/* Sequence data types */

#define	SQ_REG	0x0000		/* register */
#define	SQ_VAL	0x0100		/* value */
#define	SQ_VLT	0x0200		/* voltage */
#define	SQ_RND	0x0300		/* random */
