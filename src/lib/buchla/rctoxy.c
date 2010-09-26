/*
   =============================================================================
	rctoxy.c -- print a row,column to x,y chart
	Version 1 -- 1988-02-01 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "stdio.h"

main()
{
	register short rc, xl, xr, yt, yb;

	printf("rc   xl ..  xr    yt ..  yb\n\n");

	for (rc = 0; rc < 25; rc++) {

		xl = rc << 3;
		xr = xl + 7;

		yt = rc * 14;
		yb = yt + 13;

		printf("%2d  %3d .. %3d   %3d .. %3d\n", rc, xl, xr, yt, yb);
	}

	for (; rc < 64; rc++) {

		xl = rc << 3;
		xr = xl + 7;

		printf("%2d  %3d .. %3d\n", rc, xl, xr);
	}
}
