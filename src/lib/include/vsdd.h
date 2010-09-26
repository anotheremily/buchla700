/*
   =============================================================================
	vsdd.h -- header for Matra-Harris 82716 VSDD video display functions
	Version 10 -- 1987-05-27 -- D.N. Lynx Crowe
	(c) Copyright 1987 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	VSDD	(unsigned int *)0x200000

#define	V_CHITE	12		/* Character height */

/* object descriptor flags */

#define	V_CBS	0x0800		/* 0 = Bit Map, 1 = Character */
#define	V_RES0	0x0000		/* Resolution 0: char 16, bit - */
#define	V_RES1	0x0200		/* Resolution 1: char  6, bit - */
#define	V_RES2	0x0400		/* Resolution 2: char  8, bit 2 */
#define	V_RES3	0x0600		/* Resolution 3: char 12, bit 4 */
#define	V_CRS	0x0100		/* Conceal/Reveal Select */

#define	V_PSE	0x0080		/* Proportional space enable */
#define	V_FAD	0x0040		/* Full attributes select */
#define	V_OBL	0x0020		/* Object blink */
#define	V_BLA	0x0010		/* Object blank */

#define	V_HCR	0x0008		/* High color resolution */
#define	V_TDE	0x0004		/* Transparency detect enable */
#define	V_DCS0	0x0000		/* Default color select 0 */
#define	V_DCS1	0x0001		/* Default color select 1 */
#define	V_DCS2	0x0002		/* Default color select 2 */
#define	V_DCS3	0x0003		/* Default color select 3 */

/* Preset flags for bitmap and character objects */

#define	V_BTYPE	0
#define	V_CTYPE	(V_CBS | V_RES2 | ((V_CHITE - 1) << 12))

/* character attribute flags */

#define	C_ALTCS	0x8000
#define	C_TFGND	0x4000
#define	C_TBGND	0x2000
#define	C_WIDE	0x1000

#define	C_MASK	0x0800
#define	C_INVRT	0x0400
#define	C_BLINK	0x0200
#define	C_ULINE	0x0100

/* 
*/

struct octent {			/* Object control table entry */

	unsigned int	ysize,	/* Height of object in pixels */
			xsize;	/* Width of object in pixels */

	int		objx,	/* Object x location */
			objy;	/* Object y location */

	unsigned int	*obase;	/* Base of object data */

	char		opri,	/* Current object priority */
			obank;	/* Object bank 0..3 */

	unsigned int	odtw0,	/* Object descriptor table word 0 */
			odtw1;	/* Object descriptor table word 1 */
};

