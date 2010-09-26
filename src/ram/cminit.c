/*
   =============================================================================
	cminit.c -- C-Meta parser functions
	Version 5 -- 1987-07-10 -- D.N. Lynx Crowe

	Some parser functions in C, modelled after HyperMeta(tm).
	Designed for, but not dedicated to,  LR(k), top-down, recursive parsing.
   =============================================================================
*/

#include <stddefs.h>
#include <ctype.h>

#define	CM_MXSTR	257	/* maximum parser string result length */

#define	CM_DBLK		if (!QQanch) while (*QQip EQ ' ') ++QQip

int	QQsw;		/* parser result switch */
int	QQanch;		/* parser anchored match switch */

char	*QQin;		/* parser initial input pointer */
char	*QQip;		/* parser current input pointer */
char	*QQop;		/* parser string output pointer */

long	QQnum;		/* parser numeric result */
int	QQlnum;		/* parser list element number result */
char	QQdig;		/* parser digit result */
char	QQchr;		/* parser character result */

char	QQstr[CM_MXSTR];	/* parser string result */

/* 
*/

/*
   =============================================================================
	CMinit(ip) -- initialize the parser to work on the string at 'ip'.
   =============================================================================
*/

int
CMinit(ip)
char *ip;
{
	register int i;
	register char *t;

	QQip = ip;
	QQin = ip;
	QQsw = TRUE;
	QQanch = FALSE;
	QQdig = '?';
	QQchr = '?';
	QQnum = 0;
	QQop = QQstr;
	t = QQstr;

	for (i = 0; i < CM_MXSTR; i++)
		*t++ = '\0';
}

/* 
*/

/*
   =============================================================================
	CMchr(c) -- attempt to match character 'c' in the input.
   =============================================================================
*/

int
CMchr(c)
char c;
{
	CM_DBLK;

	if (c NE *QQip)
		return(QQsw = FALSE);

	QQchr = *QQip++;
	return(QQsw = TRUE);
}

/* 
*/

/*
   =============================================================================
	CMuchr(c) -- attempt to match character 'c' in the input, ignoring case.
   =============================================================================
*/

int
CMuchr(c)
register char c;
{
	register char t;

	CM_DBLK;

	t = *QQip;

	if (isascii(c))
		if (isupper(c))
			c = _tolower(c);

	if (isascii(t))
		if (isupper(t))
			t = _tolower(t);

	if (c NE t)
		return(QQsw = FALSE);

	QQchr = *QQip++;
	return(QQsw = TRUE);
}

/* 
*/

/*
   =============================================================================
	CMstr(s) -- attempt to match string at 's' in the input.
   =============================================================================
*/

int
CMstr(s)
char *s;
{
	register char *t;
	char *q;

	CM_DBLK;

	t = QQip;
	q = s;

	while (*s) {

		if (*t++ NE *s++)
			return(QQsw = FALSE);
	}

	QQop = QQstr;

	while (*QQop++ = *q++) ;

	QQip = t;
	return(QQsw = TRUE);
}

/* 
*/

/*
   =============================================================================
	CMustr(s) -- attempt to match string 's' in the input, ignoring case.
   =============================================================================
*/

int
CMustr(s)
register char *s;
{
	register char *t, t1, t2;
	char *q;

	CM_DBLK;

	t = QQip;
	q = s;

	while (*s) {

		t1 = *t++;
		t2 = *s++;

		if (isascii(t1))
			if (isupper(t1))
				t1 = _tolower(t1);

		if (isascii(t2))
			if (isupper(t2))
				t2 = _tolower(t2);

		if (t1 NE t2)
			return(QQsw = FALSE);
	}

	QQop = QQstr;

	while (*QQop++ = *q++) ;

	QQip = t;
	return(QQsw = TRUE);
}

/* 
*/

/*
   =============================================================================
	CMlong() -- attempt to parse a digit string in the input as a long.
   =============================================================================
*/

int
CMlong()
{
	register char *p;
	register long n;
	register char c;

	CM_DBLK;

	p = QQip;
	n = 0L;
	c = *p++;

	if (!isascii(c))
		return(QQsw = FALSE);

	if (!isdigit(c))
		return(QQsw = FALSE);

	n = c - '0';
		
	while (c = *p) {

		if (!isascii(c))
			break;

		if (!isdigit(c))
			break;

		n = (n * 10) + (c - '0');
		++p;
	}

	QQip = p;
	QQnum = n;
	return(QQsw = TRUE);
}

/* 
*/

/*
   =============================================================================
	CMdig() -- attempt to match a digit in the input string.
   =============================================================================
*/

int
CMdig()
{
	register char c;

	CM_DBLK;

	c = *QQip;

	if (!isascii(c))
		return(QQsw = FALSE);

	if (!isdigit(c))
		return(QQsw = FALSE);

	QQdig = c;
	++QQip;
	return(QQsw = TRUE);
}

/* 
*/

/*
   =============================================================================
	CMlist(l) -- attempt to match a string from the list 'l' in the input.
   =============================================================================
*/

int
CMlist(l)
register char *l[];
{
	register int	n;
	register char	*p, *q;

	CM_DBLK;

	n = 0;

	while (p = *l++) {

		q = p;

		if (CMstr(p)) {

			QQop = QQstr;

			while (*QQop++ = *q++) ;

			QQlnum = n;
			return(QQsw = TRUE);
		}

		++n;
	}

	return(QQsw = FALSE);
}

/* 
*/

/*
   =============================================================================
	CMulist(l) -- attempt to match a string from the list 'l' in the input
	ignoring case in both the list and the input string.
   =============================================================================
*/

int
CMulist(l)
register char *l[];
{
	register int	n;
	register char	*p, *q;

	CM_DBLK;

	n = 0;

	while (p = *l++) {

		q = p;

		if (CMustr(p)) {

			QQop = QQstr;

			while (*QQop++ = *q++) ;

			QQlnum = n;
			return(QQsw = TRUE);
		}

		++n;
	}

	return(QQsw = FALSE);
}

/* 
*/

/*
   =============================================================================
	CMstat(msg) -- output 'msg and dump parser status.  Returns QQsw.
   =============================================================================
*/

int
CMstat(msg)
char *msg;
{
	register char *tp;

	tp = QQin;
	printf("%s\r\n", msg);
	printf("  QQsw: %s, QQanch: %s, QQchr: 0x%02x <%c>, QQdig: %c\r\n",
		(QQsw ? "OK" : "NOGO"), (QQanch ? "anchored" : "deblanked"),
		QQchr, (isascii(QQchr) ? (isprint(QQchr) ? QQchr : ' ') : ' '),
		QQdig);
	printf("  QQnum: %ld, QQlnum: %d\r\n", QQnum, QQlnum);
	printf("  QQstr: %s\r\n", QQstr);
	printf("  {%s}\r\n", QQin);
	printf("   ");

	while (tp++ NE QQip)
		printf(" ");

	printf("^\r\n");
	return(QQsw);
}
