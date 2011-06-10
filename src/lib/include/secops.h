/*
   =============================================================================
	secops.h -- section and note operation definitions
	Version 5 -- 1988-07-16 -- D.N. Lynx Crowe
   =============================================================================
*/

#define	SOP_NUL		0	/* No section operation pending */
#define	SOP_GO		1	/* GoTo */
#define	SOP_BGN		2	/* Begin */
#define	SOP_END		3	/* End */
#define	SOP_MOV		4	/* Move */
#define	SOP_CPY		5	/* Copy */
#define	SOP_MRG		6	/* Merge */
#define	SOP_STC		7	/* SMPTE */
#define	SOP_DGR		8	/* Del Gr */
#define	SOP_DEV		9	/* Del Ev */
#define	SOP_RMV		10	/* Remove */
#define	SOP_GRP		11	/* ReGrp */

#define	NOP_NUL		0	/* No note operation pending */
#define	NOP_ACC		1	/* Begin Acc */
#define	NOP_NAT		2	/* Begin Nat */
#define	NOP_END		3	/* End Note */
#define	NOP_MVN		4	/* Move Note */
#define	NOP_MVB		5	/* Move Begin */
#define	NOP_MVE		6	/* Move End */
