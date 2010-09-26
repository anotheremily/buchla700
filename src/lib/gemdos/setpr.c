/*
   =============================================================================
	setpr.c -- send printer setup strings to the Citizen HSP-500/550
	Version 15 -- 1989-11-16 -- D.N. Lynx Crowe

	Atari GEMDOS version.
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"
#include "getopt.h"

extern	int	atoi();

#define	MSGFILE		stdout
#define	OPTARGS		"c:i:v:m:p:s:f:dqtr"

Usage()
{
	fprintf(MSGFILE, "Usage:   setpr [-cn] [-in] [-vn] [-mn] [-pn] [-sn] [-fn] [-dqrt]\n");
	fprintf(MSGFILE, "  where:\n");
	fprintf(MSGFILE, "    -cn  set Configuration:  0 = Epson FX/EX, 1 = IBM Proprinter XL\n");
	fprintf(MSGFILE, "    -in  set International character set  (Epson only):\n");
	fprintf(MSGFILE, "           0  USA      4  Denmark-1   8 Japan     12 Latin Amer.\n");
	fprintf(MSGFILE, "           1  France   5  Sweden      9 Norway\n");
	fprintf(MSGFILE, "           2  Germany  6  Italy      10 Denark-2\n");
	fprintf(MSGFILE, "           3  England  7  Spain-1    11 Spain-2\n");
	fprintf(MSGFILE, "    -vn  set Vertical spacing  (Epson only):\n");
	fprintf(MSGFILE, "           0  8 lines/inch    1  7 dots/line    2  6 lines/inch\n");
	fprintf(MSGFILE, "    -mn  set master print Mode:\n");
	fprintf(MSGFILE, "           1 elite         4 compressed  16 doublestrike   64 italics\n");
	fprintf(MSGFILE, "           2 proportional  8 emphasized  32 expanded      128 underscore\n");
	fprintf(MSGFILE, "    -pn  set master Pitch:\n");
	fprintf(MSGFILE, "           0  10 CPI (Pica)   2  17 CPI (Pica Compr.)  6  15 CPI  \n");
	fprintf(MSGFILE, "           1  12 CPI (Elite)  5  13.3 CPI              7  20 CPI\n");
	fprintf(MSGFILE, "    -sn  set print Speed:  0  normal,  1  high speed\n");
	fprintf(MSGFILE, "    -fn  set Font number  (NLQ only):  0 Roman  1 Sans Serif  2..6 External\n");
	fprintf(MSGFILE, "    -d   Draft print mode\n");
	fprintf(MSGFILE, "    -q   NLQ print mode\n");
	fprintf(MSGFILE, "    -r   master Reset\n");
	fprintf(MSGFILE, "    -t   Top of form\n");

	exit(1);
}

/*
   =============================================================================
	setpr -- send printer setup strings to the Citizen HSP-500/550
   =============================================================================
*/

main(argc, argv)
int argc;
char *argv[];
{
	int	option;			/* option "letter" */
	int	n;			/* argument value */
	int	optused;		/* option present flag */

	FILE *stdprn;

	if ((FILE *)NULL EQ (stdprn = fopenb("LST:", "w"))) {

		fprintf(MSGFILE, "ERROR:  Unable to open PRN:\n");
		exit(2);
	}

	optused = FALSE;

	while ((option = getopt (argc, argv, OPTARGS)) != EOF) {

		optused = TRUE;

		switch (option) {

		case 'c':	/* c -- configuration, 0 = std, 1 = IBM */

			n = atoi(optarg);

			if (n > 1) {

				fprintf(MSGFILE, "ERROR:  bad configuration\n");
				Usage();
			}

			fprintf(stdprn, "\033~5%d", n);
			break;

		case 'i':	/* i -- international char set, 0 = US */

			n = atoi(optarg);

			if (n > 12) {

				fprintf(MSGFILE, "ERROR:  bad character set number\n");
				Usage();
			}

			fprintf(stdprn, "\033R%c", n);
			break;

		case 'v':	/* v -- vertical line spacing */

			n = atoi(optarg);

			if (n > 2) {

				fprintf(MSGFILE, "ERROR:  bad line spacing number\n");
				Usage();
			}

			fprintf(stdprn, "\033%d", n);
			break;

		case 'm':	/* m -- master print mode */

			n = atoi(optarg);

			if (n > 255) {

				fprintf(MSGFILE, "ERROR:  bad master print mode\n");
				Usage();
			}

			fprintf(stdprn, "\033!%c", n);
			break;

		case 'p':	/* p -- master pitch */

			n = atoi(optarg);

			switch (n) {

			case 0:
			case 1:
			case 2:
			case 5:
			case 6:
			case 7:

				fprintf(stdprn, "\033~3%c", n);
				break;

			default:

				fprintf(MSGFILE, "ERROR:  bad master pitch\n");
				Usage();
			}

			break;

		case 's':	/* s -- print speed */

			n = atoi(optarg);

			if (n > 1) {

				fprintf(MSGFILE, "ERROR:  bad print speed\n");
				Usage();
			}

			fprintf(stdprn, "\033~8%d", n);
			break;

		case 'f':	/* f -- NLQ font number */

			n = atoi(optarg);

			if (n > 6) {

				fprintf(MSGFILE, "ERROR:  bad font number\n");
				Usage();
			}

			fprintf(stdprn, "\033k%c", n);
			break;

		case 't':	/* t -- top of form */

			fprintf(stdprn, "\f");
			break;

		case 'd':	/* d -- draft print mode */

			fprintf(stdprn, "\033x0");
			break;

		case 'q':	/* q -- NLQ print mode */

			fprintf(stdprn, "\033x1");
			break;

		case 'r':	/* r -- master reset */

			fprintf(stdprn, "\033@");
			break;

		default:	/* unrecognized option */

			fprintf(MSGFILE, "ERROR:  unrecognized option\n");
			Usage();
		}
	}

	if (NOT optused)
		Usage();
	else
		exit(0);
}
