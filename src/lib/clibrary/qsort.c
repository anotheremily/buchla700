/*
   =============================================================================
	qsort.c -- quicksort algorithm
	Version 2 -- 1989-01-12  -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"

static	unsigned	qscnt;		/* size of an element */
static	int		(*qscmp)();	/* comparison function */

/*
   =============================================================================
	qsexch() -- exchange two elements
   =============================================================================
*/

static
qsexch(ip, jp)
register char *ip, *jp;
{
	register unsigned n;
	register char c;

	n = qscnt;

	do {
		c = *ip;
		*ip++ = *jp;
		*jp++ = c;

	} while (--n);
}

/* 
*/

/*
   =============================================================================
	qstexc() - circular exchange of 3 elements
   =============================================================================
*/

static
qstexc(ip, jp, kp)
register char *ip, *jp, *kp;
{
	register unsigned n;
	register char c;

	n = qscnt;

	do {

		c = *ip;
		*ip++ = *kp;
		*kp++ = *jp;
		*jp++ = c;

	} while (--n);
}

/* 
*/

/*
   =============================================================================
	qsort1() -- Quicksort algorithm driver
   =============================================================================
*/

static
qsort1(a, l)
char *a, *l;
{
	register char *hp, *i, *j;
	register unsigned es, n;
	register int c;
	char *lp;

	es = qscnt;

start:
	if ((n = (long)l - (long)a) LE es)
		return;

	n = es * (n / (2 * es));
	hp = lp = a + n;
	i = a;
	j = l - es;

	while (TRUE) {

		if (i < lp) {

			if ((c = (*qscmp)(i, lp)) EQ 0) {

				qsexch(i, lp -= es);
				continue;
			}

			if (c < 0) {

				i += es;
				continue;
			}
		}

/* 
*/

loop:
		if (j > hp) {

			if ((c = (*qscmp)(hp, j)) EQ 0) {

				qsexch(hp += es, j);
				goto loop;
			}

			if (c > 0) {

				if (i EQ lp) {

					qstexc(i, hp += es, j);
					i = lp += es;
					goto loop;
				}

				qsexch(i, j);
				j -= es;
				i += es;
				continue;
			}

			j -= es;
			goto loop;
		}

		if (i EQ lp) {

			if (((long)lp - (long)a) GE ((long)l - (long)hp)) {

				qsort1(hp + es, l);
				l = lp;

			} else {

				qsort1(a, lp);
				a = hp + es;
			}

			goto start;
		}

		qstexc(j, lp -= es, i);
		j = hp -= es;
	}
}

/* 
*/

/*
   =============================================================================
	qsort(base, n, esize, comp) -- Quicksort algorithm

	base	pointer to the element at the base of the table

	n	number of elements in the table

	esize	element size	(sizeof (*base))

	comp	comparison function:	comp(e1, e2)

		where:  e1 and e2 are pointers to the elements to compare

		comp() must return an integer which describes the result
		of the comparison, as follows:

			< 0	e1 < e2		(eg. -1)
			= 0	e1 = e2
			> 0	e1 > e2		(eg. +1)

		The string function strcmp(e1, e2), for example, might be
		suitable for use with qsort (assuming equal length strings).
   =============================================================================
*/

qsort(base, n, esize, fc)
char *base;
unsigned n;
unsigned esize;
int (*fc)();
{
	qscmp = fc;
	qscnt = esize;
	qsort1(base, base + ((long)n * (long)esize));
}
