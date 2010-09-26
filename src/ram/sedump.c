/*
   =============================================================================
	sedump.c -- dump various kinds of MIDAS-VII data in readable format
	Version 42 -- 1988-08-24 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "graphdef.h"
#include "hwdefs.h"
#include "stddefs.h"
#include "score.h"
#include "scfns.h"
#include "slice.h"
#include "secdefs.h"
#include "vsdd.h"

#include "midas.h"
#include "instdsp.h"

extern	unsigned	scrl;

extern	short	curfunc;
extern	short	curvce;
extern	short	sbase;
extern	short	sd;
extern	short	se;
extern	short	soffset;
extern	short	subj;

extern	short	varmode[][16];

extern	struct	gdsel	*gdstbc[NGDSEL];
extern	struct	gdsel	*gdstbn[NGDSEL];
extern	struct	gdsel	*gdstbp[NGDSEL];

extern	struct	gdsel	*gdfsep;
extern	struct	gdsel	gdfsl[MAXFSL];

extern	struct	instdef	vbufs[];

extern	char	*idbxlbl[];
extern	char	*osclbl[];

/* 
*/

char	*A6PTR = 0L;		/* traceback a6 starting address */
char	*A7PTR = 0L;		/* traceback a7 starting address */
char	*A7TOP = 0x000FFFFFL;	/* traceback stack top */

short	SCnumv = 0;		/* voice for SCvoice()  to dump (0..11) */
short	SL_Flag;		/* ROMP trap disable flag */
short	x_unrec;		/* unrecognized event type or size flag */

long	SCdlim = MAX_SE;	/* score dump limit */

char *evkinds[N_ETYPES] = {	/* event types (must match score.h) */

	"00: EV_NULL  null event   ",	"01: EV_SCORE score begin  ",
	"02: EV_SBGN  section begin",	"03: EV_SEND  section end  ",
	"04: EV_INST  instr. change",	"05: EV_NBEG  note begin   ",
	"06: EV_NEND  note end     ",	"07: EV_STOP  stop         ",
	"08: EV_INTP  interpolate  ",	"09: EV_TMPO  tempo        ",
	"0A: EV_TUNE  tuning       ",	"0B: EV_GRP   group status ",
	"0C: EV_LOCN  location     ",	"0D: EV_DYN   dynamics     ",
	"0E: EV_ANVL  analog value ",	"0F: EV_ANRS  analog res.  ",
	"10: EV_ASGN  I/O assign   ",	"11: EV_TRNS  transposition",
	"12: EV_REPT  repeat       ",	"13: EV_PNCH  punch in/out ",
	"14: EV_PRES  poly pressure",	"15: EV_FINI  score end    ",
	"16: EV_CPRS  chan pressure",	"17: EV_BAR   bar marker   "
};

char	*hpname[N_TYPES] = {	/* header type names (must match score.h) */

	"EH_INST",	"EH_GRP ",	"EH_LOCN",	"EH_DYN ",
	"EH_ANRS",	"EH_TRNS",	"EH_INTP",	"EH_TMPO",
	"EH_TUNE",	"EH_ASGN",	"EH_SBGN",	"EH_SEND"
};

char	*var_lbl[6] = {		/* variable names */

	"Pch/Hor",	"Mod/Vrt",	"Brth/LP",	"GPC/CV1",
	"Pedal 1",	"Key Prs"
};

char	*srcname[] = {		/* source names (must match smdefs.h) */

	"NONE",	"RAND",	"CTL1",	"?03?",	"?04?",	"PTCH",	"KPRS",	"KVEL",
	"PED1",	"?09?",	"FREQ",	"HTPW",	"VTMW",	"LPBR"
};

char	*actname[] = {		/* function action names */

	"NULL",	"SUST",	"ENBL",	"JUMP",	"LOOP",	"KYUP",	"KYDN",	"HERE"
};

/* 
*/

/*
   =============================================================================
	ev_kind(sep) -- returns a pointer to a string describing the event
	at 'sep', or a NULL pointer if the event is unrecognized.
	Sets x_unrec according to the result.
   =============================================================================
*/

