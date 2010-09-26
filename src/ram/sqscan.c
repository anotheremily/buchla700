/*
   =============================================================================
	sqscan.c -- scan a string and turn it into score events
	Version 24 -- 1988-06-20 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	CHEKSTOP	1		/* non-zero to trap to ROMP on error */

#include "stddefs.h"
#include "cmeta.h"
#include "score.h"
#include "scfns.h"
#include "biosdefs.h"

#if	CHEKSTOP
short	chkstop = TRUE;
#endif

extern	short	notenum, notepit, curgrp, thescore, verbose;
extern	short	testing;
extern	short	sharp, endflg;

extern	long	curtime, noteon, noteoff, noteval, noteper, nrest;
extern	long	dvwork;

char	*nlist[]  = { "a", "b", "c", "d", "e", "f", "g", NULL };

int	notetab[] = { 0, 2, 3, 5, 7, 8, 10 };
int	octab[]   = { 21, 33, 45, 57, 69, 81, 93, 105 };

/* 
*/

/*
   =============================================================================
	nospace(et) -- print error message for no space condition
   =============================================================================
*/

nospace(et)
char *et;
{

#if	CHEKSTOP
	if (chkstop) {

		printf("\n** sqscan: ERROR - no space for %s **\n\n", et);
		xtrap15();	/* ERROR:  trap to ROMP */

		SEsnap();
		waitcr();
	}
#endif
}

/* 
*/

/*
   =============================================================================
	Pcheck(ptr, msg) -- check 'ptr' for validity -- output msg if bad.
	A pointer is invalid if it points outside the score or is null.
	Returns SUCCESS for a good pointer, FAILURE for a bad one.
   =============================================================================
*/

int
Pcheck(ptr, msg)
struct s_entry *ptr;
char *msg;
{
	register struct s_entry *cval;

	if (ptr EQ E_NULL) {

		printf("** Pcheck($%08.8lx):  ZERO - %s **\n", ptr, msg);

#if	CHEKSTOP
		if (chkstop)
			xtrap15();	/* ERROR:  trap to ROMP */
#endif
		return(FAILURE);
	}
		
	cval = spool;

	if (ptr LT cval) {

		printf("** Pcheck($%08.8lx):  LOW - %s **\n", ptr, msg);
#if	CHEKSTOP
		if (chkstop)
			xtrap15();	/* ERROR:  trap to ROMP */
#endif
		return(FAILURE);
	}
		
	cval = &spool[(long)MAX_SE-1];

	if (ptr GT cval) {

		printf("** Pcheck($%08.8lx): HIGH - %s **\n", ptr, msg);
#if	CHEKSTOP
		if (chkstop)
			xtrap15();	/* ERROR:  trap to ROMP */
#endif
		return(FAILURE);
	}
		
	return(SUCCESS);
}

/* 
*/

/*
   =============================================================================
	insnevt() -- insert a note event at t_cur/p_cur
   =============================================================================
*/

struct n_entry *
insnevt(nsp, nt, grp, note, vel)
register struct n_entry *nsp;
short nt, grp, note, vel;
{
	nsp->e_time  = t_cur;
	nsp->e_type  = nt;
	nsp->e_note  = note;
	nsp->e_group = grp;
	nsp->e_vel   = vel;

	return(e_ins((struct s_entry *)nsp, ep_adj(p_cur, 0, t_cur))->e_fwd);
}

/* 
*/

/*
   =============================================================================
	Qevent() -- 'event' syntax equation
   =============================================================================
*/

