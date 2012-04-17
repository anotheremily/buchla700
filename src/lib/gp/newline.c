/*
   ============================================================================
	newline.c -- standard pagination functions
	Version 6 -- 1988-11-02 -- D.N. Lynx Crowe
   ============================================================================
*/

#include "stdio.h"
#include "stddefs.h"
#include "pageparm.h"

#ifndef	VERSION
#define	VERSION	"0.00 (Experimental)"
#endif

int pgmhdr ();			/* forward declaration */

/* variables for pagination functions */

int npage = 1;			/* current page number */
int nline = NLPAGE;		/* current line number */
int nlpage = NLPAGE;		/* number of lines per page */
int martop = MTOP;		/* top margin size -- lines */
int marbot = MBOT;		/* bottom margin size -- lines */
int marsize = MARGIN;		/* left margin size -- characters */
int newsub = 1;			/* flag for new sub-heading */
int minleft = MINLEFT;		/* minimum remaining lines on the page */

int (*prhead) () = pgmhdr;	/* pointer to page heading routine */

char *hdr = "?????";		/* pointer to program header (for pgmhdr) */
char *subhdg = "";		/* pointer to subheading */
char *verstr = VERSION;		/* pointer to version string */

FILE *printer;			/* printer file pointer */

/*  */

/*
   ============================================================================
	newline(fp) -- advance line counter before printing a new line

		Invoke this function BEFORE printing a new line.

		Uses:	nlpage = number of lines per page,
			nline  = current line number,
			npage  = current page number

		Invokes (*prhead)(fp) if nline overflows.  

		Initialize:

			npage  = 1, 
			nline  = nlpage,
			prhead = address of page heading routine

		before first line on page is printed.
   ============================================================================
*/

newline (fp)
     FILE *fp;
{
  if (nline++ >= (nlpage - (martop + marbot)))
    {

      nline = 1;		/* set current line number = 1 */
      fprintf (fp, "\f");	/* force new page */
      (*prhead) (fp);		/* call page heading routine */
      npage++;			/* update page number */
    }
}


/*  */

/*
   ============================================================================
	margin(fp) -- print spaces for a left margin

		marsize = number of blanks for left margin
   ============================================================================
*/

margin (fp)
     FILE *fp;
{
  int n;

  for (n = 0; n < marsize; n++)
    putc (' ', fp);
}

/*  */

/*
   ============================================================================
	pgmhdr(fp) -- print program header

		Prints a standard 2+martop line program heading
		including version and page number on the second line.

		hdr = pointer to program heading line (must include a newline)
   ============================================================================
*/

pgmhdr (fp)
     FILE *fp;
{
  int n;

  for (n = 0; n < martop; n++)
    {
      fprintf (fp, "\n");
      nline++;
    }

  margin (fp);
  fprintf (fp, "%s\n", hdr);
  nline++;

  margin (fp);
  fprintf (fp, "Version %s                                Page %4.4d\n\n",
	   verstr, npage);
  nline++;
  nline++;
}


/*  */

/*
   ============================================================================
	skipnl(fp, n) -- output n newlines to fp
   ============================================================================
*/

skipnl (fp, n)
     FILE *fp;
     int n;
{
  int j;

  if (n <= 0)
    return;

  for (j = 0; j < n; j++)
    {

      newline (fp);
      fprintf (fp, "\n");
    }
}

/*  */

/*
   ============================================================================
	subhdr(fp) -- standard subheading function
   ============================================================================
*/

subhdr (fp)
     FILE *fp;
{
  if (newsub)
    {				/* new sub-heading */

      newsub = FALSE;
      newline (fp);
      newline (fp);
      margin (fp);
      fprintf (fp, "%s\n\n", subhdg);

    }
  else
    {				/* continuation of old sub heading */

      newline (fp);
      newline (fp);
      margin (fp);
      fprintf (fp, "%s -- Continued\n\n", subhdg);
    }
}

/*  */

/*
   ============================================================================
	heading(fp) -- standard heading function
   ============================================================================
*/

heading (fp)
     FILE *fp;
{
  newline (fp);
  pgmhdr (fp);
  subhdr (fp);
}

/*  */

/*
   ============================================================================
	newsect(fp) -- standard new section function

	Invoke to begin a new section BEFORE printing the first line of the
	section, and BEFORE calling newline().  For example,

		subhdg = "Sub-heading for this section.";
		newsect(fp);
		newline(fp);
		printf(fp, "your line goes here");

	This will force a page break if there are not at least 'minleft'
	left on the page.
   ============================================================================
*/

newsect (fp)
     FILE *fp;
{
  newsub = TRUE;

  if (nline != nlpage)
    {

      if (nline > (nlpage - minleft))
	nline = nlpage;
      else
	subhdr (fp);
    }
}
