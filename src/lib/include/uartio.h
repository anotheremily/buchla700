/*
   =============================================================================
	uartio.h -- Buchla 700 UART I/O definitions
	Version  3 -- 1989-07-18 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	PRT_DEV		0
#define	AUX_DEV		1
#define	CON_DEV		2
#define	MC1_DEV		3
#define	MC2_DEV		4

#define	BR_300		0x04
#define	BR_600		0x05
#define	BR_1200		0x06
#define	BR_1800		0x07
#define	BR_2400		0x08
#define	BR_3600		0x09
#define	BR_4800		0x0A
#define	BR_7200		0x0B
#define	BR_9600		0x0C
#define	BR_19K2		0x0D
#define	BR_EXT		0x0F

#define	NSB_1		0x40
#define	NSB_2		0x60

#define	NDB_5		0x80
#define	NDB_6		0xA0
#define	NDB_7		0xC0
#define	NDB_8		0xE0

#define	P_NONE		0x00
#define	P_ODD		0x04
#define	P_EVEN		0x0C
#define	P_MARK		0x14
#define	P_SPACE		0x1C

#define	U_RTS		0x01
#define	U_DTR		0x02

#define	U_PAR		0x01
#define	U_BRK		0x02

#define	L_NUL		0x00
#define	L_XON		0x01
#define	L_RTS		0x02
