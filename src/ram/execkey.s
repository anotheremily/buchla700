* ------------------------------------------------------------------------------
* execkey.s -- execute key -- FPU function start subroutine
* Version 15 -- 1988-08-29 -- D.N. Lynx Crowe
* Hand optimized version of execkey() and fnstart() from C compiler output
* ------------------------------------------------------------------------------
		.text
*
LOC_EOR		.equ	0
LOC_SUB		.equ	1
*
		.xdef	_execkey
*
		.xref	_xgetran
*
		.xref	_rsntab
		.xref	_expbit
		.xref	_io_fpu
		.xref	_legato
		.xref	_prstab
		.xref	_ptoftab
		.xref	_timemlt
		.xref	_valents
		.xref	_vbufs
		.xref	_vce2grp
		.xref	_vce2trg
		.xref	_veltab
		.xref	_vpsms
*
		.page
*
* Offsets for execkey() parameters
* -------------------------------- 
TRG		.equ	8		* WORD -- trigger
PCH		.equ	10		* WORD -- pitch
VCE		.equ	12		* WORD -- voice  (0..11)
TAG		.equ	14		* WORD -- I_TM_... inversion tag
*
* Register variables
* ------------------
A_FP		.equ	A5		* LONG -- struct idfnhdr *
A_FPU		.equ	A4		* LONG -- FPU base address
A_SMP		.equ	A3		* LONG -- struct sment *
*
R_FPMANT	.equ	D7		* WORD -- FPU time mantissa
R_FPEXP		.equ	D6		* WORD -- FPU time exponent
R_FPCTL		.equ	D5		* WORD -- FPU control word
R_I		.equ	D4		* WORD -- loop index
R_FPVAL		.equ	D3		* WORD -- FPU value
*
* Local (stack) variables
* -----------------------
PT		.equ	-4		* LONG -- instpnt *
VEP		.equ	-8		* LONG -- struct valent *
OLDI		.equ	-10		* WORD -- old IPL  (sr)
OCTYPE		.equ	-12		* WORD -- oscillator 1 mode / scratch
VP		.equ	-14		* WORD -- voice # + function # index
SRCNUM		.equ	-16		* WORD -- general source #
SRCVAL		.equ	-18		* WORD -- general source value
TSRCVAL		.equ	-20		* WORD -- temporary general source value
MLTVAL		.equ	-22		* WORD -- general source multiplier
TFPVAL		.equ	-24		* WORD -- temporary FPU value
LTMP		.equ	-28		* LONG -- long temporary
IP		.equ	-32		* LONG -- struct instdef *
GROUP		.equ	-34		* WORD -- group number << 4
*
* Symbolic constants
* ------------------
LASTLOCL	.equ	GROUP		* last local on the stack
*
FPU_DI		.equ	$2200		* sr value for disabling FPU interrupts
PITCHMAX	.equ	21920		* maximum pitch value
VALMAX		.equ	$00007D00	* maximum FPU value
VALMIN		.equ	$FFFF8300	* minimum FPU value
VBLEN		.equ	3938		* length of a voice buffer
LSPCH		.equ	2		* left shift for pitch sources
*
		.page
