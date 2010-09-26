/*
   =============================================================================
	vsetpal.c -- Buchla 700 color palette setup
	Version 2 -- 1987-11-20 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	PALETTE		((unsigned *)0x280000L)

short	dfltpal[16][3] = {

/*	 R  G  B	  color */

	{0, 0, 0},	/*  0: Black */
	{0, 1, 0},	/*  1: Dark Gray */
	{0, 0, 2},	/*  2: Dark Blue */
	{0, 0, 3},	/*  3: Light Blue */
	{0, 2, 0},	/*  4: Dark Green */
	{0, 3, 0},	/*  5: Light Green */
	{0, 2, 2},	/*  6: Dark Cyan */
	{0, 3, 3},	/*  7: Light Cyan */
	{3, 0, 0},	/*  8: Red */
	{3, 1, 0},	/*  9: Orange */
	{3, 0, 3},	/* 10: Purple */
	{3, 1, 2},	/* 11: Magenta */
	{3, 2, 0},	/* 12: Brown */
	{2, 3, 0},	/* 13: Yellow */
	{2, 2, 2},	/* 14: Light Gray */
	{3, 3, 3},	/* 15: White */
};

/* 
*/

/*
   =============================================================================
	vsetpal() -- set a palette RAM entry to a specific color

	slot	0..15	color entry number
	red	0..3	red value
	grn	0..3	green value
	blu	0..3	blue value
   =============================================================================
*/

vsetpal(slot, red, grn, blu)
register unsigned slot, red, grn, blu;
{
	register unsigned palval;
	unsigned *pal;

	pal = PALETTE;

	palval = (slot << 6) |
		 ((red & 1) << 5) | ((red & 2) << 1) |
		 ((grn & 1) << 4) | (grn & 2) |
		 ((blu & 1) << 3) | ((blu & 2) >> 1);

	*pal = palval ^ 0x003F;
}

/* 
*/

/*
   =============================================================================
	vsndpal() -- send a palette table to the video palette RAM

	pp	pointer to a short [16][3] array,

		where:
			pp[n][0]	= blue value
			pp[n][1]	= green value
			pp[n][2]	= red value

				n	= slot number (0..15)
   =============================================================================
*/

vsndpal(pp)
short pp[16][3];
{
	register short i;

	for (i = 0; i < 16; i++)
		vsetpal(i, pp[i][0], pp[i][1], pp[i][2]);
}
