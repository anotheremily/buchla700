/*
   =============================================================================
	mtsnap.c -- Multi-Tasker -- snap dump the multitasker structures
	Version 22 -- 1988-04-17 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "mtdefs.h"
#include "iorec.h"
#include "debug.h"

extern unsigned MT_IDct;
extern unsigned *MT_NTpc;
extern unsigned *MT_STpc;
extern unsigned *MT_LSpc;
extern unsigned *MT_LWpc;

extern TCB *MT_LTCB;
extern TCB *MT_TCBs;
extern TCB *MT_CurP;
extern TCB *MT_RdyQ;
extern TCB *MT_ITT[];

extern SEM MTISem1;
extern SEM MTISem2;
extern SEM MTISem3;
extern SEM MTISem4;
extern SEM MTISem5;
extern SEM SemAPI;
extern SEM SemFCLK;
extern SEM SemTick;

extern struct iorec sr1_ior;
extern struct iorec sr2_ior;
extern struct iorec mc1_ior;
extern struct iorec mc2_ior;

/* 
*/

/*
   =============================================================================
	MTPrFlg() -- convert TCB flags to printable form
   =============================================================================
*/

char *
MTPrFlg (flags, flagbuf)
     short flags;
     char *flagbuf;
{

  sprintf (flagbuf, "$%04.4X {", flags);

  if (flags & MTF_OCC)
    strcat (flagbuf, " OCC");

  if (flags & MTF_RDY)
    strcat (flagbuf, " RDY");

  if (flags & MTF_SWT)
    strcat (flagbuf, " SWT");

  if (flags & MTF_RUN)
    strcat (flagbuf, " RUN");

  if (flags & MTF_STP)
    strcat (flagbuf, " STP");

  strcat (flagbuf, " }");

  return (flagbuf);
}

/* 
*/

/*
   =============================================================================
	MTPrTCB() -- print the contents of a TCB in readable form
   =============================================================================
*/

MTPrTCB (ptcb, msg)
     register TCB *ptcb;
     char *msg;
{
  register unsigned sreg;
  register short i;
  char flbuf[64];

  if (msg != (char *)NULL)
      printf ("%s\n", msg);

  printf ("TCB Addr: $%08.8lX   Next: $%08.8lX   FWD: $%08.8lX\n",
	  ptcb, ptcb->next, ptcb->fwd);

  printf ("TID: %5u   Pri: %5u ($%04.4X)   Flags: %s\n",
	  ptcb->tid, ptcb->pri, ptcb->pri, MTPrFlg (ptcb->flags, flbuf));

  printf ("Regs   ");

  for (i = 0; i < 8; i++)
    printf (" %d_______", i);

  printf ("\nd0..d7 ");

  for (i = 0; i < 8; i++)
    printf (" %08.8lX", ptcb->reg[i]);

  printf ("\na0..a7 ");

  for (i = 8; i < 16; i++)
    printf (" %08.8lX", ptcb->reg[i]);

  sreg = ptcb->sr;
  sprintf (flbuf, "(IPL = %d, ", (sreg >> 8) & 7);
  strcat (flbuf, 0x8000 & sreg ? "T" : "-");
  strcat (flbuf, 0x2000 & sreg ? "S " : "- ");
  strcat (flbuf, 0x0010 & sreg ? "X" : "-");
  strcat (flbuf, 0x0008 & sreg ? "N" : "-");
  strcat (flbuf, 0x0004 & sreg ? "Z" : "-");
  strcat (flbuf, 0x0002 & sreg ? "V" : "-");
  strcat (flbuf, 0x0001 & sreg ? "C )" : "- )");

  printf ("\nPC = $%08.8lX  SR = $%04.4X %s\n", ptcb->pc, sreg, flbuf);

  printf ("SP: $%08.8lX   TOS: $%08.8lX   Slice:  %ld\n\n",
	  ptcb->sp, ptcb->tos, ptcb->slice);
}

/* 
*/

/*
   =============================================================================
	MTPrTst() -- print the contents of a TCB in readable (short) form
   =============================================================================
*/

