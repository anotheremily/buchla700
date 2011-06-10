/*
   ============================================================================
	ctype.c -- extended character type table, ala' Unix(tm)
	Version 8 -- 1987-06-16 -- D.N. Lynx Crowe

	This character type table supports the extended ctype.h macros.
	See the ctype.h header file for further notes.

	WARNING:  SYS5CODE must be defined properly in ctype.h for the macros
	to work with this table.
   ============================================================================
*/

#define	_CTYPE_C

#include "ctype.h"

#define	_CS	_C|_S
#define	_PS	_P|_S
#define	_NX	_N|_X
#define	_UX	_U|_X
#define	_LX	_L|_X

/* 
*/

#if	SYS5CODE
char _ctype[] = {		/* this table handles EOF as a legal value */

				/* -1   EOF */ 0,
				/* -1 EOF */
#else

___atab ()
{
  return;
}				/* dummy function to force the loader to see __atab */

char __atab[] = {		/* EOF won't work properly with this table */
#endif

  /* Octal *//* Hex */
							/* 000..007 */ _C, _C, _C, _C, _C, _C, _C, _C,
							/* 00..07 */
							/* 010..017 */ _C, _CS, _CS, _CS, _CS, _CS, _C, _C,
							/* 08..0F */
							/* 020..027 */ _C, _C, _C, _C, _C, _C, _C, _C,
							/* 10..17 */
							/* 030..037 */ _C, _C, _C, _C, _C, _C, _C, _C,
							/* 18..1F */
							/* 040..047 */ _PS, _P, _P, _P, _P, _P, _P, _P,
							/* 20..27 */
							/* 050..057 */ _P, _P, _P, _P, _P, _P, _P, _P,
							/* 28..2F */
								/* 060..067 */ _NX, _NX, _NX, _NX, _NX, _NX, _NX, _NX,
								/* 30..37 */
							/* 070..077 */ _NX, _NX, _P, _P, _P, _P, _P, _P,
							/* 38..3F */

							/* 100..107 */ _P, _UX, _UX, _UX, _UX, _UX, _UX, _U,
							/* 40..47 */
							/* 110..117 */ _U, _U, _U, _U, _U, _U, _U, _U,
							/* 48..4F */
							/* 120..127 */ _U, _U, _U, _U, _U, _U, _U, _U,
							/* 50..57 */
							/* 130..137 */ _U, _U, _U, _P, _P, _P, _P, _P,
							/* 58..5F */
							/* 140..147 */ _P, _LX, _LX, _LX, _LX, _LX, _LX, _L,
							/* 60..67 */
							/* 150..157 */ _L, _L, _L, _L, _L, _L, _L, _L,
							/* 68..6F */
							/* 160..167 */ _L, _L, _L, _L, _L, _L, _L, _L,
							/* 70..77 */
							/* 170..177 */ _L, _L, _L, _P, _P, _P, _P, _C,
							/* 78..7F */

							/* 200..207 */ _C, _C, _C, _C, _C, _C, _C, _C,
							/* 80..87 */
							/* 210..217 */ _C, _CS, _CS, _CS, _CS, _CS, _C, _C,
							/* 88..8F */
							/* 220..227 */ _C, _C, _C, _C, _C, _C, _C, _C,
							/* 90..97 */
							/* 230..237 */ _C, _C, _C, _C, _C, _C, _C, _C,
							/* 98..9F */
							/* 240..247 */ _PS, _P, _P, _P, _P, _P, _P, _P,
							/* A0..A7 */
							/* 250..257 */ _P, _P, _P, _P, _P, _P, _P, _P,
							/* A8..AF */
								/* 260..267 */ _NX, _NX, _NX, _NX, _NX, _NX, _NX, _NX,
								/* B0..B7 */
							/* 270..277 */ _NX, _NX, _P, _P, _P, _P, _P, _P,
							/* B8..BF */

							/* 300..307 */ _P, _UX, _UX, _UX, _UX, _UX, _UX, _U,
							/* C0..C7 */
							/* 310..317 */ _U, _U, _U, _U, _U, _U, _U, _U,
							/* C8..CF */
							/* 320..327 */ _U, _U, _U, _U, _U, _U, _U, _U,
							/* D0..D7 */
							/* 330..337 */ _U, _U, _U, _P, _P, _P, _P, _P,
							/* D8..DF */
							/* 340..347 */ _P, _LX, _LX, _LX, _LX, _LX, _LX, _L,
							/* E0..E7 */
							/* 350..357 */ _L, _L, _L, _L, _L, _L, _L, _L,
							/* E8..EF */
							/* 360..367 */ _L, _L, _L, _L, _L, _L, _L, _L,
							/* F0..F7 */
						/* 370..377 */ _L, _L, _L, _P, _P, _P, _P, _C
						/* F8..FF */
};
