/*
   =============================================================================
	im700.c -- variable definitions and initialization for MIDAS-VII
	Version 102 -- 1989-11-16 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGVAL	0		/* initial value for debugsw */

#define	FET_DEFS	1		/* so fields.h gets things right */
#define	M7CAT		1		/* so libdsp.h gets it right */

#include "stddefs.h"
#include "biosdefs.h"
#include "cmeta.h"
#include "curpak.h"
#include "fields.h"
#include "fpu.h"
#include "glcfns.h"
#include "glcdefs.h"
#include "graphdef.h"
#include "hwdefs.h"
#include "macros.h"
#include "memory.h"
#include "objdefs.h"
#include "panel.h"
#include "sclock.h"
#include "scwheel.h"
#include "slice.h"
#include "smdefs.h"
#include "timers.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"

#include "midas.h"

#include "asgdsp.h"
#include "instdsp.h"
#include "libdsp.h"
#include "score.h"
#include "scdsp.h"
#include "scfns.h"
#include "wsdsp.h"

extern	unsigned	tofpu();

extern	short	nokey(), stdmkey(), cxkstd(), cykstd(), cxgen(), cygen();
extern	short	cmvgen(), stdctp2();

extern	short	blakpal[16][3];

extern	PFS	t_prmtr[];

extern	char	*funcndx[256][2];

extern	char	keystat[24];
extern	char	ldcmnt[38];
extern	char	ldfile[9];
extern	char	loadedc[NLTYPES][37];
extern	char	loadedf[NLTYPES][8];
extern	char	masens[3];
extern	char	mctlval[NCTRLS];
extern	char	mdbyte[3];
extern	char	mpsust[48];
extern	char	mrstat[3];
extern	char	trgtab[NTRIGS];

extern	short	fnoff[];

extern	short	articen[12];		/* voice articulation enable status */
extern	short	grpmode[12];		/* group mode - 0 = play, 1 = stdby, 2 = rec */
extern	short	grpsel[12];		/* group select status for instrument selection */
extern	short	grpstat[12];		/* group status - 0 = off, non-0 = on */
extern	short	gtctab[12];		/* group to color table */
extern	short	lastvce[12];		/* last voice assigned in each group */
extern	short	lastvel[12];		/* last velocity sent to group */
extern	short	mpbend[48];		/* MIDI pitch bend data, by port and channel */
extern	short	msmstv[3];		/* MIDI state machine state variables */
extern	short	prstab[NTRIGS];		/* pressure */
extern	short	vce2trg[12];		/* voice to trigger map (-1 EQ NULL) */
extern	short	veltab[NTRIGS];		/* velocity */

/* 
*/

extern	PFS	(*oldsw)[];		/* saved swpt value */
extern	PFS	(*swpt)[];		/* switch dispatch table pointer */

extern	short	(*curmove)();		/* cursor move function */
extern	short	(*curtype)();		/* cursor type function */
extern	short	(*cx_key)();		/* x rate calculation */
extern	short	(*cx_upd)();		/* x update */
extern	short	(*cy_key)();		/* y rate calculation */
extern	short	(*cy_upd)();		/* y update */
extern	short	(*d_key)();		/* in-field data key processor */
extern	short	(*e_key)();		/* E key processor */
extern	short	(*m_key)();		/* M key processor */
extern	short	(*not_fld)();		/* not-in-field data key processor */
extern	short	(*premove)();		/* cursor pre-move function */
extern	short	(*pstmove)();		/* cursor post-move function */
extern	short	(*x_key)();		/* X key processor */
extern	short	(*xy_dn)();		/* cursor - finger on (down) */
extern	short	(*xy_up)();		/* cursor - finger off (up) */

extern	short	*cratex;		/* cursor x rate table pointer */
extern	short	*cratey;		/* cursor x rate table pointer */

