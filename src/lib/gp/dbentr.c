/*
   =============================================================================
	dbentr.c -- debug trace support functions
	Version 3 -- 1988-03-03 -- D.N. Lynx Crowe

	Use with the "debug.h" header file.  Define DEBUGGER to include
	debug trace code.

	Synopsis:

	  Macros:

		DB_ENTR(str)		trace an entry
		DB_EXIT(str)		trace an exit
		DB_CMNT(str)		put a comment in the trace

	  Functions:

		DB_Clr()		clear the debug buffer
		DB_Dump()		dump and clear the debug buffer

		DB_Entr(str)		trace an entry
		DB_Exit(str)		trace an exit
		DB_Cmnt(str)		put a comment in the trace

	  Variables:

		DB_Flag			don't trap to ROMP if non-zero
   =============================================================================
*/

#include "stddefs.h"

#define	DB_DEPTH	256		/* depth of the debug buffer */

#define	DB_entr		0		/* entry tag */
#define	DB_exit		1		/* exit tag */
#define	DB_cmnt		2		/* comment tag */

struct	DB_Data {			/* debug buffer entry structure */

	char	*str;
	short	tag;
};

/* 
*/

short	DB_In;				/* debug buffer 'in' pointer */
short	DB_Out;				/* debug buffer 'out' pointer */
short	DB_Flag;			/* ROMP trap disable flag */

long	DB_Levl;			/* debug function call level */

char	*DB_Last;			/* last debug string pointer */

struct	DB_Data	DB_Ents[DB_DEPTH];	/* debug buffer */

char	*DB_Type[] = {			/* debug buffer entry types */

	"-->>",		/* 0 - DB_entr */
	"<<--",		/* 1 - DB_exit */
	"Note"		/* 2 - DB_cmnt */
};

/* 
*/

/*
   =============================================================================
	DB_Entr() -- log an entry in the trace buffer
   =============================================================================
*/

DB_Entr(str)
char *str;
{
	DB_Ents[DB_In].tag = DB_entr;	/* tag an entry */
	DB_Ents[DB_In].str = str;

	DB_Last = str;

	++DB_Levl;

	if (++DB_In GE DB_DEPTH)	/* update the 'in' pointer */
		DB_In = 0;

	if (DB_In EQ DB_Out) {		/* bump the output pointer if full */

		if (++DB_Out GE DB_DEPTH)
			DB_Out = 0;
	}
}

/* 
*/

/*
   =============================================================================
	DB_Exit() -- log an exit in the trace buffer
   =============================================================================
*/

DB_Exit(str)
char *str;
{
	DB_Ents[DB_In].tag = DB_exit;	/* tag an exit */
	DB_Ents[DB_In].str = str;

	DB_Last = str;

	if (DB_Levl > 0)
		--DB_Levl;
	else
		DB_Levl = 0L;

	if (++DB_In GE DB_DEPTH)	/* update the 'in' pointer */
		DB_In = 0;

	if (DB_In EQ DB_Out) {		/* bump the output pointer if full */

		if (++DB_Out GE DB_DEPTH)
			DB_Out = 0;
	}
}

/* 
*/

/*
   =============================================================================
	DB_Cmnt() -- log a comment in the trace buffer
   =============================================================================
*/

DB_Cmnt(str)
char *str;
{
	DB_Ents[DB_In].tag = DB_cmnt;	/* tag a comment */
	DB_Ents[DB_In].str = str;

	DB_Last = str;

	if (++DB_In GE DB_DEPTH)	/* update the 'in' pointer */
		DB_In = 0;

	if (DB_In EQ DB_Out) {		/* bump the output pointer if full */

		if (++DB_Out GE DB_DEPTH)
			DB_Out = 0;
	}
}

/* 
*/

/*
   =============================================================================
	DB_Clr() -- clear the debug buffer
   =============================================================================
*/

DB_Clr()
{
	register short i;

	DB_In  = 0;
	DB_Out = 0;

	for (i = 0; i < DB_DEPTH; i++) {

		DB_Ents[i].tag = 0;
		DB_Ents[i].str = (char *)0L;
	}

	DB_Levl = 0L;
	DB_Last = (char *)0L;
}

/* 
*/

/*
   =============================================================================
	DB_Dump() -- dump and reset the trace buffer
   =============================================================================
*/

DB_Dump()
{
	register short tag;
	register long i, lev;

	if ((DB_In GE DB_DEPTH) OR (DB_In < 0)) {	/* check DB_In */

		printf("DB_In was corrupt:  %d\n", DB_In);

		xtrap15();		/* trap to ROMP */
		
		DB_Clr();		/* clear the buffer */
		return;
	}

	if ((DB_Out GE DB_DEPTH) OR (DB_Out < 0)) {	/* check DB_Out */

		printf("DB_Out was corrupt:  %d\n", DB_Out);

		xtrap15();		/* trap to ROMP */
		
		DB_Clr();		/* clear the buffer */
		return;
	}

	if (DB_In EQ DB_Out) {		/* check for an emtpy buffer */

		printf("Debug buffer is empty:  In = Out = %d\n", DB_In);

		if (DB_Levl)
			printf("Debug trace level = %ld\n", DB_Levl);

		if (DB_Last)
			printf("Latest entry = \"%s\"\n", DB_Last);

		if (DB_Flag EQ 0)	/* trap to ROMP */
			xtrap15();

		DB_Clr();		/* clear the buffer */
		return;
	}
/* 
*/
	printf("Debug trace level = %ld\n\n", DB_Levl);

	lev = 0L;

	while (DB_Out NE DB_In) {	/* print the buffer entries */

		for (i = 0L; i < lev; i++)
			printf("|");

		tag = DB_Ents[DB_Out].tag;

		printf("%s:  %s\n", DB_Type[tag], DB_Ents[DB_Out].str);

		switch (tag) {

		case DB_entr:

			++lev;
			break;

		case DB_exit:

			if (--lev < 0L) {

				lev = 0L;
				printf("\n");
			}

			break;
		}

		if (++DB_Out GE DB_DEPTH)
			DB_Out = 0;
	}

	printf("\n----- End of debug buffer -----\n\n");

	DB_Clr();			/* clear the buffer */

	if (DB_Flag EQ 0)
		xtrap15();

	return;
}
