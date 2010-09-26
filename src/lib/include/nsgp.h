/*
   =============================================================================
	NSGP.H -- variables definitions for nsgraf
	Version 6 -- 1987-03-24 -- D.N. Lynx Crowe
	(c) Copyright 1985,1986,1987 -- D.N. Lynx Crowe
   =============================================================================
*/

/*
   =============================================================================
	Variable definitions for Newman / Sproull style 2D graphics package
   =============================================================================
*/

extern double

	Clipxl,		/* left clipping limit */
	Clipxr,		/* right clipping limit */
	Clipyb,		/* bottom clipping limit */
	Clipyt,		/* top clipping limit */

	Wxl,		/* window left edge */
	Wxr,		/* window right edge */
	Wyb,		/* window bottom edge */
	Wyt,		/* window top edge */

	Vxl,		/* viewport left edge */
	Vxr,		/* viewport right edge */
	Vyb,		/* viewport bottom edge */
	Vyt,		/* viewport top edge */

	WVxm,		/* windowed view, x scale factor */
	WVxa,		/* windowed view, x offset */
	WVym,		/* windowed view, y scale factor */
	WVya,		/* windowed view, y offset */

	Cwx,		/* current x -- world units */
	Cwy;		/* current y -- world units */

#if	!GLCGRAF

extern	int

	PenBgnd,	/* current pen background color for lines */
	PenFgnd,	/* current pen foreground color for lines */
	ChrBgnd,	/* current background color for characters */
	ChrFgnd;	/* current foreground color for characters */

#endif