short	admctl;			/* assignment display submenu control variable */
short	adnamsw;		/* assignment display virtual typewriter flag */
short	aflag;			/* analog activity flag */
short	aform;			/* action buffer format */
short	amplval;		/* amplitude */
short	ancmsw;			/* analog variable r/p control source */
short	angroup;		/* analog variable group being shown */
short	asgfks;			/* first key selected */
short	asghit;			/* row hit / assignment in progress */
short	asgmod;			/* assignment number or table modified */
short	asig;			/* analog signal number */
short	asmode;			/* panel assignment mode */
short	astat;			/* analog signal status */
short	auxctl;			/* aux control flag */
short	aval;			/* analog signal value */
short	bform;			/* oscillator data buffer format */
short	catin;			/* catalog read in flag */
short	cents;			/* pitch to cents conversion buffer */
short	chtime;			/* horizontal cursor counter */
short	chwait;			/* horizontal cursor wait time */
short	cflag;			/* accidental flag */
short	clkctl;			/* clock control */
short	clkrun;			/* clock status */
short	clksrc;			/* clock source */
short	cmfirst;		/* first cursor motion switch */
short	cmtype;			/* cursor motion type */
short	cnote;			/* note value at cursor */
short	ctrsw;			/* scupd center update switch */
short	curasg;			/* current assignment table */
short	curfunc;		/* current function number */
short	curgrp;			/* current group */
short	curinst;		/* current instrument number */
short	curmop;			/* current MIDI output port */
short	curpnt;			/* current point number (absolute) */
short	curpos;			/* cursor pad position for current axis */
short	cursbox;		/* currently selected box */
short	curscor;		/* Current score number */
short	cursect;		/* current section */
short	curslim;		/* cursor type change point */
short	curtun;			/* current tuning table */
short	curvce;			/* current voice number */
short	curwave;		/* current waveshape library slot */
short	curwdth;		/* current waveshape cursor width */
short	curwfnl;		/* current waveshape final value */
short	curwhrm;		/* current waveshape harmonic number */
short	curwhrv;		/* current waveshape harmonic value */
short	curwoff;		/* current waveshape offset value */
short	curwpnt;		/* current waveshape point number */
short	curwslt;		/* current waveshape instrument slot */
short	cvtime;			/* vertical cursor counter */
short	cvwait;			/* veritcal cursor wait time */
short	cxrate;			/* cursor x rate */
short	cxval;			/* graphic cursor x value */
short	cyrate;			/* cursor y rate */
short	cyval;			/* graphic cursor y value */
short	debugne;		/* debug flag for note entry */
short	defect;			/* defect code */
short	dferror;		/* error code from BIOS or XBIOS */
short	dfsides;		/* number of sides */
short	dftype;			/* disk type code */
short	dsp_ok;			/* display update OK this cycle flag */
short	dubsw;			/* overdub / replace switch */
short	ebflag;			/* edit buffer data flag */
short	editss;			/* edit panel switch state */
short	editsw;			/* edit mode switch */
short	endflg;			/* !end flag */
short	ext_cv1;		/* CV-1 value */
short	ext_cv2;		/* CV-2 value */
short	ext_cv3;		/* CV-3 value */
short	ext_cv4;		/* CV-4 value */
short	ext_mod;		/* Aux Signal Mod value */
short	gomode;			/* go to mode */
short	grptran;		/* group translation value */
short	gtmsel;			/* group transpose/map select */
short	hitbox;			/* box we just hit */
short	hitcx;			/* x of cursor when we hit the box */
short	hitcy;			/* y of cursor when we hit the box */
short	idcfsw;			/* copy / fetch menu switch */
short	idimsw;			/* instrument display instrument menu switch */
short	idintmp;		/* temporary for instrument number */
short	idnamsw;		/* typewriter switch */
short	idsrcsw;		/* source menu switch */
short	idtdat;			/* current instrument vtyper data entry string */
short	imflag;			/* instrument modified flag */
short	initcfg;		/* initial configuration */
short	insmode;		/* score insert mode */
short	ismode;			/* instrument select mode */
short	lampio;			/* LCD timeout disable switch state at I/O time */
short	lampsw;			/* LCD timeout disable switch */
short	lasgsw;			/* assignments store switch */
short	lastam;			/* last assignment menu base */
short	ldelsw;			/* delete switch */
short	lderrsw;		/* error message displayed switch */
short	ldidsiz;		/* getcat() did showsiz() switch */
short	ldkind;			/* fetch file type */
short	ldpass;			/* librarian load state variable */
short	ldrow;			/* fetch select row */
short	ldslot;			/* fetch select slot */
short	legato;			/* execkey() "legato" mode switch */
short	lksel;			/* librarian key slot selector */
short	lmwtype;		/* librarian message window type */
short	loadrow;		/* librarian row selected for load letter */
short	loadsw;			/* panel "Load" mode state */
short	lorchl;			/* load hi (TRUE) / lo (FALSE) orchestra */
short	lorchsw;		/* hi orchestra (21 - 40) store switch */
short	lorclsw;		/* lo orchestra (01 - 20) store switch */
short	lpatsw;			/* patch table store switch */
short	lrasw;			/* append (TRUE) / replace (FALSE) score */
short	lscrsw;			/* score store switch */
short	lselsw;			/* fetch select switch */
short	lseqsw;			/* sequence table store switch */
short	lstbgnc;		/* last note begin entry table index */
short	lstendc;		/* last note end entry table index */
short	lstflag;		/* last note list end switch */
short	lstrsw;			/* store state switch */
short	lstwoff;		/* last waveshape offset value */
short	lstwpnt;		/* last waveshape point number */
short	ltagged;		/* load tag switch */
short	ltunsw;			/* tunings store switch */
short	lwavsw;			/* waveshapes store switch */
short	mascntr;		/* MIDI active sensing timeout counter */
short	mdb1;			/* current MIDI data byte 1 */
short	mdb2;			/* current MIDI data byte 2 */
short	michan;			/* current MIDI channel */
short	midiclk;		/* MIDI clock switch */
short	midigo;			/* MIDI run switch */
short	mistat;			/* current MIDI status */
short	nchwait;		/* next chwait value */
short	ncvwait;		/* next cvwait value */
short	ndisp;			/* current display number */
short	newflag;		/* new data entered while t_cur EQ t_ctr */
short	nkdown;			/* number of keys down */
short	notenum;		/* note number */
short	noteop;			/* pending note operation code */
short	notepit;		/* note pitch */
short	notesel;		/* note selection state */
short	npts;			/* number of points in function */
short	nxtflag;		/* next score flag */
short	oldltag;		/* previous load tag for tagslot */
short	oldpk;			/* previous pkctrl state */
short	oldsl;			/* previous sliders state */
short	pchsw;			/* punch-in enable switch */
short	pecase;			/* point edit case variable */
short	pkctrl;			/* local performance key state */
short	pntsv;			/* point selection state variable */
short	prgchan;		/* MIDI program change channel  (port 1) */
short	ps_dpth;		/* phase shifter -- depth */
short	ps_intn;		/* phase shifter -- intensity */
short	ps_rate;		/* phase shifter -- rate */
short	pulsclk;		/* pulse clock state */
short	recsw;			/* record / play switch */
short	runit;			/* run switch for main scan loop */
short	sbase;			/* score VSDD RAM scroll offset */
short	scmctl;			/* score submenu control flag */
short	scrlpot;		/* scroll pot state */
short	sd;			/* score display direction */
short	se;			/* score execution direction */
short	sdmcol;			/* score menu - saved stccol */
short	sdmctl;			/* score menu - control variable */
short	sdmrow;			/* score menu - saved stcrow */
short	secop;			/* pending score section operation */
short	secopok;		/* section operation OK flag */
short	sgcsw;			/* graphic cursor display switch */
short	sgoflag;		/* section number relative column 0..3 */
short	sharp;			/* sharp flag */
short	sliders;		/* slider and switch state */
short	soffset;		/* score scroll offset */
short	ss_ptsw;		/* smooth scroll patch scroll switch */
short	ss_sqsw;		/* smooth scroll sequence scroll switch */
short	stccol;			/* score cursor col */
short	stcrow;			/* score cursor row */
short	stepclk;		/* note entry - step clock state */
short	stepenb;		/* note entry - step enable flag */
short	stepint;		/* note entry - note interval */
short	stepwgt;		/* note entry - note weight */
short	subj;			/* edited point number (relative) */
short	submenu;		/* submenu active switch */
short	swctrl;			/* scroll wheel 'srolling' flag */
short	swdelta;		/* scroll wheel change while touched */
short	swdir;			/* scroll wheel direction */
short	swfiin;			/* scroll wheel fifo input pointer */
short	swflag;			/* scroll wheel touched flag */
short	swlast;			/* scroll wheel last value */
short	swstop;			/* scroll wheel stop flag */
short	swndx;			/* scroll wheel look back index */
short	tagslot;		/* tagged load slot */
short	tdnamsw;		/* tuning editor displaying typewriter */
short	temax;			/* time upper limit */
short	temin;			/* time lower limit */
short	testing;		/* test level */
short	tglclk;			/* clock on/off toggle flag */
short	tglpch;			/* punch in/out toggle flag */
short	thescore;		/* score selected from sqscan */
short	timemlt;		/* time scaling */
short	tkctrl;			/* trackball active flag */
short	tmpomlt;		/* tempo multiplier */
short	tmpoval;		/* tempo value */
short	trkball;		/* trackball selected switch */
short	ttcmdsv;		/* tuning editor edit state variable */
short	ttsel1;			/* tuning editor key select variable 1 */
short	ttsel2;			/* tuning editor key select variable 2 */
short	ttsel3;			/* tuning editor key select variable 3 */
short	tuneval;		/* fine tuning */
short	tunmod;			/* tuning table modified */
short	tunval;			/* tuning editor increment / transpose  value */
short	txfiin;			/* trackball x fifo input pointer */
short	txflag;			/* trackball x axis active flag */
short	txlast;			/* trackball x axis last value */
short	txstop;			/* trackball x axis filter counter */
short	tyfiin;			/* trackball y fifo input pointer */
short	tyflag;			/* trackball y axis active flag */
short	tylast;			/* trackball y axis last value */
short	tystop;			/* trackball y axis filter counter */
short	velflag;		/* velocity display flag */
short	verbose;		/* verbose output switch */
short	vlbtype;		/* type of message window display */
short	vrbw08;			/* score display video reset detail word */
short	vrbw09;			/* score display video reset detail word */
short	vrbw10;			/* score display video reset detail word */
short	vrbw11;			/* score display video reset detail word */
short	vrbw12;			/* score display video reset detail word */
short	vrbw13;			/* score display video reset detail word */
short	vrbw14;			/* score display video reset detail word */
short	vrbw15;			/* score display video reset detail word */
short	vrcw;			/* score display video reset control word */
short	vtccol;			/* virtual typewriter cursor column */
short	vtcrow;			/* virtual typewriter cursor row */
short	vtdechr;		/* virtual typewriter data entry character */
short	vtdecol;		/* virtual typewriter data entry column */
short	vtpcol;			/* virtual typewriter column */
short	vtprow;			/* virtual typewriter row */
short	vtwcol;			/* virtual typewriter window left column */
short	vtwrow;			/* virtual typewriter window top row */
short	vtxval;			/* virtual typewriter cursor x value */
short	vtyval;			/* virtual typewriter cursor y value */
short	wcflag;			/* ws/cf menu select flag (cf=0, ws=1) */
short	wcmcol;			/* ws/cf menu label column */
short	wcmrow;			/* ws/cf menu label row */
short	wcpage;			/* ws/cf menu page */
short	wdupdfl;		/* waveshape display needs updated flag */
short	wmcsel;			/* ws menu ws a/b select */
short	wmctag;			/* ws/cf display update flag */
short	wplast;			/* last point for interpolate operation */
short	wpntsv;			/* waveshape point selection state variable */
short	wshmax;			/* waveshape maximum value */
short	wvlast;			/* last value for interpolate operation */
short	xkcount;		/* cursor x key on count */
short	xkstat;			/* cursor x key status */
short	xycntr;			/* xy center for cursor pad */
short	ykcount;		/* cursor y key on count */
short	ykstat;			/* cursor y key status */

