/*
   =============================================================================
	swinit.c -- MIDAS-VII -- more variable definitions and initializations
	Version 12 -- 1988-10-12 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	FET_DEFS	1	/* so fields.h gets things right */

#include "stddefs.h"
#include "scwheel.h"
#include "score.h"
#include "fields.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "vsddvars.h"

#include "midas.h"

#include "scdsp.h"
#include "libdsp.h"
#include "wsdsp.h"

extern short swctrl;
extern short swdir;
extern short swfiin;
extern short swflag;

extern short swfifo[NSWFIFO];

/* 
*/

BOOL se_chg;			/* Score free list changed flag */

PFS (*oldsw)[];			/* saved swpt value */
PFS (*swpt)[];			/* switch dispatch table pointer */

     PFS aswtbl[14];		/* assignment switch table */

     short (*curmove) ();	/* cursor move function */
     short (*curtype) ();	/* cursor type function */
     short (*cx_key) ();	/* x rate calculation */
     short (*cx_upd) ();	/* x update */
     short (*cy_key) ();	/* y rate calculation */
     short (*cy_upd) ();	/* y update */
     short (*d_key) ();		/* in-field data key processor */
     short (*e_key) ();		/* E key processor */
     short (*m_key) ();		/* M key processor */
     short (*itxput) ();	/* instrument menu text put function */
     short (*not_fld) ();	/* not-in-field data key processor */
     short (*olddkey) ();	/* saved d_key value */
     short (*oldekey) ();	/* saved e_key value */
     short (*oldmkey) ();	/* saved m_key value */
     short (*oldxkey) ();	/* saved x_key value */
     short (*premove) ();	/* cursor pre-move function */
     short (*pstmove) ();	/* cursor post-move function */
     short (*vt_adv) ();	/* vtyper data cursor advance function */
     short (*vt_bsp) ();	/* vtyper data cursor backspace function */
     short (*vt_cdn) ();	/* vtyper data cursor down function */
     short (*vt_cup) ();	/* vtyper data cursor up function */
     short (*vt_dsp) ();	/* vtyper display function */
     short (*vt_stop) ();	/* vtyper exit function */
     short (*x_key) ();		/* X key processor */
     short (*xy_dn) ();		/* cursor - finger on (down) */
     short (*xy_up) ();		/* cursor - finger off (up) */

     short *cratex;		/* cursor x rate table pointer */
     short *cratey;		/* cursor x rate table pointer */

     short BotEdge;		/* configuration box bottom edge */
     short LftEdge;		/* configuration box left edge */
     short RgtEdge;		/* configuration box right edge */
     short TopEdge;		/* configuration box top edge */

     short BarBcur[14];		/* current bottom-zero bar values */
     short BarCcur[14];		/* current centered-zero bar values */
     short BGeq[14];		/* current EQ bar settings */
     short BGother[14];		/* current other pot bar settings */
     short BGprmtr[14];		/* current parameter pot bar settings */

     short articen[12];		/* voice articulation enable status */
     short grpdyn[12];		/* group dynamics table (0..9) */
     short grploc[12];		/* group location table (0..8) */
     short grpmode[12];		/* group mode - 0 = play, 1 = stdby, 2 = rec */
     short grpsel[12];		/* group select status for instrument selection */
     short grpstat[12];		/* group status - 0 = off, non-0 = on */
     short grptmap[12];		/* group map */
     short gtctab[12];		/* group to color table */
     short ins2grp[12];		/* instrument to group table (00..NINST-1) */
     short instmod[12];		/* intrument data modified */
     short key2grp[88];		/* port 1 key to group assignment table */
     short lastart[16];		/* last articulation values (by parameter) */
     short lastvce[12];		/* last voice assigned in each group */
     short lastvel[12];		/* last velocity sent to group */
     short ldmap[N_SCORES];	/* gather read map */
     short mctlnum[4];		/* MIDI controller number table (-1, 00..99) */
     short mpbend[48];		/* MIDI pitch bend data, by port and channel */
     short msmstv[3];		/* MIDI state machine state variables */
     short offsets[NUMWPCAL];	/* offsets for current waveshape */
     short oldtun[128];		/* previous tuning table for undo */
     short prstab[NTRIGS];	/* pressure */
     short s_inst[12];		/* instrument selections */
     short s_trns[12];		/* current transposition values */
     short swfifo[NSWFIFO];	/* scroll wheel fifo */
     short txfifo[NTKFIFO];	/* trackball X fifo */
     short tyfifo[NTKFIFO];	/* trackball Y fifo */
     short tuntab[128];		/* current tuning table */
     short vce2grp[12];		/* voice to group table (-1, 1..12) */
     short vce2trg[12];		/* voice to trigger map (-1 EQ NULL) */
     short veltab[NTRIGS];	/* velocity */
     short vmtab[NUMHARM];	/* harmonics for current waveshape */
     short wsbuf[NUMWPCAL];	/* final values for current waveshape */