int
Qevent()
{
	register int aux1, aux2, aux3, aux4, aux5;
	register char *chptr;
	register struct s_entry *tsp1, *tsp2;
	struct s_entry *tsp3;

	if (!CM_CHR('!'))		/* all commands start with ! */
		CM_NOGO;

	if (CM_USTR("group")) {		/* !group = n */

		if (!CM_CHR('='))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		curgrp = QQnum;

		if (verbose)
			printf("<Current group = %d>\n", curgrp);

		CM_OK;
	}

/* 
*/
	if (CM_USTR("status")) {	/* !status = {on|off} */

		if (!CM_CHR('='))
			CM_NOGO;

		if (CM_USTR("on")) {

			if (E_NULL EQ (tsp1 = e_alc(E_SIZE2))) {

				nospace("!status=on");
				CM_NOGO;
			}

			tsp1->e_time  = t_cur;
			tsp1->e_type  = EV_GRP;
			tsp1->e_data1 = curgrp;
			tsp1->e_data2 = GS_ENBL;

			p_cur = e_ins(tsp1, ep_adj(p_cur, 0, t_cur))->e_fwd;
			eh_ins(tsp1, EH_GRP);

			if (verbose)
				printf("%8ld: Group %d enabled\n",
					t_cur, curgrp);
			CM_OK;

		} else if (CM_USTR("off")) {

			if (E_NULL EQ (tsp1 = e_alc(E_SIZE2))) {

				nospace("!status=off");
				CM_NOGO;
			}

			tsp1->e_time  = t_cur;
			tsp1->e_type  = EV_GRP;
			tsp1->e_data1 = curgrp;
			tsp1->e_data2 = GS_DSBL;

			p_cur = e_ins(tsp1, ep_adj(p_cur, 0, t_cur))->e_fwd;
			eh_ins(tsp1, EH_GRP);

			if (verbose)
				printf("%8ld: Group %d disabled\n",
					t_cur, curgrp);
			CM_OK;

		} else
			CM_NOGO;
	}

/* 
*/
	if (CM_USTR("tempo")) {		/* !tempo = n */

		if (!CM_CHR('='))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		aux1 = QQnum;

		if (E_NULL EQ (tsp1 = e_alc(E_SIZE2))) {

			nospace("!tempo");
			CM_NOGO;
		}

		tsp1->e_time  = t_cur;
		tsp1->e_type  = EV_TMPO;
		tsp1->e_data1 = aux1;

		p_cur = e_ins(tsp1, ep_adj(p_cur, 0, t_cur))->e_fwd;
		eh_ins(tsp1, EH_TMPO);

		if (verbose)
			printf("%8ld: Tempo = %d\n", t_cur, aux1);
		CM_OK;
	}
/* 
*/
	if (CM_USTR("inst")) {		/* !inst = n */

		if (!CM_CHR('='))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		aux1 = QQnum;

		if (E_NULL EQ (tsp1 = e_alc(E_SIZE2))) {

			nospace("!inst");
			CM_NOGO;
		}

		tsp1->e_time  = t_cur;
		tsp1->e_type  = EV_INST;
		tsp1->e_data1 = curgrp;
		tsp1->e_data2 = aux1;

		p_cur = e_ins(tsp1, ep_adj(p_cur, 0, t_cur))->e_fwd;
		eh_ins(tsp1, EH_INST);

		if (verbose)
			printf("%8ld: group %d set to inst %d\n",
				t_cur, curgrp, aux1);
		CM_OK;
	}
/* 
*/
	if (CM_USTR("tuning")) {	/* !tuning = n */

		if (!CM_CHR('='))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		aux1 = QQnum;

		if (E_NULL EQ (tsp1 = e_alc(E_SIZE2))) {

			nospace("!tuning");
			CM_NOGO;
		}

		tsp1->e_time  = t_cur;
		tsp1->e_type  = EV_TUNE;
		tsp1->e_data1 = aux1;

		p_cur = e_ins(tsp1, ep_adj(p_cur, 0, t_cur))->e_fwd;
		eh_ins(tsp1, EH_TUNE);

		if (verbose)
			printf("%8ld: Tuning %d selected\n", t_cur, aux1);
		CM_OK;
	}
/* 
*/
	if (CM_USTR("trans")) {		/* !trans = {+|-} n */

		if (!CM_CHR('='))
			CM_NOGO;

		if (CM_CHR('+')) {

		} else if (CM_CHR('-')) {

		} else
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		aux1 = QQnum;

		if (QQchr EQ '-')
			aux1 = -aux1;

		if (E_NULL EQ (tsp1 = e_alc(E_SIZE2))) {

			nospace("!trans");
			CM_NOGO;
		}

		tsp1->e_time  = t_cur;
		tsp1->e_type  = EV_TRNS;
		tsp1->e_data1 = curgrp;
		(long)(tsp1->e_lft) = aux1;

		p_cur = e_ins(tsp1, ep_adj(p_cur, 0, t_cur))->e_fwd;
		eh_ins(tsp1, EH_TRNS);

		if (verbose)
			printf("%8ld: Transposition set to %d\n",
				t_cur, aux1);
		CM_OK;
	}
/* 
*/
	if (CM_USTR("dyn")) {		/* !dyn = n */

		if (!CM_CHR('='))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		aux1 = QQnum;

		if (E_NULL EQ (tsp1 = e_alc(E_SIZE2))) {

			nospace("!dyn");
			CM_NOGO;
		}

		tsp1->e_time  = t_cur;
		tsp1->e_type  = EV_DYN;
		tsp1->e_data1 = curgrp;
		tsp1->e_data2 = aux1;

		p_cur = e_ins(tsp1, ep_adj(p_cur, 0, t_cur))->e_fwd;
		eh_ins(tsp1, EH_DYN);

		if (verbose)
			printf("%8ld: Dynamics set to %d\n", t_cur, aux1);
		CM_OK;
	}
/* 
*/
	if (CM_USTR("loc")) {		/* !loc = n */

		if (!CM_CHR('='))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		aux1 = QQnum;

		if (E_NULL EQ (tsp1 = e_alc(E_SIZE2))) {

			nospace("!loc");
			CM_NOGO;
		}

		tsp1->e_time  = t_cur;
		tsp1->e_type  = EV_LOCN;
		tsp1->e_data1 = curgrp;
		tsp1->e_data2 = aux1;
		
		p_cur = e_ins(tsp1, ep_adj(p_cur, 0, t_cur))->e_fwd;
		eh_ins(tsp1, EH_LOCN);

		if (verbose)
			printf("%8ld: Location set to %d\n", t_cur, aux1);
		CM_OK;
	}
/* 
*/
	if (CM_USTR("goto")) {		/* !goto n @ n */

		if (!CM_NUM)
			CM_NOGO;

		curtime = QQnum * 48;

		if (!CM_CHR('@'))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		sc_goto(curtime += QQnum);

		if (verbose)
			printf("\n<Score time set to %ld>\n", t_cur);

		CM_OK;
	}

/* 
*/

	if (CM_USTR("pos")) {		/* !pos = n @ n */

		if (!CM_CHR('='))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		curtime = QQnum * 48;

		if (!CM_CHR('@'))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		t_cur = (curtime += QQnum);
		p_cur = frfind(t_cur, 0);

		if (verbose)
			printf("\n<Score time set to %ld>\n", t_cur);

		CM_OK;
	}
/* 
*/
	if (CM_USTR("beat")) {		/* !beat = n */

		if (!CM_CHR('='))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		t_cur = QQnum * 48;
		p_cur = frfind(t_cur, 0);

		if (verbose)
			printf("\n<Score time set to %ld>\n", t_cur);

		CM_OK;
	}
/* 
*/
	if (CM_USTR("frame")) {		/* !frame = n */

		if (!CM_CHR('='))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		t_cur = QQnum;
		p_cur = frfind(t_cur, 0);

		if (verbose)
			printf("\n<Score time set to %ld>\n", t_cur);

		CM_OK;
	}
/* 
*/
	if (CM_USTR("score")) {		/* !score = n */

		if (!CM_CHR('='))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		if (QQnum > 127)
			CM_NOGO;

		thescore = QQnum;

		selscor(thescore);

		if (verbose)
			printf("\n<Score %d selected>\n", thescore);
		CM_OK;
	}
/* 
*/
	if (CM_USTR("weight")) {	/* !weight = n */

		if (!CM_CHR('='))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		if (QQnum > 100L)
			CM_NOGO;

		noteper = QQnum;

		if (verbose)
			printf("<Note weight = %ld percent>\n", noteper);

		CM_OK;
	}

	if (CM_USTR("snap")) {		/* !snap */

		SEsnap();
		CM_OK;
	}

	if (CM_USTR("wait")) {		/* !wait */

		waitcr();
		CM_OK;
	}

	if (CM_USTR("stop")) {		/* !stop */

		if (E_NULL EQ (tsp1 = e_alc(E_SIZE1))) {

			nospace("!stop");
			CM_NOGO;
		}

		tsp1->e_time = t_cur;
		tsp1->e_type = EV_STOP;

		p_cur = e_ins(tsp1, ep_adj(p_cur, 0, t_cur))->e_fwd;

		if (verbose)
			printf("%8ld: Stop entered\n", t_cur);
		CM_OK;
	}
/* 
*/
	if (CM_USTR("clear")) {		/* !clear {n | * | $} */

		if (CM_NUM) {

			aux1 = QQnum;
			sc_clr(aux1);

			if (verbose)
				printf("\n<Score %d cleared>\n", aux1);

			CM_OK;
		}

		if (CM_CHR('*')) {

			scinit();

			if (verbose)
				printf("\n<All scores cleared>\n");

			CM_OK;
		}

		if (CM_CHR('$')) {

			sc_clr(curscor);

			if (verbose)
				printf("\n<Current score (%d) cleared>\n", curscor);

			CM_OK;
		}

		CM_NOGO;
	}
/* 
*/
	if (CM_USTR("show")) {		/* !show {active | names | sections} */

		if (CM_USTR("active")) {

			printf("<Active scores:\n");
			aux1 = aux2 = 0;

			for (aux3 = 0; aux3 < 4; aux3++) {

				printf("<");

				for (aux4 = 0; aux4 < 16; aux4++) {

					if (scores[aux1]) {

						printf("%3d ", aux1);
						++aux2;

					} else {

						printf("... ");
					}

					++aux1;
				}

				printf(">\n");
			}

			printf("<%d active scores, score %d is current>\n\n",
				aux2, curscor);
			CM_OK;
		}
/* 
*/
		if (CM_USTR("names")) {

			printf("<Active score names:>\n");
			aux2 = 0;

			for (aux1 = 0; aux1 < 128; aux1++) {

				if (scores[aux1] NE E_NULL) {

					printf("<%3d: ", aux1);
					chptr = scname[aux1];
					printf("[$%08.8lx, $%08.8lx] ",
						scores[aux1], chptr);

					for (aux3 = 0; aux3 < 16; aux3++)
						printf("%c", (*chptr++ & 0xFF));

					printf(">\n");
					++aux2;
				}
			}

			printf("<%d active scores, %d is current>\n\n",
				aux2, curscor);

			CM_OK;
		}
/* 
*/
		if (CM_USTR("sections")) {

			printf("<Active sections:>\n");
			aux1 = aux2 = 0;

			for (aux3 = 0; aux3 < 5; aux3++) {

				printf("<");

				for (aux4 = 0; aux4 < 10; aux4++) {

					if (seclist[curscor][aux1]) {

						printf("%3d ", aux1);
						++aux2;

					} else {

						printf("... ");
					}

					++aux1;
				}

				printf(">\n");
			}

			printf("<%d active sections, %d is current>\n\n",
				aux2, cursect);
			CM_OK;
		}

		CM_NOGO;
	}
/* 
*/
	if (CM_USTR("find")) {		/* !find {l | r} n */

		if (CM_UCHR('l'))
			aux1 = 1;
		else if (CM_UCHR('r'))
			aux1 = 0;
		else
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		tsp1 = frfind(QQnum, aux1);

		if (tsp1 NE E_NULL) {

			if (verbose)
				printf("\n<FIND:  Found %ld at $%08.8lx>\n\n",
					QQnum, tsp1);

			p_cur = tsp1;
			t_cur = QQnum;

		} else {

			if (verbose)
				printf("<FIND:  Found the current score empty>\n\n");

			CM_NOGO;
		}

		CM_OK;
	}
/* 
*/
	if (CM_USTR("chase")) {		/* !chase */

		if (E_NULL EQ (tsp1 = p_cur)) {

			printf("<CHASE:  Current score not active>\n\n");
			CM_NOGO;
		}

		tsp2 = tsp1;

		while (tsp2) {

			tsp3 = &spool[0];

			if (tsp2 LT tsp3) {

				printf("\nCHASE:  Error\n");
				printf("** Bad pointer:  $%08.8lx\n", tsp2);
				printf("**       spool:  $%08.8lx,  pspool: $%08.8lx\n",
					&spool[0], pspool);
				CM_NOGO;
			}

			tsp3 = &spool[MAX_SE-1];

			if (tsp2 GT tsp3) {

				printf("\nCHASE:  Error\n");
				printf("** Bad pointer:  $%08.8lx\n", tsp2);
				printf("**       spool:  $%08.8lx,  pspool: $%08.8lx\n",
					&spool[0], pspool);
				CM_NOGO;
			}

			SEdump(tsp2);
			tsp2 = tsp2->e_fwd;

			if ((tsp1 EQ tsp2) OR
			    (tsp2->e_type EQ EV_SCORE)) {

				printf("-- End of chain --\n\n");
				break;
			}
		}

		CM_OK;
	}
/* 
*/		
	if (CM_USTR("verbose")) {	/* !verbose */

		verbose = TRUE;
		CM_OK;
	}

	if (CM_USTR("quiet")) {		/* !quiet */

		verbose = FALSE;
		CM_OK;
	}

	if (CM_USTR("test")) {		/* !test */

		testing = TRUE;
		CM_OK;
	}

	if (CM_USTR("normal")) {	/* !normal */

		testing = FALSE;
		CM_OK;
	}
		
	if (CM_USTR("end")) {		/* !end */

		if (verbose)
			printf("\n<End command encountered>\n");

		endflg = TRUE;
		CM_OK;
	}

	CM_NOGO;
}