char *
ev_kind(sep)
struct s_entry *sep;
{
	if ((sep->e_type & 0x00FF) GE N_ETYPES) {

		x_unrec = TRUE;
		return(NULL);
	}

	x_unrec = FALSE;

	return(evkinds[sep->e_type]);
}

/* 
*/

/* 
   =============================================================================
	SEctrl() -- print current score pointers and times
   =============================================================================
*/

SEctrl()
{
	printf("curscor: %d  \"%-16.16s\"  cursect: %d  scp: $%08lX\n\n",
		curscor, scname[curscor], cursect, scp);


	printf("   fc_val: %8ld  fc_sw:  %d\n\n",
		fc_val, fc_sw);


	printf("   t_bak:  %8ld  t_cur:  %8ld  t_ctr:  %8ld  t_fwd:  %8ld\n",
		t_bak, t_cur, t_ctr, t_fwd);

	printf("   p_bak: $%08lX  p_cur: $%08lX  p_ctr: $%08lX  p_fwd: $%08lX\n\n",
		p_bak, p_cur, p_ctr, p_fwd);

}

/*
   =============================================================================
	SEsnap() -- snap dump critical score storage variables
   =============================================================================
*/

SEsnap()
{
	register short i, j;

	printf("\n");

	printf("evleft: %ld  spcount: %ld  frags: %ld\n",
		evleft(), spcount, frags);

	printf("   se1_cnt=%ld  se2_cnt=%ld  se3_cnt=%ld\n",
		se1_cnt, se2_cnt, se3_cnt);

	printf("   pspool=$%08lX  size1=$%08lX  size2=$%08lX  size3=$%08lX\n",
		pspool, size1, size2, size3);

	SEctrl();

	for (i = 0; i < N_SCORES; i++)
		printf("%2d:  \"%-16.16s\"  $%08lX %s\n",
			i + 1, scname[i], scores[i],
			(i EQ curscor) ? "<--- curscor" : "");

	printf("\n\n");

	printf("Variable modes for each group:\n\n");

	printf("V# VarName  01 02 03 04 05 06 07 08 09 10 11 12\n");
	printf("-- -------  -- -- -- -- -- -- -- -- -- -- -- --\n");

	for (i = 0; i < 6; i++) {

		printf("%02d %s  ", i, var_lbl[i]);

		for (j = 0; j < 12; j++)
			printf(" %d ", varmode[i][j]);

		printf("\n");
	}

	printf("\n");
}

/* 
*/

/*
   =============================================================================
	SEdump(sep) -- dumps the event at 'sep' in readable format.
	Returns 'sep'.  Sets x_unrec TRUE if the event is unrecognized,
	FALSE if the event is recognized.
   =============================================================================
*/

struct s_entry *
SEdump(sep)
struct s_entry *sep;
{
	char	*et;

	x_unrec = TRUE;

	switch (sep->e_size) {

	case E_SIZE1:
	case E_SIZE2:
	case E_SIZE3:

		break;

	default:

		printf("[%08lX]:  ** Bad event size: $%02.2X **\n",
			sep, sep->e_size);

		return(sep);
	}

	if (NULL EQ (et = ev_kind(sep))) {

		printf("[%08lX]:  ** Bad event type: $%02.2X **\n",
			sep, sep->e_type);

		return(sep);
	}

	x_unrec = FALSE;

	printf("$%08lX: t=%10ld  F:$%08lX  B:$%08lX * %s\n",
		sep, sep->e_time, sep->e_fwd, sep->e_bak, et);

	printf("  data = $%02.2X $%02.2X",
		0x00FF & sep->e_data1, 0x00FF & sep->e_data2);

	if (sep->e_size EQ E_SIZE1)
	    	printf(" $%04.4X $%04.4X",
			(struct n_entry *)sep->e_vel,
			(struct n_entry *)sep->e_data4);

	printf("\n");

