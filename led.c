#include <quark.h>
#include "ledim.h"
#include <led.h>

u32
getgba()
{
	u32 a;

	a = -1;
	outl(PCIC_ADDR_PORT, PCICADDR_GPIOBASE);
	inl(PCIC_DATA_PORT, &a);
	return a;
}

void
prepled(u32 gba, u16 ledn)
{
	u32 t, s;
	t = gba + OFFRGEN;
	inl(t, &s);
	outl(t, s | 1 << ledn);
	inl(t, &s);
	t = gba + OFFRGIO;
	inl(t, &s);
	outl(t, s & (~0 ^ (1 << ledn)));
}

void
toggleled(u32 gba, u16 ledn)
{
	u32 t, s;
	t = gba + OFFRGLVL;
	inl(t, &s);
	outl(t, s ^ (1 << ledn));
}

static u32 gba;

void
ledminit()
{
	gba = getgba();
	prepled(gba, 1);
}

#define BLEN	1000000
void
ledmemit(int len, int cnt)
{
	long x, j;
	int i;

	x = BLEN * len;
	for (i = 0; i < cnt * 2; ++i) {
		toggleled(gba, 1);
		for (j = 0; j < x; ++j)
			nop();
	}
}
