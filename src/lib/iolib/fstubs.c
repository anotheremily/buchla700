/*
   =============================================================================
	fstubs.c -- stubs for testing with
	Version 13 -- 1987-11-13 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "biosdefs.h"
#include "errno.h"
#include "stdio.h"
#include "io.h"
#include "stddefs.h"

/*
   =============================================================================
	readbuf() -- Reads an edited string from 'dev' into 'buf'
	in {MS|GEM}DOS buffer format.

	Buffer format:

		buf[0] = buffer length, buf[1] = number of bytes read,
		buf[2]..buf[buf[0]-1 = data area

	Editing characters:

		CR, LF	End the line
		BS, DEL	Erase last character
		^U,^X	Cancel (erase) entire line
		^R	Retype line
		^C	Terminate the process
		^Z	End of file / end of line

	Returns:
	      Value     stddef	Meaning
	      -----     ------- --------------------------------------------
		1		Terminate process (^C entered)
		0	SUCCESS	Line available in buffer  (CR or LF entered)
	       -1	EOF	End of file (^Z entered)
   =============================================================================
*/

int
readbuf(dev, buf)
int dev;
char *buf;
{
	xtrap15();
	return(1);
}