short	anrs[8][16];			/* analog variable resolution */
short	grp2prt[12][2];			/* group to port and channel table */
short	sctctab[10][64];		/* score background color table */
short	sigtab[128][2];			/* signals:  [0] = value, [1] = switch */
short	tunlib[NTUNS][128];		/* tuning table library */
short	varmode[8][16];			/* analog variable record mode */
short	wsnmod[12][2];			/* waveshape number / data modified */

struct	EXFILE	mphead;		/* MIDAS-VII program header */
struct	fet	*cfetp;		/* current fet entry pointer */
struct	fet	*curfet;	/* current fet table pointer */
struct	fet	*infetp;	/* in-field fet pointer */
struct	gdsel	*gdfsep;	/* gdsel freechain pointer */
struct	instpnt	*pntptr;	/* pointer to edited point */
struct	mlibhdr	ldhead;		/* library header */
struct	nevent	*nefree;	/* pointer to free note entries */
struct	nevent	*nelist;	/* pointer to note entry list */
struct	octent	*adoct;		/* assignment display object control pointer */
struct	octent	*ldoct;		/* librarian object control table pointer */
struct	octent	*mdoct;		/* menu display object control table pointer */
struct	octent	*tdoct;		/* tuning display object control table pointer */
struct	octent	*wdoct;		/* object control table pointer */
struct	octent	*idoct;		/* instrument display octent pointer */
struct	pflent	*pflist;	/* pendant function list header */
struct	pflent	*pfqhdr;	/* pendant function list entry freechain header */
struct	s_entry	*libsp;		/* librarian score pointer */
struct	s_entry	*peg;		/* insert mode 'peg' to hang the tail on */
struct	s_entry	*size1;		/* score E_SIZE1 entry free list pointer */
struct	s_entry	*size2;		/* score E_SIZE2 entry free list pointer */
struct	s_entry	*size3;		/* score E_SIZE3 entry free list pointer */
struct	selbox	*csbp;		/* current select box table pointer */
struct	selbox	*curboxp;	/* current select box pointer */
struct	s_entry *p_bak;		/* pointer to entry at left edge of display */
struct	s_entry	*p_cbgn;	/* pointer to start of copy */
struct	s_entry	*p_cend;	/* pointer to end of copy */
struct	s_entry *p_cur;		/* pointer to entry at current execution time */
struct	s_entry *p_ctr;		/* pointer to entry at center of display */
struct	s_entry *p_fwd;		/* pointer to entry at right edge of display */
struct	n_entry	*p_nbeg;	/* pointer to note begin */
struct	n_entry	*p_nend;	/* pointer to note end */
struct	s_entry	*p_sbgn;	/* pointer to start of section */
struct	s_entry	*p_sec1;	/* pointer to start of 1st section */
struct	s_entry	*p_sec2;	/* pointer to start of 2nd section */
struct	s_entry	*p_send;	/* pointer to end of section */
struct	s_entry *scp;		/* current score pointer */

