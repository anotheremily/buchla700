/*
   =============================================================================
	pause.c -- output a message and wait for a CR
	Version 3 -- 1987-07-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "biosdefs.h"

extern	int	waitcr(), writeln();

pause(s)
char *s;
{
	writeln(CON_DEV, s);
	writeln(CON_DEV, "\r\n");
	waitcr();
}
