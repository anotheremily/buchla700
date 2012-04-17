/*
   =============================================================================
	dbentr.c -- special debug trace support functions for the Multi-Tasker
	Version 8 -- 1988-04-13 -- D.N. Lynx Crowe

	-------------------------- Important Note ---------------------------
	These are like the usual dbentr functions, but with the variables
	located elsewhere.  See "Multi-Tasker wierdness", below, for details.
	We also shut off interrupts while making entries into the buffer.
	---------------------------------------------------------------------

	Use with the "debug.h" header file.  Define DEBUGGER to include
	debug trace code.

	Define "SNAPSHOT" non-zero for snapshot mode:  once the buffer is
	filled up no more entries will be added  (no wrap-around).

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
		DB_Full			trace buffer full
   =============================================================================
*/

/* 
*/

#include "stddefs.h"

#define	SNAPSHOT	0	/* define non-zero for snapshot mode */

#define	DB_DEPTH	512	/* depth of the debug buffer */

#define	DB_entr		0	/* entry tag */
#define	DB_exit		1	/* exit tag */
#define	DB_cmnt		2	/* comment tag */

struct DB_Data
{				/* debug buffer entry structure */

  char *str;
  short tag;
};

/*
   =============================================================================
	Multi-Tasker wierdness:

	DB_In..DB_Data are normally defined here, but for the Multi-Tasker we
	define them as equates in an assembly language module so we can relocate
	them out of harm's way  (e.g. being cleared by reset, etc.).
   =============================================================================
*/

extern short DB_In;		/* debug buffer 'in' pointer */
extern short DB_Out;		/* debug buffer 'out' pointer */
extern short DB_Flag;		/* ROMP trap disable flag */
extern short DB_Full;		/* debug buffer full flag */

extern long DB_Levl;		/* debug function call level */

extern char *DB_Last;		/* last debug string pointer */

extern struct DB_Data DB_Ents[];	/* debug buffer */

/*
   =============================================================================
	End of Multi-Tasker wierdness
   =============================================================================
*/

char *DB_Type[] = {		/* debug buffer entry types */

  "+-->>",			/* 0 - DB_entr */
  "<<--",			/* 1 - DB_exit */
  " Note"			/* 2 - DB_cmnt */
};

/* 
*/

/*
   =============================================================================
	DB_Entr() -- log an entry in the trace buffer
   =============================================================================
*/

DB_Entr (str)
     char *str;
{
  register short oldipl;

  oldipl = setipl (7);

#if	SNAPSHOT
  if (DB_Full)
    {

      setipl (oldipl);
      return;
    }
#endif

  DB_Ents[DB_In].tag = DB_entr;	/* tag an entry */
  DB_Ents[DB_In].str = str;

  DB_Last = str;

  ++DB_Levl;

  if (++DB_In >= DB_DEPTH)	/* update the 'in' pointer */
    DB_In = 0;

  if (DB_In == DB_Out)
    {				/* bump the output pointer if full */

      DB_Full = TRUE;		/* indicate that buffer got filled up */

      if (++DB_Out >= DB_DEPTH)
	DB_Out = 0;
    }

  setipl (oldipl);
}

/* 
*/

/*
   =============================================================================
	DB_Exit() -- log an exit in the trace buffer
   =============================================================================
*/

DB_Exit (str)
     char *str;
{
  register short oldipl;

  oldipl = setipl (7);

#if	SNAPSHOT
  if (DB_Full)
    {

      setipl (oldipl);
      return;
    }
#endif

  DB_Ents[DB_In].tag = DB_exit;	/* tag an exit */
  DB_Ents[DB_In].str = str;

  DB_Last = str;

  if (DB_Levl > 0)
    --DB_Levl;
  else
    DB_Levl = 0L;

  if (++DB_In >= DB_DEPTH)	/* update the 'in' pointer */
    DB_In = 0;

  if (DB_In == DB_Out)
    {				/* bump the output pointer if full */

      DB_Full = TRUE;		/* indicate that buffer got filled up */

      if (++DB_Out >= DB_DEPTH)
	DB_Out = 0;
    }

  setipl (oldipl);
}