struct	asgent	asgtab[NASGS];		/* assignment table library */
struct	fcat	filecat[FCMAX];		/* file catalog */
struct	gdsel	*gdstbc[NGDSEL];	/* center group & event status lists */
struct	gdsel	*gdstbn[NGDSEL];	/* right group & event status lists */
struct	gdsel	*gdstbp[NGDSEL];	/* left group & event status lists */
struct	gdsel	gdfsl[MAXFSL];		/* gdsel pool */
struct	instdef	idefs[NINST];		/* current instrument definitions */
struct	n_entry	*lstbgns[NLSTENTS];	/* last note begin entry table */
struct	n_entry	*lstends[NLSTENTS];	/* last note end entry table */
struct	instdef	vbufs[12];		/* voice instrument buffers */
struct	nevent	nevents[NNEVTS];	/* note event list */
struct	pflent	pfents[NPFENTS];	/* pendant function list entry pool */
struct	scndx	sindex[N_SCORES];	/* score file table of contents */
struct	sment	*vpsms[192];		/* voice parameter general S/M pointer table */
struct	sment	sments[192];		/* source multiplier assignment table */
struct	valent	valents[NGPSRS];	/* source value table */
struct	wstbl	wslib[NUMWAVS];		/* waveshape library */
struct	s_entry *scores[N_SCORES];	/* Score pointer table */