/* 
*/

/*
   =============================================================================
	Qnote() -- 'note' and rest syntax equation

		"val [#] oct [+|-|/n]" | "r n / m"
		 e.g.:  a#0+ .. a#0- .. c3/4 , r2/1 d3 , e3
   =============================================================================
*/

int
Qnote()
{
	struct n_entry *nsp1;

	if (CM_UCHR('r')) {		/* try for a rest */

		if (!CM_NUM)
			CM_NOGO;

		nrest = QQnum;

		if (!CM_CHR('/'))
			CM_NOGO;

		if (!CM_NUM)
			CM_NOGO;

		dvwork  = 192L;
		noteval = dvwork / QQnum;
		t_cur  += (noteval * nrest);
		p_cur   = ep_adj(p_cur, 0, t_cur);

		if (verbose)
			printf("%8ld: <rest>\n", t_cur);

		CM_OK;
	}
/* 
*/
	if (!CM_ULIST(nlist))		/* try for a note */
		CM_NOGO;

	notepit = QQlnum;

	if (CM_CHR('#'))
		sharp = 1;
	else
		sharp = 0;

	if (!CM_DIG)
		CM_NOGO;

	if (QQdig > '7')
		CM_NOGO;

	notenum = octab[QQdig - '0'] + notetab[notepit] + sharp;

	if (CM_CHR('+')) {	/* output note begin */

		if (E_NULL EQ (nsp1 = (struct n_entry *)e_alc(E_SIZE1))) {

			nospace("note event");
			CM_NOGO;
		}

		noteon = t_cur;
		p_cur = insnevt(nsp1, EV_NBEG, curgrp, notenum, 64);

		if (verbose)
			printf("%8ld: Note %3d ON\n", noteon, notenum);

		CM_OK;
	}
/* 
*/
	if (CM_CHR('-')) {	/* output note end */

		if (E_NULL EQ (nsp1 = (struct n_entry *)e_alc(E_SIZE1))) {

			nospace("note event");
			CM_NOGO;
		}

		noteoff = t_cur;
		p_cur = insnevt(nsp1, EV_NEND, curgrp, notenum, 64);

		if (verbose)
			printf("%8ld: Note %3d OFF\n", noteoff, notenum);

		CM_OK;
	}
/* 
*/
	if (CM_CHR('/')) {	/* output note begin and end, given value */

		if (!CM_NUM)
			CM_NOGO;

		dvwork  = 192L;
		noteval = dvwork / QQnum;
		noteon  = t_cur;
		dvwork  = 100L;
		noteoff = t_cur + ((noteper * noteval) / dvwork);

		if (E_NULL EQ (nsp1 = (struct n_entry *)e_alc(E_SIZE1))) {

			nospace("note event");
			CM_NOGO;
		}

		p_cur = insnevt(nsp1, EV_NBEG, curgrp, notenum, 64);

		if (E_NULL EQ (nsp1 = (struct n_entry *)e_alc(E_SIZE1))) {

			nospace("note event");
			CM_NOGO;
		}

		p_cur = ep_adj(p_cur, 0, (t_cur = noteoff));

		insnevt(nsp1, EV_NEND, curgrp, notenum, 64);

		p_cur = ep_adj(p_cur, 0, (t_cur = noteon));

		if (verbose)
			printf("%8ld: Note %3d ON at %8ld, OFF at %8ld\n",
				t_cur, notenum, noteon, noteoff);

		CM_OK;
	}
/* 
*/
	/* output note begin and end, use previous value */

	noteon = t_cur;
	dvwork = 100L;
	noteoff = t_cur + ((noteval * noteper) / dvwork);

	if (E_NULL EQ (nsp1 = (struct n_entry *)e_alc(E_SIZE1))) {

		nospace("note event");
		CM_NOGO;
	}

	p_cur = insnevt(nsp1, EV_NBEG, curgrp, notenum, 64);

	if (E_NULL EQ (nsp1 = (struct n_entry *)e_alc(E_SIZE1))) {

		nospace("note event");
		CM_NOGO;
	}

	p_cur = ep_adj(p_cur, 0, (t_cur = noteoff));

	insnevt(nsp1, EV_NEND, curgrp, notenum, 64);

	p_cur = ep_adj(p_cur, 0, (t_cur = noteon));

	if (verbose)		
		printf("%8ld: Note %3d ON at %8ld, OFF at %8ld\n",
			t_cur, notenum, noteon, noteoff);
	CM_OK;
}

