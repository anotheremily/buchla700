/*
   ============================================================================
	symbols.c -- symbol table lister for Alcyon / Digital Research objects
	(c) Copyright 1987,1988 -- D.N. Lynx Crowe
	See VERMSG below for version and date.
   ============================================================================
*/

#define	PGMNAME	"symbols"	/* program name */
#define	VERMSG	"Version 23 -- 1988-05-03 -- D.N. Lynx Crowe"

#define	DEBUGIT		0

#include "stdio.h"
#include "stddefs.h"
#include "ctype.h"
#include "objdefs.h"
#include "memory.h"

#define	MAXSYMS		6000	/* number of symbol table entries */

#define	B_SYM		(S_Glb | S_BSS)
#define	D_SYM		(S_Glb | S_Data)
#define	T_SYM		(S_Glb | S_Text)
#define	V_SYM		(S_Glb | S_Ext)

#define	cerror(S)	fprintf(stderr, "%s:  ERROR - %s", PGMNAME, S)
#define	PLURAL(n)	((n == 1) ? "" : "s")

extern char *basename ();
extern int qsort ();
extern long getl ();

struct syment
{				/* internal symbol table entry structure */

  char modname[12];		/* module name */
  struct SYMBOL sym;		/* symbol table entry from mdoule */
};

BOOL extonly = FALSE;		/* TRUE if only externals wanted */
BOOL notonly = FALSE;		/* TRUE if all symbols wanted */
BOOL txtonly = FALSE;		/* TRUE if text area only wanted */
BOOL varonly = FALSE;		/* TRUE if variables and data only */
BOOL flags = FALSE;		/* TRUE if hex dump of flags wanted */
BOOL ssflag = FALSE;		/* TRUE if symbols sort only */
BOOL vflag = FALSE;		/* TRUE if values are prBOOLed first */
BOOL sizes = TRUE;		/* TRUE if sizes wanted in value list */
BOOL nosyms = FALSE;		/* TRUE if no symbol list wanted */
BOOL nohead = FALSE;		/* TRUE if no file headers are wanted */

long textorg;			/* origin of text area */
long dataorg;			/* origin of data area */
long bssorg;			/* origin of bss area */
long textend;			/* end of text area */
long dataend;			/* end of data area */
long bssend;			/* end of BSS area */
long lnsyms;			/* number of symbols in current file */
long sympos;			/* symbol table file position */

FILE *curfp;			/* current file pointer */

struct EXFILE fhdr;		/* current file header */
struct SYMBOL cursym;		/* current input symbol */
struct syment symtab[MAXSYMS];	/* symbol table */

int nmods;			/* number of modules processed */
int nsyms;			/* number of symbols in current file */
int symtop;			/* total number of symbols in symtab */
int symflags = S_Glb | S_Ext;	/* default = globals or externals */

char curname[9];		/* current symbol name */
char module[13];		/* current module name */
char sfbuf[81];			/* symbol flag output buffer */

/* 
*/

/*
   ============================================================================
	usage() -- print the usage message
   ============================================================================
*/

usage ()
{
  printf ("\n%s -- %s\n\n", PGMNAME, VERMSG);

  printf ("usage:  %s [-{a|t|v|x}fhl{n|s}] file ...\n\n", PGMNAME);

  printf ("    a:  select all symbols\n");
  printf ("    t:  select text symbols only\n");
  printf ("    v:  select BSS and data symbols only\n");
  printf ("    x:  select externals only\n");
  printf ("    f:  include hex value of flags\n");
  printf ("    h:  omit file header information\n");
  printf ("    l:  don't include sizes in value list\n");
  printf ("    n:  don't print symbol sort\n");
  printf ("    s:  list by symbols only\n\n");

  printf ("At least one file name must be present.\n");
  printf ("Options a, t, v and x are mutually exclusive.\n");
  printf ("Options n and s are mutually exclusive.\n");
  printf ("Default:  print all globals sorted by name, then by value.\n");
  printf ("Symbol table capacity:  %d symbols\n", MAXSYMS);
  printf ("\n");
}

