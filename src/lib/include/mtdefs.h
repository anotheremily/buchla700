/*
   =============================================================================
	mtdefs.h -- mutlitasker definitions
	Version 22 -- 1988-04-14 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
	WARNING:  all of these data structures are highly ORDER DEPENDENT.
   =============================================================================
*/

/* #define	DEBUGGER	define to enable debugging in all MT code */

#define	MT_NilSR	(unsigned)0x2200	/* SR value for _MT_Nil() */
#define	MT_DfPri	(unsigned)0x4000	/* default task priority */

#define	MTF_RDY		0x0001	/* task ready to run  (on MT_RdyQ) */
#define	MTF_SWT		0x0002	/* task waiting on a semaphore */
#define	MTF_RUN		0x0004	/* task running (MT_CurP) */
#define	MTF_STP		0x0008	/* task to be stopped  (waiting) */

#define	MTF_OCC		0x8000	/* TCB occupied */

/* 
*/

typedef long SEM;		/* Semaphore */

typedef struct _mt_msg
{				/* Message */

  struct _mt_msg *next;		/* pointer to next message in queue */
  SEM reply;			/* reply semaphore */
  long msize;			/* message size */
  long mdata;			/* message data or pointer */
} MSG;

typedef struct _mt_mbx
{				/* Mailbox */

  SEM mail;			/* mail waiting semaphore */
  SEM mutex;			/* mutual exclusion semphore */
  MSG *head;			/* pointer to head of message queue */
  MSG *tail;			/* pointer to tail of message queue */
} MBOX;

typedef struct _mt_tcb
{				/* Task Control Block */

  struct _mt_tcb *next;		/* pointer to next TCB in queue */
  struct _mt_tcb *fwd;		/* pointer to next TCB in chain */
  unsigned tid;			/* task identifier */
  unsigned pri;			/* task priority */
  long slice;			/* time slice */
  long reg[16];			/* registers */
  long sp;			/* stack pointer */
  long pc;			/* program counter */
  unsigned sr;			/* status register */
  unsigned flags;		/* task status flags */
  long tos;			/* pointer to top of stack */
} TCB;

struct _mt_str
{				/* Multi-Tasker variable structure */

  TCB *TCBs;			/* TCB chain pointer */
  TCB *CurP;			/* current TCB pointer */
  TCB *RdyQ;			/* ready queue pointer */
  unsigned IDct;		/* TCB ID counter */
};

struct _mt_def
{				/* Multi-Tasker interface block */

  struct _mt_str *mtp;		/* pointer to mt_str structure */
  SEM *FCLK;			/* pointer to SemFCLK */
  SEM *Quit;			/* pointer to SemQuit */
  MBOX *Vid;			/* pointer to MSG_Vid */
};
