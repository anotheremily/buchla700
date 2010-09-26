/*
   ============================================================================
	putc.c -- stream file output and close functions
	Version 7 -- 1987-10-19 -- D.N. Lynx Crowe
   ============================================================================
*/

#include "stdio.h"
#include "stddefs.h"

extern	int	write(), getbuff(), close();

extern int (*_clsall)();

static int (*cls_rtn)();

int	_ClFlag;

int	fclose();

/*
   ============================================================================
	closall() -- close all files at exit
   ============================================================================
*/

static
closall()
{
	register FILE *fp;

	for (fp = Cbuffs; fp < (Cbuffs + NSTREAMS); )	/* close each file */
		fclose(fp++);

	(*cls_rtn)();		/* do final closeout */
}

/* 
*/

/*
   ============================================================================
	flush_() -- do the dirty work of flushing a file
   ============================================================================
*/

int
flush_(ptr, data)
register FILE *ptr;
int data;
{
	register int size;

	if (_ClFlag EQ 0) {

		cls_rtn = _clsall;
		_clsall = closall;
		_ClFlag = 1;
	}

	if (ptr->_flags & _DIRTY) {	/* something in the buffer ? */

		size = (int)((long)ptr->_bp - (long)ptr->_buff);

		if (write(ptr->_unit, ptr->_buff, size) EQ -1) {

ioerr:
			ptr->_flags |= _IOERR;
			return(EOF);
		}
	}

	if (data EQ -1) {	/* just flushing, not adding data */

		ptr->_flags &= ~_DIRTY;
		ptr->_bend = ptr->_bp = NULL;
		return(0);
	}

	if (ptr->_buff EQ NULL)		/* get a buffer if we don't have one */
		getbuff(ptr);

	if (ptr->_buflen EQ 1) {	/* unbuffered I/O */

		if (write(ptr->_unit, &data, 1) EQ -1)
			goto ioerr;

		return(data);
	}

	ptr->_bp = ptr->_buff;
	ptr->_bend = ptr->_buff + ptr->_buflen;
	ptr->_flags |= _DIRTY;

	return((*ptr->_bp++ = data) & 0x00FF);
}

/* 
*/

/*
   ============================================================================
	fflush() -- flush a stream file
   ============================================================================
*/

int
fflush(ptr)
FILE *ptr;
{
	return(flush_(ptr, -1));
}

/*
   ============================================================================
	fclose() -- close a stream file
   ============================================================================
*/

int
fclose(ptr)
register FILE *ptr;
{
	int err;

	err = 0;

	if (ptr->_flags) {

		if (ptr->_flags & _DIRTY)	/* if modifed, flush buffer */
			err = flush_(ptr, -1);

		err |= close(ptr->_unit);

		if (ptr->_flags & _ALLBUF) {	/* deallocate standard buffer */

			*(long **)ptr->_buff = Stdbufs;
			Stdbufs = (long *)ptr->_buff;
		}
	}

	ptr->_flags = 0;
	return(err);
}

/* 
*/

/*
   ============================================================================
	putc() -- write a character on a file
   ============================================================================
*/

int
putc(c, ptr)
int c;
register FILE *ptr;
{
	if (ptr->_bp GE ptr->_bend)
		return(flush_(ptr, c & 0xFF));

	return((*ptr->_bp++ = c) & 0xFF);
}

/*
   ============================================================================
	puterr() -- write a character to stderr
   ============================================================================
*/

int
puterr(c)
{
	return(putc(c, stderr));
}