MTPrTst (ptcb)
     register TCB *ptcb;
{
  char flbuf[64];

  printf ("$%08.8lX  $%08.8lX  $%04.4X", ptcb, ptcb->pc, ptcb->sr);

  printf ("  %5u  %5u  %s\n",
	  ptcb->tid, ptcb->pri, MTPrFlg (ptcb->flags, flbuf));
}

/*
   =============================================================================
	MTPrQ() -- print a TCB queue
   =============================================================================
*/

MTPrQ (tcp)
     register TCB *tcp;
{
  printf ("TCB______  PC_______  SR___  TID__  PRI__  Flags\n");

  while (tcp)
    {

      MTPrTst (tcp);
      tcp = tcp->next;
    }

  printf ("\n");
}

/* 
*/

/*
   =============================================================================
	MTPrTS() -- print the TCB chain -- short form
   =============================================================================
*/

MTPrTS ()
{
  register TCB *tcp;

  printf ("***** TCB Chain *****\n\n");

  printf ("TCB______  PC_______  SR___  TID__  PRI__  Flags\n");

  tcp = MT_TCBs;

  while (tcp)
    {

      MTPrTst (tcp);
      tcp = tcp->fwd;
    }

  printf ("\n");
}

/*
   =============================================================================
	MTPrTL() -- print the TCB chain -- long form
   =============================================================================
*/

MTPrTL ()
{
  register TCB *tcp;

  printf ("***** TCB Chain *****\n\n");

  tcp = MT_TCBs;

  while (tcp)
    {

      MTPrTCB (tcp, (char *) 0L);
      tcp = tcp->fwd;
    }

  printf ("\n");
}

/* 
*/

/*
   =============================================================================
	MTPrRq() -- print the ready queue
   =============================================================================
*/

MTPrRq ()
{
  if (MT_RdyQ)
    {

      printf ("***** Ready Queue *****\n\n");

      MTPrQ (MT_RdyQ);		/* print the ready queue */

    }
  else
    {

      printf ("***** Ready Queue is EMPTY *****\n\n");
    }
}

/*
   =============================================================================
	MTSnap() -- snap dump the Multi-Tasker's status
   =============================================================================
*/

MTSnap ()
{
  printf ("=============== Multi-Tasker Status ===============\n\n");

  printf ("   MT_CurP: $%08.8lX   MT_RdyQ: $%08.8lX   MT_TCBs: $%08.8lX\n",
	  MT_CurP, MT_RdyQ, MT_TCBs);

  printf ("   MT_IDct:  %8u   MT_STpc: $%08.8lX   MT_NTpc: $%08.8lX\n",
	  MT_IDct, MT_STpc, MT_NTpc);

  printf ("   MT_LWpc: $%08.8lX   MT_LSpc: $%08.8lX\n", MT_LWpc, MT_LSpc);

  printf ("   MT_LTCB: $%08.8lX\n\n", MT_LTCB);

  MTPrTCB (MT_CurP, "*** Current TCB ***   (MT_CurP)\n");


  printf ("\n========== End of Multi-Tasker Status ==========\n\n");
}

/* 
*/

/*
   =============================================================================
	MTSDump() -- snap dump a semaphore
   =============================================================================
*/

MTSDump (sem, msg)
     SEM *sem;
     char *msg;
{
  register long sv;
  register TCB *tcp;

  printf ("SEMAPHORE at $%08.8lX = $%08.8lX", sem, *sem);

  if ((char *) 0L != msg)
    printf (" --  %s", msg);

  printf ("\n");

  sv = (long) *sem;

  if (sv)
    {

      if (sv & 1L)
	{

	  printf ("   count = %ld\n\n", sv >> 1);

	}
      else
	{

	  printf ("   not-signalled -- TCBs waiting:\n\n");
	  MTPrQ (sv);
	}

    }
  else
    {

      printf ("   count = 0  (LSB = 0)\n\n");
    }
}

/* 
*/

/*
   =============================================================================
	DumpIOR() -- snap dump I/O control record  (iorec structure)
   =============================================================================
*/

