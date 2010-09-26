/*
   =============================================================================
	score.h -- score storage definitions
	Version 31 -- 1988-10-17 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	MAX_SE		32768L	/* number of longs for score storage */

#define	N_SCORES	20	/* number of scores */
#define	N_SECTS		20	/* number of sections per score */
#define	NNEVTS		32	/* number of note entry events */

struct	nevent	{	/* note event entry list structure */

	struct	nevent	*nxt;
	short	group;
	short	note;
};

struct s_time {

	short	sflags;
	char	smpte[10];
};

/* 
*/

/* score list structures */

struct	s_entry {	/* event entry structure */

	long		e_time;		/* time, in frames */
	char		e_size;		/* size of entry, in longs */
	char		e_type;		/* type of entry */
	char		e_data1;	/* 1st 8 bits of data */
	char		e_data2;	/* 2nd 8 bits of data */

	struct s_entry	*e_bak,		/* backward pointer */
			*e_fwd,		/* forward pointer */

			*e_dn,		/* down pointer or 4 bytes of data */
			*e_up,		/* up pointer or 4 bytes of data */

			*e_lft,		/* left pointer or 4 bytes of data */
			*e_rgt;		/* right pointer or 4 bytes of data */
};

struct	n_entry {	/* note entry structure */

	long		e_time;		/* time, in frames */
	char		e_size;		/* size of entry, in longs */
	char		e_type;		/* type of entry */
	char		e_note;		/* note number */
	char		e_group;	/* group */

	struct s_entry	*e_bak,		/* backward pointer */
			*e_fwd;		/* forward pointer */

	short		e_vel;		/* velocity */
	short		e_data4;
};

/* 
*/

/* constant definitions */

#define	E_NULL	(struct s_entry *)0L

#define	E_SIZE1		5	/* event size 1 -- 5 longs -- 20 bytes */
#define	E_SIZE2		6	/* event size 2 -- 6 longs -- 24 bytes */
#define	E_SIZE3		8	/* event size 3 -- 8 longs -- 32 bytes */

#define	TO_BAK		240	/* offset from current time to left edge */
#define	TO_FWD		258	/* offset from current time to right edge */

#define	D_BAK		1	/* display backward */
#define	D_FWD		0	/* display forward */

#define	GS_DSBL		0	/* group status:  off */
#define	GS_ENBL		1	/* group status:  on */

/* 
*/

/* event types  (number in comment field is event size) */

#define	EV_NULL		0	/* 1 - null event */
#define	EV_SCORE	1	/* 1 - score begin */
#define	EV_SBGN		2	/* 2 - section begin */
#define	EV_SEND		3	/* 2 - section end */
#define	EV_INST		4	/* 2 - instrument change */
#define	EV_NBEG		5	/* 1 - note begin */
#define	EV_NEND		6	/* 1 - note end */
#define	EV_STOP		7	/* 1 - stop */
#define	EV_INTP		8	/* 2 - interpolate */
#define	EV_TMPO		9	/* 2 - tempo */
#define	EV_TUNE		10	/* 2 - tuning */
#define	EV_GRP		11	/* 2 - group status */
#define	EV_LOCN		12	/* 2 - location */
#define	EV_DYN		13	/* 2 - dynamics */
#define	EV_ANVL		14	/* 2 - analog value */
#define	EV_ANRS		15	/* 2 - analog resolution */
#define	EV_ASGN		16	/* 2 - I/O assign */
#define	EV_TRNS		17	/* 3 - transposition */
#define	EV_REPT		18	/* 1 - repeat */
#define	EV_PNCH		19	/* 1 - punch in/out */
#define	EV_PRES		20	/* 1 - polyphonic pressure */
#define	EV_FINI		21	/* 1 - score end */
#define	EV_CPRS		22	/* 1 - channel pressure */
#define	EV_BAR		23	/* 1 - bar marker */
#define	EV_NEXT		24	/* 1 - next score */

#define	N_ETYPES	25	/* number of event types (1 + last type code) */

/* 
*/

/* event header types */

#define	EH_INST		0	/* instrument change */
#define	EH_GRP		1	/* group status */
#define	EH_LOCN		2	/* location */
#define	EH_DYN		3	/* dynamics */
#define	EH_ANRS		4	/* analog resolution */
#define	EH_TRNS		5	/* transposition */
#define	EH_INTP		6	/* interpolate */
#define	EH_TMPO		7	/* tempo */
#define	EH_TUNE		8	/* tuning */
#define	EH_ASGN		9	/* I/O assign */
#define	EH_SBGN		10	/* section begin */
#define	EH_SEND		11	/* section end */

#define	N_TYPES		12	/* number of event header types */
