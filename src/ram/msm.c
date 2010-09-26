/*
   =============================================================================
	msm.c -- MIDAS-VII -- MIDI state machine
	Version 75 -- 1988-11-02 -- D.N. Lynx Crowe

	List with pr -e4 option to expand tabs to 4 spaces instead of 8.
   =============================================================================
*/

#include "biosdefs.h"
#include "stddefs.h"
#include "graphdef.h"
#include "hwdefs.h"
#include "smdefs.h"
#include "sclock.h"
#include "score.h"
#include "vsdd.h"

#include "midas.h"
#include "scfns.h"

extern	short	ancmsw;		/* analog variable r/p control source */
extern	short	angroup;	/* analog variable group being displayed */
extern	short	clkctl;		/* clock control sources */
extern	short	ctrsw;		/* se_disp center update switch */
extern	short	curasg;		/* current assignment table */
extern	short	curvce;		/* current voice being edited */
extern	short	dsp_ok;		/* display update OK this cycle flag */
extern	short	editsw;		/* edit switch state */
extern	short	mascntr;	/* MIDI active sensing timeout counter */
extern	short	mdb1;		/* current MIDI data byte 1 */
extern	short	mdb2;		/* current MIDI data byte 2 */
extern	short	michan;		/* current MIDI channel */
extern	short	midiclk;	/* MIDI clock switch */
extern	short	midigo;		/* MIDI run switch */
extern	short	mistat;		/* current MIDI status */
extern	short	ndisp;		/* current display */
extern	short	recsw;		/* record/play switch */
extern	short	prgchan;	/* MIDI program change channel */

/* 
*/

extern	char	masens[];	/* MIDI port active sensing flags */
extern	char	mctlval[];	/* MIDI controller values */
extern	char	mdbyte[];	/* MIDI data byte buffers */
extern	char	mpsust[];	/* MIDI sustain status, by port and channel */
extern	char	mrstat[];	/* MIDI running status */
extern	char	trgtab[];	/* trigger status */

extern	short	grpmode[];	/* group mode table */
extern	short	grpstat[];	/* group status table */
extern	short	key2grp[];	/* key to group map */
extern	short	vce2grp[];	/* voice to group map */
extern	short	veltab[];	/* velocity */
extern	short	mctlnum[];	/* MIDI controller number map */
extern	short	mpbend[];	/* MIDI pitch bend data, by port and channel */
extern	short	msmstv[];	/* MIDI state machine state variables */
extern	short	prstab[];	/* pressure */
extern	short	src2var[];	/* source to variable number map */
extern	short	tuntab[];	/* current tuning table */
extern	short	vce2trg[];	/* voice to trigger map (-1 EQ NULL) */

extern	short	grp2prt[][2];	/* group to port/channel map */
extern	short	varmode[][16];	/* analog variable record mode */

extern	struct	gdsel	*gdstbc[];	/* group status table - center slice */

/* initialized stuff */

/* mpmap[] -- MIDI port to BIOS device table */

short	mpmap[3]   = {MC1_DEV, MC2_DEV, CON_DEV};

/* msmnext[] -- MIDI state machine -- channel message next state table */

short	msmnext[8] = {2, 2, 2, 2, 1, 1, 2, 0};

/* msmcnxt[] -- MIDI state machine -- system message next state table */

short	msmcnxt[8] = {0, 0, 2, 1, 0, 0, 0, 0};

/* ctl2src -- controller to source table */

short	ctl2src[] = {SM_VTMW, SM_LPBR, SM_CTL1, SM_PED1};

/* 
*/

/*
   =============================================================================
	msm() -- MIDI state machine
   =============================================================================
*/

