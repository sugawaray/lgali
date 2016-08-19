#include <lapic.h>
#include <tick.h>

#define BASE	0xfee00000
#define R(o)	(*(volatile u32 *)(BASE + (o)))
enum {
	Siv	= 0x00f0,
	Timer	= 0x0320,
	TIC	= 0x0380,
	TCC	= 0x0390,
	TDC	= 0x03e0
};

enum {
	Otimermode	= 0x11,
	Periodic	= 1,
	Omask	= 0x10,
	Masked	= 1,
	NotMasked	= 0,
	Ovector	= 0x00,

	DivX1	= 0xB
};

enum {
	Msv	= 0x000000ff,
	Oenable	= 0x08
};

void
lapicinit(u8 spurvec)
{
	R(Siv) = (R(Siv) & ~Msv) | spurvec;
}

void
timerinit(u8 vec)
{
	u32 t;
	t = 0;
	t |= Periodic << Otimermode;
	t |= Masked << Omask;
	t |= vec << Ovector;
	R(TDC) = DivX1;
	R(Timer) = t;
}

void
timerstart(int hz, int rhz)
{
	u32 x, t;
	u32 p, d;

	R(TIC) = 0xffffffff;
	t = ticks + rhz;
	do {
		x = ticks;
	} while (x < t);
	p = R(TCC);
	d = 0xffffffff - p;
	R(TIC) = d / hz;
	R(Timer) &= ~(1 << Omask);
}
