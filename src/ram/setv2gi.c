/*
   =============================================================================
	setv2gi.c -- MIDAS-VII -- set the voices to the instrument for a group
	Version 2 -- 1988-07-11 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "vsdd.h"
#include "vsddsw.h"
#include "graphdef.h"

#include "midas.h"
#include "instdsp.h"

extern short curinst;
extern short curvce;

extern short ins2grp[];
extern short s_inst[];
extern short vce2grp[];

extern struct instdef vbufs[];

/* 
*/

/*
   =============================================================================
	setv2gi() -- set voices to the instrument for a group
   =============================================================================
*/

setv2gi (group)
     short group;
{
  register short grp, vce, ins;
  register struct instdef *ip;
  register struct idfnhdr *fp;

  ins = ins2grp[group] & 0x00FF;
  grp = group + 1;

  for (vce = 0; vce < 12; vce++)
    {

      if (vce2grp[vce] == grp)
	{			/* for each voice in the group */

	  if (curvce == vce)
	    {

	      curinst = ins;
	      s_inst[curvce] = ins;
	    }

	  execins (vce, ins, 1);
	}
    }
}