struct	s_time	stimes[N_SCORES][N_SECTS];	/* score section SMPTE times */
struct	s_entry *hplist[N_SCORES][N_TYPES];	/* Score change list pointers */
struct	s_entry *seclist[N_SCORES][N_SECTS];	/* Section pointer table */

unsigned	*asgob;		/* assignment display object pointer */
unsigned	*consl;		/* constant slice pointer */
unsigned	*cursl;		/* current (center) slice pointer */
unsigned	*instob;	/* instrument object pointer */
unsigned	*librob;	/* librarian display object pointer */
unsigned	*menuob;	/* menu display object pointer */
unsigned	*nxtsl;		/* next (right edge) slice pointer */
unsigned	*obj0;		/* object  0 VSDD RAM pointer */
unsigned	*obj1;		/* object  1 VSDD RAM pointer */
unsigned	*obj2;		/* object  2 VSDD RAM pointer */
unsigned	*obj3;		/* object  3 VSDD RAM pointer */
unsigned	*obj4;		/* object  4 VSDD RAM pointer */
unsigned	*obj5;		/* object  5 VSDD RAM pointer */
unsigned	*obj6;		/* object  6 VSDD RAM pointer */
unsigned	*obj7;		/* object  7 VSDD RAM pointer */
unsigned	*obj8;		/* object  8 VSDD RAM pointer */
unsigned	*obj9;		/* object  9 VSDD RAM pointer */
unsigned	*obj10;		/* object 10 VSDD RAM pointer */
unsigned	*obj11;		/* object 11 VSDD RAM pointer */
unsigned	*obj12;		/* object 12 VSDD RAM pointer */
unsigned	*obj13;		/* object 13 VSDD RAM pointer */
unsigned	*obj14;		/* object 14 VSDD RAM pointer */
unsigned	*obj15;		/* object 15 VSDD RAM pointer */
unsigned	*prvsl;		/* previous (left edge) slice pointer */
unsigned	*saddr;		/* score VSDD RAM base pointer */
unsigned	*tunob;		/* tuning table display object pointer */
unsigned	*vtobj;		/* virtual typewriter display object pointer */
unsigned	*waveob;	/* waveshape display object pointer */

unsigned	curintp;	/* current interpolate time (FPU format) */
unsigned	scrl;		/* score VSDD object scroll register image */
unsigned	vtbgval;	/* virtual typewriter data background color */
unsigned	vtfgval;	/* virtual typewriter data foreground color */

unsigned	dfbuf[4096];	/* track formatting buffer */
unsigned	slices[896];	/* score display slices */

/* 
*/

/* initialized variables */

short	debugsw = DEBUGVAL;	/* debug output switch */

short	swback  = SWBACK;	/* scroll wheel look back count */
short	swthr   = SWTHR;	/* scroll wheel movement threshold */
short	swtime  = SWTIME;	/* scroll wheel timer scan time */
short	swwait	= SWWAIT;	/* scroll wheel wait count */

short	tkback  = TKBACK;	/* trackball FIFO look back count */
short	tkrmin  = TKRMIN;	/* trackball rate threshold */
short	tkthr   = TKTHR;	/* trackball movement threshold */
short	tktime  = TKTIME;	/* trackball timer scan time */
short	tkwait	= TKWAIT;	/* trackball wait count */

short	curhold = CURHOLD;	/* cursor hold time */
short	hcwval  = HCWVAL;	/* graphics cursor X wait time */
short	thcwval = THCWVAL;	/* text cursor H wait time */
short	tvcwval = TVCWVAL;	/* text cursor V wait time */
short	vcwval  = VCWVAL;	/* graphics cursor Y wait time */

/* 
*/

