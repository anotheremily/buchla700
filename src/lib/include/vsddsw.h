/*
   =============================================================================
	VSDDSW.H -- VSDD software support definitions
	Version 6 -- 1987-05-27 -- D.N. Lynx Crowe
	Copyright 1987 -- D.N. Lynx Crowe
   =============================================================================
*/

/* functions */

extern	unsigned	vbank();
extern	int	vputp(), vwputp();

/* these should be voids ... */

extern	int	vfwait(), vputs(), vputsa(), vputc(), VHinit();
extern	int	objclr(), objon(), objoff();
extern	int	vcputs(), vwputs(), vwputm(), vmput(), vmputa(), VSinit();
extern	int	SelObj(), SetPri(), SetObj(), CpyObj();
