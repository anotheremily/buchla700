/*
   =============================================================================
	vobjfns.c -- VSDD object functions
	Version 23 -- 1988-01-29 -- D.N. Lynx Crowe
	(c) Copyright 1987,1988 -- D.N. Lynx Crowe

	SelObj(obj)
	int obj;

		Select 'obj' as the current working object.

	SetPri(obj, pri)
	int obj, pri;

		Display object 'obj' with priority 'pri'.

	SetObj(obj, type, bank, base, xpix, ypix, x0, y0, flags, pri)
	int obj, type, bank, xpix, ypix, x0, y0, flags, pri;
	unsigned int *base;

		Setup object 'obj' of type 'type' at 'base' in bank 'bank'
		with height 'ypix' and width 'xpix' at initial location
		'x0','y0', with flags 'flags'.  Assumes HRS EQ 1.
		If 'pri' >= 0, display the object at priority level 'pri'.
		Define a bitmap object if 'type' EQ 0, or a character object
		if 'type' NE 0.

	CpyObj(from, to, w, h, sw)
	unsigned int *from, *to;
	int w, h, sw;

		Copy a 'w'-word by 'h'-line object from 'from' to 'to' with
		a destination width of 'sw' words.
*/

/* 
*/

/*
	VIint() is located in viint.s but is mentioned here for completeness

	VIint()

		Vertical Interrupt handler.  Enables display of any object
		whose bit is set in vi_ctl.  Bit 0 = object 0, etc.

		SetPri() uses BIOS(B_SETV, 25, VIint) to set the interrupt
		vector and lets VIint() enable the object.  If vi_dis
		is set, SetPri() won't enable the interrupt or set the vector
		so that several objects can be started up at once.
   =============================================================================
*/

#include "biosdefs.h"
#include "graphdef.h"
#include "hwdefs.h"
#include "stddefs.h"
#include "vsdd.h"
#include "vsddvars.h"

extern	int	vbank(), objon(), VIint();

short		wsize;		/* object width calculated by SetObj() */
short		vi_dis;		/* disable use of VIint */

unsigned	vi_ctl;		/* object unblank control bits */

/* 
*/

/*
   =============================================================================
	SelObj(obj) -- Select 'obj' as the current working object.
   =============================================================================
*/

SelObj(obj)
int obj;
{
	register struct octent *op;
	register unsigned newbank;

	op = &v_obtab[obj];

	newbank = ((op->obank & 0x0001) << 8) | ((op->obank & 0x0002) << 6);

	v_nobj = obj;
	v_curob = op;
	v_obpri = op->opri;

	if ((v_regs[5] & 0x0180) NE newbank)
		vbank(op->obank & 3);
}

/* 
*/

/*
   =============================================================================
	SetPri(obj, pri) -- Display object 'obj' with priority 'pri'.
	
		Blanks the object first, then sets the access table
		and unblanks the object via VIint().
   =============================================================================
*/

SetPri(obj, pri)
register int obj;
register int pri;
{
	register struct octent *op;

	if (v_regs[5] & 0x0180)		/* point at the control bank */
		vbank(0);

	op = &v_obtab[obj];		/* point at the object table */
	op->opri = pri;			/* set the priority */

	v_odtab[pri][0] = op->odtw0 | V_BLA;	/* start object as blanked */
	v_odtab[pri][1] = op->odtw1;
	v_odtab[pri][2] = ((char *)(op->obase) >> 1) & 0xFFFF;

	objon(pri, op->objy, op->ysize);	/* enable access table bits */

	if (vi_dis)				/* don't unblank if vi_dis set */
		return;

	setipl(7);				/* disable interrupts */

	vi_ctl |= (1 << pri);			/* set unblank bit */

	if (*((long *)0x000064) NE &VIint)	/* make sure VI vector is set */
		BIOS(B_SETV, 25, VIint);

	setipl(0);				/* enable VI interrupt */
}

/* 
*/

/*
   =============================================================================
	SetObj(obj, type, bank, base, xpix, ypix, x0, y0, flags, pri)
	Setup an object, and optionally display it.  Assumes HRS EQ 1.
   =============================================================================
*/

SetObj(obj, type, bank, base, xpix, ypix, x0, y0, flags, pri)
int obj, type, bank, xpix, ypix, x0, y0, flags, pri;
unsigned int *base;
{
	register struct octent *op;

	if (v_regs[5] & 0x0180)		/* point at the control bank */
		vbank(0);

	op = &v_obtab[obj];

	v_curob = op;
	v_nobj = obj;
	v_obpri = pri;

	op->ysize = ypix;
	op->xsize = xpix;
	op->objx = x0;
	op->objy = y0;
	op->obase = base;
	op->opri = pri;
	op->obank = bank & 3;

/* 
*/
	if (type) {	/* character objects */

		op->odtw0 = (flags & 0xF9FF) | V_CTYPE;

		switch (V_RES3 & op->odtw0) {

		case V_RES0:

			wsize = xpix / 128;
			break;

		case V_RES1:

			wsize = xpix / 48;
			break;

		case V_RES2:

			wsize = xpix / 64;
			break;

		case V_RES3:

			wsize = xpix / 96;
			break;
		}

		if (V_FAD & op->odtw0)
			wsize = wsize + (wsize << 1);

/* 
*/
	} else {	/* bitmap objects */

		op->odtw0 = (flags & 0x0E37) | (V_BTYPE | ((bank & 3) << 6));

		switch (V_RES3 & op->odtw0) {

		case V_RES0:
		case V_RES1:

			wsize = 0;
			break;

		case V_RES2:

			wsize = xpix / 32;
			break;

		case V_RES3:

			wsize = xpix / 16;
			break;
		}
	}

	op->odtw1 = ((x0 >> 1) & 0x03FF) | (0xFC00 & (wsize << 10));

	if (pri < 0)
		return;

	SetPri(obj, pri);
}

/* 
*/

/*
   =============================================================================
	CpyObj(from, to, w, h, sw)

	Copy a 'w'-word by 'h'-line object from 'from' to 'to' with
	a destination width of 'sw' words.  Assumes we're pointing at
	the correct bank.
   =============================================================================
*/

CpyObj(from, to, w, h, sw)
register unsigned *from, *to;
register unsigned w, h, sw;
{
	register unsigned *tp;
	register unsigned i, j;

	for (i = h; i--; ) {

		tp = to;

		for (j = w; j--; )
			*tp++ = *from++;

		to += sw;
	}
}
