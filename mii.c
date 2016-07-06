#include "miiim.h"
#include <mii.h>
#include <ser.h>

enum {
	Mgmiipa	=	0x0000F800,
	Mgmiireg	=	0x000007C0,
	Mcsrclkrng	=	0x0000003C,
	Csrclkrng	=	0x00000008,
	Gmiiwrite	=	0x00000002,
	Gmiibusy	=	0x00000001,
	Offr	=	0x06,
	Rctl	=	0x00,
	Rstatus	=	0x01,
	Ranadv	=	0x04,
	Ranlpa	=	0x05,
	Mcreset	=	0x00008000,
	Mcanen	=	0x00001000,
	Mcrsan	=	0x00000200,
	Msana	=	0x0008,

	Oselector	=	0x0,
	Mselector	=	0x001F,
	Sel8023	=	0x0001,
	Otch	=	0x5,
	Mtcha	=	0x0FE0,
	Tch10full	=	0x2,
	Oxnp	=	0xC,
	Mxnp	=	0xC,
	Oack	=	0xE,
	Mack	=	0xE,

	Mstancmp	=	0x0020
};

#define R(o)	(*(volatile u32 *)(o))

static u32 offadd;
static u32 offdat;

static
void
readr(int pa, int reg, u32 *dat)
{
	u32 a, b;

	a = 0;
	a |= (pa << 10) & Mgmiipa;
	a |= Mgmiireg & (reg << Offr);
	a |= Csrclkrng;
	a |= Gmiibusy;
	while (R(offadd) & Gmiibusy)
		;
	R(offadd) = a;
	while (R(offadd) & Gmiibusy)
		;
	b = R(offdat);
	*dat = b;
}

static
void
writer(int pa, int reg, u32 dat)
{
	u32 a;

	a = 0;
	a |= Mgmiireg & (reg << Offr);
	a |= Gmiiwrite;
	a |= (pa << 10) & Mgmiipa;
	a |= Csrclkrng;
	a |= Gmiibusy;
	while (R(offadd) & Gmiibusy)
		;
	R(offadd) = a;
	while (R(offadd) & Gmiibusy)
		;
	R(offdat) = dat;
	R(offadd) |= Gmiibusy;
	while (R(offadd) & Gmiibusy)
		;
}

static
void
readctl(int pa, u32 *dat)
{
	u32 a, b;

	a = 0;
	a |= (pa << 10) & Mgmiipa;
	a |= Csrclkrng;
	a |= Gmiibusy;
	while (R(offadd) & Gmiibusy)
		;
	R(offadd) = a;
	while (R(offadd) & Gmiibusy)
		;
	b = R(offdat);
	*dat = b;
}

static
void
writectl(int pa, u32 dat)
{
	u32 a;

	a = 0;
	a |= Gmiiwrite;
	a |= (pa << 10) & Mgmiipa;
	a |= Csrclkrng;
	a |= Gmiibusy;
	while (R(offadd) & Gmiibusy)
		;
	R(offadd) = a;
	while (R(offadd) & Gmiibusy)
		;
	R(offdat) = dat;
	R(offadd) |= Gmiibusy;
	while (R(offadd) & Gmiibusy)
		;
}

void
miisetradd(u32 v)
{
	offadd = v;
}

void
miisetrdat(u32 v)
{
	offdat = v;
}

void
miiresetphy(int pa)
{
	u32 v;

	writectl(pa, Mcreset);
	do {
		readctl(pa, &v);
	} while ((v & Mcreset) == 0);
}

void
miiinit(int pa)
{
	u32 v;

	readr(pa, Rstatus, &v);
	v = (v & Msana) != 0;
	seroutf("PHY(0x%X) Auto negotiation ability(0x%X)\r\n", pa, v);

	readr(pa, Ranadv, &v);
	writer(pa, Ranadv, defanadv(v));

	writer(pa, Rctl, Mcrsan | Mcanen);
	do {
		readr(pa, Rctl, &v);
	} while ((v & Mcrsan) != 0);
	seroutf("PHY(0x%X) Auto negotiation done\r\n", pa);

	do {
		v = 0;
		readr(pa, Rstatus, &v);
	} while (!(v & Mstancmp));
	seroutf("PHY(0x%X) status(0x%X)\r\n", pa, v);

	v = 0;
	readr(pa, Ranlpa, &v);
	seroutf("PHY(0x%X) auto neg link partner ability(0x%X)\r\n", pa, v);
}


int
miitestphy(int pa)
{
	int r;
	u32 v;

	writectl(pa, 1 << 14 | 1 << 10);
	readctl(pa, &v);
	r = ((v & (1 << 14 | 1 << 10)) == (1 << 14 | 1 << 10));
	writectl(pa, v & ~(1 << 14 | 1 << 10));
	return r;
}

u16
defanadv(u16 def)
{
	def &= ~(Mselector | Mtcha);
	def |= (Sel8023 << Oselector) & Mselector;
	def |= (Tch10full << Otch) & Mtcha;
	return def;
}