	if (sep->e_size GT E_SIZE1)
		printf("  up: $%08lX  dn: $%08lX",
			sep->e_up, sep->e_dn);
	else
		return(sep);

	if (sep->e_size GT E_SIZE2)
		printf("  lft: $%08lX  rgt: $%08lX",
			sep->e_lft, sep->e_rgt);

	printf("\n");

	return(sep);
}

/* 
*/

/*
   =============================================================================
	SEchase() -- print up to 'n' events or to the end of the score,
	starting with event 'ep'.
   =============================================================================
*/

struct s_entry *
SEchase(ep, n)
register struct s_entry *ep;
register long n;
{
	register long i;
	register struct s_entry *np;

	printf("\n");

	if (ep EQ E_NULL) {

		printf("NULL pointer\n");
		return(scp);
	}

	if (Pcheck(ep, "ep - SEchase()"))
		return(scp);

	for (i = 0; i < n; i++) {

		SEdump(ep);

		if ((ep->e_type EQ EV_FINI) OR x_unrec)
			return(scp);

		np = ep->e_fwd;

		if (Pcheck(np, "e_fwd - SEchase()"))
			return(scp);

		if (Pcheck(ep->e_bak, "e_bak - SEchase()"))
			return(scp);

		ep = np;
	}

	printf("\n");

	return(ep);
}

/* 
*/

/*
   =============================================================================
	SLdump() -- print slice control data
   =============================================================================
*/

SLdump()
{
	register short i;
	register struct gdsel *gp;

	printf("\n");

	printf("sd = %s  se = %s  sbase = %d  soffset = %d  scrl = $%04.4X\n",
		sd ? "BAK" : "FWD", se ? "BAK" : "FWD", sbase, soffset, scrl);

	printf("gdfsep = $%08lX\n\n", gdfsep);

	printf("gr  $ gdstbp  $ gdstbc  $ gdstbn\n");
	printf("    %08lX  %08lX  %08lX\n", gdstbp, gdstbc, gdstbn);
	printf("--  --------  --------  --------\n");

	for (i = 0; i < NGDSEL; i++) {

		printf("%2d  %08lX  %08lX  %08lX\n",
			i + 1, gdstbp[i], gdstbc[i], gdstbn[i]);

		if (i EQ 11)
			printf("\n");
	}

	printf("\n");

	if (SL_Flag EQ FALSE)
		xtrap15();

	SL_Flag = FALSE;
}

/* 
*/

/*
   =============================================================================
	SECdump() -- dump section variables and hplist
   =============================================================================
*/

SECdump()
{
	register short i;

	printf("p_sbgn = $%08lX  p_send = $%08lX\n",
		p_sbgn, p_send);

	printf("t_sbgn =  %8ld  t_send =  %8ld  t_sect =  %8ld\n\n",
		t_sbgn, t_send, t_sect);


	printf("p_cbgn = $%08lX  p_cend = $%08lX\n",
		p_cbgn, p_cend);

	printf("t_cbgn =  %8ld  t_cend =  %8ld\n\n",
		t_cbgn, t_cend);


	printf("seclist[curscor][]\n");
	printf("------------------\n\n");

	printf("Sec Addr_____  Sec Addr_____  Sec Addr_____  Sec Addr_____  Sec Addr_____  \n");

	for (i = 0; i < N_SECTS; i += 5) {

		printf("%2d  $%08lX  ", i + 1, seclist[curscor][i]);

		if ((i + 1) < N_SECTS)
			printf("%2d  $%08lX  ", i + 2, seclist[curscor][i + 1]);

		if ((i + 2) < N_SECTS)
			printf("%2d  $%08lX  ", i + 3, seclist[curscor][i + 2]);

		if ((i + 3) < N_SECTS)
			printf("%2d  $%08lX  ", i + 4, seclist[curscor][i + 3]);

		if ((i + 4) < N_SECTS)
			printf("%2d  $%08lX  ", i + 5, seclist[curscor][i + 4]);

		printf("\n");
	}

	printf("\n");

	printf("hplist[curscor][]\n");
	printf("-----------------\n");
	printf("Type___  Addr_____\n");

	for (i = 0; i < N_TYPES; i++)
		printf("%s  $%08lX\n", hpname[i], hplist[curscor][i]);

	printf("\n");
}

