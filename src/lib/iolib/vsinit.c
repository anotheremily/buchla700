/*
   =============================================================================
	vsinit.c -- intialize VSDD software support variables
	Version 5 -- 1988-08-15 -- D.N. Lynx Crowe
	(c) Copyright 1987,1988 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "hwdefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "vsddvars.h"
#include "memory.h"

/*
   =============================================================================
	VSinit() -- initialize VSDD software support variables
   =============================================================================
*/

VSinit ()
{
  v_nobj = 0;
  v_obpri = 0;
  memsetw (v_obtab, 0, (sizeof v_obtab) / 2);
  v_curob = &v_obtab[0];
}
