.globl	inb
.globl	inl
.globl	inw
.globl	outb
.globl	outl
.globl	nop
.globl	sgdt
.globl	lidt
.globl	sidt
.globl	ic8259_init
.globl	ontimer
.globl	ontimer2
.globl	ontrap
.text
ic8259_init:
	pushl	%ebp
	movl	%esp,	%ebp
	cli
	xor	%eax,	%eax
	inb	$0x21,	%al
	pushl	%eax
	xor	%eax,	%eax
	inb	$0xA1,	%al
	pushl	%eax
	movl	8(%ebp),	%ecx
	movb	$0x11,	%al
	outb	%al,	$0x20
	outb	%al,	$0xA0
	mov	%ecx,	%eax
	outb	%al,	$0x21
	mov	%ecx,	%eax
	addl	$0x8,	%eax
	outb	%al,	$0xA1
	movb	$0x4,	%al
	outb	%al,	$0x21
	outb	%al,	$0xA1
	movb	$0x1,	%al
	outb	%al,	$0x21
	outb	%al,	$0xA1
	movl	-4(%ebp),	%eax
	movl	$0x1,	%ecx
	not	%ecx
	and	%ecx,	%eax
	outb	%al,	$0x21
	movl	-8(%ebp),	%eax
	outb	%al,	$0xA1
	popl	%eax
	popl	%eax
	popl	%ebp
	ret
ontrap:
	pusha
	cld
	call	ontrap1
	movw	$0x20,	%dx
	movb	$0x20,	%al
	outb	%al,	%dx
	popa
	iret
ontimer:
	pusha
	cld
	call	ontimer1
	movw	$0x20,	%dx
	movb	$0x20,	%al
	outb	%al,	%dx
	popa
	iret
ontimer2:
	pusha
	movw	$0x20,	%dx
	movb	$0x20,	%al
	outb	%al,	%dx
	popa
	iret
sidt:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	8(%ebp),	%edx
	sidt	(%edx)
	popl	%ebp
	ret
lidt:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	8(%ebp),	%edx
	lidt	(%edx)
	popl	%ebp
	ret
sgdt:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	8(%ebp),	%edx
	sgdt	(%edx)
	popl	%ebp
	ret
/* 1: port, 2: value(32bit) */
outl:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	%ebp,	%ecx
	addl	$8,	%ecx
	movw	(%ecx),	%dx
	movl	%ebp,	%ecx
	addl	$12,	%ecx
	movl	(%ecx),	%eax
	outl	%eax,	%dx
	popl	%ebp
	ret
/* 1: port,	2: 32bit pointer */
inl:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	%ebp,	%ecx
	addl	$8,	%ecx
	movw	(%ecx),	%dx
	inl	%dx,	%eax
	movl	%ebp,	%ecx
	addl	$12,	%ecx
	movl	(%ecx),	%edx
	movl	%eax,	(%edx)
	popl	%ebp
	ret
/* 1: port,	2: 16bit pointer */
inw:
	pushl	%ebp
	movl	%esp,	%ebp
	mov	8(%ebp),	%dx
	xor	%ax,	%ax
	inw	%dx,	%ax
	mov	12(%ebp),	%edx
	mov	%ax,	(%edx)
	popl	%ebp
	ret
/* 1: port,	2: 8bit pointer */
inb:
	pushl	%ebp
	movl	%esp,	%ebp
	mov	8(%ebp),	%dx
	xor	%eax,	%eax
	inb	%dx,	%al
	movl	12(%ebp),	%edx
	movl	%eax,	(%edx)
	popl	%ebp
	ret
outb:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	8(%ebp),	%edx
	movl	12(%ebp),	%eax
	outb	%al,	%dx
	popl	%ebp
	ret
nop:
	nop
	ret