/*
   ============================================================================
	die -- fatal error handler
   ============================================================================
*/

die (s)
     char *s;
{
  cerror (s);
  putc ('\n', stderr);
  fprintf (stderr, "%s:  Program terminated.\n", PGMNAME);
  exit (1);
}

/*  */

/*
   ============================================================================
	findopt -- get file name and process options
   ============================================================================
*/

findopt (argc, argv)
     char *argv[];
{
  char **eargv = argv;
  int eargc = 0, c;

  while (--argc > 0)
    {

      switch (c = **++argv)
	{

	case '-':

	  if ((c = *++*argv) == '\0')
	    break;

	  do
	    {

	      switch (c)
		{

		case 'a':	/* a:  all symbols */
		  if (extonly | txtonly | varonly)
		    die ("Conflicting options");

		  notonly = TRUE;
		  symflags = 0xFFFF;
		  continue;

		case 'f':	/* f:  dump flags */
		  flags = TRUE;
		  continue;

		case 'h':	/* h:  omit headers */
		  nohead = TRUE;
		  continue;

		case 'l':	/* l:  don't include sizes */
		  sizes = FALSE;
		  continue;

		case 'n':	/* n:  don't list symbols */
		  if (ssflag)
		    die ("Conflicting options");

		  nosyms = TRUE;
		  continue;

		case 's':	/* s:  symbol sort only */
		  if (nosyms)
		    die ("Conflicting options");

		  ssflag = TRUE;
		  continue;

		case 't':	/* t:  text area only */
		  if (extonly | notonly | varonly)
		    die ("Conflicting options");

		  txtonly = TRUE;
		  continue;

		case 'v':	/* v:  variables and data only */
		  if (extonly | notonly | txtonly)
		    die ("Conflicting options");

		  varonly = TRUE;
		  continue;

		case 'x':	/* x:  externals only */
		  if (notonly | txtonly | varonly)
		    die ("Conflicting options");

		  extonly = TRUE;
		  symflags = S_Ext;
		  continue;
/* 
*/
		default:
		  usage ();
		  die ("unknown option");
		}

	    }
	  while ((c = *++*argv) != '\0');

	  continue;
	}

      *eargv++ = *argv;
      ++eargc;
    }

  return (eargc);
}

/*  */

/*
   ============================================================================
	sflags(f) -- Interpret symbol table flags
   ============================================================================
*/

char *
sflags (f)
     int f;
{
  memset (sfbuf, '\0', sizeof sfbuf);

  if (!(f & S_Def))
    strcat (sfbuf, " Und");

  if (f & S_Glb)
    strcat (sfbuf, " Glb");

  if (f & S_Ext)
    strcat (sfbuf, " Ext");

  if (f & S_Text)
    strcat (sfbuf, " Txt");

  if (f & S_Data)
    strcat (sfbuf, " Dat");

  if (f & S_BSS)
    strcat (sfbuf, " BSS");

  if (f & S_Reg)
    strcat (sfbuf, " Reg");

  if (f & S_Equ)
    strcat (sfbuf, " Equ");

  return (sfbuf);
}

/* 
*/

/*
   ============================================================================
	prntsym() -- print symbol table entry
   ============================================================================
*/

