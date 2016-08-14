	.globl	tickirqf
	.text
tickirqf:
	pusha
	call	ontick
	call	ic8259eoi
	popa
	iret
