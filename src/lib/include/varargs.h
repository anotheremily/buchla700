/*
   =============================================================================
	varargs.h -- variable argument list macros
	Version 3 -- 1987-06-16 -- D.N. Lynx Crowe

	WARNING:  Be sure that the 'mode' in the sizeof(mode) in va_arg
	matches the size of the actual argument on the stack.  If it doesn't,
	improper argument list fetches will occur.
	(Lattice has problems with this, as all arguments except double are
	the same size.)
   =============================================================================
*/

typedef char *va_list;

#define	va_dcl	int	va_alist;
#define	va_start(list)	list = (char *) &va_alist
#define	va_end(list)

#define	va_arg(list,mode)	((mode *)(list += sizeof(mode)))[-1]