prntsym (i)
     int i;
{
  register long thesize;
  register long nextloc;
  register unsigned thisorg;
  register unsigned nextorg;
  register unsigned symflag;
  register struct syment *sp;

  sp = &symtab[i];
  memset (curname, 0, sizeof (struct SYMBOL));
  strncpy (curname, sp->sym.symname, 8);

  if (vflag)
    {

      printf ("%08lx %-8.8s %-12.12s",
	      sp->sym.symvalue, curname, sp->modname);

    }
  else
    {

      printf ("%-8.8s %-12.12s %08lx",
	      curname, sp->modname, sp->sym.symvalue);
    }

/* 
*/
  if (vflag && sizes)
    {

      symflag = sp->sym.symtype;
      thisorg = symflag & (S_Data | S_Text | S_BSS);

      if (i == (symtop - 1))
	{			/* end of table ? */

	  if (thisorg == S_Text)
	    nextloc = textend + 1L;
	  else if (thisorg == S_Data)
	    nextloc = dataend + 1L;
	  else if (thisorg == S_BSS)
	    nextloc = bssend + 1L;
	  else
	    nextloc = sp->sym.symvalue;

	  nextorg = thisorg;

	}
      else
	{			/* not the end */

	  nextloc = symtab[i + 1].sym.symvalue;

	  nextorg = symtab[i + 1].sym.symtype & (S_Data | S_Text | S_BSS);
	}

      if (nextorg != thisorg)
	{

	  if (thisorg == S_Text)
	    nextloc = textend + 1L;
	  else if (thisorg == S_Data)
	    nextloc = dataend + 1L;
	  else if (thisorg == S_BSS)
	    nextloc = bssend + 1L;
	  else
	    nextloc = sp->sym.symvalue;
	}

      thesize = nextloc - sp->sym.symvalue;
/* 
*/
      switch (symflag)
	{

	case (S_Def | S_Glb | S_Text):
	case (S_Def | S_Glb | S_Text | S_Equ):
	case (S_Def | S_Glb | S_Data):
	case (S_Def | S_Data):
	case (S_Def | S_Glb | S_BSS):
	case (S_Def | S_BSS):
	  if (nmods == 1)
	    {

	      printf (" %11ld ", thesize);

	    }
	  else
	    printf (" ___________ ");

#if	DEBUGIT
	  if (nextloc < sp->sym.symvalue)
	    {

	      if (flags)
		printf (" %04x%s\n",
			sp->sym.symtype, sflags (sp->sym.symtype));

	      printf
		("\n[i, symflag, textorg, fhdr.F_Text, textend, nextloc, thisorg, nextorg]\n");

	      printf
		("[%d, $%x, $%08.8lx, $%08.8lx, $%08.8lx, $%08.8lx, %d, %d]\n\n",
		 i, symflag, textorg, fhdr.F_Text, textend, nextloc, thisorg,
		 nextorg);
	      return;
	    }
#endif

	  break;

	case (S_Def | S_Equ):
	case (S_Def | S_Equ | S_Reg):
	  printf (" %11ld ", sp->sym.symvalue);
	  break;

	case (S_Def | S_Glb | S_Ext):
	  if (sp->sym.symvalue)
	    {

	      printf (" %11ld ", sp->sym.symvalue);
	      break;
	    }

	default:
	  printf (" ___________ ");
	}

    }

  if (flags)
    printf (" %04x%s\n", sp->sym.symtype, sflags (sp->sym.symtype));
  else
    printf ("%s\n", sflags (sp->sym.symtype));
}

/*  */

/*
   ============================================================================
	grabsym() -- put symbol table entry in symbol table
   ============================================================================
*/

grabsym ()
{
  if (symtop == MAXSYMS)
    return;

  memcpy (symtab[symtop].modname, module, 12);
  memcpy (&symtab[symtop].sym, &cursym, sizeof (struct SYMBOL));

  if (++symtop == MAXSYMS)
    {

      fprintf (stderr, "%s:  Limit of %d symbols ", PGMNAME, MAXSYMS);
      fprintf (stderr, "reached while processing module \"%-.12s\".\n",
	       module);
    }
}

/* 
*/

/*
   ============================================================================
	process(fn) -- read and accumulate the symbols from file fn
   ============================================================================
*/

