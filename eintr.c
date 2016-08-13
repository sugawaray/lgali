#include <quark.h>
#include "intrim.h"
#include <led.h>
#include <lib.h>
#include <ser.h>

struct Idtrec {
	u16	offl;
	u16	ssel;
	u16	attr;
	u16	offh;
};
static struct Idtrec idt[0x100];

#define assert(e)	\
	do {	\
		if (!(e)) {	\
			seroutf("assert(0x%X)\r\n", __LINE__);	\
		}	\
	} while (0)	\
/**/

#define GMASK	(~(~0 << 3) << 8)
#define GSIG(v)	((u32)(GMASK & (v)) >> 8)
#define ISTASK(v)	(GSIG((v)) == 5)
#define ISINTR(v)	(GSIG((v)) == 6)
#define ISTRAP(v)	(GSIG((v)) == 7)
#define VALS(v, b, e)	(((u32)(~(~0 << ((e) - (b))) << (b)) & (v)) >> (b))
void
outidt(char *idtr)
{
	u16 *a;
	u32 *b, *c;
	int i;
	a = (u16 *)idtr;
	b = (u32 *)(idtr + sizeof(u16));
	seroutf("idt base(0x%X) limit(0x%hX)\r\n", *b, *a);
	assert((((u32)*a + 1) % (sizeof(u32) * 2)) == 0);
	b = *b;
	c = b + ((u32)*a + 1) / (sizeof(u32) * 2) * 2;
	for (i = 0; b < c; ++i, b += 2) {
		if (ISTASK(*(b + 1))) {
			seroutf("task gate(0x%X, 0x%X)\r\n", i, *(b + 1));
		} else if (ISINTR(*(b + 1))) {
			seroutf("interrupt gate(0x%X, 0x%X):o1(0x%X),o2(0x%X),ss(0x%X),o(0x%X)\r\n", i, *(b + 1),  VALS(b[1], 16, 32), VALS(b[0], 0, 16), VALS(b[0], 16, 32), b[1] & 0xFFFF0000 | b[0] & 0x0000FFFF);
		} else if (ISTRAP(*(b + 1))) {
			seroutf("trap gate(0x%X, 0x%X):o1(0x%X),o2(0x%X),ss(0x%X),o(0x%X)\r\n", i, *(b + 1), VALS(b[1], 16, 32), VALS(b[0], 0, 16), VALS(b[0], 16, 32), b[1] & 0xFFFF0000 | b[0] & 0x0000FFFF);
		} else {
			seroutf("unexpected descriptor(0x%X,0x%X)\r\n", i, b[1]);
		}
	}
}

static
void
intrinit()
{
	int i;
	u32 a, c;
	char b[6];

	mmemset(b, 0, sizeof b);
	sidt(b);
	mmemcpy(idt, *(u32 *)(b + 2), *(u16 *)b + 1);
	cli();
	ic8259_init(0x20);
	for (i = 0; i < 0x100; ++i) {
#if 1
		if (i == 0x20) {
			a = (u32)ontimer;
			c = 0xE;
		} else if (i == 0x23) {
			a = (u32)ontrap;
			c = 0xF;
		} else 
			continue;
#else
		if (i >= 0x21) {
			a = (u32)ontimer;
			c = 0xE;
		} else if (i == 0x20) {
			a = (u32)ontimer;
			c = 0xE;
		} else
			continue;
#endif
		idt[i].offl = (a & 0xFFFF);
		idt[i].ssel = 0x2 << 0x3;
		idt[i].attr = 0x8000 | (c << 8);
		idt[i].offh = ((u32)(a & 0xFFFF0000) >> 0x10);
	}
	packt((const char *)idt, sizeof idt / sizeof idt[0], b);
	lidt(b);
}

#define TCW	0x43
#define TM0	0x40

static
void
timerinit()
{
	outb(TCW, 0x36);
	outb(TM0, 0xFF);
	outb(TM0, 0xFF);
}

int
main()
{
	volatile int a = 2;
	int i;

	wait();
	serinit();
	ledminit();
	intrinit();
	timerinit();
	asm (	"sti\n\t");
	for (i = 0;; ++i) {
		nop();
		if (i == 5000000) {
			switch (a) {
			case 0:
				asm (	"int $0x20\n\t");
				break;
			case 1:
				asm (	"int $0x23\n\t");
				break;
			}
			i = 0;
		}
	}
}