/* 
*/

     char *lmln22;		/* message window line 22 */
     char *lmln23;		/* message window line 23 */
     char *lmln24;		/* message window line 24 */
     char *vtdeptr;		/* virtual typewriter data entry pointer */

     char *funcndx[256][2];	/* function header pointers */
     char *vlbptr[3];		/* variable label pointers */

     char bfs[128];		/* display generation buffer */
     char caname[16];		/* current assignment table name */
     char dspbuf[65];		/* display generation buffer */
     char ebuf[MAXEBUF + 1];	/* edit data entry buffer */
     char errbuf[65];		/* error message build area */
     char idbuf[65];		/* display format conversion work area */
     char keystat[24];		/* front panel key status */
     char ldcmnt[38];		/* comment field */
     char ldfile[9];		/* file name field */
     char ldmsg1[65];		/* messsage build area 1 */
     char ldmsg2[65];		/* messsage build area 2 */
     char ldmsg3[65];		/* messsage build area 3 */
     char loadedc[NLTYPES][37];	/* loaded file comments */
     char loadedf[NLTYPES][8];	/* loaded file names */
     char masens[3];		/* MIDI port active sensing flags */
     char mctlval[NCTRLS];	/* MIDI controller values */
     char mdbyte[3];		/* MIDI data byte buffers */
     char mpsust[48];		/* MIDI sustain status, by port and channel */
     char mrstat[3];		/* MIDI running status */
     char scname[N_SCORES][16];	/* Score names */
     char trgtab[NTRIGS];	/* trigger status */
     char tuncurn[33];		/* current tuning table name */
     char tunname[NTUNS][32];	/* tuning table names */

/* 
*/

     long *pspool;		/* Score storage pool pointer */

     long afi;			/* analog processor FIFO value */
     long curtime;		/* current sqscan time */
     long ctime;		/* time at cursor */
     long dvwork;		/* divide work area */
     long frags;		/* Score storage pool fragment count */
     long iotime;		/* saved lcdtime value */
     long lcdontm;		/* LCD backlight on time */
     long lcdtime;		/* LCD backlight timer */
     long lcsum;		/* library checksum */
     long ndbytes;		/* bytes needed for storage */
     long noteoff;		/* note off time */
     long noteon;		/* note on time */
     long noteper;		/* note period */
     long noteval;		/* note value */
     long nrest;		/* number of rests */
     long schksum;		/* Score checksum */
     long se1_cnt;		/* Score - free list E_SIZE1 entry count */
     long se2_cnt;		/* Score - free list E_SIZE2 entry count */
     long se3_cnt;		/* Score - free list E_SIZE3 entry count */
     long snbreq;		/* Score - disk bytes required */
     long snlreq;		/* Score - score bytes required */
     long sntbreq;		/* Score - total disk bytes required */
     long sntlreq;		/* Score - total score bytes required */
     long spcount;		/* Score - storage pool free entry count */
     long swcount;		/* scroll wheel counter */
     long swrate;		/* scroll wheel rate */
     long swtemp;		/* scroll wheel work area */
     long t_bak;		/* time at p_bak */
     long t_cbgn;		/* time at p_cbgn */
     long t_cend;		/* time at p_cend */
     long t_cur;		/* time at p_cur */
     long t_ctr;		/* time at p_ctr */
     long t_fwd;		/* time at p_fwd */
     long t_ins;		/* time at peg */
     long t_note;		/* note duration between p_nbeg and p_nend */
     long t_sbgn;		/* time at p_sbgn */
     long t_sec1;		/* time at p_sec1 */
     long t_sec2;		/* time at p_sec2 */
     long t_sect;		/* section time */
     long t_send;		/* time at p_send */
     long wshfac;		/* waveshape scale factor */

     long hwave[NUMWPCAL];	/* harmonic waveshape generation buffer */
     long nlpersc[N_SCORES];	/* number of longs per score */
     long scsums[N_SCORES];	/* score checksums */
     long spool[MAX_SE];	/* Score storage pool */

     long vknm[NUMHARM][NUMWPCAL];	/* scaled harmonics for current ws */

     long swrmin = SWRMIN;	/* scroll wheel rate threshold */

/* 
*/

/*
   =============================================================================
	swinit() -- initialize scroll wheel variables
   =============================================================================
*/

swinit ()
{
  swflag = FALSE;		/* stop the scroll wheel */
  swctrl = FALSE;		/* ... */
  swdir = D_FWD;		/* ... */
  swrate = 0;			/* ... */
  swcount = 0;			/* ... */

  memsetw (swfifo, 0, NSWFIFO);	/* clear scroll wheel fifo */
  swfiin = 0;			/* ... */

  tkinit ();			/* initialize the trackball */
}
