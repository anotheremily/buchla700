/*
   =============================================================================
	ptfield.c -- MIDAS-VII patch display field functions
	Version 35 -- 1989-11-16 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGXP		0

#define	USEVECTR	0	/* to force vector cursor control */

#include "stddefs.h"
#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "fields.h"
#include "curpak.h"
#include "patch.h"
#include "macros.h"

#include "midas.h"
#include "ptdisp.h"

#if	DEBUGXP
extern	short	debugsw;

short	debugxp = 1;
#endif

#if	USEVECTR
short	usevect = TRUE;

extern	short	(*cx_key)(), (*cy_key)();
#endif

extern	short	(*xy_up)(), (*xy_dn)();
extern	short	(*premove)(), (*pstmove)();
extern	short	(*curtype)();

extern	unsigned short	fromfpu();

extern	short	select(), nokey(), stdctp5();
extern	short	cxkstd(), cykstd(), stdmkey(), stddkey();
extern	short	stopcm(), smxupd(), smyupd();
extern	short	aftercm();
extern	short	ptdkey();

extern	short	astat;
extern	short	chtime;
extern	short	chwait;
extern	short	cmfirst;
extern	short	cmtype;
extern	short	curhold;
extern	short	cvtime;
extern	short	cvwait;
extern	short	ncvwait;
extern	short	ptbflag;
extern	short	ptecpos;
extern	short	ptedef;
extern	short	ptedfok;
extern	short	pteset;
extern	short	ptestm;
extern	short	ptestok;
extern	short	ss_ptsw;
extern	short	ss_sqsw;
extern	short	stccol;
extern	short	stcrow;
extern	short	syrate;
extern	short	tvcwval;
extern	short	vcwval;

extern	short	crate1[];

extern	char	ptdebuf[];

extern	struct	selbox	*csbp;
extern	struct	selbox	ptboxes[];

extern	struct	patch	ptebuf;

short	ptxkey();		/* forward reference */
short	ptmkey();		/* forward reference */
short	ptekey();		/* forward reference */

/* 
*/

char	*trdata[] = {	"Trans", "Stop ", "Start"	 };
char	*ssdata[] = {	"Stop ", "Start" };

char	*stdata[] = {	"Srce" , "Mult", "Time", "Valu", "Func"	};

char	*smdata[] = {

	"     ",	/* SM_NONE	0	No source - always 0 */
	"Randm",	/* SM_RAND	1	Random source */
	"GPC/V",	/* SM_CTL1	2	Control Voltage 1 / MIDI GPC 1 */
	"??3??",	/* -------	3	-unused- */
	"??4??",	/* -------	4	-unused- */
	"Pitch",	/* SM_PTCH	5	Key Pitch */
	"KeyPr",	/* SM_KPRS	6	Key Pressure */
	"KeyVL", 	/* SM_KVEL	7	Key Velocity */
	"Pedal",	/* SM_PED1	8	Pedal 1 */
	"??9??",	/* -------	9	-unused- */
	"Freq ",	/* SM_FREQ	10	Key Frequency */
	"PchWh",	/* SM_HTPW	11	Tablet X / MIDI Pitch Wheel */
	"ModWh",	/* SM_VTMW	12	Tablet Y / MIDI Mod Wheel */
	"Breth", 	/* SM_LPBR	13	LongPot / MIDI Breath Controller */
};

struct curpak pt_flds = {

	stdctp5,		/* curtype */
	nokey,			/* premove */
	aftercm,		/* pstmove */
	cxkstd,			/* cx_key */
	cykstd,			/* cy_key */
	smxupd,			/* cx_upd */
	smyupd,			/* cy_upd */
	stopcm,			/* xy_up */
	nokey,			/* xy_dn */
	ptxkey,			/* x_key */
	ptekey,			/* e_key */
	ptmkey,			/* m_key */
	ptdkey,			/* d_key */
	nokey,			/* not_fld */
	(struct fet *)NULL,	/* curfet */
	ptboxes,		/* csbp */
	crate1,			/* cratex */
	crate1,			/* cratey */
	CT_SMTH,		/* cmtype */
	CTOX(2),		/* cxval */
	RTOY(DATAROW)		/* cyval */
};

/* 
*/

/*
   =============================================================================
	ptmkey() -- M key processing for the patch display
   =============================================================================
*/

