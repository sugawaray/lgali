#include <quark.h>
#include "intrim.h"
#include <intr.h>
#include <led.h>

void
packt(const char *p, int rc, char *o)
{
	*(u16*)o = rc * 8 - 1;
	o += sizeof(u16);
	*(u32*)o = (u32)p;
}

void
outgdt(const char *p)
{
	u16 li;
	u32 ba;
	u32 (*a)[2];
	u32 (*b)[2];
	int i;

	li = *(u16 *)p;
	ba = *(u32 *)(p + sizeof(u16));
	a = (u32 (*)[2])ba;
	b = (u32 (*)[2])(ba + li + 1);
	for (i = 0, ++a; a < b; ++i, ++a) {
		seroutf("(0x%X), info(0x%X), acce(0x%X), type(0x%X)\r\n",
			i,
			((*a)[1] & 0xFF0000) >> 0x10,
			((*a)[1] & 0xF000) >> 0xC,
			((*a)[1] & 0xF00) >> 0x8);
	}
}

void
ontrap1()
{
	ledmemit(2, 1);
}

void
ontimer1()
{
	ledmemit(2, 1);
}

void
idtrsetad(struct Idtrec *o, void (*f)())
{
	o->offl = ((u32)f & 0xFFFF);
	o->offh = ((u32)((u32)f & 0xFFFF0000) >> 0x10);
}

void
idtrsettype(struct Idtrec *o, u8 t)
{
	o->attr = 0x8000 | (t << 0x8);
}

void
idtrsetssel(struct Idtrec *o, u8 v)
{
	o->ssel = v << 0x3;
}

void
idtrsetvalid(struct Idtrec *o, int isvalid)
{
	if (isvalid)
		o->attr |= 0x8000;
	else
		o->attr &= 0x7FFF;
}
