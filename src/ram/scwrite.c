/*
   =============================================================================
	scwrite.c -- librarian - write score functions
	Version 14 -- 1988-06-22 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "stddefs.h"
#include "stdio.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "score.h"
#include "scfns.h"

#include "midas.h"
#include "libdsp.h"

#if	DEBUGIT
extern	short	debugsw;
#endif

extern	long	chksum();

extern	long	schksum;	/* score checksum */
extern	long	snbreq;		/* number of disk bytes required */
extern	long	snlreq;		/* number of score bytes required */
extern	long	sntbreq;	/* total number of disk bytes required */
extern	long	sntlreq;	/* total number of score bytes required */

extern	long	nlpersc[N_SCORES];	/* number of longs per score */
extern	long	scsums[N_SCORES];	/* score checksums */

extern	struct	s_time	stimes[N_SCORES][N_SECTS];	/* section times */

/* 
*/

long	m1con = -1L;			/* null score constant */

short	scsizes[N_ETYPES][2] = {	/* score event sizes  (in bytes) */

	/* [0] = score, [1] = disk */

	{20,  0},	/*  0 - EV_NULL */
	{20,  6},	/*  1 - EV_SCORE */
	{24,  6},	/*  2 - EV_SBGN */
	{24,  6},	/*  3 - EV_SEND */
	{24,  7},	/*  4 - EV_INST */
	{20,  9},	/*  5 - EV_NBEG */
	{20,  9},	/*  6 - EV_NEND */
	{20,  5},	/*  7 - EV_STOP */
	{20,  7},	/*  8 - EV_INTP */
	{24,  6},	/*  9 - EV_TMPO */
	{24,  6},	/* 10 - EV_TUNE */
	{24,  7},	/* 11 - EV_GRP */
	{24,  7},	/* 12 - EV_LOCN */
	{24,  7},	/* 13 - EV_DYN */
	{24,  8},	/* 14 - EV_ANVL */
	{24,  7},	/* 15 - EV_ANRS */
	{24,  6},	/* 16 - EV_ASGN */
	{32,  8},	/* 17 - EV_TRNS */
	{20,  6},	/* 18 - EV_REPT */
	{20,  6},	/* 19 - EV_PNCH */
	{20,  7},	/* 20 - EV_PRES */
	{20,  6},	/* 21 - EV_FINI */
	{20,  7},	/* 22 - EV_CPRS */
	{20,  5},	/* 23 - EV_BAR */
	{20,  5}	/* 24 - EV_NEXT */
};

/* 
*/

/*
   =============================================================================
	scsize() -- return number of bytes needed for a score
   =============================================================================
*/

long
scsize(sn)
short sn;
{
	register long nb, nl, sc;
	register struct s_entry *ep;
	register short go;

	snbreq  = nb = 0L;	/* number of disk bytes required */
	snlreq  = nl = 0L;	/* number of score bytes required */
	schksum = sc = 0L;	/* score checksum */

	ep = scores[sn];	/* pointer to score begin */

	if (E_NULL EQ ep) {	/* empty scores don't take any space */

#if	DEBUGIT
	if (debugsw)
		printf("scsize(%d):  empty score\n", sn);
#endif
		return(0L);
	}

	go = TRUE;

/* 
*/
	do {
		ep->e_type &= 0x7F;		/* clear new-data flag */

		nl += scsizes[ep->e_type][0];
		nb += scsizes[ep->e_type][1];

		switch (ep->e_type) {

		case EV_BAR:		/* bar marker */
		case EV_STOP:		/* stop */
		case EV_NEXT:		/* next */

			sc += ep->e_type + ep->e_time;
			break;

		case EV_SCORE:		/* score begin */
		case EV_SBGN:		/* section begin */
		case EV_SEND:		/* section end */
		case EV_TMPO:		/* tempo */
		case EV_TUNE:		/* tuning */
		case EV_ASGN:		/* I/O assign */
		case EV_REPT:		/* repeat */
		case EV_PNCH:		/* punch in/out */

			sc += ep->e_type + ep->e_time + ep->e_data1;
			break;

		case EV_NBEG:		/* note begin */
		case EV_NEND:		/* note end */

			sc += ep->e_type + ep->e_time + ep->e_data1
			      + ep->e_data2 + (struct n_entry *)ep->e_vel;

			break;
/* 
*/
		case EV_INST:		/* instrument change */
		case EV_INTP:		/* interpolate */
		case EV_GRP:		/* group status */
		case EV_LOCN:		/* location */
		case EV_DYN:		/* dynamics */
		case EV_PRES:		/* polyphonic pressure */
		case EV_CPRS:		/* channel pressure */
		case EV_ANRS:		/* analog resolution */

			sc += ep->e_type + ep->e_time + ep->e_data1
			      + ep->e_data2;

			break;

		case EV_TRNS:		/* transposition */

			sc += ep->e_type + ep->e_time + ep->e_data1
			      + (short)ep->e_lft;

			break;

		case EV_ANVL:		/* analog value */

			sc += ep->e_type + ep->e_time + ep->e_data1
			      + (short)(0x0000FFFFL & ((long)ep->e_dn >> 16));

			break;

		case EV_FINI:		/* score end */

			sc += ep->e_type + ep->e_time + ep->e_data1;
			go = FALSE;
			break;
		}

		ep = ep->e_fwd;

	} while (go);		

	schksum = sc;
	snbreq  = nb;
	snlreq  = (nl + 3L) >> 2;

#if	DEBUGIT
	if (debugsw)
		printf("scsize(%d):  %ld bytes, checksum = %ld\n", nb, schksum);
#endif

	return(nb);
}

