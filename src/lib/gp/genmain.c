/*
   ============================================================================
	genmain.c -- general purpose main
	Version 1 -- 1987-11-10 -- D.N. Lynx Crowe

	Skeleton main program and option processing functions.
	Designed to be copied and hacked up into a real program.
	Handles all of the usual Unix(tm) style command line option forms.

	The functions Done(), Process(), Usage() and findopt() will need to be
	modified to make this into a real program.  It may also be useful
	to modify Die() for any error cleanup necessary.
   ============================================================================
*/

#include "stdio.h"
#include "ctype.h"
#include "stddefs.h"

/* Things that will change from program to program */

#define	PGMNAME		"genmain"	/* program name to use in messages */

#define	NULLFILE	"stdin"		/* null input file default */

#define	A_DFLTN		-1		/* default value for a_argn */
#define	A_ALTN		1		/* alternate default for a_argn */
#define	C_DFLTC		'C'		/* default character for c_argc */
#define	C_DFLTN		-1		/* default value for c_argn */
#define	C_ALTN		1		/* alternate default for c_argn */
#define	H_DFLTS		"h default"	/* defalut value for h_args */
#define	L_DFLTN		-1		/* default value for l_argn */
#define	S_ALTC		's'		/* alternate default for s_argc */
#define	S_DFLTC		'S'		/* default character for s_argc */

#define	NUM_DFLT	-1		/* default value for num_arg */

int	a_argn  = A_DFLTN;		/* switch a numeric argument */
int	c_argn  = C_DFLTN;		/* switch c numeric argument */
int	l_argn  = L_DFLTN;		/* switch l numeric argument */
int	num_arg = NUM_DFLT;		/* general numeric argument */

int	x_argf  = FALSE;		/* switch x value */

char	*h_args  = H_DFLTS;		/* switch h string argument */

char	c_argc  = C_DFLTC;		/* switch c character argument */
char	s_argc  = S_DFLTC;		/* switch s character argument */

/* 
*/

/* Standard stuff,  unlikely to change much,  if at all */

int	nfdone  = 0;		/* number of file arguments processed */
int	exitarg = 0;		/* exit() argument */
int	nerrors = 0;		/* number of errors encountered */

#define	L_PLURAL(N)	((N) ? ((N) GT 1 ? "s" : "") : "")
#define	U_PLURAL(N)	((N) ? ((N) GT 1 ? "S" : "") : "")

#define	EN_MSGL(X)	((X) ? "enabled" : "disabled")
#define	EN_MSGM(X)	((X) ? "Enabled" : "Disabled")
#define	EN_MSGU(X)	((X) ? "ENABLED" : "DISABLED")

#define	TF_MSGL(X)	((X) ? "true" : "false")
#define	TF_MSGM(X)	((X) ? "True" : "False")
#define	TF_MSGU(X)	((X) ? "TRUE" : "FALSE")

#define	SW_MSGL(X)	((X) ? "on" : "off")
#define	SW_MSGM(X)	((X) ? "On" : "Off")
#define	SW_MSGU(X)	((X) ? "ON" : "OFF")

#define GETCARG(N,C,D)	if ((N = intopt(argv, &C)) < 0) N = D
#define	GETARG(N,D)	if ((N = atoix(argv)) < 0) N = D

#define PGM_ERR(S)	fprintf(stderr, "%s:  ERROR - %s", PGMNAME, S)
#define PGM_WARN(S)	fprintf(stderr, "%s:  WARNING - %s", PGMNAME, S)
#define	PGM_MSG(S)	fprintf(stderr, "%s:  %s", PGMNAME, S)

#define NOFILE  ""

/* 
*/

/*
   =============================================================================
	Die() -- print an error message and exit
   =============================================================================
*/

Die(s)
char *s;	/* message string */
{
	++nerrors;
        PGM_ERR(s);
	fprintf(stderr, "\n%s:  Terminated with %d error%s.\n",
		PGMNAME, nerrors, L_PLURAL(nerrors));

        exit(1);
}

/*
   =============================================================================
	n_errs() -- create a "One|No|n errors was|were" message string

	If n is:

	 	LE 0	"No errors were"
		EQ 1	"One error was"
		GT 1	"n errors were"		(n is the number of errors)
   =============================================================================
*/

char *
n_errs(n)
int n;		/* number of errors */
{
	static char buf[100];

	if (n GT 1) {

		sprintf(buf, "%d errors were", n);
		return(buf);

	} else if (n EQ 1)
		return("One error was");
	else
		return("No errors were");
}

/* 
*/

/*
   =============================================================================
	ffiler() -- create a "Can't open" message string for Die()
   =============================================================================
*/

char *
ffiler(s)
char *s;	/* file name string pointer */
{
        static char buf[100];

        sprintf(buf, "Can't open %s", s);
        return (buf);
}

/*
   =============================================================================
	badopt() -- create an "Invalid option 'c'" message string for Die()
   =============================================================================
*/

char *
badopt(c)
char c;		/* character to complain about */
{
        static char buf[100];

        sprintf(buf, "Invalid option \047%c\047", c);
        return (buf);
}

/* 
*/

/*
   =============================================================================
	Usage() -- print the usage message

	This shows a way to do it, complete with current defaults.
   =============================================================================
*/

