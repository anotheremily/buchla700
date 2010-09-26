/*
   ===========================================================================
	graphdef.h -- graphics primitives package header
	Version 6 -- 1987-05-27 -- D.N. Lynx Crowe
	Setup for the Buchla 700 and the Matra-Harris 82716 VSDD
	Copyright 1985,1976,1987 -- D.N. Lynx Crowe
   ===========================================================================
*/

#define CLR 0
#define SET 1

#define XPIX	512
#define YPIX	350

#define XCTR	XPIX/2
#define YCTR	YPIX/2

#define XMAX	XPIX-1
#define YMAX	YPIX-1

#define	P_BLK	0		/* colors for external PROM color table */
#define	P_DKGRY	1
#define	P_DKBLU	2
#define	P_LTBLU	3
#define	P_DKGRN	4
#define	P_LTGRN	5
#define	P_DKCYN	6
#define	P_LTCYN	7
#define	P_RED	8
#define	P_ORG	9
#define	P_PUR	10
#define	P_MGN	11
#define	P_BRN	12
#define	P_YEL	13
#define	P_LTGRY	14
#define	P_WHT	15

#define	DEFCBGND	P_DKBLU
#define	DEFCFGND	P_WHT

#define	DEFPBGND	P_BLK
#define	DEFPFGND	P_DKGRY
