/*
   ============================================================================
	wordq.h -- header for word queue functions
	Version 1 -- 1988-11-02 -- D.N. Lynx Crowe
   ============================================================================
*/

struct wordq
{

  unsigned short qsize;		/* maximum queue length */
  unsigned short qlen;		/* current queue length */
  unsigned short qin;		/* in pointer */
  unsigned short qout;		/* out pointer */
  unsigned short qhi;		/* high water mark */
  unsigned short qlo;		/* low water mark */
  unsigned short *qbuf;		/* base of queue */
};

#ifndef	WORDQHDR
extern short putwq (), getwq ();
extern unsigned short setwq ();
#endif
