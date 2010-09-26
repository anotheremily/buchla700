/*
   =============================================================================
	bpb.h -- GEMDOS BIOS parameter block structure
	Version 1 -- 1988-01-27 -- D.N. Lynx Crowe
   =============================================================================
*/

struct	BPB	{

	unsigned	recsiz;
	unsigned	clsiz;
	unsigned	clsizb;
	unsigned	rdlen;
	unsigned	fsiz;
	unsigned	fatrec;
	unsigned	datrec;
	unsigned	numcl;
	unsigned	bflags;
};
