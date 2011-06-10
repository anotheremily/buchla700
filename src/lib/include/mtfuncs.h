/*
   =============================================================================
	mtfuncs.h -- multitasker function references
	Version 7 -- 1988-03-29 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
   =============================================================================
*/

extern unsigned MTID (), MTSetP (), MTGetID ();
extern short MTRun (), MTStop (), MTStat (), MTZap ();
extern short SMStat (), SMCSig (), SMCWait ();
extern short MBDel ();
extern MSG *MBRecv (), *MBChek ();
