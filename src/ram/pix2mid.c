/*
   =============================================================================
	pix2mid.c -- convert cursor position to MIDI note number and score time
	Version 9 -- 1988-10-27 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"
#include "graphdef.h"
#include "vsdd.h"
#include "score.h"

#include "midas.h"
#include "scfns.h"
#include "scdsp.h"

#define	PCENTER		256	/* pixel offset of 0 time line */

extern short cflag;		/* accidental flag */
extern short cnote;		/* note value at cursor */
extern short cyval;		/* cursor y value */
extern short cxval;		/* cursor x value */

extern long ctime;		/* time at cursor */
extern long t_cur;		/* time at p_cur */

/* 
*/

short mpixtab[52][4] = {	/* pixel to MIDI note for the white keys */

/* [0] = center pixel, [1] = MIDI note, [2] = sharp tag, [3] = flat tag */

  {1, 108, 0, 0},		/* 8C 00 --  0 */


  {5, 107, 0, 1},		/* 7B 00 --  1 */
  {9, 105, 1, 1},		/* 7A 00 --  2 */
  {13, 103, 1, 1},		/* 7G 00 --  3 */
  {17, 101, 1, 0},		/* 7F 00 --  4 */

  {21, 100, 0, 1},		/* 7E 00 --  5 */
  {25, 98, 1, 1},		/* 7D 00 --  6 */
  {29, 96, 1, 0},		/* 7C 00 --  7 */


  {33, 95, 0, 1},		/* 6B 00 --  8 */
  {37, 93, 1, 1},		/* 6A 00 --  9 */
  {41, 91, 1, 1},		/* 6G 00 -- 10 */
  {45, 89, 1, 0},		/* 6F 00 -- 11 */

  {49, 88, 0, 1},		/* 6E 00 -- 12 */
  {53, 86, 1, 1},		/* 6D 00 -- 13 */
  {57, 84, 1, 0},		/* 6C 00 -- 14 */


  {61, 83, 0, 1},		/* 5B 00 -- 15 */
  {65, 81, 1, 1},		/* 5A 00 -- 16 */
  {69, 79, 1, 1},		/* 5G 00 -- 17 */
  {73, 77, 1, 0},		/* 5F 00 -- 18 */

  {77, 76, 0, 1},		/* 5E 00 -- 19 */
  {81, 74, 1, 1},		/* 5D 00 -- 20 */
  {85, 72, 1, 0},		/* 5C 00 -- 21 */

  {89, 71, 0, 1},		/* 4B 00 -- 22 */
  {93, 69, 1, 1},		/* 4A 00 -- 23 */
  {97, 67, 1, 1},		/* 4G 00 -- 24 */
  {101, 65, 1, 0},		/* 4F 00 -- 25 */

  {105, 64, 0, 1},		/* 4E 00 -- 26 */
  {109, 62, 1, 1},		/* 4D 00 -- 27 */
  {113, 60, 1, 0},		/* 4C 00 -- 28 -- Middle C */

/* 
*/

  {117, 59, 0, 1},		/* 3B 00 -- 29 */
  {121, 57, 1, 1},		/* 3A 00 -- 30 */
  {125, 55, 1, 1},		/* 3G 00 -- 31 */
  {129, 53, 1, 0},		/* 3F 00 -- 32 */

  {133, 52, 0, 1},		/* 3E 00 -- 33 */
  {137, 50, 1, 1},		/* 3D 00 -- 34 */
  {141, 48, 1, 0},		/* 3C 00 -- 35 */


  {145, 47, 0, 1},		/* 2B 00 -- 36 */
  {149, 45, 1, 1},		/* 2A 00 -- 37 */
  {153, 43, 1, 1},		/* 2G 00 -- 38 */
  {157, 41, 1, 0},		/* 2F 00 -- 39 */

  {161, 40, 0, 1},		/* 2E 00 -- 40 */
  {165, 38, 1, 1},		/* 2D 00 -- 41 */
  {169, 36, 1, 0},		/* 2C 00 -- 42 */


  {173, 35, 0, 1},		/* 1B 00 -- 43 */
  {177, 33, 1, 1},		/* 1A 00 -- 44 */
  {181, 31, 1, 1},		/* 1G 00 -- 45 */
  {185, 29, 1, 0},		/* 1F 00 -- 46 */

  {189, 28, 0, 1},		/* 1E 00 -- 47 */
  {193, 26, 1, 1},		/* 1D 00 -- 48 */
  {197, 24, 1, 0},		/* 1C 00 -- 49 */


  {201, 23, 0, 1},		/* 0B 00 -- 50 */
  {205, 21, 1, 0}		/* 0A 00 -- 51 */
};

/* 
*/

/*
   =============================================================================
	pix2mid() -- convert (px, py) to MIDI note and score time
   =============================================================================
*/

short
pix2mid ()
{
  register short i, cy, mpc;
  register long ct;

  cnote = -1;
  ctime = -1L;
  cflag = -1;
  mpc = ac_code EQ N_SHARP ? 2 : 3;

  ct = t_cur + ((long) cxval - (long) PCENTER);

  if (ct < 0L)
    return (FAILURE);

  ctime = ct;
  cy = cyval - 14;

  for (i = 0; i < 52; i++)
    {

      if ((cy GE mpixtab[i][0]) AND (cy LE mpixtab[i][0] + 2))
	{

	  cnote = mpixtab[i][1];
	  cflag = mpixtab[i][mpc];
	  break;
	}
    }

  return ((cnote EQ - 1L) ? FAILURE : SUCCESS);
}
