#include "miiim.h"
#include <mii.h>
#include <ser.h>

enum {
	Mgmiipa	=	0x0000F800,
	Mgmiireg	=	0x000007C0,
	Mcsrclkrng	=	0x0000003C,
#if 1
	Csrclkrng	=	0x00000008,
#else
	Csrclkrng	=	0x00000004,
#endif
	Gmiiwrite	=	0x00000002,
	Gmiibusy	=	0x00000001,
	Offr	=	0x06,
	Rctl	=	0x00,
	Rstatus	=	0x01,
	Ranadv	=	0x04,
	Ranlpa	=	0x05,
	Rphysts	=	0x10,
	Rmicr	=	0x11,
	Rrbr	=	0x17,
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

	Mstancmp	=	0x0020,

	Vrbr	=	0x0020
};

#define R(o)	(*(volatile u32 *)(o))

static u32 offadd;
static u32 offdat;

static
void
readr(int pa, int reg, u16 *dat)
{
	u32 a;
	u16 b;

	a = 0;
	a |= (pa << 10) & Mgmiipa;
	a |= Mgmiireg & (reg << Offr);
	a |= Csrclkrng;
	a |= Gmiibusy;
	while (R(offadd) & Gmiibusy)
		nop();
	R(offadd) = a;
	while (R(offadd) & Gmiibusy)
		nop();
	b = R(offdat);
	*dat = b;
}

static
void
writer(int pa, int reg, u16 dat)
{
	u32 a;

	a = 0;
	a |= Mgmiireg & (reg << Offr);
	a |= Gmiiwrite;
	a |= (pa << 10) & Mgmiipa;
	a |= Csrclkrng;
	a |= Gmiibusy;
	while (R(offadd) & Gmiibusy)
		nop();
	R(offadd) = a;
	while (R(offadd) & Gmiibusy)
		nop();
	R(offdat) = dat;
	while (R(offadd) & Gmiibusy)
		nop();
	R(offadd) |= Gmiibusy;
	while (R(offadd) & Gmiibusy)
		nop();
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
	u16 v;

	writer(pa, Rctl, Mcreset);
	do {
		readr(pa, Rctl, &v);
	} while ((v & Mcreset) == 0);
	wait();
	writer(pa, Rrbr, Vrbr);
	wait();
}

void
miiinit(int pa)
{
	u16 v;

	readr(pa, Rstatus, &v);
	v = (v & Msana) != 0;
	seroutf("PHY(0x%X) Auto negotiation ability(0x%X)\r\n", pa, v);

	readr(pa, Ranadv, &v);
	writer(pa, Ranadv, defanadv(v));

	wait();

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

u16
defanadv(u16 def)
{
	def &= ~(Mselector | Mtcha);
	def |= (Sel8023 << Oselector) & Mselector;
	def |= (Tch10full << Otch) & Mtcha;
	return def;
}

void
miidbg(int pa)
{
	u16 v, v1;
	readr(pa, Rphysts, &v);
	readr(pa, Rmicr, &v1);
	seroutf("PHY S:I]0x%X,0x%X\r\n", v, v1);
}
