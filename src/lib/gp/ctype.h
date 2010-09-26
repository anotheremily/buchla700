/*
   ============================================================================
	ctype.h -- extended character macros, ala' Unix(tm)
	Version 8 -- 1987-06-16 -- D.N. Lynx Crowe

	These macros are a superset of those provided for System 5 Unix(tm).

	This set of macros will return status for characters 0x80..0xFF as
	though they were in the range 0x00..0x7F for compatibility with
	the equivalent Lattice C macros.  This means that isascii(c) should
	be used if it is necessary to restrict the results to true ASCII.

	If SYS5CODE is TRUE these macros will use the same table definitions
	as System 5 Unix(tm).  In this case these behave well even when fed
	an EOF.  The EOF character value, -1, is not defined as being in any
	of the classes tested for.  If SYS5CODE is FALSE, the Alcyon table
	will be used, and EOF will not work properly.

	Define _CTYPE_C for compiling the ctype.c file, undefine otherwise.

	The symbol _ii_ is defined to be the base of the table used.

		isascii(c)  non-zero if c is ASCII

		isprint(c)  non-zero if c is printable (including blank)
		isgraph(c)  non-zero if c is graphic (excluding blank)
		iscntrl(c)  non-zero if c is control character

		isalpha(c)  non-zero if c is alpha
		isupper(c)  non-zero if c is upper case
		islower(c)  non-zero if c is lower case

		isdigit(c)  non-zero if c is a digit (0 to 9)
		isxdigit(c) non-zero if c is a hexadecimal digit (0 to 9, A to F, a to f)

		isalnum(c)  non-zero if c is alpha or digit

		isspace(c)  non-zero if c is white space
		ispunct(c)  non-zero if c is punctuation

		iscsym(c)   non-zero if valid character for C symbols
		iscsymf(c)  non-zero if valid first character for C symbols

		toascii(c)  returns c masked to 7 bits

		_tolower(c) returns the lower case version of c
		_toupper(c) returns the upper case version of c

		tonumber(c) converts c from ASCII to integer
   ============================================================================
*/

/* 
*/

#define	SYS5CODE	0	/* define non-zero for System 5 / Aztec C */

#if	SYS5CODE

#define	_ii_	_ctype+1

/* System 5 Unix(tm) / Aztec C table definitions */

#define _U	0x01	/* upper case */
#define _L	0x02	/* lower case */
#define _N	0x04	/* numeric 0..9 */
#define _S	0x08	/* whitespace */
#define	_P	0x10	/* punctuation */
#define	_C	0x20	/* control 00..1F */
#define _B	0x40	/* blank */
#define _X	0x80	/* hex digit */

#else

#define	_ii_	__atab

/* Digital Research / Alcyon C table definitions */

#define	_C	0x01	/* control 00.1F */
#define	_P	0x02	/* punctuation */
#define	_N	0x04	/* numeric 0..9 */
#define	_U	0x08	/* upper case */
#define	_L	0x10	/* lower case */
#define	_S	0x20	/* whitespace */
#define	_B	0x40	/* blank */
#define	_X	0x80	/* hex digit */

#endif

#ifndef	_CTYPE_C

#if	SYS5CODE

extern	char _ctype[];		/* character type table */

#else	/* SYS5CODE */

extern	char ___atab();		/* so the loader sees us ... */
extern	char __atab[];		/* character type table */

#endif	/* SYS5CODE */

#endif	/* _CTYPE_C */

/* 
*/

#define	isascii(c)	(!((c)&~0x7F))

#define	isprint(c)	((_ii_)[c]&(_P|_U|_L|_N|_B))
#define	isgraph(c)	((_ii_)[c]&(_P|_U|_L|_N))
#define	iscntrl(c)	((_ii_)[c]&_C)

#define	isalpha(c)	((_ii_)[c]&(_U|_L))
#define	isupper(c)	((_ii_)[c]&_U)
#define	islower(c)	((_ii_)[c]&_L)

#define	isdigit(c)	((_ii_)[c]&_N)
#define	isxdigit(c)	((_ii_)[c]&_X)

#define	isalnum(c)	((_ii_)[c]&(_U|_L|_N))

#define	isspace(c)	((_ii_)[c]&_S)
#define	ispunct(c)	((_ii_)[c]&_P)

#define	iscsym(c)	(isalnum(c)||(((c)&127)==0x5F))
#define	iscsymf(c)	(isalpha(c)||(((c)&127)==0x5F))

#define	toascii(c)	((c)&0x7F)

#define _toupper(c)	((c)-'a'+'A')
#define _tolower(c)	((c)-'A'+'a')

#define	tonumber(c)	((((c)&0x7F)>'9')?((islower(c)?_toupper(c):c)-'A'+10):(c&0x0F))

