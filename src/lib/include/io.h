/*
   ============================================================================
	io.h -- Buchla 700 I/O library definitions
	Version 12 -- 1987-09-25 -- D.N. Lynx Crowe
   ============================================================================
*/

#include "fspars.h"		/* file system parameters */

struct channel
{				/* channel table entry */

  char c_read;			/* read routine index */
  char c_write;			/* write routine index */
  char c_ioctl;			/* ioctl routine index */
  char c_seek;			/* seek routine index */
  int (*c_close) ();		/* close function pointer */
  io_arg c_arg;			/* argument to channel driver */
};

struct device
{				/* device control structure */

  char d_read;			/* read routine code */
  char d_write;			/* write routine code */
  char d_ioctl;			/* ioctl routine code */
  char d_seek;			/* seek routine code */
  int (*d_open) ();		/* special open function */
};

struct devtabl
{				/* device table entry */

  char *d_name;			/* device name */
  struct device *d_dev;		/* pointer to device structure */
  io_arg d_arg;			/* argument to device driver */
};

#ifndef	_FS_DEF_

extern struct channel chantab[MAXCHAN];	/* defined in fsinit.c */
extern char Wrkbuf[BPSEC];	/* defined in fsinit.c */

#endif