process (fn)
     char *fn;
{
  int i;

  /* open the file */

  if (NULL == (curfp = fopenb (fn, "r")))
    {

      fprintf (stderr, "%s:  Unable to open %s\n", PGMNAME, fn);

      return;
    }

  /* read in the file header */

  if (1 != fread (&fhdr, sizeof fhdr, 1, curfp))
    {

      fprintf (stderr, "%s:  Unable to read %s\n", PGMNAME, fn);

      fclose (curfp);
      exit (1);
    }

  /* check the magic */

  if ((fhdr.F_Magic != F_R_C) && (fhdr.F_Magic != F_R_D))
    {

      fprintf (stderr, "%s:  Bad magic [0x%04x] in %s",
	       PGMNAME, fhdr.F_Magic, fn);

      fclose (curfp);
      exit (1);
    }

/* 
*/

  ++nmods;			/* udpdate module count */

  textorg = fhdr.F_Res2;

  /* if it's a discontiguous file, read the origins */

  if (fhdr.F_Magic == F_R_D)
    {

      dataorg = getl (curfp);

      if (0L == dataorg)
	dataorg = textorg + fhdr.F_Text;

      bssorg = getl (curfp);

      if (0L == bssorg)
	bssorg = dataorg + fhdr.F_Data;

    }
  else
    {

      dataorg = textorg + fhdr.F_Text;
      bssorg = dataorg + fhdr.F_Data;
    }

  textend = textorg + fhdr.F_Text - 1L;
  dataend = dataorg + fhdr.F_Data - 1L;
  bssend = bssorg + fhdr.F_BSS - 1L;

/* 
*/

  /* process the symbols, if any exist */

  nsyms = 0;

  if (fhdr.F_Symtab)
    {

      sympos = fhdr.F_Text + fhdr.F_Data;

      if (-1 == fseek (curfp, sympos, 1))
	{

	  fprintf (stderr, "%s:  Unable to seek to %ld on %s\n",
		   PGMNAME, sympos, fn);

	  fclose (curfp);
	  exit (1);
	}

      memset (module, 0, sizeof module);
      strncpy (module, basename (fn), 12);
      lnsyms = fhdr.F_Symtab / (sizeof cursym);

      if (lnsyms > 32767)
	{

	  fclose (curfp);
	  die ("File has more than 32767 symbols");
	}

      nsyms = lnsyms;

      for (i = 0; i < nsyms; i++)
	{

	  if (1 != fread (&cursym, sizeof cursym, 1, curfp))
	    {

	      fprintf (stderr, "%s:  Unable to read %s\n", PGMNAME, fn);

	      fclose (curfp);
	      exit (1);
	    }

	  if (varonly)
	    {

	      if (((cursym.symtype & B_SYM) == B_SYM) ||
		  ((cursym.symtype & D_SYM) == D_SYM) ||
		  ((cursym.symtype & V_SYM) == V_SYM))
		grabsym ();

	    }
	  else if (txtonly)
	    {

	      if (((cursym.symtype & T_SYM) == T_SYM))
		grabsym ();

	    }
	  else if (cursym.symtype & symflags)
	    {

	      grabsym ();

	    }
	  else if (notonly)
	    {

	      grabsym ();
	    }
	}

      fclose (curfp);

    }
  else
    {

      fclose (curfp);

    }

  if (nohead)
    return;

  printf ("Module \"%s\" has %d symbol%s\n", module, nsyms, PLURAL (nsyms));

  if (fhdr.F_Res2)
    {

      printf ("  Text:  %08.8lx to %08.8lx,  %ld byte%s\n",
	      textorg, textend, fhdr.F_Text, PLURAL (fhdr.F_Text));
      printf ("  Data:  %08.8lx to %08.8lx,  %ld byte%s\n",
	      dataorg, dataend, fhdr.F_Data, PLURAL (fhdr.F_Data));
      printf ("  BSS:   %08.8lx to %08.8lx,  %ld byte%s\n\n",
	      bssorg, bssend, fhdr.F_BSS, PLURAL (fhdr.F_BSS));

    }
  else
    {

      printf ("  Text:  %ld byte%s\n", fhdr.F_Text, PLURAL (fhdr.F_Text));
      printf ("  Data:  %ld byte%s\n", fhdr.F_Data, PLURAL (fhdr.F_Data));
      printf ("  BSS:   %ld byte%s\n", fhdr.F_BSS, PLURAL (fhdr.F_BSS));
    }

  printf ("\n");
}

