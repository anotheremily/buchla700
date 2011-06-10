/*
   =============================================================================
	lcdline.h -- LCD line definitions
	Version 3 -- 1988-08-03 -- D.N. Lynx Crowe

	Note:	LCD characters are 5x7 in a 6x8 envelope.

		The LCD charcter addresses start with (0,0) in the
		upper left corner.  8 rows of 85 characters.

		LCD graphic addresses start with (0,0) in the
		lower left corner.  512 X pixels by 64 Y pixels.
   =============================================================================
*/

#define	RP_PL	153		/* "R/P" underscore - "P" X left */
#define	RP_PR	161		/* "R/P" underscore - "P" X right */
#define	RP_RL	168		/* "R/P" underscore - "R" X left */
#define	RP_RR	176		/* "R/P" underscore - "R" X right */
#define	RP_Y	54		/* "R/P" underscore - Y */

#define	CK_XL	114		/* "Clock" underscore - X left */
#define	CK_XR	143		/* "Clock" underscore - X right */
#define	CK_Y	54		/* "Clock" underscore - Y */

#define	LAMP_XL	78
#define	LAMP_XR	LAMP_XL+23
#define	LAMP_Y	54

#define	GOTO_XL	186
#define	GOTO_XR	GOTO_XL+29
#define	GOTO_Y	54

#define	ASGN_XL	258
#define	ASGN_XR	ASGN_XL+29
#define	ASGN_Y	54

#define	LOAD_XL	294
#define	LOAD_XR	LOAD_XL+23
#define	LOAD_Y	54
