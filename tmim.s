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

	.globl	tickirqf2
tickirqf2:
	pusha
	pushl	$0x1
	pushl	$0x1
	call	ledmemit
	popl	%eax
	popl	%eax
	popa
	iret
