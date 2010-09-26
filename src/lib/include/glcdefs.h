/*
   =============================================================================
	glcdefs.h -- definitions for the Epson GLC
	Version 4 -- 1988-08-03 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	G_INIT		(char)0x40
#define	G_MWRITE	(char)0x42
#define	G_MREAD		(char)0x43
#define	G_SETSAD	(char)0x44
#define	G_CRSWR		(char)0x46
#define	G_CRSRD		(char)0x47
#define	G_CRSMRT	(char)0x4C
#define	G_CRSMLT	(char)0x4D
#define	G_CRSMUP	(char)0x4E
#define	G_CRSMDN	(char)0x4F
#define	G_ERASE		(char)0x52
#define	G_SLEEP		(char)0x53
#define	G_DSPCTL	(char)0x58
#define	G_HSCRL		(char)0x5A
#define	G_OVRLAY	(char)0x5B
#define	G_CGRAM		(char)0x5C
#define	G_CRSFRM	(char)0x5D

#define	LCD_WC		lcd_a1
#define	LCD_RS		lcd_a0
#define	LCD_WD		lcd_a0
#define	LCD_RD		lcd_a1

#define	G_PLANE1	(unsigned)0x0000
#define	G_PLANE2	(unsigned)0x2000

#define	G_OFF		0		/* off */
#define	G_ON		1		/* on */
#define	G_B2		2		/* blink - FR/32 */
#define	G_B1		3		/* blink - FR/64 */
