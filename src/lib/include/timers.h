/*
   =============================================================================
	timers.h -- MIDAS-VII timer definitions
	Version 5 -- 1989-07-19 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	NTIMERS		8	/* number of timers */

#define	SWTIMER		0	/* scroll wheel timer index */
#define	TXTIMER		1	/* trackball X timer index */
#define	TYTIMER		2	/* trackball Y timer index */
#define	SQTIMER		3	/* master sequence timer */
#define	MSTIMER		4	/* mouse movement timer */
#define	MUTIMER		5	/* mouse buffer flush timer */

extern short timers[NTIMERS];	/* timer array */
