/*
   =============================================================================
	htgen.c -- Harmonic table file generator for MIDAS-VII
	Written by:  D.N. Lynx Crowe

	See VERSION,  below,  for version number and date.

	This program creates a .h file to be included as harmonic
	coefficient tables for MIDAS-VII.

	WIDTH is the number of points in the table
	CENTER is the center point in the table
	N is the number of points scanned on either side of CENTER
	K and L are the lower and upper limits of the scan, respectively
	PER is the number of values generated per line

	HARM is the number of harmonics generated
	i is the harmonic number  (1 is the fundamental)
	j is the point number, K LE j LE L
   =============================================================================
*/

#include "stdio.h"
#include "math.h"
#include "stddefs.h"

#define	PGMNAME		"htgen"
#define	VERSION		"Version 2 -- 1988-09-20"

#define	FILENAME	"knmtab.h"	/* output file name */
#define	TABNAME		"knmtab"	/* table name */

#define	WIDTH		256		/* table width */
#define	RANGE		1023		/* maximum value */
#define	HARM		32		/* number of harmonics */

#define	N		115		/* number of points scanned */
#define	PER		16		/* values per line */

#define	CENTER		((WIDTH/2)-1)	/* center of the table */
#define	K		(CENTER-N)	/* first point scanned */
#define	L		(CENTER+N)	/* last point scanned */

#define M_PI		3.14159265358979323846
#define	M_HALFPI	(M_PI / 2.0)

#define	ACOSFN(x)	atan(sqrt(1.0 - (x * x)) / x)

extern	char	*now();			/* get current date-time group */
extern	char	*strcat();		/* append a string onto another */
extern	char	*strcpy();		/* copy a string into another */

/* 
*/

/* variables */

double	a;			/* unscaled harmonic coefficient */
double	c;			/* scaled harmonic coefficient */
double	y;			/* intermediate coefficient value */

double	ltend;			/* left endpoint value */
double	rtend;			/* right endpoint value */

double	hartab[WIDTH];		/* the current harmonic table */

int	nol;			/* number of values on the line so far */

FILE	*fp;			/* output file pointer */

char	bar[81];		/* bar of equal signs */
char	dtg[32];		/* date-time group */
char	intc[32];		/* value output work area */
char	result[128];		/* line output work area */

/* 
*/

/*
   =============================================================================
	CalcArc() -- Calculate the scaled Arc Cosine of 'arg'
   =============================================================================
*/

double
CalcArc(arg)
double arg;
{
	double y;

	if (arg EQ 0.0)			/* Return PI/2 if arg = 0.0 */
		return((double)M_HALFPI);

	y = ACOSFN(arg);

	if (arg < 0.0)			/* Adjust by PI if arg was negative */
		 y = y + (double)M_PI;

	return(y);
}

/*
   =============================================================================
	DoOut() -- Output result string
   =============================================================================
*/

DoOut(j, k)
int j, k;
{
	if (j EQ (WIDTH - 1)) {		/* end of line ? */

		if (k EQ HARM)		/* last table ? */
			fprintf(fp, "\t%s }\t/* %3d */\n", result, j);
		else
			fprintf(fp, "\t%s },\t/* %3d */\n", result, j);

	} else {

		fprintf(fp, "\t%s,\t/* %3d */\n", result, j);
	}

	result[0] = '\0';
	nol = 0;
}

/* 
*/

main()
{
	register int	c, i, j;

	printf("\n%s -- %s -- run on ",
		PGMNAME, VERSION);

	now(dtg);

	printf("%10.10s at %s\n\n",
		dtg, &dtg[12]);

	for (c = 0; c < 3; c++)
		bar[c] = ' ';

	for (; c < 80; c++)
		bar[c] = '=';

	bar[80] = '\0';

	printf("Generating coefficient tables for %d harmonics\n\n", HARM);

	printf("Full scale range = %d\n", RANGE);
	printf("Width of table   = %d\n", WIDTH);
	printf("Center of table  = %d\n", CENTER);
 	printf("Scan width (N)   = %d\n", N);
	printf("Points scanned   = %d thru %d\n\n", K, L);

	printf("Creating file \"%s\"\n", FILENAME);

	if ((FILE *)NULL EQ (fp = fopen(FILENAME, "w"))) {

		printf("ERROR:  unable to open \"%s\" for writing\n", FILENAME);
		exit(1);
	}

	printf("\n");

	fprintf(fp, "/*\n%s\n\t %s -- MIDAS-VII Harmonic coefficient tables\n",
		bar, FILENAME);

	fprintf(fp, "\tCalculated %10.10s -- %s\n%s\n*/\n\n",
		dtg, &dtg[12], bar);

	fprintf(fp, "/*\n%s\n", bar);
	fprintf(fp, "\tCalculated by htgen.c -- %s\n", VERSION);
	fprintf(fp, "\tfor MIDAS-VII using:\n");
	fprintf(fp, "\n");
	fprintf(fp, "\t\tFull scale range = %d\n", RANGE);
	fprintf(fp, "\n");
	fprintf(fp, "\t\tWidth of table   = %d\n", WIDTH);
	fprintf(fp, "\t\tCenter of table  = %d\n", CENTER);
 	fprintf(fp, "\t\tScan width (N)   = %d\n", N);
	fprintf(fp, "\t\tPoints scanned   = %d thru %d\n", K, L);
	fprintf(fp, "%s\n*/\n\n", bar);

	fprintf(fp, "short\t%s[%d][%d] = {\n", TABNAME, HARM, WIDTH);
	fprintf(fp, "\n");

/* 
*/
	for (i = 1; i < (HARM + 1); i++) {

		printf("Calculating harmonic %d ...\r",	i);

		for (j = K; j < (L + 1); j++) {

			y = CalcArc((double)(j - CENTER) / (double)N);
			a = cos((double)i * y);
			hartab[j] = ((double)RANGE * a) + 0.5;
		}

		ltend = hartab[K];
		rtend = hartab[L];

		printf("Outputting harmonic %d ...\r",	i);

		if (i & 1) {

			fprintf(fp, "/* \f\n");
			fprintf(fp, "*/\n");
		}

		fprintf(fp, "\n");
		fprintf(fp, "/* Harmonic table # %d */\n", i);
		fprintf(fp, "\n");

		nol = 1;
		result[0] = '\0';
		sprintf(intc, "%5d", (int)ltend);

		for (j = 0; j < K; j++) {

			if (nol EQ 1) {

				if (j EQ 0)
					strcat(strcat(result, "{"), intc);
				else
					strcat(strcat(result, " "), intc);

			} else {

				strcat(strcat(result, ","), intc);
			}

			if (nol EQ PER)
				DoOut(j, i);

			++nol;
		}
/* 
*/
		for (; j < (L + 1); j++) {

			sprintf(intc, "%5d", (int)hartab[j]);

			if (nol EQ 1)
				strcat(strcat(result, " "), intc);
			else
				strcat(strcat(result, ","), intc);

			if (nol EQ PER)
				DoOut(j, i);

			++nol;
		}

		sprintf(intc, "%5d", (int)rtend);

		for (; j < WIDTH; j++) {

			if (nol EQ 1)
				strcat(strcat(result, " "), intc);
			else
				strcat(strcat(result, ","), intc);

			if (nol EQ PER)
				DoOut(j, i);

			++nol;
		}

		if (nol NE 1)
			DoOut(j - 1, i);
	}

	fprintf(fp, "};\n");
	fclose(fp);

	printf("Output complete.  End of program.\n");
	exit(0);
}
