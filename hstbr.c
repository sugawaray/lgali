#include <quark.h>
#include <hstbr.h>

enum {
	Oppctlread	= 0x12,
	Oppctlwrite	= 0x13,
	Ohmbound	= 0x08,
	Omisc2	= 0x03,
	Ohsmmctl	= 0x04,
	Oesramctrl	= 0x81,
	Opgctlblk	= 0x82
#define MHMBOUND	0xFFFFF000
};

void
readhminfo(struct Hminfo *o)
{
	o->hmbound = mbr(Mphstbr, Ohmbound);
	o->hmisc2 = mbr(Mphstbr, Omisc2);
	o->hsmmctl = mbr(Mphstbr, Ohsmmctl);
}

void
readhbmm(struct Hbmm *o)
{
	o->esramctrl = mbr(Mpmm, Oesramctrl);
	o->pgctlblk = mbr(Mpmm, Opgctlblk);
}

#define OESRAMPGCTL(n)	((n) * 4)

u32
readesrampgctl(u8 n)
{
	return mbr1(Oppctlread, Mpmm, OESRAMPGCTL(n));
}
