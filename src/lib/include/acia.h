/*
   =============================================================================
	acia.h -- R65C52 ACIA definitions
	Version 1 -- 1988-11-29 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	ACIA_IER	0
#define	ACIA_ISR	0
#define	ACIA_CSR	2
#define	ACIA_CFR	2
#define	ACIA_TBR	4
#define	ACIA_CDR	4
#define	ACIA_TDR	6
#define	ACIA_RDR	6

#define	ANY		0x0080
#define	THRE		0x0040
#define	CTS		0x0020
#define	DCD		0x0010
#define	DSR		0x0008
#define	PAR		0x0004
#define	ERR		0x0002
#define	RDAV		0x0001
