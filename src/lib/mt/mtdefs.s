* ------------------------------------------------------------------------------
* mtdefs.s -- Multi-Tasker variable definitions
* Version 17 -- 1988-04-17 -- D.N. Lynx Crowe
* (c) Copyright 1988 -- D.N. Lynx Crowe
* ------------------------------------------------------------------------------
DEBUGGER	.equ	0		* define non-zero for debug
* ------------------------------------------------------------------------------
		.bss
* ------------------------------------------------------------------------------
		.xdef	MTStruct	* multi-takser system RAM
		.xdef	MT_END		* end of multi-tasker system RAM
*
		.xdef	_MSG_Vid	* video mailbox structure
*
		.xdef	_MT_TCBs	* pointer to base of TCB pool
		.xdef	_MT_CurP	* current TCB pointer
		.xdef	_MT_RdyQ	* ready queue pointer
		.xdef	_MT_IDct	* task ID counter
*
		.xdef	MT_ITCB1	* Interrupt level 1 TCB
		.xdef	MT_ITCB2	* Interrupt level 2 TCB
		.xdef	MT_ITCB3	* Interrupt level 3 TCB
		.xdef	MT_ITCB4	* Interrupt level 4 TCB
		.xdef	MT_ITCB5	* Interrupt level 5 TCB
		.xdef	__MTptcb	* Private TCBs
*
		.xdef	MT_ISEM1	* Interrupt level 1 semaphore
		.xdef	_MTISem1
		.xdef	MT_ISEM2	* Interrupt level 2 semaphore
		.xdef	_MTISem2
		.xdef	MT_ISEM3	* Interrupt level 3 semaphore
		.xdef	_MTISem3
		.xdef	MT_ISEM4	* Interrupt level 4 semaphore
		.xdef	_MTISem4
		.xdef	MT_ISEM5	* Interrupt level 5 semaphore
		.xdef	_MTISem5
		.xdef	_SemTick	* Timer semaphore  (1MS ticks)
		.xdef	_SemFCLK	* Score frame clock semaphore
		.xdef	_SemAPI		* Analog FIFO not-empty semaphore
		.xdef	_SemQuit
*
		.xdef	__MT_Vc1	* Saved TRAP 8 vector
		.xdef	__MT_Vc2	* Saved TRAP 9 vector
		.xdef	_MT_NTpc	* Logged PC for 'next' trap
		.xdef	_MT_STpc	* Logged PC for 'swap' trap
		.xdef	_MT_LTCB	* Last TCB address from MT_CurP
		.xdef	_MT_LSpc	* Last PC from SM_Sig
		.xdef	_MT_LWpc	* Last PC from SM_Wait
*
		.xdef	_sr1_ior	* Serial port 1 iorec
		.xdef	_sr2_ior	* Serial port 2 iorec
		.xdef	_mc1_ior	* MIDI port 1 iorec
		.xdef	_mc2_ior	* MIDI port 2 iorec
*
		.page
*
		.ifeq	DEBUGGER
*
		.xdef	MTVARS		* second part of MT variables
*
		.endc
*
* ------------------------------------------------------------------------------
TCBLEN_B	.equ	96		* length of a TCB in bytes
TCBLEN_W	.equ	(TCBLEN_B/2)	* length of a TCB in words
*
IORECLN		.equ	56		* length of an iorec structure in bytes
MBOXLEN		.equ	16		* length of an MBOX structure in bytes
* ------------------------------------------------------------------------------
		.page
*
		.ifne	DEBUGGER
*
MT_base		.equ	$50000			* base of Multi-Tasker RAM area
*
		.endc
*
		.ifeq	DEBUGGER
*
MT_base:	.ds.w	7			* base of Multi-Tasker RAM area
*
		.endc
*
* Multi-Tasker control variables    (MTStruct)
* ------------------------------
_MT_TCBs	.equ	MT_base			* LONG - TCB pool chain pointer
_MT_CurP	.equ	_MT_TCBs+4		* LONG - pointer to current TCB
_MT_RdyQ	.equ	_MT_CurP+4		* LONG - head of ready queue
_MT_IDct	.equ	_MT_RdyQ+4		* WORD - task ID counter
*
* Interrupt level TCBs
* --------------------
MT_ITCB1	.equ	_MT_IDct+2			* Level 1 -- VSDD
MT_ITCB2	.equ	MT_ITCB1+TCBLEN_B		* Level 2 -- FPU
MT_ITCB3	.equ	MT_ITCB2+TCBLEN_B		* Level 3 -- Analog
MT_ITCB4	.equ	MT_ITCB3+TCBLEN_B		* Level 4 -- Timer
MT_ITCB5	.equ	MT_ITCB4+TCBLEN_B		* Level 5 -- Serial I/O
*
__MTptcb	.equ	MT_ITCB5+TCBLEN_B		* Private TCBs
*
* Interrupt level sempahores
* --------------------------
MT_ISEM1	.equ	__MTptcb+(2*TCBLEN_B)	* LONG - VSDD
MT_ISEM2	.equ	MT_ISEM1+4		* LONG - FPU
MT_ISEM3	.equ	MT_ISEM2+4		* LONG - analog
MT_ISEM4	.equ	MT_ISEM3+4		* LONG - timer
MT_ISEM5	.equ	MT_ISEM4+4		* LONG - serial I/O
*
* Other multi-tasker variables
* ----------------------------
_SemTick	.equ	MT_ISEM5+4		* LONG - 1 Ms Timer semaphore
_SemFCLK	.equ	_SemTick+4		* LONG - Frame clock semaphore
_SemAPI		.equ	_SemFCLK+4		* LONG - Analog FIFO semaphore
_SemQuit	.equ	_SemAPI+4		* LONG - Terminate semaphore
*
_MSG_Vid	.equ	_SemQuit+4		* STRUCT - Video mailbox
*
__MT_Vc1	.equ	_MSG_Vid+MBOXLEN	* LONG - Save TRAP 8 vector
__MT_Vc2	.equ	__MT_Vc1+4		* LONG - Save TRAP 8 vector
_MT_NTpc	.equ	__MT_Vc2+4		* LONG - Logged PC for 'next'
_MT_STpc	.equ	_MT_NTpc+4		* LONG - Logged PC for 'swap'
_MT_LTCB	.equ	_MT_STpc+4		* LONG - Last TCB address
_MT_LSpc	.equ	_MT_LTCB+4		* LONG - Last PC from SM_Sig
_MT_LWpc	.equ	_MT_LSpc+4		* LONG - Last PC from SM_Wait
*
_sr1_ior	.equ	_MT_LWpc+4		* STRUCT - Serial-1 iorec
_sr2_ior	.equ	_sr1_ior+IORECLN	* STRUCT - Serial-2 iorec
_mc1_ior	.equ	_sr2_ior+IORECLN	* STRUCT - MIDI-1 iorec
_mc2_ior	.equ	_mc1_ior+IORECLN	* STRUCT - MIDI-2 iorec
*
MT_END		.equ	_mc2_ior+IORECLN
MT_LEN		.equ	MT_END-MT_base
*
_MTISem1	.equ	MT_ISEM1
_MTISem2	.equ	MT_ISEM2
_MTISem3	.equ	MT_ISEM3
_MTISem4	.equ	MT_ISEM4
_MTISem5	.equ	MT_ISEM5
*
MTStruct	.equ	MT_base
* ------------------------------------------------------------------------------
*
		.ifeq	DEBUGGER
*
MTVARS:		.ds.b	MT_LEN-14
*
		.endc
*
		.end
