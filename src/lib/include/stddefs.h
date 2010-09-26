/*
   ============================================================================
	stddefs.h -- Standard definitions for C programs
	Version 12 -- 1987-12-15 -- D.N. Lynx Crowe

	Must follow stdio.h if stdio.h is used as both define:

		NULL, EOF.

	Must follow define.h on the Atari if define.h is used as both define:

		NULL, EOF, FOREVER, TRUE, FALSE, FAILURE, SUCCESS,
		YES, NO, EOS, NIL.

	Released to Public Domain - 1987-06 - D.N. Lynx Crowe
   ============================================================================
*/

#ifndef	STD_DEFS		/* so we only define these once */

#define	STD_DEFS	1

/* relational operators */

#define	EQ	==
#define	NE	!=
#define	GE	>=
#define	LE	<=
#define	GT	>
#define	LT	<

/* logical operators */

#define	NOT	!
#define	AND	&&
#define	OR	||

/* infinite loop constructs */

#ifndef	FOREVER
#define	FOREVER		for(;;)
#endif

#ifndef	REPEAT
#define	REPEAT		for(;;)
#endif

/* 
*/

/* various terminators */

#ifndef	EOF
#define	EOF		(-1)
#endif

#ifndef	EOS
#define	EOS		'\0'
#endif

#ifndef	NIL
#define	NIL		0
#endif

/* manifest constants for function return and flag values */

#ifndef	NULL
#define	NULL		0
#endif

#ifndef	YES
#define	YES		1
#endif

#ifndef	NO
#define	NO		0
#endif

#ifndef	FALSE
#define	FALSE		0
#endif

#ifndef	TRUE
#define	TRUE		1
#endif

#ifndef	SUCCESS
#define	SUCCESS		0
#endif

#ifndef	FAILURE
#define	FAILURE		(-1)
#endif

/* BOOL type definition for flag variables */

typedef	char	BOOL;

#endif
