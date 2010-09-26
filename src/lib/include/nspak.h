/*
   =============================================================================
	NSPAK.H -- function definition header for nsgraf
	Version 6 -- 1987-03-23 -- D.N. Lynx Crowe
	Copyright 1985,1986,1987 -- D.N. Lynx Crowe
   =============================================================================
*/

/*
   =============================================================================
	Function definitions for Newman / Sproull style 2D graphics package
   =============================================================================
*/

/* Functions in NSCLIP.O */
int Clip();

/* Functions in NSGRAF.O */
int SetWind(), SetView(), WctoSc(), MoveTo(), LineTo(), ShowLine();
int NSinit(), VPbord();

/* Functions in NSSNAP.O */
int SnapWV();
