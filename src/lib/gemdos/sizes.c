/*
   ============================================================================
	sizes.c -- stat an absolute format Alcyon object file
	Version 9 -- 1989-02-01 -- D.N. Lynx Crowe
   ============================================================================
*/

#include "stdio.h"
#include "stddefs.h"
#include "portab.h"
#include "objdefs.h"
#include "biosdefs.h"

extern	FILE	*fopenb();
extern	int	fclose(), fread(), flread();
extern	long	getl();

FILE	*B_file;	/* file pointer */

struct	EXFILE B_fhdr;	/* executable file header */

long	B_txt_o,	/* text origin from file header */
	B_dat_o,	/* data origin from file header */
	B_bss_o,	/* bss origin from file header */
	B_txt_l,	/* text length from file header */
	B_dat_l,	/* data length from file header */
	B_bss_l,	/* bss length from file header */
	B_lod_l;	/* total data length checked */

long	B_end;		/* end address */

/*  */

/*
   ============================================================================
	lstat(fn) -- stat an object file named by string 'fn'.
   ============================================================================
*/

lstat(fn)
char *fn;
{
	register long i, bgnbss, endbss, total_l;

	/* initialize the origins and lengths to 0 */

	B_txt_o = 0L;
	B_dat_o = 0L;
	B_bss_o = 0L;
	B_txt_l = 0L;
	B_dat_l = 0L;
	B_bss_l = 0L;
	B_lod_l = 0L;

	/* open the file */

	if (NULL EQ (B_file = fopenb(fn, "r"))) {

		printf("sizes:  Unable to open \"%s\"\n", fn);
		return(1);
	}

	/* read in the file header */

	if (1 NE fread(&B_fhdr, sizeof B_fhdr, 1, B_file)) {

		printf("sizes:  Unable to read header for \"%s\"\n", fn);
		fclose(B_file);
		return(2);
	}

	/* check the magic */

	if ((B_fhdr.F_Magic NE F_R_C) AND (B_fhdr.F_Magic NE F_R_D)) {

		printf("sizes:  Bad magic [$%04x] in file \"%s\"\n",
			B_fhdr.F_Magic, fn);

		fclose(B_file);
		return(3);
	}

/* 
*/

	/* if it's a discontinuous file, read the origins */

	if (B_fhdr.F_Magic EQ F_R_D) {

		B_dat_o = getl(B_file);
		B_bss_o = getl(B_file);
	}

	B_txt_o = B_fhdr.F_Res2;
	B_lod_l = B_fhdr.F_Text + B_fhdr.F_Data;

	fclose(B_file);

	B_txt_l = B_fhdr.F_Text;
	B_dat_l = B_fhdr.F_Data;
	B_bss_l = B_fhdr.F_BSS;
	B_end = B_txt_o + B_lod_l - 1L;

	if (B_bss_o)
		bgnbss = B_bss_o;
	else
		bgnbss = B_end + 1L;

		endbss = bgnbss + B_bss_l - 1L;

	printf("File \"%s\" is a %s loaded file and\n",
		fn, (B_fhdr.F_Magic EQ F_R_D) ? "Scatter" : "Contiguously");

	printf("loads from $%08lx to $%08lx  (%ld bytes)\n",
		B_txt_o, B_end, B_lod_l);

	printf("  with BSS $%08lx to $%08lx  (%ld bytes)\n\n",
		bgnbss, endbss, B_bss_l);

	printf("   B_txt_o = $%08lx, B_dat_o = $%08lx, B_bss_o = $%08lx\n",
		B_txt_o, B_dat_o, B_bss_o);

	printf("   B_txt_l = $%08lx, B_dat_l = $%08lx, B_bss_l = $%08lx\n\n",
		B_txt_l, B_dat_l, B_bss_l);

	total_l = B_txt_l + B_dat_l + B_bss_l;

	printf("Total program length = %ld ($%08lx) bytes\n",
		total_l, total_l);

	return(0);
}

/* 
*/

main(argc, argv)
int argc;
char *argv[];
{
	int	rc;

	if (argc NE 2) {

		printf("sizes:  ERROR - file name required\n");
		exit(1);
	}

	if (lstat(argv[1]))
		exit(1);
	else
		exit(0);
}
