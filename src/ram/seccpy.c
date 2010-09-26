/*
   =============================================================================
	seccpy.c -- section operation functions
	Version 12 -- 1988-07-16 -- D.N. Lynx Crowe

	This file contains the section operation functions for:

		SOP_CPY		copy			sec_cpy()
		SOP_MRG		merge			sec_mrg()
		SOP_MOV		move			sec_mov()
		SOP_RMV		remove			sec_rmv()
		SOP_GRP		regroup			sec_grp()
		SOP_DGR		delete groups		sec_dgr()
		SOP_DEV		delete events		sec_dev()
   =============================================================================
*/

#undef	DEBUGGER		/* define to enable debug trace */
#define	DEBUGIT		0

#include "stddefs.h"
#include "debug.h"
#include "score.h"
#include "scfns.h"
#include "secops.h"
#include "secdefs.h"

#if	DEBUGIT
extern	short	debugsw;
#endif

extern	short	chksec(), oktocm(), oktode(), oktodg();

extern	long	sizesec();

extern	struct	s_entry	*madjsec();

extern	short	grptran;

extern	char	cmgtags[];
extern	char	cmgtype[];

extern	short	ehdlist[];
extern	short	grptmap[];

/* 
*/

/*
   =============================================================================
	sec_cpy() -- copy section 'ns' into the score at t_cur

	ns	section number to be copied

	returns SUCCESS or FAILURE
   =============================================================================
*/

