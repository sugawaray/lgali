#include <quark.h>
#include <ioapic.h>
#include <ser.h>

enum {
	Oid	=	0x00,
	Ovs	=	0x01,
	Orteb	=	0x10,
#define IDX	0xFEC00000
#define WDW	0xFEC00010
	Rtecnt	=	0x24,
	Pidaid	=	0x18,
	Midaid	=	0x0F000000
};

#define R8(a)	(*(volatile u8 *)(a))
#define R32(a)	(*(volatile u32 *)(a))
#define ORTEL(i)	(Orteb + (i) * 2)
#define ORTEH(i)	(ORTEL(i) + 1)

void
setapicid(int id)
{
	u32 v;

	v = (id << Pidaid) & Midaid;
	R8(IDX) = Oid;
	R32(WDW) = v;
}

void
setrte(int li, u32 lo, u32 hi)
{
	R8(IDX) = ORTEL(li);
	R32(WDW) = lo;
	R8(IDX) = ORTEH(li);
	R32(WDW) = hi;
}

void
ioapicpri()
{
	int i;
	u32 v32, v32a;

	R8(IDX) = Oid;
	v32 = R32(WDW);
	seroutf("ID(0x%X)\r\n", v32);

	R8(IDX) = Ovs;
	v32 = R32(WDW);
	seroutf("Version(0x%X)\r\n", v32);

	for (i = 0; i < Rtecnt; ++i) {
		R8(IDX) = ORTEL(i);
		v32 = R32(WDW);

		R8(IDX) = ORTEH(i);
		v32a = R32(WDW);

		seroutf("RTE[0x%X](0x%X,0x%X)\r\n", i, v32a, v32);
	}
}
