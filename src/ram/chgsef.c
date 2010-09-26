/*
   =============================================================================
	chgsef.c -- MIDAS-VII -- change score direction
	Version 1 -- 1988-08-01 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "graphdef.h"
#include "hwdefs.h"
#include "sclock.h"
#include "slice.h"
#include "vsdd.h"

#include "midas.h"
#include "score.h"
#include "scdsp.h"
#include "scfns.h"

extern	short	sd;
extern	short	se;

extern	struct	gdsel	*gdstbc[];
extern	struct	gdsel	*gdstbn[];
extern	struct	gdsel	*gdstbp[];

/* 
*/

/*
   =============================================================================
	chgsef() -- change execution pointers to D_FWD
   =============================================================================
*/

chgsef()
{
	register struct s_entry *ep;
	register long rt;

	se = D_FWD;

	ep = p_cur;
	rt = t_cur;

	if (EV_FINI NE ep->e_type) {

		ep = ep->e_fwd;

		while (ep->e_time EQ rt) {

			if (EV_FINI EQ ep->e_type)
				break;

			se_exec(ep, se);
			ep = ep->e_fwd;
		}

		p_cur = ep;
	}
}

/*
   =============================================================================
	chgseb() -- change execution pointers to D_BAK
   =============================================================================
*/

chgseb()
{
	register struct s_entry *ep;
	register long rt;

	se = D_BAK;

	ep = p_cur;
	rt = t_cur;

	if (EV_SCORE NE ep->e_type) {

		ep = ep->e_bak;

		while (ep->e_time EQ rt) {

			if (EV_SCORE EQ ep->e_type)
				break;

			se_exec(ep, se);
			ep = ep->e_bak;
		}

		p_cur = ep;
	}
}

/*
   =============================================================================
	chgsdf() -- change display pointers to D_FWD
   =============================================================================
*/

chgsdf()
{
	register struct s_entry *ep;
	register long rt;

	sd = D_FWD;

	rslice(gdstbc);
	rslice(gdstbn);
	rslice(gdstbp);

	ep = p_ctr;
	rt = t_ctr;

	if (EV_FINI NE ep->e_type) {

		ep = ep->e_fwd;

		while (ep->e_time EQ rt) {

			if (EV_FINI EQ ep->e_type)
				break;

			se_disp(ep, D_FWD, gdstbc, 1);
			ep = ep->e_fwd;
		}

		p_ctr = ep;
	}

	ep = p_bak;
	rt = t_bak;

	if (EV_FINI NE ep->e_type) {

		ep = ep->e_fwd;

		while (ep->e_time EQ rt) {

			if (EV_FINI EQ ep->e_type)
				break;

			se_disp(ep, D_FWD, gdstbp, 0);
			ep = ep->e_fwd;
		}

		p_bak = ep;
	}

	ep = p_fwd;
	rt = t_fwd;

	if (EV_FINI NE ep->e_type) {

		ep = ep->e_fwd;

		while (ep->e_time EQ rt) {

			if (EV_FINI EQ ep->e_type)
				break;

			se_disp(ep, D_FWD, gdstbn, 0);
			ep = ep->e_fwd;
		}

		p_fwd = ep;
	}
}

/*
   =============================================================================
	chgsdb() -- change display pointers to D_BAK
   =============================================================================
*/

chgsdb()
{
	register struct s_entry *ep;
	register long rt;

	sd = D_BAK;

	rslice(gdstbc);
	rslice(gdstbn);
	rslice(gdstbp);

	ep = p_ctr;
	rt = t_ctr;

	if (EV_SCORE NE ep->e_type) {

		ep = ep->e_bak;

		while (ep->e_time EQ rt) {

			if (EV_SCORE EQ ep->e_type)
				break;

			se_disp(ep, D_BAK, gdstbc, 1);
			ep = ep->e_bak;
		}

		p_ctr = ep;
	}

	ep = p_bak;
	rt = t_bak;

	if (EV_SCORE NE ep->e_type) {

		ep = ep->e_bak;

		while (ep->e_time EQ rt) {

			if (EV_SCORE EQ ep->e_type)
				break;

			se_disp(ep, D_BAK, gdstbp, 0);
			ep = ep->e_bak;
		}

		p_bak = ep;
	}

	ep = p_fwd;
	rt = t_fwd;

	if (EV_SCORE NE ep->e_type) {

		ep = ep->e_bak;

		while (ep->e_time EQ rt) {

			if (EV_SCORE EQ ep->e_type)
				break;

			se_disp(ep, D_BAK, gdstbn, 0);
			ep = ep->e_bak;
		}

		p_fwd = ep;
	}
}
