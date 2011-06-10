/*
   =============================================================================
	getopt.c -- process the options on a Unix{tm} style command line
	Version 1 -- 1987-10-16 -- D.N. Lynx Crowe
   =============================================================================
*/

extern int optind;		/* Index into nargv */
extern char *optarg;		/* Pointer to option argument or NULL */

extern int opterr;		/* Error output flag */
extern int optopt;		/* Most recently parsed option letter */

extern int getopt ();
