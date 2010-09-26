/*
   =============================================================================
	tundsp.h -- tuning editor common parameter header file
	Version 5 -- 1987-12-21 -- D.N. Lynx Crowe

	Uses definitions from:  "graphdef.h", "midas.h", "vsdd.h"
   =============================================================================
*/

#define	TUNOBJ		8		/* display object number */
#define	TUNPRI		8		/* display object priority */

#define	TUNFL		(V_RES3)	/* display object flags */

#define	TDCURX		CTOX(61)	/* initial cursor x location */
#define	TDCURY		RTOY(24)	/* initial cursor y location */

#define	TDCURSR		1		/* cursor color */
#define	TDTCURC		3		/* typewriter cursor color */
#define	TDTCURB		7		/* typewriter background color */
#define	TDTCHRC		11		/* typewriter text color */

#define	TDCHGD		14		/* hilite color for changed table # */
#define	TDSELD		13		/* hilite color for selected data */
#define	TDENTRY		12		/* hilite color for data entry */
#define	TDLABEL		9		/* pitch label color */
#define	TDMKEYC		10		/* MIDI key number label color */

#define	TCFBX00		11		/* foreground color */
#define	TCFBX01		11		/* foreground color */
#define	TCFBX02		11		/* foreground color */
#define	TCFBX03		11		/* foreground color */
#define	TCFBX04		11		/* foreground color */
#define	TCFBX05		11		/* foreground color */
#define	TCFBX06		11		/* foreground color */
#define	TCFBX07		11		/* foreground color */

#define	TCBBX00		5		/* background color */
#define	TCBBX01		8		/* background color */
#define	TCBBX02		5		/* background color */
#define	TCBBX03		8		/* background color */
#define	TCBBX04		5		/* background color */
#define	TCBBX05		8		/* background color */
#define	TCBBX06		6		/* background color */
#define	TCBBX07		4		/* background color */

