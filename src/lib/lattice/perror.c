/*
	perror.c -- print a system error message
	Version 1 -- 1988-05-02
*/

#define	CII

/*
 * Print the error indicated
 * in the cerror cell.
 */

#ifdef	CII
#define	void	int
extern int errno, s_nerr, strlen(), write();
extern char *s_errl[];
#else
extern int errno, sys_nerr, strlen(), write();
extern char *sys_errlist[];
#endif

void
perror(s)
char	*s;
{
	register char *c;
	register int n;

	c = "Unknown error";

#ifdef	CII
	if(errno < s_nerr)
		c = s_errl[errno];
#else
	if(errno < sys_nerr)
		c = sys_errlist[errno];
#endif

	n = strlen(s);

	if(n) {
		(void) write(2, s, (unsigned)n);
		(void) write(2, ": ", 2);
	}

	(void) write(2, c, (unsigned)strlen(c));
	(void) write(2, "\n", 1);
}
