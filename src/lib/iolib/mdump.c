/*
   =============================================================================
	mdump.c -- Hexadecimal memory dump
	Version 5 -- 1987-10-29 -- D.N. Lynx Crowe

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

#include "stddefs.h"
#include "stdio.h"
#include "ctype.h"

#define PERLINE 16

/* 
*/

/*
   =============================================================================
	pipc() -- print if printable characters
   =============================================================================
*/

static
pipc(chars, length)
char chars[];
int length;
{
	int	i;

	for (i = 0; i < length; i++)
		if (isascii(0x00FF & chars[i]) AND (isprint(0x00FF & chars[i])))
			printf("%c", chars[i]);
		else
			printf(".");
}

/* 
*/

/*
   =============================================================================
	mdump() -- dump a memory area in hexadecimal
   =============================================================================
*/

mdump(begin, end, start)
char *begin, *end;
long start;
{
	long	i, ii;
	int	j, jj, k;
	char	c, chars[PERLINE];

	i = 0L;
	ii = start;
	j = 0;

	if (begin GT end)
		return;

	while (begin LE end) {

		c = *begin++;

		if (! (i % PERLINE)) {

			if (i) {

				j=0;
				printf("  ");
				pipc(chars, PERLINE);
			}
                
			printf("\n%08lX:", ii);
		}

		ii++;
		i++;

		printf(" %02.2X", (c & 0x00FF));
		chars[j++] = c;
	}

	if (k = (i % PERLINE)) {

		k = PERLINE - k;

		for (jj = 0; jj < (3 * k); ++jj)
			printf(" ");
	}

	printf("  ");
	pipc(chars, PERLINE);
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
