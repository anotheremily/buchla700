/*
   =============================================================================
	fprintf.c -- fprintf function
	Version 2 -- 1987-06-26 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"
#include "varargs.h"

static FILE *Stream;

extern	int	_dofmt();
extern	int	aputc();

int
fprintf(stream, fmt, va_alist)
FILE *stream;
char *fmt;
va_dcl
{
	register int count;
	int fpsub();
	va_list aptr;

	Stream = stream;
	va_start(aptr);
	count = _dofmt(fpsub, fmt, aptr);
	va_end(aptr);
	return(count);
}

static
int
fpsub(c)
{
	return(aputc(c, Stream));
}
