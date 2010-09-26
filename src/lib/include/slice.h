/*
   =============================================================================
	slice.h -- slice data structure
	Version 4 -- 1988-09-23 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	MAXFSL	256			/* size of the gdsel free list */
#define	NGDSEL	17			/* number of gdstb elements */

#define	I_SBASE		0		/* initial sbase value */
#define	I_OFFSET	0		/* initial soffset value */

struct	gdsel	{

	struct	gdsel	*next;		/* pointer to next element */
	short	note;			/* note offset 0..87 */
	short	code;			/* update code */
};

/*
   =============================================================================
	WARNING:  the 'next' pointer in the gdsel structure MUST be the first
	long word of the structure, as it is assumed to be there by the element
	deletion code in uslice.  Moving it would be a real disaster, so don't.
   =============================================================================
*/