/* 
*/

/*
   =============================================================================
	scsizer() -- return number of bytes necessary for storing active scores
   =============================================================================
*/

long
scsizer()
{
	register short i;
	register long nb;

	sntlreq = 0L;
	nb = 0L;

	for (i = 0; i < N_SCORES; i++) {

		nlpersc[i] = 0L;
		scsums[i] = 0L;

		nb += 4L;	/* overhead for number of longs or -1L marker */

		if (scores[i] NE E_NULL) {

			/* add storage length of events + name + times */

			nb += scsize(i) + 16L
			      + (long)(N_SECTS * sizeof (struct s_time));

			nlpersc[i] = snlreq;	/* update size table */
			scsums[i]  = schksum;	/* update checksum table */
			sntlreq   += snlreq;	/* update total longs count */
		}
	}

	nb += 4L;	/* overhead for total longs count */

#if	DEBUGIT
	if (debugsw)
		printf("scsizer():  %ld bytes required\n", nb);
#endif

	sntbreq = nb;

	return(nb);
}

/* 
*/

/*
   =============================================================================
	scwrite() -- store a score
   =============================================================================
*/

short
scwrite(sn, fp)
short sn;
register FILE *fp;
{
	register struct s_entry *ep;
	register short go;
	char scid[48];

	ep = scores[sn];		/* pointer to score begin */

#if	DEBUGIT
	if (debugsw)
		printf("scwrite(%d, $%08lX):  ep = $%08lX\n", sn, fp, ep);
#endif
	
	if (E_NULL EQ ep) {		/* empty scores only get a flag */

		sprintf(scid, "score %d was empty", sn + 1);

		ldwmsg((char *)0L, scid, (char *)0L, LCFBX10, LCBBX10);

		if (wr_ec(fp, &m1con, 4L))
			return(FAILURE);
		else
			return(SUCCESS);
	}

	sprintf(scid, "  writing score %d", sn + 1);

	ldwmsg("Busy -- Please stand by", (char *)0L, scid,
		LCFBX10, LCBBX10);

	if (wr_ec(fp, &nlpersc[sn], 4L))	/* number of longs required */
		return(FAILURE);

	if (wr_ec(fp, &scname[sn], 16L))	/* score name */
		return(FAILURE);

	if (wr_ec(fp, &stimes[sn], (long)(N_SECTS * 12)))  /* section times */
		return(FAILURE);

	go = TRUE;

	do {

		switch (ep->e_type) {

		case EV_BAR:		/* bar marker */
		case EV_STOP:		/* stop */
		case EV_NEXT:		/* next */

			if (wr_ec(fp, &ep->e_type, 1L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_time, 4L))
				return(FAILURE);			

			break;

		case EV_SCORE:		/* score begin */
		case EV_SBGN:		/* section begin */
		case EV_SEND:		/* section end */
		case EV_TMPO:		/* tempo */
		case EV_TUNE:		/* tuning */
		case EV_ASGN:		/* I/O assign */
		case EV_REPT:		/* repeat */
		case EV_PNCH:		/* punch in/out */

			if (wr_ec(fp, &ep->e_type, 1L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_time, 4L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_data1, 1L))
				return(FAILURE);

			break;
/* 
*/
		case EV_INST:		/* instrument change */
		case EV_INTP:		/* interpolate */
		case EV_GRP:		/* group status */
		case EV_LOCN:		/* location */
		case EV_DYN:		/* dynamics */
		case EV_PRES:		/* polyphonic pressure */
		case EV_CPRS:		/* channel pressure */
		case EV_ANRS:		/* analog resolution */

			if (wr_ec(fp, &ep->e_type, 1L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_time, 4L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_data1, 1L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_data2, 1L))
				return(FAILURE);

			break;
/* 
*/
		case EV_NBEG:		/* note begin */
		case EV_NEND:		/* note end */

			if (wr_ec(fp, &ep->e_type, 1L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_time, 4L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_data1, 1L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_data2, 1L))
				return(FAILURE);

			if (wr_ec(fp, &(((struct n_entry *)ep)->e_vel), 2L))
				return(FAILURE);

			break;

		case EV_TRNS:		/* transposition */

			if (wr_ec(fp, &ep->e_type, 1L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_time, 4L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_data1, 1L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_lft, 2L))
				return(FAILURE);

			break;
/* 
*/
		case EV_ANVL:		/* analog value */

			if (wr_ec(fp, &ep->e_type, 1L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_time, 4L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_data1, 1L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_dn, 2L))
				return(FAILURE);

			break;

		case EV_FINI:		/* score end */

			if (wr_ec(fp, &ep->e_type, 1L))
				return(FAILURE);

			if (wr_ec(fp, &ep->e_time, 4L))
				return(FALSE);

			if (wr_ec(fp, &ep->e_data1, 1L))
				return(FAILURE);

			go = FALSE;

#if	DEBUGIT
	if (debugsw)
		printf("scwrite():  EV_FINI\n");
#endif
			break;

		}

		ep = ep->e_fwd;

	} while (go);		

#if	DEBUGIT
	if (debugsw)
		printf("scwrite():  SUCCESS\n");
#endif

	return(SUCCESS);
}