/* 
*/

/*
   =============================================================================
	Qadv() -- 'adv' syntax equation
   =============================================================================
*/

int
Qadv()
{
	if (CM_CHR('.')) {	/* advance by 1 frame */

		++t_cur;
		p_cur = ep_adj(p_cur, 0, t_cur);
		CM_OK;
	}

	if (CM_CHR(',')) {	/* advance by current note value */

		t_cur += noteval;
		p_cur = ep_adj(p_cur, 0, t_cur);
		CM_OK;
	}

	if (CM_CHR(';')) {	/* avance to next beat */

		dvwork = 48L;
		t_cur = ((t_cur / dvwork) + 1L) * 48L;
		p_cur = ep_adj(p_cur, 0, t_cur);
		CM_OK;
	}

	if (CM_CHR(':')) {	/* advance by one beat interval */

		t_cur += 48L;
		p_cur = ep_adj(p_cur, 0, t_cur);
		CM_OK;
	}

	CM_NOGO;
}

/* 
*/

/*
   =============================================================================
	Qseq() -- 'seq' syntax equation
   =============================================================================
*/

int
Qseq()
{
	CM_DBLK;

	if (!*QQip)
		CM_OK;

	if (Qnote() OR Qadv() OR (Qevent() AND !endflg)) {

		CM_DBLK;

		if (!*QQip)
			return(QQsw);

		while (QQsw AND !endflg) {

			if (!Qadv())
				if (!Qnote())
					Qevent();

			CM_DBLK;

			if (!*QQip)
				return(QQsw);
		}
	}

	return(QQsw);
}

/* 
*/

/*
   =============================================================================
	sqinit() -- setup score interpreter variables
   =============================================================================
*/

sqinit()
{
	verbose = FALSE;
	testing = FALSE;
	endflg  = FALSE;

	noteval = 48L;		/* default value = 1/4 note  (192/48) */
	noteper = 80L;		/* default weight = 80 percent */

	curtime = t_cur = t_ctr = 0L;
	t_bak   = t_cur - TO_BAK;
	t_fwd   = t_cur + TO_FWD;

	p_bak = p_cur = p_ctr = p_fwd = E_NULL;

	curgrp   = 0;
	thescore = 0;
}

/* 
*/

/*
   =============================================================================
	sqscan(ip) -- scans the string at 'ip' and converts the event
	descriptions therein to events in the current score.  Returns
	the value of the parser switch.
   =============================================================================
*/

int
sqscan(ip)
char *ip;
{
	endflg = FALSE;
	CMinit(ip);

	if (!Qseq())
		CMstat("Syntax error");

	return(QQsw);
}