Usage()
{
	fprintf(stderr, "\n%s usage:\n\n", PGMNAME);

	fprintf(stderr, "   %s -an -ccn -h string -ln -sc -x\n\n", PGMNAME);

	fprintf(stderr, "     -an        argument a (%d)\n",
		A_ALTN);
	fprintf(stderr, "     -ccn       argument c (\047%c\047, %d)\n",
		C_DFLTC, C_DFLTN);
	fprintf(stderr, "     -h string  argument h (\042%s\042)\n",
		h_args);
	fprintf(stderr, "     -ln        argument l (%d)\n",
		0);
	fprintf(stderr, "     -sc        argument s (\047%c\047)\n",
		S_DFLTC);
	fprintf(stderr, "     -x         argument x \042%s\042\n",
		TF_MSGM(x_argf));

	fprintf(stderr, "\n");
}

/* 
*/

/*
   =============================================================================
	Done() -- final processing

	This is where any end-of-program processing would be done.
   =============================================================================
*/

Done()
{
	printf("\n%s:  Final values were -\n\n", PGMNAME);

	printf("   a_argn  = %d\n", a_argn);
	printf("   c_argn  = %d\n", c_argn);
	printf("   c_argc  = \047%c\047\n", c_argc);
	printf("   h_args  = \042%s\042\n", h_args);
	printf("   l_argn  = %d\n", l_argn);
	printf("   num_arg = %d\n", num_arg);
	printf("   s_argc  = \047%c\047\n", s_argc);
	printf("   x_argf  = %s\n", TF_MSGM(x_argf));

	if (nerrors)		/* if any errors, exit with an error value */
		exitarg = 1;

	printf("\n%s:  Processing complete.  %s encountered\n",
		PGMNAME, n_errs(nerrors));
}

/* 
*/

/*
   =============================================================================
	Process() -- process a file name argument

	This is where each file in the argument list would be processed.
   =============================================================================
*/

Process(s)
char *s;		/* file name string pointer */
{
	if (*s) {	/* process a file */

		printf("%s processed\n", s);

	} else {	/* process a null file (usually means use stdin) */

		printf("null file (%s) processed\n", NULLFILE);
	}
}

/* 
*/

/*
   =============================================================================
	atoix() -- get a numeric argument or return 0
   =============================================================================
*/

atoix(p)
register char **p;	/* pointer to the string pointer */
{
        register int n, c;

	n = 0;

        while (isdigit(c = *++*p))
		n = 10 * n + c - '0';

        --*p;
        return (n);
}

/*
   =============================================================================
	intopt() -- get an optional character and numeric argument

		option form:	-cn

		where:		c	is the optional character
				n	is the (non-zero) numeric value
   =============================================================================
*/

intopt(argv, optp)
char *argv[]; 		/* argv pointer */
char *optp;		/* pointer to the character variable to be set */
{
        int c;

        if ((c = (*argv)[1]) NE '\0' && !isdigit(c)) {
		*optp = c;
		++*argv;
	}

        return ((c = atoix(argv)) NE 0 ? c : -1);
}

/* 
*/

/*
   =============================================================================
	findopt() -- extract and process options up to the first file argument
   =============================================================================
*/

int
findopt(argc, argv)
int argc;		/* argument count */
char *argv[];		/* argument pointer array pointer */
{
        char	**eargv;	/* pointer to argument pointer */
        int	eargc;		/* remaining arguments after options */
	int	c;		/* current character being processed */
	int	strsw;		/* string argument grabbed switch */

	eargv = argv;		/* start with first argument */
	eargc = 0;		/* set file count to zero */
	strsw = FALSE;		/* indicate no string grabbed */

        while (--argc > 0) {

                switch (c = **++argv) {

                case '-':	/* options start with '-' ... */

                case '+':	/* ... or they can start with '+' */

                        if ((c = *++*argv) EQ '\0')	/* just '-' is end */
				break;
/* 
*/
                        do {
				if (isdigit(c)) {	/* general numeric arg */

					--*argv;
					num_arg = atoix(argv);

				} else

				switch (c) {	/* switch on option letter */

                                case 'a':	/* -an */

					GETARG(a_argn, A_ALTN);
                                        continue;

				case 'c':	/* -ccn */

					GETCARG(c_argn, c_argc, C_ALTN);
					continue;

                                case 'h':	/* -h string */

					if (--argc > 0) {

						strsw = TRUE;
						h_args = argv[1];
					}

                                        continue;

                                case 'l':	/* -ln */

					l_argn = atoix(argv);
					continue;

                                case 's':	/* -sc */

					if ((s_argc = (*argv)[1]) NE '\0')
						++*argv;
                                        else
						s_argc = S_ALTC;

                                        continue;
/* 
*/

				case 'x':	/* -x */

					x_argf = NOT x_argf;
					continue;

                                default :	/* unrecognized option */

					Usage();
					Die(badopt(c));
                                }

                        } while ((c = *++*argv) NE '\0');

                        if (strsw) {	/* if we've eaten an argument */

				strsw = FALSE;
				++argv;
			}

                        continue;
                }

                *eargv++ = *argv;	/* update the argv list */
                ++eargc;		/* update the argument count */
        }

        return (eargc);
}

/* 
*/

/*
   =============================================================================
	main() -- the main function for this program
   =============================================================================
*/

main(argc, argv)
int argc;
char *argv[];
{
	nfdone = 0;

	for (argc = findopt(argc, argv); argc > 0; --argc, ++argv) {

		Process(*argv);
		++nfdone;
	}

        if (!nfdone)		/* no files named, use NOFILE as argument */
                Process(NOFILE);

	Done();			/* final processing */

        exit(exitarg);
}
