/*
   =============================================================================
	cmeta.h -- C-Meta parser macro header
	Version 3 -- 1987-04-30 -- D.N. Lynx Crowe
   =============================================================================
*/

extern	int	CMinit(), CMstat();
extern	int	CMchr(), CMuchr();
extern	int	CMstr(), CMustr();
extern	int	CMlong(), CMdig();
extern	int	CMlist(), CMulist();

extern	int	QQsw;
extern	int	QQanch;

extern	char	*QQin;
extern	char	*QQip;
extern	char	*QQop;

extern	long	QQnum;
extern	int	QQlnum;
extern	char	QQchr;
extern	char	QQdig;

extern	char	QQstr[];

#define	CM_CHR(c)	CMchr(c)
#define	CM_UCHR(c)	CMuchr(c)

#define	CM_STR(s)	CMstr(s)
#define	CM_USTR(s)	CMustr(s)

#define	CM_NUM		CMlong()
#define	CM_DIG		CMdig()

#define	CM_LIST(l)	CMlist(l)
#define	CM_ULIST(l)	CMulist(l)

#define	CM_OK		return(QQsw = TRUE)
#define	CM_NOGO		return(QQsw = FALSE)

#define	CM_DBLK		if (!QQanch) while (*QQip EQ ' ') ++QQip
