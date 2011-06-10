
/*
   =============================================================================
	vctrl.c -- VSDD control functions
	Version 7 -- 1988-08-15 -- D.N. Lynx Crowe
	(c) Copyright 1987,1988 -- D.N. Lynx Crowe

	unsigned int
	vbank(b)
	unsigned int b;

		Set VSDD Data Segment bank to b.
		Return old bank select value.


	vfwait()

		Wait for a FRAMESTOP update to transpire.
   =============================================================================
*/

#include "stddefs.h"
#include "hwdefs.h"
#include "vsdd.h"

/* 
*/

/*
   =============================================================================
	vbank(b) -- change VSDD Data Segment bank to b.  Return old bank.
	Assumes a 128K byte window, sets S15..S11 to zero.
   =============================================================================
*/

unsigned int
vbank (b)
     unsigned int b;
{
  register unsigned int newb, oldb;

  oldb = v_regs[5] >> 6;	/* get old bank value */
  oldb = (oldb & 2) | ((oldb >> 2) & 1);

  if (oldb EQ b)		/* done if same as b */
    return (b);

  newb = b << 6;		/* calculate new bank value */
  newb = (newb & 0x0080) | ((newb << 2) & 0x0100);

  v_regs[5] = newb;		/* set new bank value */

  while (v_regs[11] GT 300);	/* wait for FRAMESTOP */
  while (v_regs[11] LT 175);
  while (v_regs[11] GE 175);

  return (oldb);		/* return old bank value */
}

/* 
*/

/*
   =============================================================================
	vfwait() -- Wait for a FRAMESTOP update to transpire.
   =============================================================================
*/

vfwait ()
{
  while (v_regs[11] GT 300);	/* wait for FRAMESTOP */
  while (v_regs[11] LT 175);
  while (v_regs[11] GE 175);
}
