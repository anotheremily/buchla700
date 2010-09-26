/*
   =============================================================================
	sclock.h -- score clock sources and controls
	Version 1 -- 1988-01-20 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	CK_LOCAL	0
#define	CK_MIDI		1
#define	CK_SMPTE	2
#define	CK_PLS24	3
#define	CK_PLS48	4
#define	CK_PLS96	5
#define	CK_STEP		6

#define	TIME_CRX	io_time[0]
#define	TIME_CR2	io_time[2]
#define	TIME_T1H	io_time[4]
#define	TIME_T1L	io_time[6]
#define	TIME_T2H	io_time[8]
#define	TIME_T2L	io_time[10]
#define	TIME_T3H	io_time[12]
#define	TIME_T3L	io_time[14]
