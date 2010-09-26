/*
   =============================================================================
	rdcheck.c -- test program for rdline.c
	Version 1 -- 1988-12-29 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

#define	MAXC	512

extern	short	rdline();

char	buf[MAXC+2];

/*
   =============================================================================
	test rdline
   =============================================================================
*/

main(argc, argv)
int argc;
char *argv[];
{
	short rc, runtag, line;
	register FILE *fp;

	line = 0;

	if (argc NE 2) {

		printf("ERROR:  File name required\n");
		exit(1);
	}

	if ( (FILE *)NULL EQ (fp = fopenb(argv[1], "r")) ) {

		printf("ERROR:  Unable to open \"%s\"\n", argv[1]);
		exit(1);
	}

	runtag = TRUE;

	while (runtag) {

		if (rc = rdline(fp, buf, MAXC)) {

			switch (rc) {

			default:
			case -1:

				runtag = FALSE;
				break;

			case 1:
			case 2:
				runtag = FALSE;
				strcat(buf, '\n');
				++line;
				printf("%5d:  %s", line, buf);
				break;
			}

		} else {

			++line;
			printf("%5d:  %s", line, buf);
		}
	}

	fclose(fp);
	exit(0);
}
