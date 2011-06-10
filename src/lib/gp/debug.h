/*
   =============================================================================
	debug.h -- some simple debug trace macros
	Version 1 -- 1988-03-03 -- D.N. Lynx Crowe
   =============================================================================
*/

#ifdef	DEBUGGER
#define	DB_ENTR(mod)	DB_Entr(mod)
#define	DB_EXIT(mod)	DB_Exit(mod)
#define	DB_CMNT(mod)	DB_Cmnt(mod)
#else
#define	DB_ENTR(mod)
#define	DB_EXIT(mod)
#define	DB_CMNT(mod)
#endif
