.globl	inb
.globl	inl
.globl	inw
.globl	outb
.globl	outw
.globl	outl
.globl	nop
.globl	sgdt
.globl	lidt
.globl	sidt
.globl	ic8259init
.globl	ic8259setmsk
.globl	ic8259eoi
.globl	cpuid
.globl	eflags
.globl	ontimer
.globl	ontimer2
.globl	ontrap
.globl	sti
.globl	cli
.globl	oneth
.text
sti:
	sti
	ret
cli:
	cli
	ret
ic8259init:
	pushl	%ebp
	movl	%esp,	%ebp
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
	pushl	$0xff
	pushl	$0xff
	call	ic8259setmsk
	popl	%eax
	popl	%eax
	popl	%eax
	popl	%eax
	popl	%ebp
	ret
ic8259setmsk:
	pushl	%ebp
	mov	%esp,	%ebp
	mov	8(%ebp),	%al
	out	%al,	$0x21
	mov	12(%ebp),	%al
	out	%al,	$0xA1
	popl	%ebp
	ret
ic8259eoi:
	mov	$0x20,	%al
	mov	$0x20,	%dx
	out	%al,	%dx
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
	xor	%al,	%al
	inb	%dx,	%al
	mov	12(%ebp),	%edx
	mov	%al,	(%edx)
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
outw:
	pushl	%ebp
	movl	%esp,	%ebp
	mov	8(%ebp),	%dx
	mov	12(%ebp),	%ax
	out	%ax,	%dx
	popl	%ebp
	ret
nop:
	nop
	ret
oneth:
	pusha
	cld
	call oneth1
	mov	$0xFEE000B0,	%edx
	movl	$0x30,	(%edx)
	popa
	iret
cpuid:
	pushl	%ebp
	movl	%esp,	%ebp
	push	%ebx
	mov	$0x01,	%eax
	cpuid
	shr	$0x18,	%ebx
	mov	%ebx,	%eax
	popl	%ebx
	popl	%ebp
	ret
eflags:
	pushf
	pop	%eax
	ret
