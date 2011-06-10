/*
   =============================================================================
	fpu.h -- FPU definitions
	Version 4 -- 1987-11-05 -- D.N. Lynx Crowe
   =============================================================================
*/

/* preprocessor functions */

#define	FPUVAL(x)	((x) << 5)

/* FPU data areas (word offsets) */

#define	FPU_OWST	0x00000000L	/* waveshape tables */
#define	FPU_OFNC	0x00002000L	/* functions */
#define	FPU_OINT	0x00002000L	/* interrupt read address */
#define	FPU_OICL	0x00003000L	/* interrupt clear address */
#define	FPU_OCFG	0x00002FF0L	/* configuration words */
#define	FPU_OSYN	0x00002FFFL	/* sync word */

/* FPU time limits */

#define	FPU_MAXT	0xFFF0	/* FPU maximum time (65520 ms) */
#define	FPU_MINT	0x800F	/* FPU minimum time (1 ms) */

/* FPU value limits */

#define	VALMAX		(1000 << 5)
#define	VALMIN		(-1000 << 5)

/* FPU data types */

#define	FPU_TCTL	0	/* control */
#define	FPU_TNV0	1	/* new value[0] -- '10' in new value select */
#define	FPU_TCV1	4	/* voltage 1 */
#define	FPU_TSF1	5	/* scale factor 1 */
#define	FPU_TCV2	6	/* voltage 2 */
#define	FPU_TSF2	7	/* scale factor 2 */
#define	FPU_TCV3	8	/* voltage 3 */
#define	FPU_TSF3	9	/* scale factor 3 */
#define	FPU_TMNT	10	/* time mantissa */
#define	FPU_TEXP	11	/* time exponent */
#define	FPU_TNV1	14	/* new value[1] -- '01' in new value select */
