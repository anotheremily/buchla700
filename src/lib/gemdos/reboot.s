* ------------------------------------------------------------------------------
* reboot.s -- re-boot the system
* Version 3 -- 1988-10-03 -- D.N. Lynx Crowe
*
* Since the Atari likes to do warm starts when a reset is attempted, we
* have to zap low RAM to get rid of optional installed device drivers
* we don't want any more.  We also do a 'reset' just to be sure.
* ------------------------------------------------------------------------------
		.text
*
		.xdef	reboot
*
reboot:		clr.l	-(sp)		* get into supervisor state
		move.w	#$20,-(sp)	* ...
		trap	#1		* ...
*
		move.w	#$2700,sr	* disable interrupts
*
		move.w	#1022,d0	* set the zap loop count
		movea.l	#8,a0		* set the starting address		
*
rb1:		clr.l	(a0)+		* zap a long word
		dbra	d0,rb1		* loop until all are cleared
*
		reset			* reset the hardware
*
		movea.l	4,a0		* get boot pc
		jmp	(a0)		* go to the boot PROM
*
		.end