ptmkey()
{
	register short nc;

	if (astat) {

		if (stccol EQ 48) {

			if (ss_sqsw EQ 0) {

				(*xy_dn)();		/* handle KEY_DOWN functions */

				(*premove)();		/* handle PRE-MOVE functions */

				nc = (*curtype)();	/* get new CURSOR TYPE wanted */

				cvtime  = syrate;
				ncvwait = curhold;

				ss_ptsw = -1;
				cmtype  = nc;
				cvwait  = 1;
			}

		} else 
			stdmkey();

	} else {

		if (stccol EQ 48) {

			ss_ptsw = 0;
			cvwait  = 1;
			ncvwait = cvtime;
			cmfirst = TRUE;

			if (ss_sqsw EQ 0)
				(*xy_up)();

			(*pstmove)();		/* handle POST-MOVE functions */

		} else
			stdmkey();
	}
}

/* 
*/

/*
   =============================================================================
	ptekey() -- E key processing for the patch display
   =============================================================================
*/

ptekey()
{
	register short nc;

	if (astat) {

		if (stccol EQ 48) {

			if (ss_sqsw EQ 0) {

				(*xy_dn)();		/* handle KEY_DOWN functions */

				(*premove)();		/* handle PRE-MOVE functions */

				nc = (*curtype)();	/* get new CURSOR TYPE wanted */

				cvtime  = syrate;
				ncvwait = curhold;

				ss_ptsw = 1;
				cmtype  = nc;
				cvwait  = 1;
			}

		} else
			select();

	} else {

		if (stccol EQ 48) {

			ss_ptsw = 0;
			cvwait  = 1;
			ncvwait = cvtime;
			cmfirst = TRUE;

			if (ss_sqsw EQ 0)
				(*xy_up)();

			(*pstmove)();		/* handle POST-MOVE functions */

		} else
			select();
	}
}

/* 
*/

/*
   =============================================================================
	ptxkey() -- X key processing for the patch display
   =============================================================================
*/

ptxkey()
{
	register short i, cp, pp, sp, stm;

	if (NOT astat)				/* only on key closures ... */
		return;

	if (inrange(stccol, 2, 13)) {			/* definer -- clear definer */

		if (ptecpos AND NOT ptbflag) {		/* refresh data entry stuff */

			memcpyw(&ptebuf.defnum, &patches[ptecpos].defnum, 6);
			pteset = TRUE;
			pte2buf();
		}

		ptedef 	= NULL_DEF;
		ptedfok = TRUE;

		memset(&ptdebuf[2], ' ', 12);

		for (i = 0; i < 48; i++)
			if (ptdebuf[i] EQ '\0')
				ptdebuf[i] = ' ';

		ptdebuf[0] = '\260';
		ptdebuf[48] = '\0';

		UpdVid(7, 0, ptdebuf, PTEATR);
		movectc(stcrow, stccol);

/* 
*/
	} else if (inrange(stccol, 15, 26)) {		/* stimulus -- clear stimulus */

		if (ptecpos AND NOT ptbflag) {		/* refresh data entry stuff */

			memcpyw(&ptebuf.defnum, &patches[ptecpos].defnum, 6);
			pteset = TRUE;
			pte2buf();
		}

		ptestm 	= NULL_DEF;
		ptestok = FALSE;

		memset(&ptdebuf[15], ' ', 12);

		for (i = 0; i < 48; i++)
			if (ptdebuf[i] EQ '\0')
				ptdebuf[i] = ' ';

		ptdebuf[0] = '\260';
		ptdebuf[48] = '\0';

		UpdVid(7, 0, ptdebuf, PTEATR);
		movectc(stcrow, stccol);

	} else if (inrange(stccol, 28, 40)) {		/* destination -- clear line */

		voidpb();		/* void the patch buffer */

		ptdebuf[0] = '\260';
		ptdebuf[48] = '\0';

		UpdVid(7, 0, ptdebuf, PTEATR);
		movectc(stcrow, stccol);
/* 
*/
	} else if (inrange(stccol, 42, 46)) {		/* datum -- delete the patch */

		if (ptecpos) {

			cp  = ptecpos;			/* setup indices */
			pp  = patches[cp].prevstm;
			sp  = patches[cp].nextstm;

			stm = patches[cp].stmnum;	/* get stimulus used */
#if	DEBUGXP
	if (debugsw AND debugxp)
		printf("ptxkey():  DELETING PATCH  pp = %d  cp = %d  sp = %d\n",
			pp, cp, sp);
#endif

			if (pp)				/* adjust predecessor */
				patches[pp].nextstm = sp;
			else				/* adjust stmptr */
				stmptr[TRG_MASK & stm] = sp;

			if (sp)				/* adjust successor */
				patches[sp].prevstm = pp;

			if (sp = findnxt(cp))		/* adjust ptecpos */
				ptecpos = sp;
			else if (pp = findprv(cp))
				ptecpos = pp;
			else
				ptecpos = 0;

			pt_del(cp);			/* delete the patch entry */
/* 
*/
			for (i = 0; i < NDEFSTMS; ++i) {	/* adjust defptr list */

				if (cp = defptr[i]) {		/* in use ? */

					pp = 0;		/* initial predecessor */

					while (cp) {		/* check the chain */

					sp = defents[cp].nextdef;	/* successor */

						if (defents[cp].stm EQ stm) {

#if	DEBUGXP
	if (debugsw AND debugxp) {

		printf("ptxkey():  DELETING DEF  trg=$%04.4X  stm=$%04.4X\n",
			i, stm);

		printf("ptxkey():  DELETING DEF  pp=%d  cp=%d  sp=%d\n",
			pp, cp, sp);
	}
#endif

							if (pp)		/* adjust predecessor */
								defents[pp].nextdef = sp;
							else		/* adjust defptr */
								defptr[i] = sp;

							dt_del(cp);	/* delete def entry */

						} else {

							pp = cp;	/* update predecessor index */
						}

						cp = sp;		/* next defent */
					}
				}
			}

			dptw();		/* update the display */
		}
	}
}