short	tmultab[] = {		/* time multiplier table - indexed by voltage */

	0xFFFF, 0xFAFA, 0xF627, 0xF182, 0xED09, 	/*   0 */
	0xE8BA, 0xE492, 0xE08F, 0xDCB0, 0xD8F2, 	/*   5 */
	0xD555, 0xD1D6, 0xCE73, 0xCB2C, 0xC800, 	/*  10 */
	0xC4EC, 0xC1F0, 0xBF0B, 0xBC3C, 0xB981, 	/*  15 */
	0xB6DB, 0xB448, 0xB1C7, 0xAF57, 0xACF9, 	/*  20 */
	0xAAAA, 0xA86B, 0xA63B, 0xA41A, 0xA206, 	/*  25 */
	0xA000, 0x9E06, 0x9C18, 0x9A37, 0x9861, 	/*  30 */
	0x9696, 0x94D6, 0x9320, 0x9174, 0x8FD1, 	/*  35 */
	0x8E38, 0x8CA8, 0x8B21, 0x89A2, 0x882B, 	/*  40 */
	0x86BC, 0x8555, 0x83F5, 0x829C, 0x814A, 	/*  45 */
	0x8000, 0x7EBB, 0x7D7D, 0x7C45, 0x7B13, 	/*  50 */
	0x79E7, 0x78C1, 0x77A0, 0x7684, 0x756E, 	/*  55 */
	0x745D, 0x7350, 0x7249, 0x7146, 0x7047, 	/*  60 */
	0x6F4D, 0x6E58, 0x6D66, 0x6C79, 0x6B90, 	/*  65 */
	0x6AAA, 0x69C8, 0x68EB, 0x6810, 0x6739, 	/*  70 */
	0x6666, 0x6596, 0x64C9, 0x6400, 0x6339, 	/*  75 */
	0x6276, 0x61B5, 0x60F8, 0x603D, 0x5F85, 	/*  80 */
	0x5ED0, 0x5E1E, 0x5D6E, 0x5CC0, 0x5C16, 	/*  85 */
	0x5B6D, 0x5AC7, 0x5A24, 0x5982, 0x58E3, 	/*  90 */
	0x5846, 0x57AB, 0x5713, 0x567C, 0x55E7, 	/*  95 */
	0x5555						/* 100 */
};

/* 
*/

short	crate0[] = {			/* cursor rate table 1 */

	 0,  0,  0,  0,  0,  0,  0,  0,		/* 0..7 */
	 0,  0,  0,  0,  1,  1,  1,  1,		/* 8..15 */
	 1,  1,  1,  1,  1,  1,  1,  1,		/* 16..23 */
	 1,  1,  1,  1,  1,  1,  1,  1,		/* 24..31 */
	 1,  1,  1,  1,  1,  1,  1,  1,		/* 32..39 */
	 1,  1,  1,  1,  1,  1,  1,  1,		/* 40..47 */
	 1,  1,  1,  1,  1,  1,  1,  1,		/* 48..55 */
	 1,  1,  1,  1,  1,  1,  1,  1		/* 56..63 */
};

short	crate1[] = {			/* cursor rate table 1 */

	 0,  0,  0,  0,  0,  0,  0,  0,		/* 0..7 */
	 1,  1,  1,  1,  1,  1,  1,  1,		/* 8..15 */
	 3,  3,  3,  3,  3,  3,  3,  3,		/* 16..23 */
	 5,  5,  5,  5,  5,  5,  5,  5,		/* 24..31 */
	 7,  7,  7,  7,  7,  7,  7,  7,		/* 32..39 */
	 9,  9,  9,  9,  9,  9,  9,  9,		/* 40..47 */
	11, 11, 11, 11, 11, 11, 11, 11,		/* 48..55 */
	13, 13, 13, 13, 13, 13, 13, 13		/* 56..63 */
};

short crate2[] = {			/* cursor rate table 2 */

	  0,  0,  0,  0,  0,  0,  0,  0,	/* 0..7 */
	  1,  1,  1,  1,  1,  1,  1,  1,	/* 8..15 */
	  2,  2,  2,  2,  2,  2,  2,  2,	/* 16..23 */
	  4,  4,  4,  4,  8,  8,  8,  8,	/* 24..31 */
	 16, 16, 16, 16, 32, 32, 32, 32,	/* 32..39 */
	 64, 64, 64, 64,128,128,128,128,	/* 40..47 */
	 96, 96, 96, 96,256,256,256,256,	/* 48..55 */
	256,256,256,256,256,256,256,256		/* 56..63 */
};

/* 
*/

short	loctab[9] = {

	0,		/* 0 - left */

	125 << 5,	/* 1 */
	250 << 5,	/* 2 */
	375 << 5,	/* 3 */

	500 << 5,	/* 4 - center */

	625 << 5,	/* 5 */
	750 << 5,	/* 6 */
	875 << 5,	/* 7 */

	1000 << 5	/* 8 - right */
};

/* 
*/

/*
   =============================================================================
	valof() -- return initial value of a source
   =============================================================================
*/

short
valof(srcv)
short srcv;
{
	switch (srcv) {

	case SM_NONE:		/* -none- */
		return(0);

	case SM_RAND:		/* Random */
		return(xgetran(0));

	case SM_CTL1:		/* Ctl V1 */
		return(SM_SCALE(sigtab[79][0]));

	case SM_PTCH:		/* Pitch */
	case SM_KPRS:		/* Key Prs */
	case SM_KVEL:		/* Key Vel */
		return(0);

	case SM_PED1:		/* Pedal 1 */
		return(SM_SCALE(sigtab[77][0]));

	case SM_HTPW:		/* Hor Tab / Pch Whl */
		return(0);

	case SM_VTMW:		/* Vrt Tab / Mod Whl */
		return(SM_SCALE(sigtab[54][0]));

	case SM_LPBR:		/* LongPot / Breath */
		return(SM_SCALE(sigtab[57][0]));

	default:
		return(0);
	}
}

