/*
   =============================================================================
	infield.c -- field input functions
	Version 6 -- 1988-10-20 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "fields.h"
#include "stddefs.h"

extern short stcrow, stccol;

/*
   =============================================================================
	infield() -- Determines whether or not a given position is in a field.
	Returns TRUE if in a field, FALSE if not.  If in a field, sets 'infetp'
	to point at the fet entry.  If not in a field, sets 'infetp' to NULL.
   =============================================================================
*/

short
infield (row, col, fetp)
     register short row, col;
     register struct fet *fetp;
{
  infetp = (struct fet *) NULL;	/* setup for NULL infetp return */

  if ((struct fet *) NULL EQ fetp)	/* handle NULL fet pointer */
    return (FALSE);

  while (fetp->redisp)
    {				/* redisp EQ NULL is end of table */

      if ((row EQ fetp->frow) AND	/* check the entry */
	  (col GE fetp->flcol) AND (col LE fetp->frcol))
	{

	  infetp = fetp;	/* set new fet pointer */
	  return (TRUE);	/* return 'found' */
	}

      ++fetp;			/* advance field pointer */
    }

  return (FALSE);		/* return 'not found' */
}