/* 
*/

/*
   =============================================================================
	dspdfst() -- convert a patch DEF or STM value for display
   =============================================================================
*/

dspdfst(buf, val)
char *buf;
register unsigned short val;
{
	register unsigned short chan, port, stim;

	if (val EQ NULL_DEF) {		/* handle possible blank definer */

		strcpy(buf, "            ");
		return;
	}

	port = (val >> 11) & 0x0003;
	chan = (val >>  7) & 0x000F;
	stim = val & 0x007F;

	switch (port) {		/* use port to decode possible types */

	case 0:		/* MIDI-1 - Keys */
	case 1:		/* MIDI-2 - Keys */

		sprintf(buf, "%s %03u %u %02u",
			(val & 0x8000) ? "Rel" : "Key",
			1 + stim, 1 + port, 1 + chan);

		break;
/* 
*/

	case 2:		/* Local Keys */

		switch (chan) {		/* use channel to further decode */

		case 0:		/* Keys - Local Keyboard */

			sprintf(buf, "%s %03u L   ",
				(val & 0x8000) ? "Rel" : "Key",
				1 + stim);

			break;

		case 2:		/* Triggers */

			sprintf(buf, "Trig %02u     ", 1 + stim);
			break;

		case 3:		/* Pulses */

			sprintf(buf, "Pulse %u     ", 1 + stim);
			break;

		default:	/* spurious type -- shouldn't happen */

			sprintf(buf, "%s?? %03u L %02u",
				(val & 0x8000) ? "r" : "c",
				1 + stim, 1 + chan);
			break;

		}

		break;

	default:	/* spurious port -- should never happen */

		sprintf(buf, "??? %03u %u %02u", 1 + stim, 1 + port, 1 + chan);
		break;
	}
}

/* 
*/

/*
   =============================================================================
	dfpdat() -- convert FPU datum for display
   =============================================================================
*/

char *
dfpdat(pp)
register struct patch *pp;
{
	static char buf[32];
	unsigned short sgn, vp1, vp2;
	short val;

	val = pp->padat2;

	switch (pp->padat1) {

	case PSA_SRC:		/* source */

		strcpy(buf, smdata[val]);
		break;

	case PSA_MLT:		/* multiplier */

		fr2dec(val, ebuf);

		buf[0] = ebuf[3];
		buf[1] = ebuf[0];
		buf[2] = '.';
		buf[3] = ebuf[1];
		buf[4] = ebuf[2];
		buf[5] = '\0';
		break;
/* 
*/
	case PSA_VAL:		/* value */

		val >>= 5;

		if (val < 0) {

			val = -val;
			sgn = '-';

		} else {

			sgn = '+';
		}

		vp1 = val / 100;
		vp2 = val - (vp1 * 100);

		if (vp1 > 9) {

			vp1 = 0;

			if (sgn EQ '+')
				sgn = '\240';
			else
				sgn = '\241';
		}

		sprintf(buf, "%c%u.%02u", sgn, vp1, vp2);
		break;
/* 
*/
	case PSA_TIM:		/* time */

		vp1 = fromfpu(val);

		sprintf(buf, "%5u", vp1);
		break;

	case PSA_FNC:		/* function / control */

		if ((PE_SPEC & pp->paspec) EQ PA_AUX)
			sprintf(buf, "%s", (val & 0x0001) ? "On   " : "Off  ");
		else
			sprintf(buf, "%s", (val & 0x0001) ? "Start" : "Stop ");

		break;

	default:

		sprintf(buf, "$%04.4X", val);
		break;
	}

	return(buf);
}