short
sec_cpy(ns)
register short ns;
{
	register struct s_entry *cp, *lp, *rp;
	register long newet;

	DB_ENTR("sec_cpy");

	secopok = TRUE;

	if (chksec(ns)) {		/* check that section is OK */

		DB_EXIT("sec_cpy - FAILED chksec");
		return(FAILURE);
	}

	/* see if we have enough free event space to make the copy */

	if (sizesec() > evleft()) {

		DB_EXIT("sec_cpy - FAILED sizesec");
		return(FAILURE);
	}

	/* make sure we won't overflow the time range */

	newet = t_sect + ((scp->e_bak)->e_bak)->e_time;

	if ((newet < 0) OR (newet GE 0x00FFFFFFL)) {

		DB_EXIT("sec_cpy - FAILED time check");
		return(FAILURE);
	}

/* 
*/
	/* make a time adjusted copy of the section */

	if (E_NULL EQ (cp = madjsec(p_sbgn, t_cur))) {

		DB_EXIT("sec_cpy - FAILED madjsec");
		return(FAILURE);
	}

	/* point at the events in the score that will surround the copy */

	lp = ep_adj(p_cur, 0, t_cur);		/* events left of the copy */
	rp = lp->e_fwd;				/* events right of the copy */
	
	/* adjust the times in the score past the copy */

	edelta(lp, t_cur, t_sect);

	/* insert the copy into the score */

	lp->e_fwd = p_cbgn;	/* link copy to left events */
	p_cbgn->e_bak = lp;

	rp->e_bak = p_cend;	/* link copy to right events */
	p_cend->e_fwd = rp;

	/* fix-up the event headers in the copy */

	ehfix(p_cbgn, p_cend);

	DB_EXIT("sec_cpy - SUCCESS");
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	sec_mrg() -- merge section 'ns' into the score at t_cur

	ns	section number to be merged

	returns SUCCESS or FAILURE
   =============================================================================
*/

short
sec_mrg(ns)
register short ns;
{
	register struct s_entry *cp, *lp, *rp;
	register long newet;
	register short et;

	DB_ENTR("sec_mrg");

	secopok = TRUE;

	if (chksec(ns)) {		/* check that section is OK */

		DB_EXIT("sec_mrg - FAILED chksec");
		return(FAILURE);
	}

	/* see if we have enough free event space to make the copy */

	if (sizesec() > evleft()) {

		DB_EXIT("sec_mrg - FAILED sizesec");
		return(FAILURE);
	}

	/* make sure we won't overflow the time range */

	newet = t_sect + ((scp->e_bak)->e_bak)->e_time;

	if ((newet < 0) OR (newet GE 0x00FFFFFFL)) {

		DB_EXIT("sec_mrg - FAILED time check");
		return(FAILURE);
	}

/* 
*/
	/* make a time adjusted copy of the section */

	if (E_NULL EQ (cp = madjsec(p_sbgn, t_cur))) {

		DB_EXIT("sec_mrg - FAILED madjsec");
		return(FAILURE);
	}

	DB_CMNT("sec_mrg - merging events");

	lp = ep_adj(p_cur, 0, t_cur);	/* get merge point */

	while (cp) {		/* merge events into score starting at p_cur */

		rp = cp->e_fwd;				/* point at next event */
		lp = ep_adj(lp, 0, cp->e_time);		/* update merge point */
		lp = e_ins(cp, lp);			/* insert the element */
		et = cp->e_type & 0x007F;		/* get event type */

		if (-1 NE ehdlist[et])			/* see if it's a header */
			eh_ins(cp, ehdlist[et]);	/* update header list */

		cp = rp;				/* update copy pointer */
	}

	DB_EXIT("sec_mrg - SUCCESS");
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	sec_grp() -- regroup section 'ns'

	ns	section number to be re-grouped

	returns SUCCESS or FAILURE
   =============================================================================
*/

short
sec_grp(ns)
register short ns;
{
	register struct s_entry *cp, *rp;
	register short et, nv, grp;

	DB_ENTR("sec_grp");

	secopok = TRUE;

	if (chksec(ns)) {		/* check that section is OK */

		DB_EXIT("sec_grp - FAILED chksec");
		return(FAILURE);
	}

	cp = p_sbgn;			/* point at start of section */

	while (cp NE p_send) {		/* regroup events in section */

		rp = cp->e_fwd;			/* point at next event */
		et = cp->e_type & 0x007F;	/* get event type */

		if (cmgtags[et]) {		/* group sensitive ? */

			grp = 0x000F & (cmgtype[et] ?
				cp->e_data2 : cp->e_data1);

			if ((et EQ EV_NBEG) OR (et EQ EV_NEND)) {

				/* regroup */

				cp->e_data2 = (cp->e_data2 & 0x00F0) |
					grptmap[grp];

				/* transpose */

				nv = cp->e_data1 + grptran;

				if (nv > 127)
					nv = 127;
				else if (nv < 0)
					nv = 0;

				cp->e_data1 = nv;

			} else if ((et EQ EV_ANRS) OR (et EQ EV_ANVL)) {

				/* regroup */

				cp->e_data1 = (cp->e_data1 & 0x000F) |
					(grptmap[grp] << 4);

			} else {

				/* regroup */

				if (cmgtype[et])
					cp->e_data2 = (cp->e_data2 & 0x00F0) |
						grptmap[grp];
				else
					cp->e_data1 = (cp->e_data1 & 0x00F0) |
						grptmap[grp];
			}
		}

		cp = rp;		/* update event pointer */
	}

	DB_EXIT("sec_grp - SUCCESS");
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	sec_mov() -- move section 'ns' to t_cur

	ns	section number to be moved

	returns SUCCESS or FAILURE
   =============================================================================
*/

short
sec_mov(ns)
register short ns;
{
	register struct s_entry *cp, *lp, *rp;
	register long newet;
	register short et, grp, nv;

	DB_ENTR("sec_mov");

	secopok = TRUE;

	if (chksec(ns)) {		/* check that section is OK */

		DB_EXIT("sec_mov - FAILED chksec");
		return(FAILURE);
	}

#if	DEBUGIT
	if (debugsw) {

		printf("sec_mov:  t_cur = %ld, t_sbgn = %ld, t_send = %ld, t_sect = %ld\n",
			t_cur, t_sbgn, t_send, t_sect);

		printf("sec_mov:  p_cur = $%08lX, p_sbgn = $%08lX, p_send = $%08lX\n",
			p_cur, p_sbgn, p_send);
	}
#endif

	/* verify that section isn't being moved into itself */

	if ((t_cur GE t_sbgn) AND (t_cur LE t_send)) {

		DB_EXIT("sec_mov -- bad target");
		return(FAILURE);
	}

/* 
*/
	lp = ep_adj(p_cur, 0, t_cur);	/* get left move point */
	cp = p_send->e_fwd;		/* get adjustment point */

#if	DEBUGIT
	if (debugsw)
		printf("sec_mov: lp = $%08lX, cp = $%08lX\n", lp, cp);
#endif

	/* clip out the section and close up the hole */

	(p_sbgn->e_bak)->e_fwd = p_send->e_fwd;
	(p_send->e_fwd)->e_bak = p_sbgn->e_bak;
	p_sbgn->e_bak = E_NULL;
	p_send->e_fwd = E_NULL;

	/* adjust the times above the clip point to end of score */

	if (t_cur GE t_send)	/* adjust t_cur if above clip point */
		t_cur -= t_sect;

#if	DEBUGIT
	if (debugsw)
		printf("sec_mov: adjusted t_cur = %ld\n", t_cur);
#endif

	while (EV_FINI NE (et = 0x007F & cp->e_type)) {

		cp->e_time -= t_sect;	/* adjust event time */
		cp = cp->e_fwd;		/* point at next event */
	}

/* 
*/
#if	DEBUGIT
	if (debugsw)
		printf("sec_mov: adjusted p_cur->e_time = %ld\n",
			p_cur->e_time);
#endif

	/* relativize the section to 0 and unlink event headers from hplist */

	rp = p_sbgn;			/* start at the beginning */
	newet = p_sbgn->e_time;		/* relativize to begin time EQ 0 */

	while (rp) {

		rp->e_time -= newet;		/* relativize the time */
		et = 0x007F & rp->e_type;	/* get event type */

		if (cmgtags[et]) {	/* group sensitive ? */

			grp = 0x000F & (cmgtype[et] ?
				rp->e_data2 : rp->e_data1);

			if ((et EQ EV_NBEG) OR (et EQ EV_NEND)) {

				/* regroup */

				rp->e_data2 = (rp->e_data2 & 0x00F0) |
					grptmap[grp];

				/* transpose */

				nv = rp->e_data1 + grptran;

				if (nv > 127)
					nv = 127;
				else if (nv < 0)
					nv = 0;

				rp->e_data1 = nv;

			} else if ((et EQ EV_ANRS) OR (et EQ EV_ANVL)) {

				/* regroup */

				rp->e_data1 = (rp->e_data1 & 0x000F) |
					(grptmap[grp] << 4);

			} else {

				/* regroup */

				if (cmgtype[et])
					rp->e_data2 = (rp->e_data2 & 0x00F0) |
						grptmap[grp];
				else
					rp->e_data1 = (rp->e_data1 & 0x00F0) |
						grptmap[grp];
			}
		}

		if (-1 NE ehdlist[et])			/* if it's a header ... */
			eh_rmv(rp, ehdlist[et]);	/* ... remove it from hplist */

		rp = rp->e_fwd;			/* point at the next event */
	}

	rp = lp->e_fwd;				/* get right insert pointer */

	/* insert the moved section */

	p_sbgn->e_bak = lp;
	p_send->e_fwd = rp;
	lp->e_fwd = p_sbgn;
	rp->e_bak = p_send;

/* 
*/
	/* derelativize the moved section and put headers back on hplist */

	cp = p_sbgn;
	newet = t_cur;

#if	DEBUGIT
	if (debugsw)
		printf("sec_mov:  lp = $%08lX, cp = $%08lX, rp = $%08lX, newet = %ld\n",
			lp, cp, rp, newet);
#endif

	while (cp NE rp) {

		et = 0x007F & cp->e_type;	/* get event type */
		cp->e_time += newet;		/* derelativize the time */

		if (-1 NE ehdlist[et])			/* if event is a header ... */
			eh_ins(cp, ehdlist[et]);	/* ... put event on hplist */

		cp = cp->e_fwd;			/* point at next event */
	}

#if	DEBUGIT
	if (debugsw)
		printf("sec_mov: adjusting times above $%08lx (%ld) by %ld\n",
			cp, cp->e_time, t_sect);
#endif

	/* adjust times above move point */

	while (EV_FINI NE (et = 0x007F & cp->e_type)) {

		cp->e_time += t_sect;		/* adjust the time */
		cp = cp->e_fwd;			/* point at next event */
	}

	DB_EXIT("sec_mov - SUCCESS");
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	sec_rmv() --remove section 'ns'

	ns	section number to be removed

	returns SUCCESS or FAILURE
   =============================================================================
*/

short
sec_rmv(ns)
register short ns;
{
	register struct s_entry *cp, *lp, *rp;
	register short et;
	struct s_entry *pp;

	DB_ENTR("sec_rmv");

	secopok = TRUE;

	if (chksec(ns)) {		/* check that section is OK */

		DB_EXIT("sec_rmv - FAILED chksec");
		return(FAILURE);
	}

	pp = cp = p_send->e_fwd;	/* get adjustment point */

#if	DEBUGIT
	if (debugsw) {

		printf("sec_rmv:  t_cur = %ld, t_sbgn = %ld, t_send = %ld, t_sect = %ld\n",
			t_cur, t_sbgn, t_send, t_sect);

		printf("sec_rmv:  p_cur = $%08lX, p_sbgn = $%08lX, p_send = $%08lX\n",
			p_cur, p_sbgn, p_send);

		printf("sec_rmv: cp = $%08lX\n", cp);
	}
#endif

/* 
*/
	/* clip out the section and close up the hole */

	(p_sbgn->e_bak)->e_fwd = p_send->e_fwd;
	(p_send->e_fwd)->e_bak = p_sbgn->e_bak;
	p_sbgn->e_bak = E_NULL;
	p_send->e_fwd = E_NULL;

	/* adjust the times above the clip point to end of score */

	if (t_cur GE t_send)	/* adjust t_cur if above clip point */
		t_cur -= t_sect;

#if	DEBUGIT
	if (debugsw)
		printf("sec_rmv: adjusted t_cur = %ld\n", t_cur);
#endif

	while (EV_FINI NE (et = 0x007F & cp->e_type)) {

		cp->e_time -= t_sect;	/* adjust event time */
		cp = cp->e_fwd;		/* point at next event */
	}

#if	DEBUGIT
	if (debugsw)
		printf("sec_rmv: adjusted p_cur->e_time = %ld\n",
			p_cur->e_time);
#endif

	/* unlink event headers from hplist, fix pointers, and delete events */

	rp = p_sbgn;			/* start at the beginning */

	while (rp) {

		lp = rp->e_fwd;			/* get next event pointer */
		et = 0x007F & rp->e_type;	/* get event type */

		if (p_bak EQ rp)		/* fix p_bak */
			p_bak = pp;

		if (p_cur EQ rp)		/* fix p_cur */
			p_cur = pp;

		if (p_ctr EQ rp)		/* fix p_ctr */
			p_ctr = pp;

		if (p_fwd EQ rp)		/* fix p_fwd */
			p_fwd = pp;

		if (-1 NE ehdlist[et])			/* if it's a header ... */
			eh_rmv(rp, ehdlist[et]);	/* ... remove it from hplist */

		e_del(e_rmv(rp));		/* delete the event */
		rp = lp;			/* point at next event */
	}

	seclist[curscor][ns] = E_NULL;		/* delete section from seclist */
	DB_EXIT("sec_rmv");
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	sec_dgr() --delete notes in enabled groups in section 'ns'

	ns	section number to be processed

	returns SUCCESS or FAILURE
   =============================================================================
*/

short
sec_dgr(ns)
register short ns;
{
	register struct s_entry *lp, *rp;

	DB_ENTR("sec_dgr");

	secopok = TRUE;

	if (chksec(ns)) {		/* check that section is OK */

		DB_EXIT("sec_dgr - FAILED chksec");
		return(FAILURE);
	}

#if	DEBUGIT
	if (debugsw) {

		printf("sec_dgr:  t_cur = %ld, t_sbgn = %ld, t_send = %ld, t_sect = %ld\n",
			t_cur, t_sbgn, t_send, t_sect);

		printf("sec_dgr:  p_cur = $%08lX, p_sbgn = $%08lX, p_send = $%08lX\n",
			p_cur, p_sbgn, p_send);
	}
#endif

/* 
*/
	/* delete note events for record enabled groups */

	DB_CMNT("sec_dgr - deleting");

	rp = p_sbgn->e_fwd;			/* start at the beginning */

	while (rp NE p_send) {

		lp = rp->e_fwd;			/* get next event pointer */

		if (oktodg(rp)) {		/* if it's one we want ... */

			if (p_bak EQ rp)	/* fix p_bak */
				p_bak = lp;

			if (p_cur EQ rp)	/* fix p_cur */
				p_cur = lp;

			if (p_ctr EQ rp)	/* fix p_ctr */
				p_ctr = lp;

			if (p_fwd EQ rp)	/* fix p_fwd */
				p_fwd = lp;

			e_del(e_rmv(rp));	/* ... delete it */
		}

		rp = lp;			/* point at next event */
	}

	DB_EXIT("sec_dgr");
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	sec_dev() --delete non-note events in enabled groups in section 'ns'

	ns	section number to be processed

	returns SUCCESS or FAILURE
   =============================================================================
*/

short
sec_dev(ns)
register short ns;
{
	register struct s_entry *lp, *rp;

	DB_ENTR("sec_dev");

	secopok = TRUE;

	if (chksec(ns)) {		/* check that section is OK */

		DB_EXIT("sec_dev - FAILED chksec");
		return(FAILURE);
	}

#if	DEBUGIT
	if (debugsw) {

		printf("sec_dev:  t_cur = %ld, t_sbgn = %ld, t_send = %ld, t_sect = %ld\n",
			t_cur, t_sbgn, t_send, t_sect);

		printf("sec_dev:  p_cur = $%08lX, p_sbgn = $%08lX, p_send = $%08lX\n",
			p_cur, p_sbgn, p_send);
	}
#endif

/* 
*/
	/* delete non-note events for record enabled groups */

	DB_CMNT("sec_dev - deleting");

	rp = p_sbgn->e_fwd;			/* start at the beginning */

	while (rp NE p_send) {

		lp = rp->e_fwd;			/* get next event pointer */

		if (oktode(rp)) {		/* if it's one we want ... */

			if (p_bak EQ rp)	/* fix p_bak */
				p_bak = lp;

			if (p_cur EQ rp)	/* fix p_cur */
				p_cur = lp;

			if (p_ctr EQ rp)	/* fix p_ctr */
				p_ctr = lp;

			if (p_fwd EQ rp)	/* fix p_fwd */
				p_fwd = lp;

			e_del(e_rmv(rp));	/* ... delete it */
		}

		rp = lp;			/* point at next event */
	}

	DB_EXIT("sec_dev");
	return(SUCCESS);
}
