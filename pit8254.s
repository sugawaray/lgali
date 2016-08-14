	.text
	.globl	stimerstart
	.local TCW, C0TS, RWSLM, CMSQU, FREQ
	.type	TCW,	STT_OBJECT
	.type	C0TS,	STT_OBJECT
	.type	RWSLM,	STT_OBJECT
	.type	CMSQU,	STT_OBJECT
	.type	FREQ,	STT_OBJECT
TCW:
	.int	0x43
C0TS:
	.int	0x40
RWSLM:
	.int	0x30
CMSQU:
	.int	0x06
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
