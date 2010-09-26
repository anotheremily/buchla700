/*
   ============================================================================
	objdefs.h -- Object file format for as68  (Atari 1040ST TOS objects)
	Version 7 -- 1987-12-30 -- D.N. Lynx Crowe
   ============================================================================
*/

struct EXFILE {			/* executable file header */

	unsigned	F_Magic;	/* File type magic */
	long		F_Text;		/* SIze of text segment */
	long		F_Data;		/* Size of data segment */
	long		F_BSS;		/* Size of BSS segment */
	long		F_Symtab;	/* Size of symbol table */
	long		F_Res1;		/* Reserved area #1 */
	long		F_Res2;		/* Reserved area #2 -- text origin */
	unsigned	F_Res3;		/* Reserved area #3 -- flag word */

					/* data origin - long */
					/* bss origin - long */
};

#define	F_R_C	0x601A		/* Magic for contiguous file */
#define	F_R_D	0x601B		/* Magic for discontiguous file */

struct SYMBOL {			/* Symbol table entry -- 14 bytes */

	char		symname[8];	/* Symbol name  (LJZF) */
	unsigned	symtype;	/* Symbol type flags */
	long		symvalue;	/* Symbol value */
};

#define	S_Def	0x8000		/* Defined */
#define	S_Equ	0x4000		/* Equated */
#define	S_Glb	0x2000		/* Global */
#define	S_Reg	0x1000		/* Equated register */
#define	S_Ext	0x0800		/* External reference */
#define	S_Data	0x0400		/* Data based relocatable */
#define	S_Text	0x0200		/* Text based relocatable */
#define	S_BSS	0x0100		/* BSS based relocatable */
