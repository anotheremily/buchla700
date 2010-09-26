/*
   =============================================================================
	mdump.c -- Hexadecimal memory dump
	Version 6 -- 1989-02-02 -- D.N. Lynx Crowe

	Displays the contents of memory in hexadecimal.
	If a byte is printable, it is also printed.

	The format of the output is :

	   hex_add byte byte byte byte ... byte byte byte byte  string  

	where:

		hex_add   Start address for that line in hex. 
		byte      Representation of a byte in hex. Two characters
				for each byte.  PERLINE bytes per line.
		string    If the character is printable, it is printed,
				otherwise a '.' is printed.
   =============================================================================
*/

#define	TESTER		0

#include "stdio.h"
#include "ctype.h"
#include "stddefs.h"

#define PERLINE 16

/* 
*/

/*
   =============================================================================
	prifp() -- print printable characters from a buffer
   =============================================================================
*/

static
prifp(chars, length)
register char *chars;
register int length;
{
	register int	i, c;

	for (i = 0; i < length; i++) {

		c = 0x00FF & *chars++;

		if (isascii(c) AND (isprint(c)))
			printf("%c", c);
		else
			printf(".");
	}
}

/* 
*/

/*
   =============================================================================
	mdump() -- dump a memory area in hexadecimal
   =============================================================================
*/

mdump(begin, end, start)
register char *begin, *end;
long start;
{
	register long	i, ii;
	int	j, k;
	char	c, chars[PERLINE];

	if (begin > end)
		return;

	i  = 0L;
	ii = start;
	j  = 0;

	while (begin LE end) {

		c = *begin++;

		if (NOT (i % PERLINE)) {

			if (i) {

				j=0;
				printf("  ");
				prifp(chars, PERLINE);
			}
                
			printf("\n%08lX:", ii);
		}

		ii++;
		i++;

		printf(" %02.2X", (c & 0x00FF));
		chars[j++] = c;
	}

/* 
*/
	if (k = (i % PERLINE)) {

		k = PERLINE - k;

		for (ii = 0; ii < (3 * k); ++ii)
			printf(" ");
	}

	printf("  ");
	prifp(chars, PERLINE);
	printf("\n");
}

/* 
*/

#if	TESTER

char	area[128];

main()
{
	register short i;

	for (i = 0; i < 128; i++)
		area[i] = i + 128;

	mdump(area, (area + 127), area);
}

#endif
