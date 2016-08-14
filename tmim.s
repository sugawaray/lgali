	.data
	.globl	ticks
ticks:
	.long	0

	.text
	.globl	tickirqf
tickirqf:
	pusha
	incl	ticks
	call	ic8259eoi
	popa
	iret
