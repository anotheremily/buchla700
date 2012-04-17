/*
   ============================================================================
	booter.c -- load an absolute format Alcyon object file
	Version 18 -- 1987-11-06 -- D.N. Lynx Crowe
   ============================================================================
*/

#define	PRINTIT	1		/* define non-zero to get printf output from booter */

#include <stdio.h>
#include "io.h"
#include "stddefs.h"
#include "portab.h"
#include "objdefs.h"
#include "biosdefs.h"

extern FILE *fopenb ();

extern int fclose (), fread (), flread ();

extern long getl ();


#if	PRINTIT
extern struct fcb *SnapFCB ();

extern int ClusMap ();

#endif /* 
        */

static FILE *B_file;		/* boot file pointer */


struct EXFILE B_fhdr;		/* executable file header */


long B_txt_o,			/* test origin from file header */
  B_dat_o,			/* data origin from file header */
  B_bss_o,			/* bss origin from file header */
  B_txt_l,			/* text length from file header */
  B_dat_l,			/* data length from file header */
  B_bss_l,			/* bss length from file header */
  B_lod_l,			/* total data length loaded */
  B_end,			/* end address */
  B_chk;			/* checksum */


char *B_buf_a;			/* boot load address */


short B_log_s;			/* boot log switch */

short B_dbg_s;			/* boot debug switch */


/*  */

/*
   ============================================================================
	booter(fn, textadr) -- load file named by string 'fn' at 'textadr'.
	If 'textadr' is 0, the text origin from the file will be used.
	Returns 0 if load was OK, non-zero error code otherwise.
   ============================================================================
*/

short
booter (fn, textadr)
     char *fn;

     long textadr;

{

  register long i, bgnbss, endbss;

  register char *cp;

#if	PRINTIT
  register struct fcb *fcp;

#endif /* 
        */

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


#if	PRINTIT
      if (B_log_s)

	printf ("booter:  Unable to open \042%s\042\n", fn);

#endif /* 
        */
      return (1);

    }


#if	PRINTIT
  if (B_dbg_s)
    {				/* if we're debugging, print the FCB stuff */


      fcp = (struct fcb *) (chantab[B_file->_unit].c_arg);


      SnapFCB (fcp);

      ClusMap (fcp);

      waitcr ();

    }

#endif /* 
        */

  /* read in the file header */

  if (1 != fread (&B_fhdr, sizeof B_fhdr, 1, B_file))
    {


#if	PRINTIT
      if (B_log_s)

	printf ("booter:  Unable to read header for \042%s\042\n", fn);

#endif /* 
        */
      fclose (B_file);

      return (2);

    }


  /* check the magic */

  if ((B_fhdr.F_Magic != F_R_C) && (B_fhdr.F_Magic != F_R_D))
    {


#if	PRINTIT
      if (B_log_s)

	printf ("booter:  Bad magic [0x%04x] in file \042%s\042",
		B_fhdr.F_Magic, fn);

#endif /* 
        */
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


  B_buf_a = textadr ? textadr : B_txt_o;

  B_lod_l = B_fhdr.F_Text + B_fhdr.F_Data;


  if (0 != flread (B_buf_a, B_lod_l, B_file))
    {


#if	PRINTIT
      if (B_log_s)

	printf ("booter:  Unable to read \042%s\042\n", fn);

#endif /* 
        */
      fclose (B_file);

      return (4);

    }


  B_end = B_buf_a + B_lod_l - 1L;


  B_txt_l = B_fhdr.F_Text;

  B_dat_l = B_fhdr.F_Data;

  B_bss_l = B_fhdr.F_BSS;


  cp = B_buf_a;			/* calculate checksum */

  B_chk = 0L;


  for (i = 0; i < B_lod_l; i++)

    B_chk += *cp++ & 0x000000FFL;


  if (B_bss_o)

    bgnbss = B_bss_o;

  else

    bgnbss = B_end + 1L;


  endbss = bgnbss + B_bss_l - 1L;


#if	PRINTIT
  if (B_log_s)
    {


      printf ("File \042%s\042 loaded from $%08lX to $%08lX\r\n",
	      fn, B_buf_a, B_end);

      printf ("   BSS $%08lX to $%08lX\r\n", bgnbss, endbss);

      printf ("Checksum   = $%08lX, Load length = %ld ($%08lX)\r\n",
	      B_chk, B_lod_l, B_lod_l);

      printf
	("   B_txt_o = $%08lX, B_dat_o = $%08lX, B_bss_o = $%08lX\r\n",
	 B_txt_o, B_dat_o, B_bss_o);

      printf
	("   B_txt_l = $%08lX, B_dat_l = $%08lX, B_bss_l = $%08lX\r\n",
	 B_txt_l, B_dat_l, B_bss_l);

    }

#endif /* 
        */

  fclose (B_file);

  return (0);

}
