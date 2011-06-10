/*
   =============================================================================
	stderrs.c -- standard error message list
	Version 2 -- 1989-01-10 -- D.N. Lynx Crowe

	Set up for GEMDOS, which is almost like Unix(tm).
   =============================================================================
*/

#define	MAXERRNO	36	/* maximum error number in s_errl[] */

char *s_errl[] = {		/* standard system error messages (errno.h) */

  /* 0 */ "",
  /* 1 */ "EPERM - Not owner",
  /* 2 */ "ENOENT - No such file or directory",
  /* 3 */ "ESRCH - No such process",
  /* 4 */ "EINTR - Interrupted system call",
  /* 5 */ "EIO - I/O Error",
  /* 6 */ "ENXIO - No such device or address",
  /* 7 */ "E2BIG - Arg list too long",
  /* 8 */ "ENOEXEC - Exec format error",
  /* 9 */ "EBADF - Bad file number",
  /* 10 */ "ECHILD - No child processes",
  /* 11 */ "EAGAIN - No more processes",
  /* 12 */ "ENOMEM - Not enough space",
  /* 13 */ "EACCES - Permission denied",
  /* 14 */ "EFAULT - Bad address",
  /* 15 */ "ENOTBLK - Block device required",
  /* 16 */ "EBUSY - Device or resource busy",
  /* 17 */ "EEXIST - File exists",
  /* 18 */ "EXDEV - Cross-device link",
  /* 19 */ "ENODEV - No such device",
  /* 20 */ "ENOTDIR - Not a directory",
  /* 21 */ "EISDIR - File is a directory",
  /* 22 */ "EINVAL - Invalid argument",
  /* 23 */ "ENFILE - File table overflow",
  /* 24 */ "EMFILE - Too many open files",
  /* 25 */ "ENOTTY - Not a character device",
  /* 26 */ "ETXTBSY - Text file busy",
  /* 27 */ "EFBIG - File too large",
  /* 28 */ "ENOSPC - No space left on device",
  /* 29 */ "ESPIPE - Illegal seek on a pipe",
  /* 30 */ "EROFS - Read-only file system",
  /* 31 */ "EMLINK - Too many links",
  /* 32 */ "EPIPE - Broken pipe",

/* math software */

  /* 33 */ "EDOM - Bad math argument",
  /* 34 */ "ERANGE - Result too large",

/* hereafter is available for specials */

  /* 35 */ "ENODSPC - No directory space",
  /* 36 */ "ERENAME - Rename error",
};

int s_nerr = MAXERRNO;		/* maximum error number we know about */