/* 
*/

/*
   ============================================================================
	namcmp() -- compare two symbol entries by symname, modname, symvalue
   ============================================================================
*/

int
namcmp (e1, e2)
     struct syment *e1, *e2;
{
  long rv;

  if (rv = memcmp (e1->sym.symname, e2->sym.symname, 8))
    return (rv > 0 ? 1 : -1);

  if (rv = memcmp (e1->modname, e2->modname, 12))
    return (rv > 0 ? 1 : -1);

  if (rv = e1->sym.symvalue - e2->sym.symvalue)
    return (rv > 0 ? 1 : -1);

  return (0);
}

/* 
*/

/*
   ============================================================================
	typesym() -- return a symbol type number for a symbol
   ============================================================================
*/

int
typesym (ep)
     struct syment *ep;
{
  if (ep->sym.symtype & S_Text)
    return (0);
  else if (ep->sym.symtype & S_Data)
    return (1);
  else if (ep->sym.symtype & S_BSS)
    return (2);
  else if (ep->sym.symtype & S_Ext && ep->sym.symvalue)
    return (3);
  else
    return (4);
}

/* 
*/

/*
   ============================================================================
	valcmp() -- compare two symbol entries by:
		symtype, symvalue, symname, modname
   ============================================================================
*/

int
valcmp (e1, e2)
     struct syment *e1, *e2;
{
  long rv;

  if (rv = typesym (e1) - typesym (e2))
    return (rv > 0 ? 1 : -1);

  if (rv = e1->sym.symvalue - e2->sym.symvalue)
    return (rv > 0 ? 1 : -1);

  if (rv = memcmp (e1->sym.symname, e2->sym.symname, 8))
    return (rv > 0 ? 1 : -1);

  if (rv = memcmp (e1->modname, e2->modname, 12))
    return (rv > 0 ? 1 : -1);

  return (0);
}

/* 
*/

/*
   ============================================================================
	main processing loop -- process arguments and files
   ============================================================================
*/

main (argc, argv)
     int argc;
     char *argv[];
{
  register int i;

  nmods = 0;			/* reset module count */
  symtop = 0;			/* clear the symbol table */

  for (i = 0; i < MAXSYMS; i++)
    memset (&symtab[i], 0, sizeof (struct syment));

  for (argc = findopt (argc, argv); argc > 0; --argc, ++argv)
    process (*argv);		/* process each file for symbols */

  if (nmods == 0)
    {				/* give usage if no files processed */

      usage ();
      exit (1);
    }

  if (! nohead)
    printf ("%d module%s processed.  %d symbol%s selected.\n",
	    nmods, PLURAL (nmods), symtop, PLURAL (symtop));

  if (symtop < 1)		/* done if no symbols found */
    exit (0);

  if (! nohead)
    printf ("\f");		/* forms feed */

  if (! nosyms)
    {

      vflag = FALSE;		/* sort and print by name */
      qsort (symtab, symtop, sizeof (struct syment), namcmp);

      for (i = 0; i < symtop; i++)
	prntsym (i);

      if (ssflag)		/* done if ssflag set */
	exit (0);

      if (! nohead)
	printf ("\f");		/* forms feed */
    }

  vflag = TRUE;			/* sort and print by value */
  qsort (symtab, symtop, sizeof (struct syment), valcmp);

  for (i = 0; i < symtop; i++)
    prntsym (i);
}
