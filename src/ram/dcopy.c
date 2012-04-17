/*
   =============================================================================
	dcopy.c -- copy MIDAS-VII to disk
	Version 3 -- 1988-09-01 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "stdio.h"
#include "objdefs.h"

#define	PROGID	"midas.abs"	/* MIDAS-VII program file name */

extern short defect;		/* error code */

extern char end, edata, etext;	/* loader symbols */

extern long p_dlen;		/* size of data (from basepage) */

extern Lo_RAM ();		/* lowest address loaded (lowram.s) */

extern struct EXFILE mphead;	/* MIDAS-VII program header */

/* 
*/

/*
   =============================================================================
	dcopy() -- copy MIDAS-VII to disk
   =============================================================================
*/

short
dcopy ()
{
  register FILE *fp;
  register char *from;
  register long wrtlen, loadlen, bsslen, txtlen;

  defect = 0;			/* reset error word */

  txtlen = (long) &etext - (long) &Lo_RAM;	/* calculate text length */
  bsslen = (long) &end - (long) &edata;	/* calculate BSS length */

  loadlen = (long) &edata - (long) &Lo_RAM;	/* calculate write length */

  /* create the object file header */

  mphead.F_Magic = F_R_C;	/* magic = contiguous file */
  mphead.F_Text = txtlen;	/* text length */
  mphead.F_Data = p_dlen;	/* data length */
  mphead.F_BSS = bsslen;	/* BSS length */
  mphead.F_Symtab = 0L;		/* symbol table length */
  mphead.F_Res1 = 0L;		/* reserved area #1 */
  mphead.F_Res2 = &Lo_RAM;	/* text base */
  mphead.F_Res3 = 0xFFFF;	/* flag word */

  /* ***** initialize for a (possibly) new disk here ***** */

  /* open MIDAS-VII object file for writing */

  preio ();			/* kill the LCD backlight */

  if ((FILE *) NULL == (fp = fopenb (PROGID, "w")))
    {

      defect = 1;		/* couldn't open file */
      postio ();		/* restore LCD backlight */
      return (FAILURE);
    }

/* 
*/
  /* write program header to disk */

  from = &mphead;

  for (wrtlen = sizeof mphead; wrtlen--;)
    if (EOF == putc (*from++, fp))
      {

	defect = 2;		/* couldn't write program header */
	fclose (fp);
	postio ();		/* restore LCD backlight */
	return (FAILURE);
      }

  /* write MIDAS-VII to disk */

  from = &Lo_RAM;

  for (wrtlen = loadlen; wrtlen--;)
    if (EOF == putc (*from++, fp))
      {

	defect = 3;		/* couldn't write program */
	fclose (fp);
	postio ();		/* restore LCD backlight */
	return (FAILURE);
      }

  /* flush and close file */

  fflush (fp);
  fclose (fp);
  postio ();			/* restore LCD backlight */
  return (SUCCESS);
}
