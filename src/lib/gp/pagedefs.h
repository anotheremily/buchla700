/*
   ============================================================================
	pagedefs.h -- externals for pagination functions
	Version 1 -- 1986-11-08 -- D.N. Lynx Crowe
   ============================================================================
*/

extern int newsub, minlines, npage, nline, nlpage, martop, marbot, marsize;

extern int (*prhead) ();
extern int newline (), margin (), pgmhdr (), skipnl (), subhdr (), newsect (),
heading ();

extern char *hdr, *verstr, *subhdg;

extern FILE *printer;
