/*
   =============================================================================
	dofmt.c -- integer and string formatted output function
	Version 4 -- 1987-06-12 -- D.N. Lynx Crowe

	Supports most of the BSD & SysV Unix(tm) [f|s]printf conversion codes -
	No floating point support in this implementation.

	Returns a long instead of the int returned by the Unix(tm) functions.

	WARNING - "Crufty code":

		This code contains hacks to work around a bug in
		the Alcyon C compiler,  which,  among other things,
		doesn't handle % properly for longs.  Conversions for
		o and x are hacked to use shifts instead of modulus.

		Alcyon 'cruft' is enabled by defining CRUFTY non-zero.
   =============================================================================
*/

#define	CRUFTY	1

#include "stddefs.h"
#include "ctype.h"
#include "varargs.h"

#define	MAXDIGS	11
#define	HIBITL	(1L << ( (8 * sizeof(long)) - 1))

#define	LONGHMSK	0x0FFFFFFFL
#define	LONGOMSK	0x1FFFFFFFL

#define	tonum(x)	((x)-'0')
#define	todigit(x)	((x)+'0')
#define	max(a,b)	((a)>(b)?(a):(b))
#define	min(a,b)	((a)<(b)?(a):(b))

#if	CRUFTY
extern	long	uldiv(), uldivr;
#endif

long
dofmt_(putsub, format, args)
register int (*putsub)();
register char *format;
va_list args;
{
	register int fcode;

	long	k,
		n,
		lzero,
		count;

	int	hradix,
		lowbit,
		length,
		fplus,
		fminus,
		fblank,
		fsharp,
		flzero,
		width,
		prec;

	register char *bp, *p;

	char	*prefix,
		*tab,
		buf[MAXDIGS];

	long	val;

/* 
*/

	count = 0;

	for ( ; ; ) {

		for (bp = format;
		     (fcode = *format) NE '\0' AND fcode NE '%';
		     format++)
			;

		if (n = (long)format - (long)bp) {

			count += n;

			while (n--)
				(*putsub)(*bp++);
		}

		if (fcode EQ '\0')
			return(count);

		fplus = fminus = fblank = fsharp = flzero = 0;

		for ( ; ; ) {

			switch (fcode = *++format) {

			case '+':
				fplus++;
				continue;

			case '-':
				fminus++;
				continue;

			case ' ':
				fblank++;
				continue;

			case '#':
				fsharp++;
				continue;
			}

			break;
		}

/* 
*/

		if (fcode EQ '*') {

			width = va_arg(args, int);

			if (width < 0) {

				width = -width;
				fminus++;
			}

			format++;

		} else {

			if (fcode EQ '0')
				flzero++;

			for (width = 0; isdigit(fcode = *format); format++)
				width = width * 10 + tonum(fcode);
		}

		if (*format NE '.')
			prec = -1;
		else if (*++format EQ '*') {

			prec = va_arg(args, int);
			format++;
		} else
			for (prec = 0; isdigit(fcode = *format); format++)
				prec = prec * 10 + tonum(fcode);

		length = 0;

		if (*format EQ 'l') {

			length++;
			format++;
		}

		prefix = "";
		lzero = 0;

/* 
*/
		switch (fcode = *format++) {

		case 'd':
		case 'u':
			hradix = 10/2;
			goto fixed;

		case 'o':
			hradix = 8/2;
			goto fixed;

		case 'X':
		case 'x':
			hradix = 16/2;

		fixed:

			if (prec < 0)
				if (flzero AND width > 0)
					prec = width;
				else
					prec = 1;

			if (length)
				val = va_arg(args, long);
			else if (fcode EQ 'd')
				val = va_arg(args, int);
			else
				val = va_arg(args, unsigned);

			if (fcode EQ 'd') {

				if (val < 0) {

					prefix = "-";

					if (val NE HIBITL)
						val = -val;

				} else if (fplus)
					prefix = "+";
				else if (fblank)
					prefix = " ";
			}

			tab = (fcode EQ 'X') ?
				"0123456789ABCDEF" : "0123456789abcdef";

			p = bp = buf + MAXDIGS;

/* 
*/
			switch (hradix) {

			case 4:

				while (val NE 0) {

					*--bp = tab[val & 0x07L];
					val = (val >> 3) & LONGOMSK;
				}

				break;

			case 5:

				while (val NE 0) {

#if	CRUFTY
					/* uldiv does a long divide */
					/* ... with remainder in uldivr */

					val = uldiv(val, 10L);
					*--bp = tab[uldivr];
#else
					/* on most compilers, this works */

					lowbit = val & 1;
					val = (val >> 1) & ~HIBITL;
					*--bp = tab[val % hradix * 2 + lowbit];
					val /= hradix;
#endif
				}

				break;

			case 8:

				while (val NE 0) {

					*--bp = tab[val & 0x0FL];
					val = (val >> 4) & LONGHMSK;
				}
			}

			lzero = (long)bp - (long)p + (long)prec;

			if (fsharp AND bp NE p)
				switch (fcode) {

				case 'o':
					if (lzero < 1)
						lzero = 1;
					break;

				case 'x':
				case 'X':
					prefix = "0x";
					break;
				}

			break;

/* 
*/
		default:
			buf[0] = fcode;
			goto c_merge;

		case 'c':
			buf[0] = va_arg(args, int);

		c_merge:

			p = (bp = &buf[0]) + 1;
			break;

		case 's':
			p = bp = va_arg(args, char *);

			if (prec < 0)
				p += strlen(bp);
			else {
				while (*p++ NE '\0' AND --prec GE 0)
					;
				--p;
			}

			break;

		case '\0':
			return(-1);

		}

/* 
*/
		if (lzero < 0)
			lzero = 0;

		k = (n = (long)p - (long)bp) + (long)(lzero +
			(prefix[0] EQ '\0' ? 0 : (prefix[1] EQ '\0' ? 1 : 2)));

		count += (width >k) ? width : k;

		if (!fminus)
			while (--width GE k)
				(*putsub)(' ');

		while (*prefix NE '\0')
			(*putsub)(*prefix++);

		while (--lzero GE 0)
			(*putsub)('0');

		if (n > 0)
			while (n--)
				(*putsub)(*bp++);

		while (--width GE k)
			(*putsub)(' ');
	}
}
