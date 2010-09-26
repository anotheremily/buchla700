/*
   =============================================================================
	pardump.c -- Dump GEMDOS partition information
	Version 1 -- 1988-09-23 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "osbind.h"
#include "stdio.h"
#include "bootsec.h"

#define	MAXSECLN	512

struct ParInfo {		/* partition information structure */

	char	p_flg;		/* partition presence flag */
	char	p_id[3];	/* partition ID */
	long	p_st;		/* partition starting sector */
	long	p_siz;		/* partition size */
};

short	secbuf[MAXSECLN];

/* 
*/

/*
   =============================================================================
	main() -- dump partition data for the Atari hard disk
   =============================================================================
*/

main(argc, argv)
int argc;
char *argv[];
{
	register struct BootSec *bsp;
	register struct ParInfo *pip;
	register short i;
	register short *wp;
	register long *lp;

	if (Rwabs(2, secbuf, 1, 0, 2)) {

		printf("ERROR -- Sector 0 of Drive C is unreadable\n");
		exit(1);
	}

	bsp = (struct BootSec *)secbuf;

	lp  = &bsp->boot[422];

	pip = &bsp->boot[426];

	printf("Hard Disk Partition Information\n\n");

	printf("Hard disk size:  %ld sectors\n\n", *lp);

	for (i = 0; i < 4; i++) {

		if (pip->p_flg) {

			printf("Partition %d:  \"%3.3s\" is at %ld and has %ld sectors\n",
				(i + 1), pip->p_id, pip->p_st, pip->p_siz);
		} else {

			printf("Partition %d:  Inactive partition\n", (i + 1));
		}

		++pip;
	}

	printf("\n");

/* 
*/

	lp = &bsp->boot[474];
	wp = &bsp->boot[478];

	if (*lp) {

		printf("Bad Sector Map is at %ld,  %d entries\n",
			*lp, *wp);

	} else {

		printf("Bad Sector Map not present\n");
	}

	exit(0);
}
