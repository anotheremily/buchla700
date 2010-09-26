/*
   =============================================================================
	curpak.h -- header for cursor and data entry functions
	Version 5 -- 1988-10-12 -- D.N. Lynx Crowe
   =============================================================================

*/

/* cursor types */

#define	CT_GRAF		0	/* graphics */
#define	CT_TEXT		1	/* text -- general */
#define	CT_VIRT		2	/* virtual -- graphic */
#define	CT_SCOR		3	/* text -- score */
#define	CT_SMTH		4	/* text -- smooth scroll */
#define	CT_MENU		5	/* virtual -- character */

/* cursor and data entry function parameter structure */

struct curpak {

	short		(*curtype)();
	short		(*premove)();
	short		(*pstmove)();
	short		(*cx_key)();
	short		(*cy_key)();
	short		(*cx_upd)();
	short		(*cy_upd)();
	short		(*xy_up)();
	short		(*xy_dn)();
	short		(*x_key)();
	short		(*e_key)();
	short		(*m_key)();
	short		(*d_key)();
	short		(*not_fld)();
	struct fet	*curfet;
	struct selbox	*csbp;
	short		*cratex;
	short		*cratey;
	short		cmtype;
	short		cxval;
	short		cyval;
};
