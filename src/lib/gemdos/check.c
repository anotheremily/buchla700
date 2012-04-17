/*
   ============================================================================
	check.c -- checksum and stat an absolute format Alcyon object file
	Version 8 -- 1988-10-06 -- D.N. Lynx Crowe
   ============================================================================
*/

#include "stdio.h"
#include "stddefs.h"
#include "portab.h"
#include "objdefs.h"
#include "biosdefs.h"

extern FILE *fopenb ();
extern int fclose (), fread (), flread ();
extern long getl ();

FILE *B_file;			/* file pointer */

struct EXFILE B_fhdr;		/* executable file header */

long B_txt_o,			/* text origin from file header */
  B_dat_o,			/* data origin from file header */
  B_bss_o,			/* bss origin from file header */
  B_txt_l,			/* text length from file header */
  B_dat_l,			/* data length from file header */
  B_bss_l,			/* bss length from file header */
  B_lod_l;			/* total data length checked */

long B_chk,			/* checksum */
  B_end;			/* end address */

/*  */

/*
   ============================================================================
	check(fn) -- checksum file named by string 'fn'.
   ============================================================================
*/

check (fn)
     char *fn;
{
  register long i, sum, bgnbss, endbss, total_l;

  /* initialize the origins and lengths to 0 */

  B_txt_o = 0L;
  B_dat_o = 0L;
  B_bss_o = 0L;
  B_txt_l = 0L;
  B_dat_l = 0L;
  B_bss_l = 0L;
  B_lod_l = 0L;

  /* open the file */

  if (NULL == (B_file = fopenb (fn, "r")))
    {

      printf ("check:  Unable to open \042%s\042\n", fn);
      return (1);
    }

  /* read in the file header */

  if (1 != fread (&B_fhdr, sizeof B_fhdr, 1, B_file))
    {

      printf ("check:  Unable to read header for \042%s\042\n", fn);
      fclose (B_file);
      return (2);
    }

  /* check the magic */

  if ((B_fhdr.F_Magic != F_R_C) && (B_fhdr.F_Magic != F_R_D))
    {

      printf ("check:  Bad magic [$%04x] in file \042%s\042",
	      B_fhdr.F_Magic, fn);

      fclose (B_file);
      return (3);
    }

/* 
*/

  /* if it's a discontinuous file, read the origins */

  if (B_fhdr.F_Magic == F_R_D)
    {

      B_dat_o = getl (B_file);
      B_bss_o = getl (B_file);
    }

  B_txt_o = B_fhdr.F_Res2;
  B_lod_l = B_fhdr.F_Text + B_fhdr.F_Data;
  sum = 0L;

  for (i = 0; i < B_lod_l; i++)
    {

      sum += getc (B_file) & 0x000000FFL;

      if (ferror (B_file))
	{

	  printf ("check:  Unable to read \042%s\042\n", fn);
	  fclose (B_file);
	  return (4);
	}
    }

  fclose (B_file);

  B_chk = sum;
  B_txt_l = B_fhdr.F_Text;
  B_dat_l = B_fhdr.F_Data;
  B_bss_l = B_fhdr.F_BSS;
  B_end = B_txt_o + B_lod_l - 1L;

  if (B_bss_o)
    bgnbss = B_bss_o;
  else
    bgnbss = B_end + 1L;

  endbss = bgnbss + B_bss_l - 1L;

  printf ("File \042%s\042 is a %s file and\n",
	  fn, (B_fhdr.F_Magic == F_R_D) ? "Scattered" : "Contiguous");

  printf ("loads from $%08lx to $%08lx  (%ld bytes)\n",
	  B_txt_o, B_end, B_lod_l);

  printf ("  with BSS $%08lx to $%08lx  (%ld bytes)\n\n",
	  bgnbss, endbss, B_bss_l);

  printf ("   B_txt_o = $%08lx, B_dat_o = $%08lx, B_bss_o = $%08lx\n",
	  B_txt_o, B_dat_o, B_bss_o);

  printf ("   B_txt_l = $%08lx, B_dat_l = $%08lx, B_bss_l = $%08lx\n\n",
	  B_txt_l, B_dat_l, B_bss_l);

  total_l = B_txt_l + B_dat_l + B_bss_l;

  printf ("Checksum   = $%08lx,  Total program length = %ld ($%08lx) bytes\n",
	  B_chk, total_l, total_l);

  return (0);
}

/* 
*/

main (argc, argv)
     int argc;
     char *argv[];
{
  int rc;

  if (argc != 2)
    {

      printf ("check:  ERROR - file name required\n");
      exit (1);
    }

  if (rc = check (argv[1]))
    {

      printf ("ERROR:  check(\042%s\042) returned %d.\n", argv[1], rc);
      exit (1);

    }

  exit (0);
}
