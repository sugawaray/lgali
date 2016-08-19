	.data
	.local	ticks
ticks:
	.long	0

	.text
	.local	reoi
reoi:
	.long	0xfee000b0

	.globl	timerirqf
timerirqf:
	pusha
	incl	ticks
	mov	reoi,	%edx
	movl	$0x0,	(%edx)
	popa
	iret

	.globl	timerticks
timerticks:
	mov	ticks,	%eax
	ret

	.globl intrdef
intrdef:
	iret
