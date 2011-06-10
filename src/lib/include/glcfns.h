/*
   =============================================================================
	glcfns.h -- definitions for the Epson GLC driver
	Version 6 -- 1988-01-10 -- D.N. Lynx Crowe
   =============================================================================
*/

extern unsigned GLCcrc (), GLCcxy ();
extern short GLCplot ();
extern short GLCinit (), GLCwrts (), GLCtext (), GLCdisp (), GLCcurs ();

extern unsigned lcdx, lcdy, lcdbit, lcdrow, lcdcol, lcdbase;
extern unsigned lcdctl1, lcdctl2;
