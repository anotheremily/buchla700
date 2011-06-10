/*
   ==========================================================================
	ascii.h -- Standard ASCII character definitions
	Version 5 -- 1987-06-03 -- D.N. Lynx Crowe
   ==========================================================================
*/

#define	A_NUL	0x00		/* ^@ */
#define	A_SOH	0x01		/* ^A */
#define	A_STX	0x02		/* ^B */
#define	A_ETX	0x03		/* ^C */
#define	A_EOT	0x04		/* ^D */
#define	A_ENQ	0x05		/* ^E */
#define	A_ACK	0x06		/* ^F */
#define	A_BEL	0x07		/* ^G */
#define	A_BS	0x08		/* ^H */
#define	A_HT	0x09		/* ^I */
#define	A_LF	0x0A		/* ^J */
#define	A_VT	0x0B		/* ^K */
#define	A_FF	0x0C		/* ^L */
#define	A_CR	0x0D		/* ^M */
#define	A_SO	0x0E		/* ^N */
#define	A_SI	0x0F		/* ^O */

#define	A_DLE	0x10		/* ^P */
#define	A_DC1	0x11		/* ^Q */
#define	A_DC2	0x12		/* ^R */
#define	A_DC3	0x13		/* ^S */
#define	A_DC4	0x14		/* ^T */
#define	A_NAK	0x15		/* ^U */
#define	A_SYN	0x16		/* ^V */
#define	A_ETB	0x17		/* ^W */
#define	A_CAN	0x18		/* ^X */
#define	A_EM	0x19		/* ^Y */
#define	A_SUB	0x1A		/* ^Z */
#define	A_ESC	0x1B		/* ^[ */
#define	A_FS	0x1C		/* ^\ */
#define	A_GS	0x1D		/* ^] */
#define	A_RS	0x1E		/* ^^ */
#define	A_US	0x1F		/* ^_ */

#define	A_DEL	0x7F		/* DEL */

#define	A_XON	0x11		/* DC1 = old ASCII XON */
#define	A_XOFF	0x13		/* DC3 = old ASCII XOFF */

#define	CTL(x)	(x - '@')