/* 
*/

/*
   =============================================================================
	DOA() -- do a simple stack traceback
   =============================================================================
*/

DOA()
{
	register long *olda6, *cura6;
	register short n, *prptr;

	if (A6PTR AND A7PTR) {

		printf("Stack dump:  $%08lX to $%08lX\n\n", A7PTR, A7TOP);
		mdump(A7PTR, A7TOP, A7PTR);
		printf("\n\n");
		printf("Stack traceback:  from A6 = $%08lX\n\n", A6PTR);
		printf("A6          Old A6    Return\n");

	} else {

		printf("Set A6PTR ($%08lX) and A7PTR ($%08lX) first\n",
			&A6PTR, &A7PTR);

		xtrap15();
	}

	cura6 = A6PTR;

	while (cura6) {

		olda6 = *cura6;

		printf("$%08lX:  $%08lX  $%08lX\n",
			cura6, olda6, *(cura6 + 4L));

		prptr = cura6 + 8L;
		n = 8;

		while (prptr < olda6) {

			printf("  +%-4d [$%08lX]:  $%04.4X\n",
				n, prptr, *prptr);

			n += 2;
			++prptr;
		}

		cura6 = olda6;
	}

	xtrap15();
}

/* 
*/

/*
   =============================================================================
	SCPanic() -- print the score control variables
   =============================================================================
*/

SCPanic()
{
	SEsnap();		/* control data */
	xtrap15();
}

/*
   =============================================================================
	SCdump() -- print the score control variables and the current score
   =============================================================================
*/

SCdump()
{
	SEsnap();		/* control data */
	SECdump();		/* section variables */
	SEchase(scp, SCdlim);	/* current score */
	xtrap15();
}

/*
   =============================================================================
	SCcrash() -- print all of the score related data and the current score
   =============================================================================
*/

SCcrash()
{
	SL_Flag = TRUE;
	SLdump();		/* slice data */
	SCdump();		/* control data and current score */
}

/*
   =============================================================================
	SCtimes() -- print the score times and pointers
   =============================================================================
*/

SCtimes()
{
	SEctrl();
	xtrap15();
}

/* 
*/

/*
   =============================================================================
	SCslice() -- print details of the slices
   =============================================================================
*/

SCslice()
{
	register short i, s;
	register struct gdsel *gp;

	/* print details of gdstbp */

	s = FALSE;

	for (i = 0; i < NGDSEL; i++) {

		if ((struct gdsel *)NULL NE (gp = gdstbp[i])) {

			if (NOT s) {

				printf("gdstbp:");
				s = TRUE;
			}

			while (gp) {

				printf("  %02d:%02d:%d",
					i + 1, gp->note, gp->code);

				gp = gp->next;
			}
		}
	}

	if (s)
		printf("\n");

/* 
*/
	/* print details of gdstbc */

	s = FALSE;

	for (i = 0; i < NGDSEL; i++) {

		if ((struct gdsel *)NULL NE (gp = gdstbc[i])) {

			if (NOT s) {

				printf("gdstbc:");
				s = TRUE;
			}

			while (gp) {

				printf("  %02d:%02d:%d",
					i + 1, gp->note, gp->code);

				gp = gp->next;
			}
		}
	}

	if (s)
		printf("\n");
/* 
*/
	/* print details of gdstbn */

	s = FALSE;

	for (i = 0; i < NGDSEL; i++) {

		if ((struct gdsel *)NULL NE (gp = gdstbn[i])) {

			if (NOT s) {

				printf("gdstbn:");
				s = TRUE;
			}

			while (gp) {

				printf("  %02d:%02d:%d",
					i + 1, gp->note, gp->code);

				gp = gp->next;
			}
		}
	}

	if (s)
		printf("\n");

}

/* 
*/

