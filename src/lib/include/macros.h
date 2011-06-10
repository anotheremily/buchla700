/*
   =============================================================================
	macros.h -- utility macros
	Version 3 -- 1988-10-19 -- D.N. Lynx Crowe

	All of the usual comments about side-effects apply.
   =============================================================================
*/

#ifndef	abs
#define	abs(x)		((x) < 0 ? -(x) : (x))
#endif

#ifndef	sign
#define	sign(x,y)	((x) < 0 ? -(y) : (y))
#endif

#ifndef	min
#define	min(x,y)	((x) > (y) ? (y) : (x))
#endif

#ifndef	max
#define	max(x,y)	((x) > (y) ? (x) : (y))
#endif

#ifndef	inrange
#define	inrange(var,lo,hi)	(((var) >= (lo)) && ((var) <= (hi)))
#endif
