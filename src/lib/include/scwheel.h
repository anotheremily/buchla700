/*
   =============================================================================
	scwheel.h -- MIDAS-VII scroll wheel and trackball constants
	Version 14 -- 1989-07-20 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	NSWFIFO		100		/* scroll wheel FIFO size */

#define	SWBACK		10		/* scroll wheel FIFO look back count */
#define	SWRMIN		8		/* scroll wheel rate threshold */
#define	SWTHR		4		/* scroll wheel movement threshold */
#define	SWTIME		10		/* scroll wheel timer scan rate */
#define	SWWAIT		2		/* scroll wheel wait count */

#define	NTKFIFO		100		/* trackball FIFO size */

#define	TKBACK		8		/* trackball FIFO look back count */
#define	TKRMIN		6		/* trackball rate threshold */
#define	TKTHR		3		/* trackball movement threshold */
#define	TKTIME		10		/* trackball timer scan rate */
#define	TKWAIT		2		/* trackball wait count */

#define	TKXDVR		4		/* text cursor X divisor */
#define	TKYDVR		6		/* text cursor Y divisor */

#define	TKCDVR		6		/* text cursor movement threshold */

#define	MSTOVAL		125		/* mouse movement timer value */
#define	MUTIME		1000		/* mouse reset timer value */
