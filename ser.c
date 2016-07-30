#include <quark.h>
#include "serim.h"
#include <ser.h>

#define MMBASE	0x100000
static u32 mmend;
#if 0
static volatile struct Ruart *ruart = (struct Ruart *)MMBASE;
#else
volatile struct Ruart *ruart = (struct Ruart *)MMBASE;
#endif

#define DIVH	0x00
#define DIVL	0X18

static
void
setmmba()
{
	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, OFFBAR0));
	outl(PCIC_DATA_PORT, MMBASE);
}

static
void
setbr()
{
	ruart->lcr |= 1 << 7;
	ruart->rbr_thr_dll = (ruart->rbr_thr_dll & ~0xFF) | DIVL;
	ruart->ier_dlh = (ruart->ier_dlh & ~0xFF) | DIVH;

	ruart->lcr ^= 1 << 7;
	ruart->ier_dlh &= 0xFFFFFF00 | (~(~0 << 3) << 4);
	ruart->iir_fcr |= 1;
}

static
void
clearmm()
{
	int *p, *q;
	p = (int *)MMBASE;
	q = (int *)mmend;
	while (p != q) {
		*p = 0;
		++p;
	}
}

int
serinit()
{
	/*int msz;*/

	detmmap();
	/*msz = detmmsz();*/
	/*mmend = MMBASE + msz;*/
	/*clearmm();*/
	/*setmmba();*/
	setbr();
	return 0;
}

#if 0
int
serout(const char *m)
{
	while (*m) {
		while (!(ruart->lsr & (1 << 5)))
			;
		ruart->rbr_thr_dll = (ruart->rbr_thr_dll & 0xFFFFFF00) | *m;
		++m;
	}
	return 0;
}
#endif

int
serrcv(char *b, int sz)
{
	int i;
	char *p;
	p = b;
	while (1) {
		for (i = 0; !(ruart->lsr & 0x01) && i < 10000; ++i)
			nop();
		if (!(ruart->lsr & 0x01))
			break;
		*p = ruart->rbr_thr_dll & 0x0000007F;
		if (*p == '\n') {
			*p++ = '\r';
			*p = '\n';
		} else if (*p == '\r') {
			*(++p) = '\n';
		}
		++p;
		if (p - b >= sz)
			break;
	}
	return p - b;
}

static
int
detmmsz()
{
	u32 u, v;

	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, OFFBAR0));
	inl(PCIC_DATA_PORT, &u);

	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, OFFBAR0));
	v = ~0;
	outl(PCIC_DATA_PORT, v);

	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, OFFBAR0));
	inl(PCIC_DATA_PORT, &v);
	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, OFFBAR0));
	outl(PCIC_DATA_PORT, u);
	return ~v + 1;
}

void
detmmap()
{
	u32 u;

	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, OFFBAR0));
	inl(PCIC_DATA_PORT, &u);
	ruart = (struct Ruart *)u;
}

