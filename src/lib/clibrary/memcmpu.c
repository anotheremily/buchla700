/*
   ============================================================================
	memcmpu -- compare two memory areas while ignoring case
	Version 4 -- 1987-06-15 -- D.N. Lynx Crowe
   ============================================================================
*/

#include "ctype.h"

int
memcmpu(s1, s2, n)
register char *s1, *s2;
int n;
{
	register char c1, c2;

	while (n) {

		c1 = *s1++;
		c2 = *s2++;

		if (isascii(c1) && islower(c1))
			c1 = _toupper(c1);

		if (isascii(c2) && islower(c2))
			c2 = _toupper(c2);

		if (c1 < c2)
			return(-1);

		if (c1 > c2)
			return(1);

		n--;
	}

	return(0);
}
