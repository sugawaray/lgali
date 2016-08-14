	.text
	TCW	= 0x43
	C0TS	= 0x40
	RWSLM	= 0x30
	CMSQU	= 0x06
	.globl	stimerstart
	.local FREQ
	.type	FREQ,	STT_OBJECT
FREQ:
	.long	1193182
stimerstart:
	pushl	%ebp
	mov	%esp,	%ebp

	mov	FREQ,	%eax
	mov	FREQ,	%edx
	shr	$0x10,	%edx

	mov	8(%ebp),	%ecx
	div	%cx
	mov	%ax,	%cx

	mov	TCW,	%edx
	mov	RWSLM,	%eax
	or	CMSQU,	%eax
	out	%al,	%dx

	mov	%cl,	%al
	mov	C0TS,	%dx
	out	%al,	%dx

	mov	%ch,	%al
	out	%al,	%dx

	popl	%ebp
	ret
