* ------------------------------------------------------------------------------
* mtfpu.s -- Multi-Tasker null FPU second level interrupt handler
* Version 3 -- 1988-04-13 -- D.N. Lynx Crowe
* Initially, resvec3 points at nullfpu.  MIDAS can change it with a BIOS trap.
* ------------------------------------------------------------------------------
*
		.text
*
		.xdef	__MTInt2	* FPU Second Level Interrupt Handler
		.xdef	nullfpu		* FPU null Third Level Interrupt Handler
*
		.xref	SM_Wait		* Sempahore wait function
*
		.xref	MT_ISEM2	* FPU interrupt semaphore
		.xref	resvec3		* FPU interrupt vector
*
FPUBASE		.equ	$180000		* FPU base address
*
* FPU address offsets and misc. values
*
FPU_CTL		.equ	$4000		* FPU control offset
*
FPU_IN		.equ	$4000		* FPU input address
FPU_CLR		.equ	$6000		* FPU interrupt reset address
*
FPU_RST		.equ	$0015		* FPU reset value
*
		.page
*
* __MTInt2 -- FPU Second Level Interrupt Handler
* --------    ----------------------------------
__MTInt2:	move.l	#MT_ISEM2,-(a7)		* Wait on interrupt semaphore
		jsr	SM_Wait			* ...
		tst.l	(a7)+			*
		movea.l	resvec3,a0		* (*resvec3)()
		jsr	(a0)			* ...
		bra	__MTInt2		* do it again
*
* ------------------------------------------------------------------------------
*
* nullfpu -- FPU null Third Level Interrupt Handler
* -------    --------------------------------------
nullfpu:	movem.l	d0-d0/a0-a0,-(a7)	* Save registers
		movea.l	#FPUBASE,a0		* Setup FPU base address in a0
		move.w	FPU_IN(a0),d0		* Read FPU interrupt port
		andi.l	#$000000FF,d0		* Mask for voice & parameter
		lsl.l	#5,d0			* Shift for word offset
		addi.l	#FPU_CTL,d0		* Add FPU control offset
		move.w	#FPU_RST,0(a0,d0.L)	* Reset the function
		clr.w	FPU_CLR(a0)		* Clear the interrupt
		movem.l	(a7)+,d0-d0/a0-a0	* Restore registers
		rts				* Return to interrupted code
*
		.end
