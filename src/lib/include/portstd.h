/*
   ============================================================================
	portstd.h -- Some standard definitions for portability
	Version 1 -- 1987-02-05 -- D.N. Lynx Crowe
   ============================================================================
*/

#ifndef PORTA_H
#define PORTA_H

#define CHAR    char
#define SCHAR   char
typedef unsigned char UCHAR;
typedef char BYTE;
typedef unsigned char UBYTE;

#define INT     int
#define SINT    int
typedef unsigned int UINT;
typedef int WORD;
typedef unsigned int UWORD;

#define SHORT   short
#define SSHORT  short
typedef unsigned short USHORT;

#define LONG    long
#define SLONG   long
typedef unsigned long ULONG;

#define DOUBLE  double

#define BOOL    int
#define SUCC_FAIL   int 

#ifndef VOID
#define VOID
#endif

#define STATIC  static      /* Names not needed outside this src module  */

#define LOCAL               /* Names not needed outside this software module */
#define LCL_XTRN extern     /* Names defined within this software module */

#define PUBLIC              /* Names needed outside this software module */
#define EXTERN  extern      /* Names defined outside this software module */

#define TRUE    1
#define FALSE   0

#define SUCCESS 0
#define FAIL    (-1)

#endif PORTA_H
