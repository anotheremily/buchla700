/*
   =============================================================================
	unlink.c -- unlink (delete) a file
	Version 6 -- 1987-11-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	DEBUGIT		0

#include "biosdefs.h"
#include "stddefs.h"

#if	DEBUGIT
extern	short	fsdebug;

extern	int	waitcr();
#endif

extern	int	DelFile();

/*
   =============================================================================
	unlink(name) -- delete file 'name'.
   =============================================================================
*/

int
unlink(name)
char *name;
{
	struct fcb delfcb;

	if (fcbinit(name, &delfcb))
		return(FAILURE);

#if	DEBUGIT
	if (fsdebug) {

		printf("unlink(%s):  deletion FCB created\n", name);
		SnapFCB(&delfcb);
		waitcr();
	}
#endif
	return(DelFile(&delfcb));
}