/* 
*/

/*
   =============================================================================
	inismtb() -- initialize source / multiplier data structures
   =============================================================================
*/

inismtb()
{
	register short i, oldi;

	oldi = setipl(FPU_DI);

/* ++++++++++++++++++++++++++++ FPU interrupts disabled +++++++++++++++++++++ */

	for (i = 0; i < NGPSRS; i++) {

		valents[i].nxt = &valents[i];
		valents[i].prv = &valents[i];
		valents[i].val = valof(i & 0x000F);
	}

	for (i = 0; i < 192; i++) {

		vpsms[i] = &sments[i];

		sments[i].prv = (struct sment *)&valents[0];
		sments[i].nxt = valents[0].nxt;

		valents[0].nxt->prv = &sments[i];
		valents[0].nxt      = &sments[i];

		sments[i].vp = i;
		sments[i].sm = 0;
	}

	setipl(oldi);

/* ++++++++++++++++++++++++++++ Interrupts restored +++++++++++++++++++++++++ */

}

/* 
*/

/*
   =============================================================================
	clrpfl() -- clear pendant function list
   =============================================================================
*/

clrpfl()
{
	register short i;

	memsetw(&pfents[0], 0, (NPFENTS * sizeof (struct pflent))/2);

	for (i = 0; i < NPFENTS-1; i++)
		pfents[i].nextpf = &pfents[i+1];

	pfqhdr = &pfents[0];
	pflist = (struct pflent *)NULL;
}

/* 
*/

/*
   =============================================================================
	im700() -- initialize MIDAS 700
   =============================================================================
*/

