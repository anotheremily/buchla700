/*
   =============================================================================
	wsdsp.h -- waveshape editor common parameter header file
	Version 12 -- 1987-12-14 -- D.N. Lynx Crowe

	Uses definitions from:  "graphdef.h", "midas.h", "vsdd.h"
	Note that the definition of wstbl MUST match the corresponding
	area in instdef, or chaos will result.
   =============================================================================
*/

struct	wstbl	{		/* waveshape table library entry */

	short	final[NUMWPNT];		/* final values */
	short	offset[NUMWPNT];	/* offset values */
	short	harmon[NUMHARM];	/* harmonic values */
};

#define	WAVEOBJ		8		/* waveshape display object number */
#define	WAVEPRI		8		/* waveshape display object priority */

#define	WAVEFL		(V_RES3)	/* waveshape display object flags */

#define	WCURX		256		/* initial cursor x location */
#define	WCURY		128		/* initial cursor y location */

#define	WBSF1		128		/* waveshape bar scaling multiplier */
#define	WBSF2		100		/* waveshape bar scaling divisor */
#define	WBOFF		306		/* waveshape bar offset */

#define	WPSF1		1		/* waveshape point scaling multiplier */
#define	WPSF2		8		/* waveshape point scaling divisor */
#define	WPOFF		133		/* waveshape point offset */

#define	WBORD		2		/* waveshape display border color */
#define	WZBC		10		/* waveshape zero line color */

#define	WS_CHGC		13		/* waveshape changed color */
#define	WS_ENTRY	12		/* waveshape data entry color */

#define	WDCURS		1		/* waveshape normal cursor color */
#define	WS_GRAB		13		/* waveshape pnt grabbed cursor color */

#define	WBCN		8		/* waveshape bar color - negative */
#define	WBCP		9		/* waveshape bar color - positive */

#define	WOVC		6		/* waveshape offset point color */
#define	WFVC		7		/* waveshape final point color */

#define	WCFBX00		11		/* foreground color */
#define	WCFBX01		11		/* foreground color */
#define	WCFBX02		11		/* foreground color */
#define	WCFBX03		11		/* foreground color */
#define	WCFBX04		11		/* foreground color */
#define	WCFBX05		11		/* foreground color */

#define	WCBBX00		3		/* background color */
#define	WCBBX01		4		/* background color */
#define	WCBBX02		5		/* background color */
#define	WCBBX03		14		/* background color */
#define	WCBBX04		5		/* background color */
#define	WCBBX05		14		/* background color */

#define	WAVE_OFF	9
#define	WDTH_OFF	6
#define	WINS_OFF	11
#define	WHRM_OFF	10
#define	WHRV_OFF	6
#define	WOFF_OFF	14
#define	WPNT_OFF	4
#define	WSLT_OFF	19
#define	WVCE_OFF	18
#define	WFNL_OFF	14
