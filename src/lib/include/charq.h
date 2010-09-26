/*
   ============================================================================
	charq.h -- header for character queue functions
	Version 1 -- 1988-11-02 -- D.N. Lynx Crowe
   ============================================================================
*/

struct	charq {

	unsigned short	qsize;		/* maximum queue length */
	unsigned short	qlen;		/* current queue length */
	unsigned short	qin;		/* in pointer */
	unsigned short 	qout;		/* out pointer */
	unsigned short	qhi;		/* high water mark */
	unsigned short	qlo;		/* low water mark */
	char		*qbuf;		/* base of queue */
};

#ifndef	CHARQHDR
extern short	putq(), getq();
extern unsigned short setq();
#endif
