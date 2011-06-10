/*
   =============================================================================
	fcntl.h -- file control function header
	Version 3 -- 1987-06-26 -- D.N. Lynx Crowe
   =============================================================================
*/

/*
   =============================================================================
	The following symbols are used with open(), creat() and fcntl().
	The first 3 can only be set by open().
   =============================================================================
*/

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

/*
   =============================================================================
	The following symbols define requests used with the fcntl() function.
   =============================================================================
*/

#define	F_DUPFD	0		/* Duplicate file descriptor */

#define	F_GETFD	1		/* Get file descriptor flags */
#define	F_SETFD	2		/* Set file descriptor flags */

#define	F_GETFL	3		/* Get file flags */
#define	F_SETFL	4		/* Set file flags */
