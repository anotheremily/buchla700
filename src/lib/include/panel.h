/*
   =============================================================================
	panel.h -- front panel LCD, switch, key and pot control definitions
	Version 6 -- 1988-07-31 -- D.N. Lynx Crowe
   =============================================================================
*/

/* ----- performance key states (in pkctrl) ----- */

#define	PK_PFRM		0	/* performance keys */
#define	PK_NOTE		1	/* note weight */
#define	PK_GOTO		2	/* go to (sectn / score) */
#define	PK_INST		3	/* instrument selection (01..20 / 21..40) */
#define	PK_ASGN		4	/* assignment selection */
#define	PK_LIBR		5	/* library selection */
#define	PK_NGRP		6	/* group selection */
#define	PK_LOAD		7	/* load selection */

/* ----- slider and switch display state definitions (in sliders) ----- */

#define	LS_PRMTR	0	/* parameter articulation */
#define	LS_VOICE	1	/* voice articulation enable */
#define	LS_OTHER	2	/* aux, dome, cv */
#define	LS_EQ		3	/* EQ adjust */
#define	LS_INST		4	/* instrument select */
#define	LS_LIBR		5	/* library select */
#define	LS_NGRP		6	/* group select */
#define	LS_LOAD		7	/* load select */

/* ----- go to state definitions (in gomode) ----- */

#define	GO_NULL		0	/* null - not selected */
#define	GO_SECT		1	/* go to section */
#define	GO_SCOR		2	/* go to score */

/* ----- instrument selection state definitions (in ismode) ----- */

#define	IS_NULL		0	/* null - not selected */
#define	IS_LORC		1	/* 01..20 - low orchestra */
#define	IS_HORC		2	/* 21..40 - high orchestra */