/* 
*/

/*
   =============================================================================
	DB_Cmnt() -- log a comment in the trace buffer
   =============================================================================
*/

DB_Cmnt (str)
     char *str;
{
  register short oldipl;

  oldipl = setipl (7);

#if	SNAPSHOT
  if (DB_Full)
    {

      setipl (oldipl);
      return;
    }
#endif

  DB_Ents[DB_In].tag = DB_cmnt;	/* tag a comment */
  DB_Ents[DB_In].str = str;

  DB_Last = str;

  if (++DB_In >= DB_DEPTH)	/* update the 'in' pointer */
    DB_In = 0;

  if (DB_In == DB_Out)
    {				/* bump the output pointer if full */

      DB_Full = TRUE;		/* indicate that buffer got filled up */

      if (++DB_Out >= DB_DEPTH)
	DB_Out = 0;
    }

  setipl (oldipl);
}

/* 
*/

/*
   =============================================================================
	DB_Clr() -- clear the debug buffer
   =============================================================================
*/

DB_Clr ()
{
  register short i;

  DB_In = 0;
  DB_Out = 0;
  DB_Full = FALSE;

  for (i = 0; i < DB_DEPTH; i++)
    {

      DB_Ents[i].tag = 0;
      DB_Ents[i].str = (char *) 0L;
    }

  DB_Levl = 0L;
  DB_Last = (char *) 0L;
}

/* 
*/

/*
   =============================================================================
	DB_Dump() -- dump and reset the trace buffer
   =============================================================================
*/

DB_Dump ()
{
  register short tag;
  register long i, lev;

  if ((DB_In >= DB_DEPTH) || (DB_In < 0))
    {				/* check DB_In */

      printf ("DB_In was corrupt:  %d\n", DB_In);

      xtrap15 ();		/* trap to ROMP */

      DB_Clr ();		/* clear the buffer */
      return;
    }

  if ((DB_Out >= DB_DEPTH) || (DB_Out < 0))
    {				/* check DB_Out */

      printf ("DB_Out was corrupt:  %d\n", DB_Out);

      xtrap15 ();		/* trap to ROMP */

      DB_Clr ();		/* clear the buffer */
      return;
    }

#if	SNAPSHOT
  if (DB_Full)
    {

      DB_In = DB_DEPTH;
      DB_Out = 0;
    }
#endif

  if (DB_In == DB_Out)
    {				/* check for an emtpy buffer */

      printf ("Debug buffer is empty:  In = Out = %d\n", DB_In);

      if (DB_Levl)
	printf ("Debug trace level = %ld\n", DB_Levl);

      if (DB_Last)
	printf ("Latest entry = \"%s\"\n", DB_Last);

      if (DB_Flag == 0)		/* trap to ROMP */
	xtrap15 ();

      DB_Clr ();		/* clear the buffer */
      return;
    }
/* 
*/
#if	SNAPSHOT
  printf ("Snapshot mode -- buffer is %s\n", DB_Full ? "full" : "part full");
#endif

  printf ("Debug trace level = %ld\n\n", DB_Levl);

  lev = 0L;

  while (DB_Out != DB_In)
    {				/* print the buffer entries */

      for (i = 0L; i < lev; i++)
	printf ("|");

      tag = DB_Ents[DB_Out].tag;

      printf ("%s:  %s\n", DB_Type[tag], DB_Ents[DB_Out].str);

      switch (tag)
	{

	case DB_entr:

	  ++lev;
	  break;

	case DB_exit:

	  if (--lev < 0L)
	    {

	      lev = 0L;
	      printf ("\n");
	    }

	  break;
	}

      if (++DB_Out >= DB_DEPTH)
	{

#if	SNAPSHOT
	  break;
#else
	  DB_Out = 0;
#endif
	}
    }

  printf ("\n----- End of debug buffer -----\n\n");

  DB_Clr ();			/* clear the buffer */

  if (DB_Flag == 0)
    xtrap15 ();

  return;
}