DumpIOR (ip, msg)
     struct iorec *ip;
     char *msg;
{
  printf ("########## IOREC at $%08.8lX", ip);

  if ((char *) 0L != msg)
    printf (" -- %s", msg);

  printf (" ##########\n");

  printf ("   ibuf     $%08.8lX", ip->ibuf);
  printf ("   ibsize   %5u", ip->ibsize);
  printf ("   obuf     $%08.8lX", ip->obuf);
  printf ("   obsize   %5u\n", ip->obsize);

  printf ("       ibufhd   %5u", ip->ibufhd);
  printf ("   ibuftl   %5u", ip->ibuftl);
  printf ("   ibuflow  %5u", ip->ibuflow);
  printf ("   ibufhi   %5u\n", ip->ibufhi);

  printf ("       obufhd   %5u", ip->obufhd);
  printf ("   obuftl   %5u", ip->obuftl);
  printf ("   obuflow  %5u", ip->obuflow);
  printf ("   obufhi   %5u\n", ip->obufhi);

  printf ("   cfr0     $%02.2X", ip->cfr0 & 0x00FF);
  printf ("   cfr1     $%02.2X", ip->cfr1 & 0x00FF);
  printf ("   flagxon  $%02.2X", ip->flagxon & 0x00FF);
  printf ("   flagxof  $%02.2X\n", ip->flagxof & 0x00FF);

  printf ("   linedis  $%02.2X", ip->linedis & 0x00FF);
  printf ("   erbyte   $%02.2X", ip->erbyte & 0x00FF);
  printf ("   isr      $%02.2X", ip->isr & 0x00FF);
  printf ("   csr      $%02.2X\n", ip->csr & 0x00FF);

  printf ("   errct    %5u", ip->errct);
  printf ("   ibfct    %5u\n\n", ip->ibfct);

  MTSDump (&ip->inp_nf, "inp_nf");
  MTSDump (&ip->inp_ne, "inp_ne");
  MTSDump (&ip->out_nf, "out_nf");
  MTSDump (&ip->out_ne, "out_ne");

  printf ("########## End of IOREC at $%08.8lX ##########\n\n", ip);
}

/* 
*/

/*
   =============================================================================
	MTIDump() -- snap dump interrupt TCBs and semaphores
   =============================================================================
*/

MTIDump ()
{
  register TCB *tcp;
  register short i;
  char buf[81];
  printf ("========== I/O System Status ==========\n");

  printf ("\n----- Interrupt TCBs -----\n\n");

  for (i = 0; i < 8; i++)
    {

      if (tcp = MT_ITT[i])
	{

	  sprintf (buf, "** Level %d Interrupt TCB **", i);
	  MTPrTCB (tcp, buf);
	}
    }

  printf ("\n----- Internal Interrupt Semaphores -----\n\n");

  MTSDump (&MTISem1, "1 - VSDD");
  MTSDump (&MTISem2, "2 - FPU");
  MTSDump (&MTISem3, "3 - panel");
  MTSDump (&MTISem4, "4 - timer");
  MTSDump (&MTISem5, "5 - SIO");

  printf ("\n----- External Interrupt Semaphores -----\n\n");

  MTSDump (&SemTick, "SemTick");
  MTSDump (&SemFCLK, "SemFCLK");
  MTSDump (&SemAPI, "SemAPI");

  printf ("\n----- I/O Control Records -----\n\n");

  DumpIOR (&sr1_ior, "Serial-1");
  DumpIOR (&sr2_ior, "Serial-2");
  DumpIOR (&mc1_ior, "MIDI-1");
  DumpIOR (&mc2_ior, "MIDI-2");

  printf ("\n========== End of I/O System Status ==========\n\n");
}

/* 
*/

/*
   =============================================================================
	MTPanic() -- snap dump the Multi-Tasker's status and trap to ROMP
   =============================================================================
*/

MTPanic ()
{
  MTSnap ();
  MTPrRq ();
  MTIDump ();
  MTPrTL ();
  xtrap15 ();
}
