/*
   =============================================================================
	mbinit.c -- Multi-Tasker -- Mailbox support
	Version 7 -- 1988-03-29 -- D.N. Lynx Crowe
	(c) Copyright 1988 -- D.N. LYnx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "mtdefs.h"

extern short setipl ();		/* set processor IPL function */
extern short SMStat ();		/* semaphore check function */

/*
   =============================================================================
	MBInit() -- initialize a mailbox
   =============================================================================
*/

MBInit (pmbox)
     register MBOX *pmbox;
{
  register short oldipl;

  oldipl = setipl (7);		/* DISABLE INTERRUPTS */

  pmbox->mail = (SEM) 1L;	/* reset the mail semaphore */
  pmbox->mutex = (SEM) 3L;	/* reset the mutex semaphore */
  pmbox->head = (MSG *) NIL;	/* clear the MSG queue head pointer */
  pmbox->tail = (MSG *) NIL;	/* clear the MSG queue tail pointer */

  setipl (oldipl);		/* RESTORE INTERRUPTS */
}

/* 
*/

/*
   =============================================================================
	MBSend() -- send a message to a mailbox
   =============================================================================
*/

MBSend (pmbox, pmsg)
     register MBOX *pmbox;
     register MSG *pmsg;
{
  SMWait (&pmbox->mutex);	/* ACQUIRE THE MAILBOX */

  if (pmbox->head)		/* is anything in the  queue ? */
    (pmbox->tail)->next = pmsg;	/* set next of tail message */
  else				/* ... queue was empty */
    pmbox->head = pmsg;		/* set the head pointer */

  pmbox->tail = pmsg;		/* set the tail pointer */
  pmsg->next = (MSG *) NIL;	/* set next of new message */

  SMSig (&pmbox->mail);		/* signal that there's mail */

  SMSig (&pmbox->mutex);	/* RELEASE THE MAILBOX */
}

/* 
*/

/*
   =============================================================================
	MBRecv() -- receive a message from a mailbox
   =============================================================================
*/

MSG *
MBRecv (pmbox)
     register MBOX *pmbox;
{
  register MSG *pmsg;

  SMWait (&pmbox->mail);	/* WAIT FOR SOME MAIL */

  SMWait (&pmbox->mutex);	/* ACQUIRE THE MAILBOX */

  pmsg = pmbox->head;		/* get the first message in the queue */

  if (pmbox->head EQ pmbox->tail)
    {				/* only message ? */

      pmbox->head = (MSG *) NIL;	/* clear queue head pointer */
      pmbox->tail = (MSG *) NIL;	/* clear queue tail pointer */

    }
  else
    {

      pmbox->head = pmsg->next;	/* update queue head pointer */
    }

  pmsg->next = (MSG *) NIL;	/* clear next message pointer */

  SMSig (&pmbox->mutex);	/* RELEASE THE MAILBOX */

  return (pmsg);		/* return the address of the message */
}

/* 
*/

/*
   =============================================================================
	MBChek() -- check for and conditionally receive a message from a mailbox

		returns NIL if no messages were in the mailbox, or the
		address of the first (oldest) message in the mailbox,
		which will be dequeued from the mailbox as if by MBRecv().
   =============================================================================
*/

MSG *
MBChek (pmbox)
     register MBOX *pmbox;
{
  register MSG *pmsg;

  if (SMCWait (&pmbox->mail))
    {				/* try for some mail */

      SMWait (&pmbox->mutex);	/* ACQUIRE THE MAILBOX */

      pmsg = pmbox->head;	/* get the first message in the queue */

      if (pmbox->head EQ pmbox->tail)
	{			/* only message ? */

	  pmbox->head = (MSG *) NIL;	/* clear queue head pointer */
	  pmbox->tail = (MSG *) NIL;	/* clear queue tail pointer */

	}
      else
	{

	  pmbox->head = pmsg->next;	/* update queue head pointer */
	}

      pmsg->next = (MSG *) NIL;	/* clear next message pointer */

      SMSig (&pmbox->mutex);	/* RELEASE THE MAILBOX */

      return (pmsg);		/* return the message pointer */

    }
  else
    {

      return ((MSG *) NIL);	/* return -- no mail */
    }
}

/* 
*/

/*
   =============================================================================
	MBDel() -- delete (cancel) a message from a mailbox

		FAILURE		unable to find the message
		SUCCESS		message found and deleted from mailbox
   =============================================================================
*/

short
MBDel (pmbox, pmsg)
     register MBOX *pmbox;
     register MSG *pmsg;
{
  register MSG *mprv, *mcur;

  if (SMStat (&pmbox->mail) EQ 1)
    {				/* anything in the mailbox ? */

      SMWait (&pmbox->mutex);	/* ACQUIRE THE MAILBOX */

      mcur = pmbox->head;	/* point at the first message */

      while (TRUE)
	{

	  mprv = mcur;		/* previous MSG = current MSG */
	  mcur = mprv->next;	/* current MSG = next MSG */

	  if (mcur EQ (MSG *) NIL)
	    {			/* end of queue ? */

	      SMSig (&pmbox->mutex);	/* RELEASE MAILBOX */

	      return (FAILURE);	/* return -- not found */
	    }

	  if (mcur EQ pmsg)
	    {			/* message we want ? */

	      mprv->next = mcur->next;	/* dequeue it */
	      mcur->next = (MSG *) NIL;	/* ... */

	      SMSig (&pmbox->mutex);	/* RELEASE MAILBOX */

	      return (SUCCESS);	/* return -- deleted */
	    }
	}

    }
  else
    {

      return (FAILURE);		/* return -- mailbox emtpy */
    }
}
