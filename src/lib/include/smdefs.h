/*
   =============================================================================
	smdefs.h -- MIDAS Source / Multiplier definitions
	Version 6 -- 1987-12-06 -- D.N. Lynx Crowe
   =============================================================================
*/

struct sment
{

  struct sment *nxt;
  struct sment *prv;
  unsigned vp;
  unsigned sm;
};

struct valent
{

  struct sment *nxt;
  struct sment *prv;
  unsigned val;
};

#define	SM_NONE		0	/* No source - always 0 */
#define	SM_RAND		1	/* Random source */
#define	SM_CTL1		2	/* Control Voltage 1 / MIDI GPC 1 */

#define	SM_PTCH		5	/* Key Pitch     (not updated in scan table) */
#define	SM_KPRS		6	/* Key Pressure */
#define	SM_KVEL		7	/* Key Velocity */
#define	SM_PED1		8	/* Pedal 1 */

#define	SM_FREQ		10	/* Key frequency (not updated in scan table) */
#define	SM_HTPW		11	/* Tablet X / MIDI Pitch Wheel */
#define	SM_VTMW		12	/* Tablet Y / MIDI Mod Wheel */
#define	SM_LPBR		13	/* LongPot / MIDI Breath Controller */

/* Sources 3, 4, and 9 no longer exist */

#define	NSRCS		14	/* Highest source number + 1 */
#define	NGPSRS		(12 << 4)	/* Number of group/source pairs */