im700()
{
	register short i, j, k;
	register char *ioadr;
	register struct instdef *ip;

	debugsw = DEBUGVAL;		/* initialize debug switch */

	GLCinit();			/* initialize the GLC */
	GLCcurs(G_ON);
	GLCtext(2, 1, "MIDAS 7 -- Initialization in progress -- Please stand by");
	GLCcurs(G_OFF);

	vsndpal(blakpal);		/* setup the blackout palette */

	VHinit();			/* clear the video hardware */
	VSinit();			/* ... and the video software */

	memset(keystat, 0, 24);		/* clear front panel key status */
	memset(trgtab, 	0, NTRIGS);	/* clear trigger status table */
	memset(mctlval, 0, NCTRLS);	/* clear MIDI controller values */

	memsetw(grpmode, 0, 12);	/* set all groups to PLAY */
	memsetw(grpstat, GS_ENBL, 12);	/* enable all groups for param. artic. */
	memsetw(grpsel, TRUE, 12);	/* enable all groups for instr. sel. */
	memsetw(sigtab, 0, ((sizeof sigtab) / (sizeof sigtab[0][0])));

	memsetw(veltab, SM_SCALE(64), NTRIGS);	/* clear velocity table */
	memsetw(lastvel, SM_SCALE(64), 12);	/* clear last velocity table */
	memsetw(prstab, 0, NTRIGS);	/* clear pressure table */

	curintp = tofpu(100);		/* set default interpolate time */

	angroup = -1;			/* analog values:  group 1, not shown */
	ancmsw  = FALSE;		/* analog variable mode from grpmode */
	velflag = FALSE;		/* velocity not displayed */

	dubsw   = TRUE;			/* set overdub mode */

	sd = D_FWD;			/* set display direction */
	se = D_FWD;			/* set execution direction */

	swinit();			/* reset the scroll wheel variables */

	inismtb();			/* clear the S/M data structures */
	clrnl();			/* clear note entry lists */
	clrpfl();			/* clear pendant function list */

	lstbgnc = 0;			/* reset last note begin count */
	lstendc = 0;			/* reset last note end count */
	lstflag = TRUE;			/* set end of note entry list flag */

	memsetw(lstbgns, 0, (NLSTENTS << 1));	/* clear note begin list */
	memsetw(lstends, 0, (NLSTENTS << 1));	/* clear note end list */

	/* set funcndx[] entries */

	for (i = 0; i < 12; i++) {

		ip = &vbufs[i];

		for (j = 0; j < NFINST; j++) {

			k = (i << 4) + fnoff[j];

			funcndx[k][0] = &ip->idhfnc[j];
			funcndx[k][1] = ip->idhpnt;
		}
	}

/* 
*/
	for (i = 0; i < 3; i++) {	/* reset the state machines */

		msmstv[i] = 0;		/* state = 0 */
		mrstat[i] = 0;		/* clear running status */
		mdbyte[i] = 0;		/* clear first byte buffer */
		masens[i] = 0;		/* clear active sensing */
	}

	for (i = 0; i < 48; i++) {

		mpbend[i] = 0;		/* clear bend values */
		mpsust[i] = 0;		/* clear sustain values */
	}

	for (i = 0; i < 12; i++) {

		for (j = 0; j < 6; j++) {

			varmode[j][i] = 0;	/* set r/p mode */
			anrs[j][i]    = 4;	/* set analog resolution */
		}

		setdyn(i, 6);		/* set group dynamics */
		setloc(i, 4);		/* set group locations */
		lastvce[i] = 0;		/* clear last voice assigned */
		vce2trg[i] = -1;	/* clear voice to trigger table */
	}

	for (i = 0; i < NLTYPES; i++) {

		memset(loadedc[i], ' ', 37);
		memset(loadedf[i], ' ', 8);
	}

	memset(ldfile, ' ', 8);		/* clear the library file name field */
	ldfile[8] = '\0';

	memset(ldcmnt, ' ', 37);	/* clear the library comment field */
	ldcmnt[37] = '\0';

/* 
*/
	xkcount = 1;			/* reset cursor control variables */
	ykcount = 1;
	xycntr  = XYCENTRE;

	clksrc  = CK_LOCAL;		/* clock source = local */
	clkctl  = CK_LOCAL;		/* clock control = local */
	clkrun  = FALSE;		/* clock stopped */
	midiclk = FALSE;		/* ... */
	pulsclk = FALSE;		/* ... */
	stepclk = FALSE;		/* ... */
	fc_sw   = 0;			/* stop the frame clock */
	fc_val  = 0L;			/* ... and reset it */
	scrlpot = 0;			/* not scrolling */
	tmpomlt = 50;			/* initialize tempo multiplier */
	io_leds = 0x18;			/* ... and turn off the LEDs */
	io_leds = 0x19;			/* ... */
	settmpo(60);			/* set default tempo */

	timemlt = tmultab[50];		/* initialize time scaling */
	io_leds = 0x1A;
	io_leds = 0x1B;

	tuneval = 0;			/* initialize fine tuning */
	io_leds = 0x1C;
	io_leds = 0x1D;

	for (i = 0; i < 12; i++)	/* setup default values for gtctab */
		gtctab[i] = ((i+3) << 12) | ((i+3) << 8) | ((i+3) << 4) | (i+3);

	editsw  = FALSE;		/* setup edit switch state */
	io_leds = 0x1E;
	ioadr   = &io_ser + 2L;
	editss  = *ioadr & 0x0008;

	ac_code = N_SHARP;	/* default accidentals to display as sharps */

	memsetw(articen, TRUE, 12);	/* enable articulation for all voices */
/* 
*/
	curmove = cmvgen;
	premove = nokey;
	pstmove = nokey;
	curtype = stdctp2;

	x_key = nokey;		/* setup default key processors */
	e_key = nokey;
	m_key = stdmkey;
	d_key = nokey;

	not_fld = nokey;

	cx_key  = cxkstd;	/* setup default cursor processors */
	cy_key  = cykstd;
	cx_upd  = cxgen;
	cy_upd  = cygen;
	xy_up   = nokey;
	xy_dn   = nokey;

	curslim = 349;		/* cursor limit for graphics to text switch */

	chtime  = hcwval;	/* initialize default cursor wait times */
	cvtime  = vcwval;
	nchwait = curhold;
	ncvwait = curhold;
	chwait  = 1;
	cvwait  = 1;

	cratex  = crate1;	/* initialize default cursor x rate table */
	cratey  = crate1;	/* initialize default cursor y rate table */

	cmfirst = TRUE;		/* setup for first motion of cursor */
	cmtype  = CT_GRAF;

	cxval   = XCTR;		/* default cursor to center of screen */
	cyval   = YCTR;

	stcrow  = cyval / 14;	/* keep text position in sync */
	stccol  = cxval / 8;

	submenu = FALSE;	/* no submenu up to start with */
/* 
*/
	curinst = 0;		/* current instrument = 0 */
	curfunc = 12;		/* current function = 12 = level */
	curvce  = 0;		/* current voice = 0 */

	asmode  = FALSE;	/* deselect assignment mode */
	gomode  = GO_NULL;	/* deselect go to mode */
	ismode  = IS_NULL;	/* deselect instrument select mode */
	pkctrl  = PK_PFRM;	/* select local performance key state */
	oldpk   = pkctrl;	/* ... */
	sliders = LS_PRMTR;	/* setup the LCD display pots and switches */
	oldsl   = sliders;	/* ... */
	swpt    = t_prmtr;	/* ... */
	oldsw   = swpt;		/* ... */
	setleds();		/* set performance keyboard leds */

	initpt();		/* initialize the patch table */
	initsq();		/* initialize the sequence table */
	initil();		/* initialize the instrument library */
	iniwslb();		/* initialize the waveshape library */
	inital();		/* initialize the assignment library */
	inittl();		/* initialize the tuning table library */
	scinit();		/* initialize the score */

	clearer(1, 0);		/* reset the fpu to its nominal values */
	getasg(curasg = 0);	/* setup default assignments */

	lcdlbls();		/* put up the LCD display */
	lampsw = FALSE;		/* enable LCD timeout */
}