/* 
*/

/*
   =============================================================================
	pafpu1() -- convert patch for display -- aaaaa ssss tn  ddddd

		Level, Filtr, Fil Q, Loctn, Dynmc
   =============================================================================
*/

pafpu1(bf, ad, gt, gn, pp)
char *bf;
char *ad;
unsigned short gt, gn;
register struct patch *pp;
{
	sprintf(bf, "%s %s %c%c %s", ad, stdata[pp->padat1], gt, gn, dfpdat(pp));
}

/*
   =============================================================================
	pafpu2() -- convert patch for display -- aaa n ssss tn  ddddd

		Index, Freq
   =============================================================================
*/

pafpu2(bf, ad, os, gt, gn, pp)
char *bf;
char *ad;
unsigned short os, gt, gn;
register struct patch *pp;
{
	sprintf(bf, "%s %d %s %c%c %s", ad, os, stdata[pp->padat1], gt, gn, dfpdat(pp));
}

/*
   =============================================================================
	pafpu3() -- convert patch for display -- aaaaa ssss     ddddd

		Aux, Rate, Inten, Depth, Vlt
   =============================================================================
*/

pafpu3(bf, ad, pp)
char *bf;
char *ad;
register struct patch *pp;
{
	sprintf(bf, "%s %s    %s", ad, stdata[pp->padat1], dfpdat(pp));
}

/* 
*/

/*
   =============================================================================
	dspdest() -- convert patch destination and datum for display
   =============================================================================
*/

