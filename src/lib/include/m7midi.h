/*
   =============================================================================
	m7midi.h -- MIDI definitions for MIDAS on the Buchla 700
	Version 1 -- 1987-11-18 -- D.N. Lynx Crowe

	#define PT_DEFS in the module that gets the variables defined in it.
   =============================================================================
*/

/* group assignment table definitions */

#define	RX_PORT		0	/* receiver input port 0..2 */
#define	RX_CHAN		1	/* receiver base channel 0, 1..16 */
#define	RX_MODE		2	/* receiver mode 0..3 */
#define	RX_MVAL		3	/* receiver M value 0, 1..16 */
#define	RX_MSMV		4	/* receiver MIDI state machine variable */

#define	TX_PORT		5	/* transmitter output port 0..2 */
#define	TX_CHAN		6	/* transmitter base channel 0, 1..16 */
#define	TX_MODE		7	/* transmitter mode 0..3 */
#define	TX_MVAL		8	/* transmitter M value 0, 1..16 */
#define	TX_MSMV		9	/* transmitter MIDI state machine variable */

#define	RT_SIZE		10	/* group assignment table width */

/* port setup table */

#define	PT_ENBL		0	/* port enable flag */
#define	PT_BASE		1	/* port master base channel (0 = off, 1..16) */
#define	PT_PORT		2	/* port BIOS port number */
#define	PT_GRPS		3	/* port group flags (bit no. = group no.) */

#define	PT_SIZE		4	/* port setup table width */

#ifdef	PT_DEFS
char grpasg[12][RT_SIZE];	/* group assignment data */
short portasg[3][PT_SIZE];	/* port setup data */
#else
extern char grpasg[12][RT_SIZE];
extern short portasg[3][PT_SIZE];
#endif
