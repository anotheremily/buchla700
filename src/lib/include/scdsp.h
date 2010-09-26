/*
   =============================================================================
	scdsp.h -- score display definitions
	Version 14 -- 1988-08-16 -- D.N. Lynx Crowe

	Uses definitions from "graphdef.h", "midas.h" and "vsdd.h"
   =============================================================================
*/

#define	SCOROBP	1			/* score object priority */

#define	N_SHARP	4			/* note control code for sharps */
#define	N_FLAT	1			/* note control code for flats */

#define	OBFL_01	(BIT3)			/* cursor - underline */

#define	OBFL_08	(CHR3)			/* character text */

#define	OBFL_11	(BIT3)			/* window */

#define	OBFL_13	(BIT3)			/* keyboard */
#define	OBFL_14	(BIT3 | V_TDE)		/* score */
#define	OBFL_15	(BIT3 | V_TDE)		/* lines */

#define	C1X	256			/* initial arrow cursor position */
#define	C1Y	175

/* 
*/

/* Background colors */

#define	SDBGMM		4
#define	SDBG00		3
#define	SDBG01		4
#define	SDBG02		3
#define	SDBG03		4
#define	SDBG04		3
#define	SDBG05		4
#define	SDBG06		3
#define	SDBG07		4
#define	SDBG08		3
#define	SDBG09		4
#define	SDBG10		3
#define	SDBG11		2
#define	SDBG12		2
#define	SDBG13		3
#define	SDBG14		4
#define	SDBG15		3
#define	SDBG16		2

/* other colors -- Note:  groups are colors 3..14 */

#define	SD_TEXT		1		/* text */

#define	SD_ENTR		6		/* data entry highlight */
#define	SD_CHNG		5		/* changed data highlight */

#define	SD_WKEY		1		/* white keys */

#define	LN_VRT		1		/* score cursor - vertical line */
#define	LN_HOR		4		/* score cursor - ledger lines */

#define	NDMC		4		/* new data margin color */

#define	SDCURSR		15		/* score cursor - arrow */

/* 
*/

/* character attributes */

#define	SDMENUBG	(SDBGMM | (SD_TEXT << 4))	/* menu */

#define	SDW00ATR	(SDBG00 | (SD_TEXT << 4))	/* normal */
#define	SDW00DEA	(SDBG00 | (SD_ENTR << 4))	/* data entry */

#define	SDW01ATR	(SDBG01 | (SD_TEXT << 4))	/* normal */
#define	SDW01DEA	(SDBG01 | (SD_ENTR << 4))	/* data entry */

#define	SDW02ATR	(SDBG02 | (SD_TEXT << 4))	/* normal */
#define	SDW02DEA	(SDBG02 | (SD_ENTR << 4))	/* data entry */

#define	SDW03ATR	(SDBG03 | (SD_TEXT << 4))	/* normal */
#define	SDW03DEA	(SDBG03 | (SD_ENTR << 4))	/* data entry */

#define	SDW04ATR	(SDBG04 | (SD_TEXT << 4))	/* normal */
#define	SDW04DEA	(SDBG04 | (SD_ENTR << 4))	/* data entry */

#define	SDW05ATR	(SDBG05 | (SD_TEXT << 4))	/* normal */
#define	SDW05DEA	(SDBG05 | (SD_ENTR << 4))	/* data entry */

#define	SDW06ATR	(SDBG06 | (SD_TEXT << 4))	/* normal */
#define	SDW06DEA	(SDBG06 | (SD_ENTR << 4))	/* data entry */

#define	SDW07ATR	(SDBG07 | (SD_TEXT << 4))	/* normal */
#define	SDW07DEA	(SDBG07 | (SD_ENTR << 4))	/* data entry */

#define	SDW08ATR	(SDBG08 | (SD_TEXT << 4))	/* normal */
#define	SDW08DEA	(SDBG08 | (SD_ENTR << 4))	/* data entry */

#define	SDW09ATR	(SDBG09 | (SD_TEXT << 4))	/* normal */
#define	SDW09DEA	(SDBG09 | (SD_ENTR << 4))	/* data entry */

#define	SDW10ATR	(SDBG10 | (SD_TEXT << 4))	/* normal */
#define	SDW10DEA	(SDBG10 | (SD_ENTR << 4))	/* data entry */

#define	SDW11ATR	(SDBG11 | (SD_TEXT << 4))	/* normal */
#define	SDW11DEA	(SDBG11 | (SD_ENTR << 4))	/* data entry */

#define	SDW12ATR	(SDBG12 | (SD_TEXT << 4))	/* normal */
#define	SDW12DEA	(SDBG12 | (SD_ENTR << 4))	/* data entry */

#define	SDW13ATR	(SDBG13 | (SD_TEXT << 4))	/* normal */
#define	SDW13DEA	(SDBG13 | (SD_ENTR << 4))	/* data entry */

#define	SDW14ATR	(SDBG14 | (SD_TEXT << 4))	/* normal */
#define	SDW14DEA	(SDBG14 | (SD_ENTR << 4))	/* data entry */

#define	SDW15ATR	(SDBG15 | (SD_TEXT << 4))	/* normal */
#define	SDW15DEA	(SDBG15 | (SD_ENTR << 4))	/* data entry */
