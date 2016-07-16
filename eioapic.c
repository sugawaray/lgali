#include <quark.h>
#include <ser.h>

enum {
	Oid	=	0x00,
	Ovs	=	0x01,
	Orteb	=	0x10,
#define IDX	0xFEC00000
#define WDW	0xFEC00010
	Rtecnt	=	0x24
};

#define R8(a)	(*(volatile u8 *)(a))
#define R32(a)	(*(volatile u32 *)(a))
#define ORTEL(i)	(Orteb + (i) * 2)
#define ORTEH(i)	(ORTEL(i) + 1)

int
main()
{
	int i;
	u32 v32, v32a;
	if (serinit() != 0)
		while (1)
			;
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
	return 0;
}
