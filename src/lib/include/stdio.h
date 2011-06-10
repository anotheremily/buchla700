/*
   =============================================================================
	stdio.h -- Standard I/O Package header for the Buchla 700
	Version 6 -- 1987-10-20 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "fspars.h"		/* file system parameters */

#define NULL	0
#define EOF	-1

#define	fgetc	getc
#define	fputc	putc

#define	_BUSY	0x01
#define	_ALLBUF	0x02
#define	_DIRTY	0x04
#define	_EOF	0x08
#define	_IOERR	0x10

typedef struct
{

  char *_bp;			/* current position in buffer */
  char *_bend;			/* last character in buffer + 1 */
  char *_buff;			/* address of buffer */
  char _flags;			/* {_BUSY, _ALLBUF, _DIRTY, _EOF, _IOERR} */
  char _unit;			/* fd token returned by open */
  char _bytbuf;			/* single byte buffer for unbuffered streams */
  char _pad;			/* pad for alignment -- possibly use later */
  int _buflen;			/* length of buffer */

} FILE;

#ifndef	_FS_DEF_

extern FILE Cbuffs[NSTREAMS];	/* table of FILE structures */
extern char *Stdbufs;		/* free list of buffers */
extern long Stdbuf[MAXDFILE][BUFSIZL];	/* buffers */

#endif

extern long ftell ();
extern char *gets (), *fgets ();
extern FILE *fopen (), *fopena (), *fopenb ();

#define	stdin	(&Cbuffs[0])
#define	stdout	(&Cbuffs[1])
#define	stderr	(&Cbuffs[2])

#define	getchar()	getc(stdin)
#define	putchar(c)	putc(c, stdout)
#define	feof(fp)	(((fp)->_flags & _EOF) != 0)
#define	ferror(fp)	(((fp)->_flags & _IOERR) != 0)
#define	clearerr(fp)	((fp)->_flags &= ~(_IOERR | _EOF))
#define	fileno(fp)	((fp)->_unit)

#ifndef	O_RDONLY		/* only define these once */

#define	O_RDONLY	0x0000	/* Read-only value  */
#define	O_WRONLY	0x0001	/* Write-only value */
#define	O_RDWR		0x0002	/* Read-write value */

#define O_NDELAY	0x0004	/* Non-blocking I/O flag */
#define O_APPEND	0x0008	/* Append mode flag (write only at end) */

#define O_CREAT		0x0100	/* File creation flag (uses 3rd argument) */
#define O_TRUNC		0x0200	/* File truncation flag */
#define O_EXCL		0x0400	/* Exclusive access flag */

#define	O_RAW		0x8000	/* Raw (binary) I/O flag for getc and putc */

#endif
