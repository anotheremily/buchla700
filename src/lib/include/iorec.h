/*
   =============================================================================
	iorec.h -- structure defintion for Multi-Tasking BIOS io control record
	Version 1 -- 1988-04-02 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe

	Requires mtdefs.h for the SEM definition.
   =============================================================================
*/

struct iorec {

	char	*ibuf;		/* input buffer base address */
	short	ibsize;		/* input buffer size - bytes */
	short	ibufhd;		/* input buffer head index */
	short	ibuftl;		/* input buffer tail index */
	short	ibuflow;	/* input buffer low water mark */
	short	ibufhi;		/* input bufrer high water mark */
	char	*obuf;		/* output buffer base address */
	short	obsize;		/* output buffer size - bytes */
	short	obufhd;		/* output buffer head index */
	short	obuftl;		/* output buffer tail index */
	short	obuflow;	/* output buffer low water mark */
	short	obufhi;		/* output buffer high water mark */
	char	cfr0;		/* ACIA CFR, MS bit EQ 0 */
	char	cfr1;		/* ACIA CFR, MS bit EQ 1 */
	char	flagxon;	/* XON flag	(non-zero EQ XOFF sent) */
	char	flagxof;	/* XOFF flag	(non-zero EQ active) */
	char	linedis;	/* line discipline */
	char	erbyte;		/* last error byte */
	char	isr;		/* ACIA ISR on interrupt */
	char	csr;		/* ACIA CSR on interrupt */
	short	errct;		/* error count  (FRM/OVR/BRK) */
	short	ibfct;		/* input buffer overflow error count */
	SEM	inp_nf;		/* input buffer not-full semaphore */
	SEM	inp_ne;		/* input buffer not-empty semaphore */
	SEM	out_nf;		/* output buffer not-full semaphore */
	SEM	out_ne;		/* output buffer not-empty semaphore */
};