dspdest(buf, pp)
char *buf;
register struct patch *pp;
{
	unsigned short chan, osc, port, stim, vg, vgt, vgn, vp1, vp2, sgn;
	short val;
	char buf2[16], *rtype;

	port = (pp->pasuba >> 11) & 0x0003;		/* for trg types */
	chan = (pp->pasuba >>  7) & 0x000F;
	stim =  pp->pasuba        & 0x007F;

	vg   = (pp->pasuba >>  8) & 0x00FF;		/* for vg types */
	osc  = (pp->pasuba        & 0x00FF) + 1;

	if (vg > 11) {

		vgt = 'G';
		vgn = vg - 11;

	} else {

		vgt = 'V';
		vgn = vg + 1;
	}

	vgn = vgn + ((vgn > 9) ? '\242' : '0');

/* 
*/
	switch (PE_SPEC & pp->paspec) {

	case PA_KEY:		/* key action */

		sprintf(buf, "K %03u %u %02u    %s",
			1 + stim, 1 + port, 1 + chan,
			trdata[pp->padat2 & 0x0003]);

		break;

	case PA_TRG:

		sprintf(buf, "Trigger %02u    %s",
			1 + stim,
			ssdata[pp->padat2 & 0x0001]);

		break;

	case PA_PLS:

		sprintf(buf, "Pulse %u       %s",
			1 + stim,
			trdata[pp->padat2 & 0x0003]);

		break;
/* 
*/
	case PA_LED:

		if (pp->pasuba & 0x0001) {

			sprintf(buf, "LED %c         %c%c%c%c",
				'A' + pp->pasuba,
				'0' + ((pp->padat1 >> 14) & 0x0003),
				'0' + ((pp->padat1 >> 12) & 0x0003),
				'0' + ((pp->padat1 >> 10) & 0x0003),
				'0' + ((pp->padat1 >>  8) & 0x0003));

		} else {

			sprintf(buf, "LED %c         %c%c%c",
				'A' + pp->pasuba,
				'0' + ((pp->padat1 >> 14) & 0x0003),
				'0' + ((pp->padat1 >> 12) & 0x0003),
				'0' + ((pp->padat1 >> 10) & 0x0003));
		}

		break;

	case PA_SLIN:

		sprintf(buf, "Seq %02u Lin    %03u",
			1 + pp->pasuba,
			pp->padat1);

		break;

	case PA_SCTL:

		sprintf(buf, "Seq %02u Ctl    %s",
			1 + pp->pasuba,
			ssdata[pp->padat2 & 0x0001]);

		break;

	case PA_TUNE:

		sprintf(buf, "Tune Table    %u",
			pp->padat1);

		break;
/* 
*/
	case PA_RSET:

		rtype = "Set";
		goto doregs;

	case PA_RADD:

		rtype = "Add";

doregs:

		if (pp->padat1 & 0x0001) {

			sprintf(buf, "Reg %02u %s    R%02u",
				1 + pp->pasuba, rtype,
				1 + pp->padat2);

		} else {

			if ((val = pp->padat2) < 0) {

				sgn = '-';
				val = - val;

			} else {

				sgn = '+';
			}

			sprintf(buf, "Reg %02u %s    %c%02u",
				1 + pp->pasuba, rtype,
				sgn, val);
		}

		break;

	case PA_INST:

		sprintf(buf, "Instrument %c%c %02u",
			vgt, vgn, pp->padat1);

		break;
/* 
*/
	case PA_OSC:

		val = pp->padat2 >> 1;

		switch (pp->padat1) {

		case PSO_INT:		/* interval */

			if (val < 0) {

				sgn = '-';
				val = -val;

			} else {

				sgn = '+';
			}

			sprintf(buf, "Osc %u Int  %c%c %c%04u",
				osc, vgt, vgn, sgn, val);

			break;

		case PSO_RAT:		/* ratio */

			int2rat(val);

			sprintf(buf, "Osc %u Rat  %c%c %s",
				osc, vgt, vgn, ebuf);

			break;

		case PSO_FRQ:		/* frequency */

			vp1 = val / 10;
			vp2 = val - (vp1 * 10);

			sprintf(buf, "Osc %u Frq  %c%c %02u.%u",
				osc, vgt, vgn, vp1, vp2);

			break;
/* 
*/
		case PSO_PCH:		/* pitch */

			cnvc2p(ebuf, val);

			sprintf(buf, "Osc %u Pch  %c%c %s",
				osc, vgt, vgn, ebuf);

			break;

		default:

			break;
		}

		break;
/* 
*/
	case PA_WAVA:

		sgn = 'A';
		goto dowaves;

	case PA_WAVB:

		sgn = 'B';

dowaves:

		sprintf(buf, "Waveshape%c %c%c %02u",
			sgn, vgt, vgn, pp->padat1);

		break;

	case PA_CNFG:

		sprintf(buf, "Configure  %c%c %02u",
			vgt, vgn, pp->padat1);

		break;
/* 
*/
	case PA_LEVL:

		pafpu1(buf, "Level", vgt, vgn, pp);
		break;

	case PA_INDX:

		pafpu2(buf, "Ind", osc, vgt, vgn, pp);
		break;

	case PA_FREQ:

		pafpu2(buf, "Frq", osc, vgt, vgn, pp);
		break;

	case PA_FILT:

		pafpu1(buf, "Filtr", vgt, vgn, pp);
		break;

	case PA_FILQ:

		pafpu1(buf, "Fil Q", vgt, vgn, pp);
		break;

	case PA_LOCN:

		pafpu1(buf, "Loctn", vgt, vgn, pp);
		break;

	case PA_DYNM:

		pafpu1(buf, "Dynmc", vgt, vgn, pp);
		break;
/* 
*/
	case PA_AUX:

		pafpu3(buf, "Aux  ", pp);
		break;

	case PA_RATE:

		pafpu3(buf, "Rate ", pp);
		break;

	case PA_INTN:

		pafpu3(buf, "Inten", pp);
		break;

	case PA_DPTH:

		pafpu3(buf, "Depth", pp);
		break;

	case PA_VOUT:

		sprintf(buf2, "Vlt %u", pp->pasuba);
		pafpu3(buf, buf2, pp);
		break;

	default:

		break;
	}
}

/* 
*/

/*
   =============================================================================
	ptfield() -- setup field routines for the patch editor
   =============================================================================
*/

ptfield()
{
	curset(&pt_flds);

#if	USEVECTR
	if (usevect) {

		cx_key = cxkstd;
		cy_key = cykstd;
	}
#endif

}
