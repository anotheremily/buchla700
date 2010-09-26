/*
   ============================================================================
	conwr.c -- write on the console
	Version 4 -- 1987-06-29 -- D.N. Lynx Crowe
   ============================================================================
*/

#define	_FS_DEF_		/* to avoid unnecessary externals */

#include "biosdefs.h"
#include "io.h"
#include "errno.h"
#include "stddefs.h"

/*
   ============================================================================
	_conwr(kind, buff, len) -- write 'len' bytes from 'buff' on the console
	using op 'kind' as the BIOS argument.
   ============================================================================
*/
int
_conwr(kind, buff, len)
int kind;
register char *buff;
int len;
{
	register int count;

	for (count = 0; count < len; ++count)
		BIOS(B_PUTC, kind, *buff++);

	return(count);
}
