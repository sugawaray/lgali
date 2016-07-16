#include "ethim.h"
#include <led.h>

enum {
	Msarc	= 0x70000000,
	Sarc	= 0x20000000,
	Mjd	= 0x00400000,
	Jdng	= 0x00400000,
	Mfes	= 0x00004000,
	Fes10	= 0x00000000,
	Mdm	= 0x00000800,
	Dmfull	= 0x00000800,
	Mte	= 0x00000008,
	Te	= 0x00000008,
	Mre	= 0x00000004,
	Re	= 0x00000004,
/* #define AE	0x80000000 */
#define MRA	0x80000000
#define RA	0x80000000
	Mrdscdic	= 0x8000,
	Mrdscb2sz	= 0x1FFF,
	Mrdscrer	= 0x8000,
	Mrdscb1sz	= 0x1FFF,
	Mtdscic	= 0x40000000,
	Mtdscls	= 0x20000000,
	Mtdscfs	= 0x10000000,
	Mtdscter	= 0x00200000,
	Mtdscb2sz	= 0x1FFF,
	Mtdscb1sz	= 0x1FFF
};

u32
ethdefmcnf(u32 orig)
{
	orig &= ~Msarc;
	orig |= Sarc;

	orig &= ~Mjd;
	orig |= Jdng;

	orig &= ~Mfes;
	orig |= Fes10;

	orig &= ~Mdm;
	orig |= Dmfull;

	orig &= ~Mte;
	orig |= Te;

	orig &= ~Mre;
	orig |= Re;

	return orig;
}

u32
ethdefmff(u32 orig)
{
	orig &= MRA;
	orig |= RA;
	return orig;
}

void
rdescinit(struct Rdesc *o, void *b1, int b1sz, void *b2, int b2sz)
{
	o->des1h = 0;
	o->des1l = 0;
	o->des1l |= b1sz & Mrdscb1sz;
	o->des1l &= ~Mrdscrer;
	o->des1l |= Mrdscrer & 0xFFFF;
	o->des1h |= b2sz & Mrdscb2sz;
	o->b1addr = (u32)b1;
	o->b2addr = (u32)b2;
}

void
tdescinit(struct Tdesc *o, void *b1, int b1sz, void *b2, int b2sz)
{
	o->des0 |= Mtdscic & 0xFFFFFFFF;
	o->des0 |= Mtdscls & 0xFFFFFFFF;
	o->des0 |= Mtdscfs & 0xFFFFFFFF;
	o->des0 |= Mtdscter & 0xFFFFFFFF;
	o->des1h |= Mtdscb2sz & b2sz;
	o->des1l |= Mtdscb1sz & b1sz;
	o->b1addr = (u32)b1;
	o->b2addr = (u32)b2;
}

#define REOI	0xFEE000B0
#define R2(a)	(*(volatile u32 *)(a))
void
oneth1()
{
	ledmemit(2, 1);
	pcicw32(POFFVECPEND, 0);
	R2(REOI) = 1;
}
