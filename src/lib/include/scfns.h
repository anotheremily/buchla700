/*
   =============================================================================
	scfns.h -- score function and variable definitions
	Version 14 -- 1988-04-20 -- D.N. Lynx Crowe

	Must follow stddefs.h and score.h if used.
   =============================================================================
*/

extern	int	eh_ins(), eh_rmv(), sc_clr();

extern	int	e_del(), selscor(), sc_goto();

extern	long	scinit(), evleft();

extern	struct	s_entry	*e_alc(), *e_ins(), *e_rmv(), *e_clr(), *ehfind();
extern	struct	s_entry *frfind(), *ep_adj(), *se_exec(), *findev();

extern	char	ac_code;
extern	char	scname[][16];

extern	BOOL	se_chg;

extern	int	curscor, cursect;

extern	long	se1_cnt, se2_cnt, se3_cnt;
extern	long	spool[], *pspool;
extern	long	spcount, frags;
extern	long	t_bak, t_cur, t_ctr, t_fwd;

extern	struct	s_entry	*size1, *size2, *size3;
extern	struct	s_entry	*scores[], *scp;
extern	struct	s_entry	*seclist[][N_SECTS], *hplist[][N_TYPES];
extern	struct	s_entry	*p_bak, *p_cur, *p_ctr, *p_fwd;
