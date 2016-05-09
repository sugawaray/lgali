#include <quark.h>
#include "mcim.h"
#include <mc.h>

#define MINADDR	0x100000

static volatile u32 hmbound;

static
int
ckdb()
{
	volatile int *p;
	int q;


	p = (int *)MINADDR;
	for (q = 1; q != 0; q <<= 1) {
		*p = q;
		if (*p != q)
			return q;
	}
	return 0;
}

int
ckab()
{
#define P	0x0A0A0A0A
#define AP	(~(P))
	volatile int *p;
	int o, q;
	u32 limit;

	limit = hmbound;
	p = (int *)MINADDR;
	*p = P;
	if (*p != P)
		return -1;
	for (o = sizeof(u32); MINADDR + o < limit; o <<= 1) {
		p = (int *)(MINADDR + o);
		*p = P;
		if (*p != P)
			return -1;
	}
	p = (int *)MINADDR;
	*p = AP;
	for (o = sizeof(u32); MINADDR + o < limit; o <<= 1) {
		p = (int *)(MINADDR + o);
		if (*p != P)
			return -1;
	}
	p = (int *)MINADDR;
	*p = P;
	for (o = sizeof(u32); MINADDR + o < limit; o <<= 1) {
		p = (int *)(MINADDR + o);
		*p = AP;
		for (q = sizeof(u32); MINADDR + q < limit; q <<= 1) {
			p = (int *)(MINADDR + q);
			if (q != o && *p != P)
				return -1;
		}
		p = (int *)(MINADDR + o);
		*p = P;
	}
	return 0;
}

static
void
init()
{
	u32 v;

	v = (0x10 << 24 | 0x3 << 16 | 0x8 << 8 | 0xf0);
	outl(PCIC_ADDR_PORT, PCICADDR(0, 0, 0, 0xD0));
	outl(PCIC_DATA_PORT, v);
	outl(PCIC_ADDR_PORT, PCICADDR(0, 0, 0, 0xD4));
	inl(PCIC_DATA_PORT, &v);
	hmbound = v;
}

int
mchk()
{
	init();
	if (ckdb() != 0)
		return -1;
	else if (ckab() != 0)
		return -1;
	else
		return 0;
}