*
* execkey(trg, pch, vce)
* short trg, pch, vce;
* {
*
_execkey:	link	A6,#LASTLOCL
		movem.l	D2-R_FPMANT/A_SMP-A_FP,-(sp)
*
*	ip = &vbufs[vce];
*
		move.w	VCE(A6),D0
		asl.w	#2,D0
		lea	vbtab,A0
		move.l	0(A0,D0.W),IP(A6)
*
*	vce2trg[vce] = trg;
*
		lsr.w	#1,D0
		lea	_vce2trg,A0
		move.w	TRG(A6),0(A0,D0.W)
*
*	group = (vce2grp[vce] - 1) << 4;
*
		move.w	VCE(A6),A0
		add.l	A0,A0
		move.l	#_vce2grp,A1
		clr.l	D0
		move.w	0(A0,A1.l),D0
		sub.w	#1,D0
		asl.w	#4,D0
		move.w	D0,GROUP(A6)
*
		.page
* ------------------------------------------------------------------------------
* Start function 0 -- Frq 1
* ------------------------------------------------------------------------------
*
*    if (ip->idhfnc[0].idftmd & I_TM_KEY) {
*
		move.l	IP(A6),A0
		move.b	93(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN00A
*
		jmp	FN01
*
*	vp = (vce << 4) + 1;
*
FN00A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#1,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[0];
*
		move.l	IP(A6),A_FP
		add.l	#86,A_FP
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F00L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F00L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F00L122
*
		asl	#2,D0
		lea	F00L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F00L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F00L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F00L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F00L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F00L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F00L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F00L119:	move.w	PCH(A6),D0
		asr.w	#7,D0
		and.w	#$00FF,D0
		move.w	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move.w	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F00L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F00L120:	move.w	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move.w	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F00L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F00L121:	move.w	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move.w	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F00L114
*
*	default:
*		tsrcval = vep->val;
*
F00L122:	move.l	VEP(A6),A0
		move.w	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		srcval = addpch(tsrcval, 0);
*
F00L114:	move.w	TSRCVAL(A6),D0
		ext.l	D0
		asr.l	#5,D0
*		sub.l	#500,D0
		asl.l	#LSPCH,D0
		cmp.l	#PITCHMAX,D0
		ble	F00L129A
*
		move.l	#PITCHMAX,D0
*
F00L129A:	move	D0,SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F00L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F00L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F00L144
*
		asl	#2,D0
		lea	F00L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F00L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F00L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F00L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F00L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F00L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F00L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F00L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F00L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F00L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F00L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F00L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F00L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F00L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F00L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F00L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F00L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F00L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F00L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F00L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptim & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F00L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		octype = ip->idhos1c & OC_MOD;
*
F00L152:	move.l	IP(A6),A0
		move.b	74(A0),D0
		and	#3,D0
*
*		if ((octype EQ OC_FRQ) OR (octype EQ OC_PCH))
*
		cmp	#2,D0
		beq	F00L1001
*
		cmp	#3,D0
		bne	F00L153
*
*			fp->idfpch = ip->idhos1v;
*
F00L1001:	move.l	IP(A6),A0
		move	78(A0),(A_FP)
		bra	F00L154
*
*		else
*			fp->idfpch = pch + ip->idhos1v;
*
F00L153:	move.w	PCH(A6),D0
		ext.l	D0
		move.w	78(A0),D1
		ext.l	D1
		add.l	d1,d0
*
*		if (fp->idfpch > PITCHMAX)
*			fp->idfpch = PITCHMAX;
*
		cmp.l	#PITCHMAX,d0
		ble	F00153A
*
		move.l	#PITCHMAX,d0
*
F00153A:	move.w	d0,(A_FP)
*
*		fpval = addpch(tfpval, fp->idfpch);
*
F00L154:	move.w	TFPVAL(A6),D1
		ext.l	D1
		asr.l	#5,D1
		sub.l	#500,D1
		asl.l	#LSPCH,D1
		move.w	(A_FP),D0
		ext.l	D0
		add.l	D1,D0
		cmp.l	#PITCHMAX,D0
		ble	F00L154A
*
		move.l	#PITCHMAX,D0
*
F00L154A:	move	D0,R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F00L168:	clr.b	10(A0)
		add.l	#12,A0
		dbra	D0,F00L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F00L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F00L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F00L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F00L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F00L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*    }
*
		.page
* ------------------------------------------------------------------------------
* Start function 1 -- Frq 2
* ------------------------------------------------------------------------------
*    if (legato) {
*
*	legato = 0;
*	return;
*    }
FN01:		tst.w	_legato
		beq	FN01AA
*
		clr.w	_legato
		bra	FNEXIT
*
*    if (ip->idhfnc[1].idftmd & I_TM_KEY) {
*
FN01AA:		move.l	IP(A6),A0
		move.b	105(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN01A
*
		jmp	FN02
*
*	vp = (vce << 4) + 3;
*
FN01A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#3,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[1];
*
		move.l	#12,A_FP
		add.l	IP(A6),A_FP
		add.l	#86,A_FP
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F01L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F01L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F01L122
*
		asl	#2,D0
		lea	F01L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F01L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F01L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F01L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F01L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F01L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F01L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F01L119:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F01L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F01L120:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F01L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F01L121:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F01L114
*
*	default:
*		tsrcval = vep->val;
*
F01L122:	move.l	VEP(A6),A0
		move	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		srcval = addpch(tsrcval, 0);
*
F01L114:	move.w	TSRCVAL(A6),D0
		ext.l	D0
		asr.l	#5,D0
*		sub.l	#500,D0
		asl.l	#LSPCH,D0
		cmp.l	#PITCHMAX,D0
		ble	F01L129A
*
		move.l	#PITCHMAX,D0
*
F01L129A:	move	D0,SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F01L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F01L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F01L144
*
		asl	#2,D0
		lea	F01L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F01L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F01L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F01L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F01L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F01L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F01L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F01L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F01L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F01L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F01L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F01L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F01L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F01L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F01L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F01L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F01L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F01L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F01L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F01L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptom & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F01L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		fp->idfpch = ip->idhos2v;
*
F01L155:	move.l	IP(A6),A0
		move.w	80(A0),(A_FP)
*
*		fpval = addpch(tfpval, fp_>idfpch);
*
		move.w	TFPVAL(A6),D1
		ext.l	D1
		asr.l	#5,D1
		sub.l	#500,D1
		asl.l	#LSPCH,D1
		move.w	(A_FP),D0
		ext.l	D0
		add.l	D1,D0
		cmp.l	#PITCHMAX,D0
		ble	F01L155A
*
		move.l	#PITCHMAX,D0
*
F01L155A:	move	D0,R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F01L168:	clr.b	10(A0)
		add.l	#12,a0
		dbra	D0,F01L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F01L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F01L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F01L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F01L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F01L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*
		.page
* ------------------------------------------------------------------------------
* Start function 2 -- Frq 3
* ------------------------------------------------------------------------------
*
*    if (ip->idhfnc[2].idftmd & I_TM_KEY) {
*
FN02:		move.l	IP(A6),A0
		move.b	117(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN02A
*
		jmp	FN03
*
*	vp = (vce << 4) + 5;
*
FN02A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#5,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[2];
*
		move.l	#24,A_FP
		add.l	IP(A6),A_FP
		add.l	#86,A_FP
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F02L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F02L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F02L122
*
		asl	#2,D0
		lea	F02L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F02L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F02L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F02L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F02L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F02L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F02L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F02L119:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F02L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F02L120:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F02L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F02L121:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F02L114
*
*	default:
*		tsrcval = vep->val;
*
F02L122:	move.l	VEP(A6),A0
		move	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		srcval = addpch(tsrcval, 0);
*
F02L114:	move.w	TSRCVAL(A6),D0
		ext.l	D0
		asr.l	#5,D0
*		sub.l	#500,D0
		asl.l	#LSPCH,D0
		cmp.l	#PITCHMAX,D0
		ble	F02L129A
*
		move.l	#PITCHMAX,D0
*
F02L129A:	move	D0,SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F02L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F02L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F02L144
*
		asl	#2,D0
		lea	F02L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F02L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F02L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F02L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F02L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F02L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F02L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F02L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F02L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F02L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F02L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F02L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F02L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F02L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F02L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F02L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F02L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F02L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F02L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F02L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptom & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F02L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		fp->idfpch = ip->idhos3v;
*
F02L156:	move.l	IP(A6),A0
		move.w	82(A0),(A_FP)
*
*		fpval = addpch(tfpval, fp->idfpch);
*
		move.w	TFPVAL(A6),D1
		ext.l	D1
		asr.l	#5,D1
		sub.l	#500,D1
		asl.l	#LSPCH,D1
		move.w	(A_FP),D0
		ext.l	D0
		add.l	D1,D0
		cmp.l	#PITCHMAX,D0
		ble	F02L156A
*
		move.l	#PITCHMAX,D0
*
F02L156A:	move	D0,R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F02L168:	clr.b	10(A0)
		add.l	#12,a0
		dbra	D0,F02L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F02L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F02L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F02L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F02L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F02L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*
		.page
* ------------------------------------------------------------------------------
* Start function 3 -- Frq 4
* ------------------------------------------------------------------------------
*
*    if (ip->idhfnc[3].idftmd & I_TM_KEY) {
*
FN03:		move.l	IP(A6),A0
		move.b	129(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN03A
*
		jmp	FN04
*
*	vp = (vce << 4) + 7;
*
FN03A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#7,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[3];
*
		move.l	#36,A_FP
		add.l	IP(A6),A_FP
		add.l	#86,A_FP
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F03L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F03L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F03L122
*
		asl	#2,D0
		lea	F03L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F03L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F03L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F03L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F03L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F03L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F03L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F03L119:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F03L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F03L120:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F03L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F03L121:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F03L114
*
*	default:
*		tsrcval = vep->val;
*
F03L122:	move.l	VEP(A6),A0
		move	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		srcval = addpch(tsrcval, 0);
*
F03L114:	move.w	TSRCVAL(A6),D0
		ext.l	D0
		asr.l	#5,D0
*		sub.l	#500,D0
		asl.l	#LSPCH,D0
		cmp.l	#PITCHMAX,D0
		ble	F03L129A
*
		move.l	#PITCHMAX,D0

F03L129A:	move	D0,SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F03L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F03L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F03L144
*
		asl	#2,D0
		lea	F03L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F03L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F03L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F03L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F03L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F03L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F03L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F03L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F03L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F03L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F03L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F03L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F03L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F03L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F03L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F03L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F03L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F03L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F03L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F03L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptom & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F03L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		fp->idfpch = ip->idhos4v;
*
F03L157:	move.l	IP(A6),A0
		move.w	84(A0),(A_FP)
*
*		fpval = addpch(tfpval, fp->idfpch);
*
		move.w	TFPVAL(A6),D1
		ext.l	D1
		asr.l	#5,D1
		sub.l	#500,D1
		asl.l	#LSPCH,D1
		move.w	(A_FP),D0
		ext.l	D0
		add.l	D1,D0
		cmp.l	#PITCHMAX,D0
		ble	F03L157A
*
		move.l	#PITCHMAX,D0
*
F03L157A:	move	D0,R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F03L168:	clr.b	10(A0)
		add.l	#12,a0
		dbra	D0,F03L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F03L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F03L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F03L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F03L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F03L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*
		.page
* ------------------------------------------------------------------------------
* Start function 4 -- Filtr
* ------------------------------------------------------------------------------
*
*    if (ip->idhfnc[4].idftmd & I_TM_KEY) {
*
FN04:		move.l	IP(A6),A0
		move.b	141(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN04A
*
		jmp	FN05
*
*	vp = (vce << 4) + 10;
*
FN04A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#10,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[4];
*
		move.l	#48,A_FP
		add.l	IP(A6),A_FP
		add.l	#86,A_FP
*
		.page
*
*	Added code:
*
*		output resonance via table lookup with FPU interrupts off
*
		lea	_rsntab,A0
		clr.w	D0
		move.b	9(A_FP),D0
		add.w	D0,D0
		move.w	0(A0,D0.W),D1
		move.w	VCE(A6),D0
		asl.w	#4,d0
		add.w	#6,d0
		asl.w	#5,D0
		ext.l	D0
		move.l	D0,A0
		add.l	#_io_fpu+$4000,A0
		move.w	sr,OLDI(A6)
		move.w	#FPU_DI,sr
		move.w	d1,2(A0)
		add.w	#1,OCTYPE(A6)
		move.w	d1,$1C(A0)
		add.w	#1,OCTYPE(A6)
		move.w	#$0015,(A0)
		move.w	OLDI(A6),sr
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F04L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F04L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F04L122
*
		asl	#2,D0
		lea	F04L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F04L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F04L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F04L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F04L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F04L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F04L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F04L119:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F04L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F04L120:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F04L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F04L121:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F04L114
*
*	default:
*		tsrcval = vep->val;
*
F04L122:	move.l	VEP(A6),A0
		move	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		ltmp = ((long)tsrcval >> 1) + ((long)tsrcval >> 2);
*
F04L114:	move	TSRCVAL(A6),D0
		ext.l	D0
		asr.l	#1,D0
		move	TSRCVAL(A6),D1
		ext.l	D1
		asr.l	#2,D1
		add.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F04L131
*
		move.l	#VALMAX,LTMP(A6)
		bra	F04L132
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F04L131:	cmp.l	#VALMIN,LTMP(A6)
		bge	F04L132
*
		move.l	#VALMIN,LTMP(A6)
*
*		srcval = (short)ltmp;
*
F04L132:	move.l	LTMP(A6),D0
		move	D0,SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F04L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F04L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F04L144
*
		asl	#2,D0
		lea	F04L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F04L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F04L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F04L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F04L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F04L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F04L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F04L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F04L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F04L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F04L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F04L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F04L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F04L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F04L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F04L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F04L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F04L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F04L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F04L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptom & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F04L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		fp->idfpch = pch;
*
F04L158:	move	PCH(A6),(A_FP)
*
*		ltmp = ((long)tfpval >> 1) + ((longtfpval >>2)
*			+ (long)fp->idfpch;
*
		move	TFPVAL(A6),D0
		ext.l	D0
		asr.l	#1,D0
		move	TFPVAL(A6),D1
		ext.l	D1
		asr.l	#2,D1
		add.l	D1,D0
		move	(A_FP),D1
		ext.l	D1
		add.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F04L159
*
		move.l	#VALMAX,LTMP(A6)
		bra	F04L160
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F04L159:	cmp.l	#VALMIN,LTMP(A6)
		bge	F04L160
*
		move.l	#VALMIN,LTMP(A6)
*
*		fpval = (short)ltmp;
*
F04L160:	move.l	LTMP(A6),D0
		move	D0,R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F04L168:	clr.b	10(A0)
		add.l	#12,a0
		dbra	D0,F04L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F04L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F04L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F04L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F04L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F04L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*
		.page
* ------------------------------------------------------------------------------
* Start function 5 -- Loctn
* ------------------------------------------------------------------------------
*
*    if (ip->idhfnc[5].idftmd & I_TM_KEY) {
*
FN05:		move.l	IP(A6),A0
		move.b	153(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN05A
*
		jmp	FN06
*
*	vp = (vce << 4) + 4;
*
FN05A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#4,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[5];
*
		move.l	#60,A_FP
		add.l	IP(A6),A_FP
		add.l	#86,A_FP
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F05L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F05L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F05L122
*
		asl	#2,D0
		lea	F05L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F05L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F05L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F05L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F05L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F05L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F05L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F05L119:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F05L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F05L120:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F05L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F05L121:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F05L114
*
*	default:
*		tsrcval = vep->val;
*
F05L122:	move.l	VEP(A6),A0
		move	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		srcval = tsrcval;
*
F05L114:	move	TSRCVAL(A6),SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F05L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F05L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F05L144
*
		asl	#2,D0
		lea	F05L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F05L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F05L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F05L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F05L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F05L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F05L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F05L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F05L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F05L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F05L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F05L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F05L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F05L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F05L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F05L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F05L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F05L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F05L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F05L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptom & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F05L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		fp->idfpch = pch;
*
F05L163:	move	PCH(A6),(A_FP)
*
*		if (tfpval > VALMAX)
*			tfpval = VALMAX;
*		else if (tfpval < 0)
*			tfpval = 0;
*
		move.w	TFPVAL(A6),D0
		cmp.w	#$7D00,D0
		ble	F05L163B
*
		move.w	#$7D00,D0
		bra	F05L163A
		
F05L163B:	tst.w	D0
		bpl	F05L163A
*
		clr.w	D0
*
*#if	LOC_EOR
*		fpval = (tfpval << 1) ^ 0x8000;
*#endif
*
		.ifne	LOC_EOR
F05L163A:	add.w	D0,D0
		eor.w	#$8000,D0
		.endc
*
*#if	LOC_SUB
*		fpval = ((tfpval >> 5) - 500) << 6;
*#endif
*
		.ifne	LOC_SUB
F05L163A:	asr.w	#5,D0
		sub.w	#500,D0
		asl.w	#6,D0
		.endc
*
		move.w	D0,R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F05L168:	clr.b	10(A0)
		add.l	#12,a0
		dbra	D0,F05L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F05L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F05L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F05L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F05L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F05L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*
		.page
* ------------------------------------------------------------------------------
* Start function 6 -- Ind 1
* ------------------------------------------------------------------------------
*
*    if (ip->idhfnc[2].idftmd & I_TM_KEY) {
*
FN06:		move.l	IP(A6),A0
		move.b	165(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN06A
*
		jmp	FN07
*
*	vp = (vce << 4) + 9;
*
FN06A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#9,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[6];
*
		move.l	#72,A_FP
		add.l	IP(A6),A_FP
		add.l	#86,A_FP
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F06L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F06L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F06L122
*
		asl	#2,D0
		lea	F06L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F06L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F06L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F06L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F06L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F06L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F06L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F06L119:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F06L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F06L120:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F06L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F06L121:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F06L114
*
*	default:
*		tsrcval = vep->val;
*
F06L122:	move.l	VEP(A6),A0
		move	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		srcval = tsrcval;
*
F06L114:	move	TSRCVAL(A6),SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F06L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F06L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F06L144
*
		asl	#2,D0
		lea	F06L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F06L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F06L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F06L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F06L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F06L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F06L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F06L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F06L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F06L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F06L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F06L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F06L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F06L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F06L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F06L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F06L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F06L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F06L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F06L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptom & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F06L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		fp->idfpch = pch;
*
F06L163:	move	PCH(A6),(A_FP)
*
*		fpval = tfpval;
*
		move	TFPVAL(A6),R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F06L168:	clr.b	10(A0)
		add.l	#12,a0
		dbra	D0,F06L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F06L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F06L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F06L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F06L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F06L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*
		.page
* ------------------------------------------------------------------------------
* Start function 7 -- Ind 2
* ------------------------------------------------------------------------------
*
*    if (ip->idhfnc[7].idftmd & I_TM_KEY) {
*
FN07:		move.l	IP(A6),A0
		move.b	177(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN07A
*
		jmp	FN08
*
*	vp = (vce << 4) + 11;
*
FN07A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#11,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[7];
*
		move.l	#84,A_FP
		add.l	IP(A6),A_FP
		add.l	#86,A_FP
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F07L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F07L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F07L122
*
		asl	#2,D0
		lea	F07L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F07L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F07L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F07L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F07L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F07L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F07L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F07L119:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F07L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F07L120:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F07L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F07L121:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F07L114
*
*	default:
*		tsrcval = vep->val;
*
F07L122:	move.l	VEP(A6),A0
		move	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		srcval = tsrcval;
*
F07L114:	move	TSRCVAL(A6),SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F07L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F07L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F07L144
*
		asl	#2,D0
		lea	F07L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F07L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F07L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F07L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F07L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F07L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F07L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F07L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F07L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F07L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F07L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F07L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F07L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F07L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F07L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F07L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F07L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F07L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F07L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F07L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptom & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F07L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		fp->idfpch = pch;
*
F07L163:	move	PCH(A6),(A_FP)
*
*		fpval = tfpval;
*
		move	TFPVAL(A6),R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F07L168:	clr.b	10(A0)
		add.l	#12,a0
		dbra	D0,F07L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F07L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F07L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F07L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F07L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F07L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*
		.page
* ------------------------------------------------------------------------------
* Start function 8 -- Ind 3
* ------------------------------------------------------------------------------
*
*    if (ip->idhfnc[8].idftmd & I_TM_KEY) {
*
FN08:		move.l	IP(A6),A0
		move.b	189(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN08A
*
		jmp	FN09
*
*	vp = (vce << 4) + 12;
*
FN08A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#12,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[8];
*
		move.l	#96,A_FP
		add.l	IP(A6),A_FP
		add.l	#86,A_FP
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F08L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F08L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F08L122
*
		asl	#2,D0
		lea	F08L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F08L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F08L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F08L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F08L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F08L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F08L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F08L119:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F08L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F08L120:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F08L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F08L121:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F08L114
*
*	default:
*		tsrcval = vep->val;
*
F08L122:	move.l	VEP(A6),A0
		move	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		srcval = tsrcval;
*
F08L114:	move	TSRCVAL(A6),SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F08L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F08L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F08L144
*
		asl	#2,D0
		lea	F08L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F08L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F08L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F08L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F08L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F08L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F08L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F08L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F08L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F08L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F08L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F08L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F08L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F08L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F08L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F08L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F08L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F08L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F08L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F08L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptom & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F08L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		fp->idfpch = pch;
*
F08L163:	move	PCH(A6),(A_FP)
*
*		fpval = tfpval;
*
		move	TFPVAL(A6),R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F08L168:	clr.b	10(A0)
		add.l	#12,a0
		dbra	D0,F08L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F08L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F08L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F08L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F08L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F08L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*
		.page
* ------------------------------------------------------------------------------
* Start function 9 -- Ind 4
* ------------------------------------------------------------------------------
*
*    if (ip->idhfnc[9].idftmd & I_TM_KEY) {
*
FN09:		move.l	IP(A6),A0
		move.b	201(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN09A
*
		jmp	FN10
*
*	vp = (vce << 4) + 13;
*
FN09A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#13,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[9];
*
		move.l	#108,A_FP
		add.l	IP(A6),A_FP
		add.l	#86,A_FP
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F09L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F09L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F09L122
*
		asl	#2,D0
		lea	F09L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F09L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F09L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F09L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F09L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F09L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F09L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F09L119:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F09L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F09L120:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F09L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F09L121:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F09L114
*
*	default:
*		tsrcval = vep->val;
*
F09L122:	move.l	VEP(A6),A0
		move	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		srcval = tsrcval;
*
F09L114:	move	TSRCVAL(A6),SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F09L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F09L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F09L144
*
		asl	#2,D0
		lea	F09L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F09L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F09L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F09L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F09L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F09L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F09L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F09L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F09L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F09L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F09L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F09L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F09L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F09L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F09L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F09L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F09L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F09L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F09L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F09L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptom & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F09L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		fp->idfpch = pch;
*
F09L163:	move	PCH(A6),(A_FP)
*
*		fpval = tfpval;
*
		move	TFPVAL(A6),R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F09L168:	clr.b	10(A0)
		add.l	#12,a0
		dbra	D0,F09L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F09L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F09L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F09L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F09L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F09L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*
		.page
* ------------------------------------------------------------------------------
* Start function 10 -- Ind 5
* ------------------------------------------------------------------------------
*
*    if (ip->idhfnc[10].idftmd & I_TM_KEY) {
*
FN10:		move.l	IP(A6),A0
		move.b	213(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN10A
*
		jmp	FN11
*
*	vp = (vce << 4) + 14;
*
FN10A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#14,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[10];
*
		move.l	#120,A_FP
		add.l	IP(A6),A_FP
		add.l	#86,A_FP
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F10L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F10L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F10L122
*
		asl	#2,D0
		lea	F10L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F10L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F10L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F10L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F10L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F10L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F10L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F10L119:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F10L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F10L120:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F10L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F10L121:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F10L114
*
*	default:
*		tsrcval = vep->val;
*
F10L122:	move.l	VEP(A6),A0
		move	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		srcval = tsrcval;
*
F10L114:	move	TSRCVAL(A6),SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F10L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F10L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F10L144
*
		asl	#2,D0
		lea	F10L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F10L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F10L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F10L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F10L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F10L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F10L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F10L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F10L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F10L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F10L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F10L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F10L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F10L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F10L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F10L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F10L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F10L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F10L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F10L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptom & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F10L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		fp->idfpch = pch;
*
F10L163:	move	PCH(A6),(A_FP)
*
*		fpval = tfpval;
*
		move	TFPVAL(A6),R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F10L168:	clr.b	10(A0)
		add.l	#12,a0
		dbra	D0,F10L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F10L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F10L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F10L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F10L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F10L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*
		.page
* ------------------------------------------------------------------------------
* Start function 11 -- Ind 6
* ------------------------------------------------------------------------------
*
*    if (ip->idhfnc[11].idftmd & I_TM_KEY) {
*
FN11:		move.l	IP(A6),A0
		move.b	225(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN11A
*
		jmp	FN12
*
*	vp = (vce << 4) + 15;
*
FN11A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#15,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[11];
*
		move.l	#132,A_FP
		add.l	IP(A6),A_FP
		add.l	#86,A_FP
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F11L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F11L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F11L122
*
		asl	#2,D0
		lea	F11L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F11L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F11L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F11L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F11L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F11L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F11L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F11L119:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F11L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F11L120:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F11L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F11L121:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F11L114
*
*	default:
*		tsrcval = vep->val;
*
F11L122:	move.l	VEP(A6),A0
		move	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		srcval = tsrcval;
*
F11L114:	move	TSRCVAL(A6),SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F11L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F11L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F11L144
*
		asl	#2,D0
		lea	F11L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F11L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F11L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F11L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F11L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F11L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F11L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F11L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F11L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F11L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F11L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F11L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F11L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F11L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F11L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F11L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F11L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F11L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F11L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F11L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptom & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F11L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		fp->idfpch = pch;
*
F11L163:	move	PCH(A6),(A_FP)
*
*		fpval = tfpval;
*
		move	TFPVAL(A6),R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F11L168:	clr.b	10(A0)
		add.l	#12,a0
		dbra	D0,F11L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F11L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F11L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F11L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F11L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F11L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*
		.page
* ------------------------------------------------------------------------------
* Start function 12 -- Level
* ------------------------------------------------------------------------------
*
*    if (ip->idhfnc[12].idftmd & I_TM_KEY) {
*
FN12:		move.l	IP(A6),A0
		move.b	237(A0),D0
		move.w	TAG(A6),D1
		eor.w	D1,D0
		btst	#0,D0
		bne	FN12A
*
		jmp	FNEXIT
*
*	vp = (vce << 4) + 2;
*
FN12A:		move	VCE(A6),D0
		asl	#4,D0
		add.w	#2,D0
		move	D0,VP(A6)
*
*	fpu = io_fpu + FPU_OFNC + (vp << 4);
*
		asl	#5,D0
		ext.l	D0
		move.l	D0,A_FPU
		add.l	#_io_fpu+$4000,A_FPU
*
*	fp = &ip->idhfnc[12];
*
		move.l	#144,A_FP
		add.l	IP(A6),A_FP
		add.l	#86,A_FP
*
*	pt = &ip->idhpnt[fp->idfpt1];
*
		clr.l	D0
		move.b	6(A_FP),D0
		lsl.l	#2,D0
		move.l	D0,D1
		add.l	D0,D0
		add.l	D1,D0
		add.l	IP(A6),D0
		add.l	#242,D0
		move.l	D0,PT(A6)
*
		.page
*
*	srcnum = group | fp->idfsrc;
*
		move.w	GROUP(A6),D0
		ext.l	D0
		clr.l	D1
		move.b	4(A_FP),D1
		or	D1,D0
		move	D0,SRCNUM(A6)
*
*	vep = &valents[srcnum];
*
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		add.l	#_valents,D0
		move.l	D0,VEP(A6)
*
*	smp = vpsms[vp];
*
		move	VP(A6),A0
		add.l	A0,A0
		add.l	A0,A0
		add.l	#_vpsms,A0
		move.l	(A0),A_SMP
*
*	if (srcnum NE smp->sm) {
*
		clr	D0
		move	10(A_SMP),D0
		cmp	SRCNUM(A6),D0
		beq	F12L113
*
*		(smp->prv)->nxt = smp->nxt;
*
		move.l	4(A_SMP),A0
		move.l	(A_SMP),(A0)
*
*		(smp->nxt)->prv = smp->prv;
*
		move.l	(A_SMP),A0
		move.l	4(A_SMP),4(A0)
*
*		smp->prv = (struct sment *)vep;
*
		move.l	VEP(A6),4(A_SMP)
*
*		smp->nxt = vep->nxt;
*
		move.l	VEP(A6),A0
		move.l	(A0),(A_SMP)
*
*		(vep->nxt)->prv = smp;
*
		move.l	VEP(A6),A0
		move.l	(A0),A0
		move.l	A_SMP,4(A0)
*
*		vep->nxt = smp;
*
		move.l	VEP(A6),A0
		move.l	A_SMP,(A0)
*
*		smp->sm = srcnum;
*
		move	SRCNUM(A6),10(A_SMP)
*
*	}
*
*	mltval = fp->idfmlt;
*
F12L113:	move	2(A_FP),MLTVAL(A6)
*
		.page
*
*	switch (fp->idfsrc) {
*
		move.b	4(A_FP),D0
		ext.w	d0
		cmp	#10,D0
		bhi	F12L122
*
		asl	#2,D0
		lea	F12L123,A0
		movea.l	0(A0,D0.W),A0
		jmp	(A0)
*
*	case SM_NONE:
*		mltval = 0;
*
F12L116:	clr	MLTVAL(A6)
*
*		tsrcval = 0;
*
		clr	TSRCVAL(A6)
*
*		break;
*
		bra	F12L114
*
*	case SM_RAND:
*		tsrcval = xgetran(mltval);
*
F12L117:	move	MLTVAL(A6),(sp)
		jsr	_xgetran
		move	D0,TSRCVAL(A6)
*
*		break;
*
		bra	F12L114
*
*	case SM_PTCH:
*		tsrcval = pch;
*
F12L118:	move	PCH(A6),TSRCVAL(A6)
*
*		break;
*
		bra	F12L114
*
		.page
*
*	case SM_FREQ:
*		tsrcval = ptoftab[(pch >> 7) & 0x00FF];
*
F12L119:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		add.l	#_ptoftab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F12L114
*
*	case SM_KVEL:
*		tsrcval = veltab[trg];
*
F12L120:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_veltab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F12L114
*
*	case SM_KPRS:
*		tsrcval = prstab[trg];
*
F12L121:	move	TRG(A6),A0
		add.l	A0,A0
		add.l	#_prstab,A0
		move	(A0),TSRCVAL(A6)
*
*		break;
*
		bra	F12L114
*
*	default:
*		tsrcval = vep->val;
*
F12L122:	move.l	VEP(A6),A0
		move	8(A0),TSRCVAL(A6)
*
*	}
*
		.page
*
*		srcval = tsrcval;
*
F12L114:	move	TSRCVAL(A6),SRCVAL(A6)
*
		.page
*
*	if (pt->ipvsrc) {
*
F12L124:	move.l	PT(A6),A0
		tst.b	6(A0)
		beq	F12L136
*
*		switch (pt->ipvsrc) {
*
		move.l	PT(A6),A0
		move.b	6(A0),D0
		ext.w	D0
		sub	#1,D0
		cmp	#9,D0
		bhi	F12L144
*
		asl	#2,D0
		lea	F12L145,A0
		move.l	0(A0,D0.W),A0
		jmp	(A0)
*
*		case SM_RAND:
*			ltmp = xgetran(pt_>ipvmlt);
*
F12L139:	move.l	PT(A6),A0
		move	4(A0),(sp)
		jsr	_xgetran
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F12L137
*
*		case SM_PTCH:
*			ltmp = pch;
*
F12L140:	move	PCH(A6),A0
		move.l	A0,LTMP(A6)
*
*			break;
*
		bra	F12L137
*
		.page
*
*		case SM_FREQ:
*			ltmp = ptoftab[(pch >> 7) & 0x00FF];
*
F12L141:	move	PCH(A6),D0
		asr	#7,D0
		and	#255,D0
		move	D0,A0
		add.l	A0,A0
		move.l	#_ptoftab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F12L137
*
*		case SM_KVEL:
*			ltmp = veltab[trg];
*
F12L142:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_veltab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F12L137
*
		.page
*
*		case SM_KPRS:
*			ltmp = prstab[trg];
*
F12L143:	move	TRG(A6),A0
		add.l	A0,A0
		move.l	#_prstab,A1
		move	0(A0,A1.l),D0
		ext.l	D0
		move.l	D0,LTMP(A6)
*
*			break;
*
		bra	F12L137
*
		.page
*
*		default:
*			ltmp = valents[group | pt->ipvsrc].val;
*
F12L144:	move.l	PT(A6),A0
		clr.l	D0
		move.b	6(A0),D0
		or.w	GROUP(A6),D0
		add.l	D0,D0
		move.l	D0,D1
		lsl.l	#2,D0
		add.l	D1,D0
		move.l	D0,A0
		move.l	#_valents,A1
		clr.l	D0
		move	8(A0,A1.l),D0
		move.l	D0,LTMP(A6)
*
*		}
*
*
		.page
*
*		ltmp = (ltmp * pt->ipvmlt) >> 15;
*
F12L137:	move.l	PT(A6),A0
		move.w	4(A0),D0
		move.w	LTMP+2(A6),D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move.l	D0,LTMP(A6)
*
*		ltmp += (long)pt->ipval;
*
		move.l	PT(A6),A0
		move	2(A0),D0
		ext.l	D0
		add.l	D0,LTMP(A6)
*
*		if (ltmp GT (long)VALMAX)
*			ltmp = (long)VALMAX;
*
		cmp.l	#VALMAX,LTMP(A6)
		ble	F12L146
*
		move.l	#VALMAX,LTMP(A6)
		bra	F12L147
*
*		else if (ltmp LT (long)VALMIN)
*			ltmp = (long)VALMIN;
*
F12L146:	cmp.l	#VALMIN,LTMP(A6)
		bge	F12L147
*
		move.l	#VALMIN,LTMP(A6)
*
*		tfpval = (short)ltmp;
*
F12L147:	move.w	LTMP+2(A6),TFPVAL(A6)
		bra	F12L149
*
*	} else {
*
*		tfpval = pt->ipval;
*
F12L136:	move.l	PT(A6),A0
		move	2(A0),TFPVAL(A6)
*
*	}
*
		.page
*
*	fpmant = (((long)pt->iptom & 0x0000FFF0L)
*		* ((long)timemlt & 0x0000FFFFL)) >> 15;
*
F12L149:	move.l	PT(A6),A0
		move.w	(A0),D0
		move.w	D0,D2
		andi.w	#$FFF0,D0
		move.w	_timemlt,D1
		muls	D1,D0
		move.l	#15,D1
		asr.l	D1,D0
		move	D0,R_FPMANT
*
*	fpexp = expbit[pt->iptim & 0x000F];
*
		and	#$000F,D2
		move	D2,A0
		add.l	A0,A0
		add.l	#_expbit,A0
		move	(A0),R_FPEXP
*
		.page
*
*		fp->idfpch = pch;
*
F12L162:	move	PCH(A6),(A_FP)
*
*		fpval = ((tfpval >> 5) - 500) << 6;
*
		move	TFPVAL(A6),R_FPVAL
		asr	#5,R_FPVAL
		add	#-500,R_FPVAL
		asl	#6,R_FPVAL
*
		.page
*
		move.b	5(A_FP),D0
		ext.w	D0
		sub.w	#1,D0
		movea.l	PT(A6),A0
*
*	oldi = setipl(FPU_DI);
*
		move	sr,OLDI(A6)
		move	#FPU_DI,sr
*
F12L168:	clr.b	10(A0)
		add.l	#12,a0
		dbra	D0,F12L168
*
		.page
*
*	fp->idftmd ^= I_NVBITS;
*
F12L165:	eor.b	#24,7(A_FP)
*
*	fpctl = (fp->idftmd & (I_NRATIO | I_NVBITS)) | 0x0003;
*
		move.b	7(A_FP),R_FPCTL
		and	#28,R_FPCTL
		or	#3,R_FPCTL
*
*	fp->idfcpt = fp->idfpt1;
*
		move.b	6(A_FP),8(A_FP)
*
*	fp->idftmd |= I_ACTIVE;
*
		or.b	#2,7(A_FP)
*
*	fp->idftrf = trg;
*
		move	TRG(A6),10(A_FP)
*
*	*(fpu + (long)FPU_TCV1) = srcval;
*
		move	SRCVAL(A6),$8(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TSF1) = mltval;
*
		move	MLTVAL(A6),$A(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TMNT) = fpmant;
*
		move	R_FPMANT,$14(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TEXP) = fpexp;
*
		move	R_FPEXP,$16(A_FPU)
*
*	++octype;
*
		add	#1,OCTYPE(A6)
*
		.page
*
*	if (fp->idftmd & I_VNSUBN)
*
		btst	#3,7(A_FP)
		beq	F12L169
*
*		*(fpu + (long)FPU_TNV1) = fpval;
*
		move	R_FPVAL,$1C(A_FPU)
		bra	F12L170
*
*	else
*		*(fpu + (long)FPU_TNV0) = fpval;
*
F12L169:	move	R_FPVAL,2(A_FPU)
*
*	++octype;
*
F12L170:	add	#1,OCTYPE(A6)
*
*	*(fpu + (long)FPU_TCTL) = fpctl;
*
		move	R_FPCTL,(A_FPU)
*
*	setipl(oldi);
*
		move	OLDI(A6),sr
*
*    }
*
		.page
*
FNEXIT:		tst.l	(sp)+
		movem.l	(sp)+,R_FPVAL-R_FPMANT/A_SMP-A_FP
		unlk	A6
		rts
*
* }
*
		.page
*
		.data
*
* Jump tables for switch statements
* ---------------------------------
*
F00L123:	.dc.l	F00L116
		.dc.l	F00L117
		.dc.l	F00L122
		.dc.l	F00L122
		.dc.l	F00L122
		.dc.l	F00L118
		.dc.l	F00L121
		.dc.l	F00L120
		.dc.l	F00L122
		.dc.l	F00L122
		.dc.l	F00L119
*
F00L145:	.dc.l	F00L139
		.dc.l	F00L144
		.dc.l	F00L144
		.dc.l	F00L144
		.dc.l	F00L140
		.dc.l	F00L143
		.dc.l	F00L142
		.dc.l	F00L144
		.dc.l	F00L144
		.dc.l	F00L141
*
F01L123:	.dc.l	F01L116
		.dc.l	F01L117
		.dc.l	F01L122
		.dc.l	F01L122
		.dc.l	F01L122
		.dc.l	F01L118
		.dc.l	F01L121
		.dc.l	F01L120
		.dc.l	F01L122
		.dc.l	F01L122
		.dc.l	F01L119
*
F01L145:	.dc.l	F01L139
		.dc.l	F01L144
		.dc.l	F01L144
		.dc.l	F01L144
		.dc.l	F01L140
		.dc.l	F01L143
		.dc.l	F01L142
		.dc.l	F01L144
		.dc.l	F01L144
		.dc.l	F01L141
*
F02L123:	.dc.l	F02L116
		.dc.l	F02L117
		.dc.l	F02L122
		.dc.l	F02L122
		.dc.l	F02L122
		.dc.l	F02L118
		.dc.l	F02L121
		.dc.l	F02L120
		.dc.l	F02L122
		.dc.l	F02L122
		.dc.l	F02L119
*
F02L145:	.dc.l	F02L139
		.dc.l	F02L144
		.dc.l	F02L144
		.dc.l	F02L144
		.dc.l	F02L140
		.dc.l	F02L143
		.dc.l	F02L142
		.dc.l	F02L144
		.dc.l	F02L144
		.dc.l	F02L141
*
F03L123:	.dc.l	F03L116
		.dc.l	F03L117
		.dc.l	F03L122
		.dc.l	F03L122
		.dc.l	F03L122
		.dc.l	F03L118
		.dc.l	F03L121
		.dc.l	F03L120
		.dc.l	F03L122
		.dc.l	F03L122
		.dc.l	F03L119
*
F03L145:	.dc.l	F03L139
		.dc.l	F03L144
		.dc.l	F03L144
		.dc.l	F03L144
		.dc.l	F03L140
		.dc.l	F03L143
		.dc.l	F03L142
		.dc.l	F03L144
		.dc.l	F03L144
		.dc.l	F03L141
*
F04L123:	.dc.l	F04L116
		.dc.l	F04L117
		.dc.l	F04L122
		.dc.l	F04L122
		.dc.l	F04L122
		.dc.l	F04L118
		.dc.l	F04L121
		.dc.l	F04L120
		.dc.l	F04L122
		.dc.l	F04L122
		.dc.l	F04L119
*
F04L145:	.dc.l	F04L139
		.dc.l	F04L144
		.dc.l	F04L144
		.dc.l	F04L144
		.dc.l	F04L140
		.dc.l	F04L143
		.dc.l	F04L142
		.dc.l	F04L144
		.dc.l	F04L144
		.dc.l	F04L141
*
F05L123:	.dc.l	F05L116
		.dc.l	F05L117
		.dc.l	F05L122
		.dc.l	F05L122
		.dc.l	F05L122
		.dc.l	F05L118
		.dc.l	F05L121
		.dc.l	F05L120
		.dc.l	F05L122
		.dc.l	F05L122
		.dc.l	F05L119
*
F05L145:	.dc.l	F05L139
		.dc.l	F05L144
		.dc.l	F05L144
		.dc.l	F05L144
		.dc.l	F05L140
		.dc.l	F05L143
		.dc.l	F05L142
		.dc.l	F05L144
		.dc.l	F05L144
		.dc.l	F05L141
*
F06L123:	.dc.l	F06L116
		.dc.l	F06L117
		.dc.l	F06L122
		.dc.l	F06L122
		.dc.l	F06L122
		.dc.l	F06L118
		.dc.l	F06L121
		.dc.l	F06L120
		.dc.l	F06L122
		.dc.l	F06L122
		.dc.l	F06L119
*
F06L145:	.dc.l	F06L139
		.dc.l	F06L144
		.dc.l	F06L144
		.dc.l	F06L144
		.dc.l	F06L140
		.dc.l	F06L143
		.dc.l	F06L142
		.dc.l	F06L144
		.dc.l	F06L144
		.dc.l	F06L141
*
F07L123:	.dc.l	F07L116
		.dc.l	F07L117
		.dc.l	F07L122
		.dc.l	F07L122
		.dc.l	F07L122
		.dc.l	F07L118
		.dc.l	F07L121
		.dc.l	F07L120
		.dc.l	F07L122
		.dc.l	F07L122
		.dc.l	F07L119
*
F07L145:	.dc.l	F07L139
		.dc.l	F07L144
		.dc.l	F07L144
		.dc.l	F07L144
		.dc.l	F07L140
		.dc.l	F07L143
		.dc.l	F07L142
		.dc.l	F07L144
		.dc.l	F07L144
		.dc.l	F07L141
*
F08L123:	.dc.l	F08L116
		.dc.l	F08L117
		.dc.l	F08L122
		.dc.l	F08L122
		.dc.l	F08L122
		.dc.l	F08L118
		.dc.l	F08L121
		.dc.l	F08L120
		.dc.l	F08L122
		.dc.l	F08L122
		.dc.l	F08L119
*
F08L145:	.dc.l	F08L139
		.dc.l	F08L144
		.dc.l	F08L144
		.dc.l	F08L144
		.dc.l	F08L140
		.dc.l	F08L143
		.dc.l	F08L142
		.dc.l	F08L144
		.dc.l	F08L144
		.dc.l	F08L141
*
F09L123:	.dc.l	F09L116
		.dc.l	F09L117
		.dc.l	F09L122
		.dc.l	F09L122
		.dc.l	F09L122
		.dc.l	F09L118
		.dc.l	F09L121
		.dc.l	F09L120
		.dc.l	F09L122
		.dc.l	F09L122
		.dc.l	F09L119
*
F09L145:	.dc.l	F09L139
		.dc.l	F09L144
		.dc.l	F09L144
		.dc.l	F09L144
		.dc.l	F09L140
		.dc.l	F09L143
		.dc.l	F09L142
		.dc.l	F09L144
		.dc.l	F09L144
		.dc.l	F09L141
*
F10L123:	.dc.l	F10L116
		.dc.l	F10L117
		.dc.l	F10L122
		.dc.l	F10L122
		.dc.l	F10L122
		.dc.l	F10L118
		.dc.l	F10L121
		.dc.l	F10L120
		.dc.l	F10L122
		.dc.l	F10L122
		.dc.l	F10L119
*
F10L145:	.dc.l	F10L139
		.dc.l	F10L144
		.dc.l	F10L144
		.dc.l	F10L144
		.dc.l	F10L140
		.dc.l	F10L143
		.dc.l	F10L142
		.dc.l	F10L144
		.dc.l	F10L144
		.dc.l	F10L141
*
F11L123:	.dc.l	F11L116
		.dc.l	F11L117
		.dc.l	F11L122
		.dc.l	F11L122
		.dc.l	F11L122
		.dc.l	F11L118
		.dc.l	F11L121
		.dc.l	F11L120
		.dc.l	F11L122
		.dc.l	F11L122
		.dc.l	F11L119
*
F11L145:	.dc.l	F11L139
		.dc.l	F11L144
		.dc.l	F11L144
		.dc.l	F11L144
		.dc.l	F11L140
		.dc.l	F11L143
		.dc.l	F11L142
		.dc.l	F11L144
		.dc.l	F11L144
		.dc.l	F11L141
*
F12L123:	.dc.l	F12L116
		.dc.l	F12L117
		.dc.l	F12L122
		.dc.l	F12L122
		.dc.l	F12L122
		.dc.l	F12L118
		.dc.l	F12L121
		.dc.l	F12L120
		.dc.l	F12L122
		.dc.l	F12L122
		.dc.l	F12L119
*
F12L145:	.dc.l	F12L139
		.dc.l	F12L144
		.dc.l	F12L144
		.dc.l	F12L144
		.dc.l	F12L140
		.dc.l	F12L143
		.dc.l	F12L142
		.dc.l	F12L144
		.dc.l	F12L144
		.dc.l	F12L141
*
* vbtab -- vbuf pointer table -- indexed by voice number
* -----    ---------------------------------------------
vbtab:		.dc.l	_vbufs
		.dc.l	_vbufs+VBLEN
		.dc.l	_vbufs+(2*VBLEN)
		.dc.l	_vbufs+(3*VBLEN)
		.dc.l	_vbufs+(4*VBLEN)
		.dc.l	_vbufs+(5*VBLEN)
		.dc.l	_vbufs+(6*VBLEN)
		.dc.l	_vbufs+(7*VBLEN)
		.dc.l	_vbufs+(8*VBLEN)
		.dc.l	_vbufs+(9*VBLEN)
		.dc.l	_vbufs+(10*VBLEN)
		.dc.l	_vbufs+(11*VBLEN)
*
		.end