msm()
{
	register short miport;
	register short midiin;
	register short i;
	register short t1;
	register short t2;
	register struct s_entry *ep;
	short j, nvel;
	long where;

	for (miport = 0; miport < NMPORTS; miport++) {	/* for each port ... */

		while (BIOS(B_RDAV, mpmap[miport])) {	/* anything there ? */

			dsp_ok = FALSE;			/* disallow display update */

			midiin = 0x00FF & BIOS(B_GETC, mpmap[miport]);	/* get input */

			if (masens[miport])		/* update active sensing */
				masens[miport] |= 0x0F;

			switch (msmstv[miport]) {	/* switch on state */
/* 
*/
			case 0:		/* waiting for status */

				if (0x0080 & midiin) {	/* status */

					michan = 0x000F & midiin;
					mistat = 0x000F & (midiin >> 4);

					if (midiin < 0x00F0) {		/* channel */

						msmstv[miport] = msmnext[mistat & 0x07];
						mrstat[miport] = midiin;

					} else if (midiin < 0x00F8) {	/* common */

						msmstv[miport] = msmcnxt[michan & 0x07];
						mrstat[miport] = midiin;
/* 
*/
					} else {			/* real-time */

						switch (0x0007 & midiin) {

						case 0:		/* clock */

							if (midiclk AND midigo)
								if ((fc_val += 2) > 0x00FFFFFFL)
									fc_val = 0x00FFFFFFL;

							break;

						case 2:		/* start */

							if ((clkctl EQ CK_MIDI) AND
							    (NOT midigo)) {

								midigo = TRUE;

								if (fc_val)
									sc_goto(fc_val = 0L);

								clkset(TRUE);
								dsclk();
							}

							break;

						case 3:		/* continue */

							if ((clkctl EQ CK_MIDI) AND
							    (NOT midigo)) {

								midigo = TRUE;
								clkset(TRUE);
								dsclk();
							}

							break;
/* 
*/
						case 4:		/* stop */

							if ((clkctl EQ CK_MIDI) AND
							    midigo) {

								midigo = FALSE;
								clkset(FALSE);
								dsclk();
							}

							break;

						case 6:		/* active sensing */

							masens[miport] |= 0x00FF;
							break;

						case 7:		/* reset */

							for (i = 0; i < NMPORTS; i++) {

								mrstat[i] = 0;
								msmstv[i] = 0;
								masens[i] = 0;
							}

							break;
						}
					}
				}

				continue;
/* 
*/
			case 1:		/* waiting for 1 data byte */

				if (0x80 & midiin) {	/* status */

					michan = 0x0F & midiin;
					mistat = 0x0F & (midiin >> 4);

					if (midiin < 0x00F0) {		/* channel */

						msmstv[miport] = msmnext[mistat & 0x07];
						mrstat[miport] = midiin;

					} else if (midiin < 0x00F8) {	/* common */

						msmstv[miport] = msmcnxt[michan & 0x07];
						mrstat[miport] = midiin;
/* 
*/
					} else {			/* real-time */

						switch (0x0007 & midiin) {

						case 0:		/* clock */

							if (midiclk AND midigo)
								if ((fc_val += 2) > 0x00FFFFFFL)
									fc_val = 0x00FFFFFFL;

							break;

						case 2:		/* start */

							if ((clkctl EQ CK_MIDI) AND
							    (NOT midigo)) {

								midigo = TRUE;

								if (fc_val)
									sc_goto(fc_val = 0L);

								clkset(TRUE);
								dsclk();
							}

							break;

						case 3:		/* continue */

							if ((clkctl EQ CK_MIDI) AND
							    (NOT midigo)) {

								midigo = TRUE;
								clkset(TRUE);
								dsclk();
							}

							break;
/* 
*/
						case 4:		/* stop */

							if ((clkctl EQ CK_MIDI) AND
							    midigo) {

								midigo = FALSE;
								clkset(FALSE);
								dsclk();
							}

							break;

						case 6:		/* active sensing */

							masens[miport] |= 0x00FF;
							break;

						case 7:		/* reset */

							for (i = 0; i < NMPORTS; i++) {

								mrstat[i] = 0;
								msmstv[i] = 0;
								masens[i] = 0;
							}

							break;
						}
					}
/* 
*/
				} else {	/* data */

					michan = 0x000F & mrstat[miport];
					mistat = 0x000F & (mrstat[miport] >> 4);

					if (mistat EQ 0x0C) {

						/* program change */

						if ((miport EQ 0) AND
						    ((michan + 1) EQ prgchan) AND
						    (midiin LT NASGS)) {

							getasg(curasg = midiin);
							mpcupd();
						}
/* 
*/
					} else if (mistat EQ 0x0D) {

						/* channel pressure */

						mdb1 = SM_SCALE(midiin);
						t1 = (miport << 11) | (michan << 7);
						t2 = t1 + 128;

						for (i = t1; i < t2; i++)
							prstab[i] = mdb1;

						for (i = 0; i < 12; i++) {

							if ((grp2prt[i][0] EQ (miport + 1)) AND
							    (grp2prt[i][1] EQ (michan + 1))) {

								if (newsv(i, SM_KPRS, mdb1)) {

									if (recsw AND grpstat[i] AND
									    (2 EQ (ancmsw ? varmode[5][i] : grpmode[i]))) {

										if (E_NULL NE (ep = e_alc(E_SIZE2))) {

											ep->e_time = t_cur;
											ep->e_type = EV_ANVL;
											ep->e_data1 = 0x0050 | i;
											ep->e_dn = (struct s_entry *)((long)mdb1 << 16);
											p_cur = e_ins(ep, ep_adj(p_cur, 0, t_cur))->e_fwd;
											ctrsw = TRUE;
											se_disp(ep, D_FWD, gdstbc, 1);
											ctrsw = FALSE;
										}

									} else if ((angroup - 1) EQ i) {

										dsanval(5);
									}
								}
							}
						}
/* 
*/
					} else if ((mistat EQ 0x0F) AND
						   (michan EQ 3) AND
						   (NOT midigo)) {	/* song select */

						if (midiin < N_SCORES) {

							selscor(midiin);

							if (ndisp EQ 2)
								sdwins();
						}

					} else {	/* error -- state mismatch */

						msmstv[miport] = 0;
						mrstat[miport] = 0;
					}
				}

				continue;
/* 
*/
			case 2:		/* waiting for 1st data byte */

				if  (0x80 & midiin) {	/* status */

					michan = 0x0F & midiin;
					mistat = 0x0F & (midiin >> 4);

					if (midiin < 0x00F0) {		/* channel */

						msmstv[miport] = msmnext[mistat & 0x07];
						mrstat[miport] = midiin;

					} else if (midiin < 0x00F8) {	/* common */

						msmstv[miport] = msmcnxt[michan & 0x07];
						mrstat[miport] = midiin;
/* 
*/
					} else {			/* real-time */

						switch (0x0007 & midiin) {

						case 0:		/* clock */

							if (midiclk AND midigo)
								if ((fc_val += 2) > 0x00FFFFFFL)
									fc_val = 0x00FFFFFFL;

							break;

						case 2:		/* start */

							if ((clkctl EQ CK_MIDI) AND
							    (NOT midigo)) {

								midigo = TRUE;

								if (fc_val)
									sc_goto(fc_val = 0L);

								clkset(TRUE);
								dsclk();
							}

							break;

						case 3:		/* continue */

							if ((clkctl EQ CK_MIDI) AND
							    (NOT midigo)) {

								midigo = TRUE;
								clkset(TRUE);
								dsclk();
							}

							break;
/* 
*/
						case 4:		/* stop */

							if ((clkctl EQ CK_MIDI) AND
							    midigo) {

								midigo = FALSE;
								clkset(FALSE);
								dsclk();
							}

							break;

						case 6:		/* active sensing */

							masens[miport] |= 0x00FF;
							break;

						case 7:		/* reset */

							for (i = 0; i < NMPORTS; i++) {

								mrstat[i] = 0;
								msmstv[i] = 0;
								masens[i] = 0;
							}

							break;
						}
					}

				} else {	/* data */

					mdbyte[miport] = midiin;
					msmstv[miport] = 3;
				}

				continue;
/* 
*/
			case 3:		/* waiting for 2nd data byte */

				if (0x80 & midiin) {	/* status */

					michan = 0x0F & midiin;
					mistat = 0x0F & (midiin >> 4);

					if (midiin < 0x00F0) {		/* channel */

						msmstv[miport] = msmnext[mistat & 0x07];
						mrstat[miport] = midiin;

					} else if (midiin < 0x00F8) {	/* common */

						msmstv[miport] = msmcnxt[michan & 0x07];
						mrstat[miport] = midiin;
/* 
*/
					} else {			/* real-time */

						switch (0x0007 & midiin) {

						case 0:		/* clock */

							if (midiclk AND midigo)
								if ((fc_val += 2) > 0x00FFFFFFL)
									fc_val = 0x00FFFFFFL;

							break;

						case 2:		/* start */

							if ((clkctl EQ CK_MIDI) AND
							    (NOT midigo)) {

								midigo = TRUE;

								if (fc_val)
									sc_goto(fc_val = 0L);

								clkset(TRUE);
								dsclk();
							}

							break;

						case 3:		/* continue */

							if ((clkctl EQ CK_MIDI) AND
							    (NOT midigo)) {

								midigo = TRUE;
								clkset(TRUE);
								dsclk();
							}

							break;
/* 
*/
						case 4:		/* stop */

							if ((clkctl EQ CK_MIDI) AND
							    midigo) {

								midigo = FALSE;
								clkset(FALSE);
								dsclk();
							}

							break;

						case 6:		/* active sensing */

							masens[miport] |= 0x00FF;
							break;

						case 7:		/* reset */

							for (i = 0; i < NMPORTS; i++) {

								mrstat[i] = 0;
								msmstv[i] = 0;
								masens[i] = 0;
							}

							break;
						}
					}
/* 
*/
				} else {	/* data */

					michan = 0x0F & mrstat[miport];
					mistat = 0x0F & (mrstat[miport] >> 4);
					mdb1 = mdbyte[miport];
					mdb2 = midiin;
					msmstv[miport] = 2;

					switch (mistat) {	/* switch on status */

					case 9:		/* note on */

						if (mdb2) {

							t1 = (miport << 11) + (michan << 7) + mdb1;
							trgtab[t1] |= mpsust[(miport << 4) + michan] | M_KSTATE;
							veltab[t1] = nvel = SM_SCALE(mdb2);
							prstab[t1] = 0;

							stmproc(t1);	/* process as a patch stimulus */

							if (editsw) {	/* edit mode */

								execkey(t1, tuntab[mdb1], curvce, 0);
								break;
							}

							if (miport) {	/* port 1 or 2 */

								for (i = 0; i < 12; i++)
									if ((grp2prt[i][0] EQ (miport + 1)) AND
									    (grp2prt[i][1] EQ (michan + 1))) {

										asgvce(i, miport, michan, mdb1, nvel);
										ne_bgn(i, mdb1, nvel);
									}

							} else {	/* port 0 */

								t2 = ((mdb1 < 21) OR (mdb1 > 108)) ?
									0 : key2grp[mdb1 - 21];

								for (i = 0; i < 12; i++)
									if ((t2 & (0x0001 << i)) AND
									    (grp2prt[i][1] EQ (michan + 1))) {

										asgvce(i, miport, michan, mdb1, nvel);
										ne_bgn(i, mdb1, nvel);
									}
							}
		
							break;
						}
/* 
*/
					case 8:		/* note off */

						t1 = (miport << 11) + (michan << 7) + mdb1;
						trgtab[t1] &= ~M_KSTATE;

						if (0 EQ trgtab[t1]) {

							for (i = 0; i < 12; i++) {

								if (vce2trg[i] EQ t1) {

									vce2trg[i] = -1;
									procpfl(t1);
								}

								if ((grp2prt[i][0] EQ (miport + 1)) AND
								    (grp2prt[i][1] EQ (michan + 1)))
									ne_end(t1, i);
							}

							stmproc(0x8000 | t1);	/* process as a patch stimulus */
						}

						break;

					case 10:	/* poly pressure */

						t1 = (miport << 11) + (michan << 7) + mdb1;
						t2 = SM_SCALE(mdb2);
						prstab[t1] = t2;

						for (i = 0; i < 12; i++)
							if ((grp2prt[i][0] EQ (miport + 1)) AND
							    (grp2prt[i][1] EQ (michan + 1)))
								newpps(t1, i, SM_KPRS, t2);

						break;
/* 
*/
					case 11:	/* control change */

						mctlval[(miport << 11) + (michan << 7) + mdb1] = mdb2;

						if (mdb1 EQ MIDISUS) {

							/* sustain changed */

							t1 = (miport << 11) | (michan << 7);
							t2 = t1 + 128;

							if (mdb2 GT MSW_ON) {	/* switch on */

								mpsust[(miport << 4) + michan] = M_CHNSUS;

								for (i = t1; i < t2; i++)
									if (trgtab[i])
										trgtab[i] |= M_CHNSUS;

							} else if (mdb2 LT MSW_OFF) {	/* switch off */

								mpsust[(miport << 4) + michan] = 0;

								for (i = t1; i < t2; i++) {

									if (trgtab[i]) {

										trgtab[i] &= ~M_CHNSUS;

										if (0 EQ trgtab[i]) {

											for (t1 = 0; t1 < 12; t1++) {

												if (vce2trg[t1] EQ i) {

													vce2trg[t1] = -1;
													procpfl(i);
												}

												if ((grp2prt[t1][0] EQ (miport + 1)) AND
												    (grp2prt[t1][1] EQ (michan + 1)))
													ne_end(i, t1);
											}
										}
									}
								}
							}
/* 
*/
						} else if (mdb1 EQ MIDIHLD) {

							/* hold changed */

							t1 = (miport << 11) | (michan << 7);
							t2 = t1 + 128;

							if (mdb2 GT MSW_ON) {	/* switch on */

								for (i = t1; i < t2; i++)
									if (trgtab[i] & M_KSTATE) {

										for (t1 = 0; t1 < 12; t1++)
											if ((vce2trg[t1] & ~(MKEYHELD << 8)) EQ i)
												vce2trg[t1] |= (M_CHNHLD << 8);
									}

							} else if (mdb2 LT MSW_OFF) {	/* switch off */

								for (i = 0; i <12; i++) {

									if (vce2trg[i] NE -1) {

										vce2trg[i] &= ~(M_CHNHLD << 8);

										if (0 EQ (vce2trg[i] & (MKEYHELD << 8))) {

											t1 = vce2trg[i];

											if (0 EQ trgtab[t1]) {

												vce2trg[i] = -1;
												procpfl(t1);
											}
										}
									}
								}
							}
/* 
*/
						} else {	/* other controllers */

							t1 = (miport << 11) | (michan << 7) | mdb1;
							t2 = SM_SCALE(mdb2);
							mctlval[t1] = t2;

							for (j = 0; j < 4; j++) {

								if ((mctlnum[j] & 0x00FF) EQ mdb1) {

									i = ctl2src[j];

									for (t1 = 0; t1 < 12; t1++) {

										if ((mctlnum[j] & CTAG1) OR
										    ((grp2prt[t1][0] EQ (miport + 1)) AND
										     (grp2prt[t1][1] EQ (michan + 1)))) {

											if (newsv(t1, i, t2)) {

												if (recsw AND grpstat[t1] AND
												    (2 EQ (ancmsw ? varmode[src2var[i]][t1] : grpmode[t1]))) {

													if (E_NULL NE (ep = e_alc(E_SIZE2))) {

														ep->e_time = t_cur;
														ep->e_type = EV_ANVL;
														ep->e_data1 = (src2var[i] << 4) | t1;
														ep->e_dn = (struct s_entry *)((long)t2 << 16);
														p_cur = e_ins(ep, ep_adj(p_cur, 0, t_cur))->e_fwd;
														ctrsw = TRUE;
														se_disp(ep, D_FWD, gdstbc, 1);
														ctrsw = FALSE;
													}

												} else if ((angroup - 1) EQ t1) {

													dsanval(src2var[i]);
												}
											}
										}
									}
								}
							}
						}

						break;
/* 
*/
					case 14:	/* pitch bend */

						t1 = (miport << 4) | michan;
						t2 = ((mdb2 << 9) | (mdb1 << 2)) ^ 0x8000;
						mpbend[t1] = t2;

						for (i = 0; i < 12; i++) {

							if ((grp2prt[i][0] EQ (miport + 1)) AND
							    (grp2prt[i][1] EQ (michan + 1))) {

								if (newsv(i, SM_HTPW, t2)) {

									if (recsw AND grpstat[i] AND
									    (2 EQ (ancmsw ? varmode[0][i] : grpmode[i]))) {

										if (E_NULL NE (ep = e_alc(E_SIZE2))) {

											ep->e_time = t_cur;
											ep->e_type = EV_ANVL;
											ep->e_data1 = i;
											ep->e_dn = (struct s_entry *)((long)t2 << 16);
											p_cur = e_ins(ep, ep_adj(p_cur, 0, t_cur))->e_fwd;
											ctrsw = TRUE;
											se_disp(ep, D_FWD, gdstbc, 1);
											ctrsw = FALSE;
										}

									} else if ((angroup - 1) EQ i) {

										dsanval(0);
									}
								}
							}
						}

						break;
/* 
*/
					case 15:	/* common */

						if (michan EQ 2) {	/* song position */

							if ((clkctl EQ CK_MIDI) AND
							    (NOT midigo)) {

								where = 12 * ((mdb1 << 7) | mdb2);

								if (fc_val NE where)
									sc_goto(fc_val = where);
							}

							break;

						} else {		/* bad running status */

							msmstv[miport] = 0;
							mrstat[miport] = 0;
							break;
						}				

					default:	/* error -- bad status */

						msmstv[miport] = 0;
						mrstat[miport] = 0;
						break;

					}	/* end switch (on status) */

				}	/* end if (status) */

				continue;

			default:	/* error -- bad state */

				msmstv[miport] = 0;
				mrstat[miport] = 0;
				break;

			}	/* end switch (on state) */
		}		/* end while (data available) */
	}			/* end for (each port) */
}
