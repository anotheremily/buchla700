/*
   =============================================================================
	rscript.c -- define the score test script for scordsp.c
	Version 13 -- 1988-11-03 -- D.N. Lynx Crowe
   =============================================================================
*/

extern	int	sqscan();

extern	int	endflg;

char	*script0[] = {		/* initialization score selection string */

	"!quiet !normal !score=0 !weight=60 !end",
	(char *)0L
};

/*
   =============================================================================
	rscript() -- run a script and return the end condition

		-1	error returned by sqscan
		 0	end of strings, no errors
		 1	!end statement encountered
   =============================================================================
*/

short
rscript(p)
register char *p[];
{
	short rc;

	sqinit();		/* initialize the score string interpreter */
	rc = 0;			/* preset rc for end of script return */

	while (*p) {		/* feed the interpreter the score strings */

		if (!sqscan(*p++)) {	/* ... until we have an error ... */

			rc = -1;		/* error return */
			break;
		}

		if (endflg) {		/* ... or until we hit the !end */

			rc = 1;			/* !end return */
			break;
		}
	}

	return(rc);
}
