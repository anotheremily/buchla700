/*
   =============================================================================
	sprintf.c -- sprintf function
	Version 2 -- 1987-06-11 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "varargs.h"

extern long	dofmt_();

static char	*buff;

/*
   =============================================================================
	sprintf(str, fmt, args) -- format args into str according to fmt
   =============================================================================
*/

long
sprintf(str, fmt, va_alist)
char *str, *fmt;
va_dcl
{
	int spsub();

	register long count;
	va_list aptr;

	va_start(aptr);
	buff = str;
	count = dofmt_(spsub, fmt, aptr);
	*buff = '\0';
	return(count);
}

/*
   =============================================================================
	spsub(c) - put c into the output string
   =============================================================================
*/

static
int
spsub(c)
{
	return((*buff++ = c) & 0xFF);
}