/*
   =============================================================================
	SCvce() -- dump voice buffer instrument definition
   =============================================================================
*/

SCvce(n)
{
	register short i, j, pif, pt1;
	register struct instdef *ip;
	register struct idfnhdr *fp;
	register struct instpnt *pp;

	ip = &vbufs[n];

	/* dump instrument header */

	printf("VOICE %2d:  %-16.16s  %-16.16s %-16.16s %-16.16s\n",
		(1 + n), ip->idhname, ip->idhcom1, ip->idhcom2, ip->idhcom3);

	printf("  flag=%04.4X  Cfg=%d  #plft=%d  WsA=%d  WsB=%d\n",
		ip->idhflag, (0x00FF & ip->idhcfg), (0x00FF & ip->idhplft),
		(1 + (0x00FF & ip->idhwsa)), (1 + (0x00FF & ip->idhwsb)));

	printf("  Osc 1:%s %c %04.4X  2:%s %c %04.4X  3:%s %c %04.4X  4:%s %c %04.4X\n",
		osclbl[ip->idhos1c & OC_MOD],
		((ip->idhos1c & OC_SYN) ? 'S' : ' '),
		ip->idhos1v,
		osclbl[ip->idhos2c & OC_MOD],
		((ip->idhos2c & OC_SYN) ? 'S' : ' '),
		ip->idhos2v,
		osclbl[ip->idhos3c & OC_MOD],
		((ip->idhos3c & OC_SYN) ? 'S' : ' '),
		ip->idhos3v,
		osclbl[ip->idhos4c & OC_MOD],
		((ip->idhos4c & OC_SYN) ? 'S' : ' '),
		ip->idhos4v);

	/* dump function headers */

	printf("\nFunction headers\n");

	printf("  Fn Pch  Mult Sr Pif Pt1 Cpt Md Pr Trg \n");
	printf("  -- ---- ---- -- --- --- --- -- -- ----\n");

	for (i = 0; i < NFINST; i++) {

		fp = &ip->idhfnc[i];

		printf("  %2d %04.4X %04.4X %02X %3d %3d %3d %02x %02x %04.4x  %s\n",
			i, fp->idfpch, fp->idfmlt, (0x00FF & fp->idfsrc),
			(0x00FF & fp->idfpif), (0x00FF & fp->idfpt1),
			(0x00FF & fp->idfcpt), (0x00FF & fp->idftmd),
			(0x00FF & fp->idfprm), fp->idftrg, idbxlbl[i]);

	}

	/* dump occupied points for each function */

	printf("\nOccupied points\n");
	printf("  Fn Fpt Ipt Time Val  Mult Src  Act  P1 P2 P3 Pd\n");
	printf("  -- --- --- ---- ---- ---- ---- ---- -- -- -- --\n");

	for (i = 0; i < NFINST; i++) {
	
		fp  = &ip->idhfnc[i];
		pif = 0x00FF & fp->idfpif;
		pt1 = 0x00FF & fp->idfpt1;

		for (j = 0; j < pif; j++) {

			pp  = &ip->idhpnt[pt1 + j];

			printf("  %2d %3d %3d %04.4X %04.4X %04.4X %4s %4s %2X %2X %2X %2X\n",
				i, j, (pt1 + j), pp->iptim, pp->ipval, pp->ipvmlt,
				srcname[0x00FF & pp->ipvsrc],
				actname[0x00FF & pp->ipact],
				(0x00FF & pp->ippar1), (0x00FF & pp->ippar2),
				(0x00FF & pp->ippar3), (0x00FF & pp->ippad));
		}
	}

	printf("\n");
}

/* 
*/

/*
   =============================================================================
	SCvces() -- dump voice buffer instrument definitions
   =============================================================================
*/

SCvces()
{
	register short i;

	for (i = 0; i < 12; i++)
		SCvce(i);

	xtrap15();
}

/*
   =============================================================================
	SCvoice() -- dump voice buffer instrument definition
   =============================================================================
*/

SCvoice()
{
	SCvce(SCnumv);
	xtrap15();
}
