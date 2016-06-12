#include <quark.h>

u32 ethdefmcnf(u32 orig);
u32 ethdefmff(u32 orig);

enum {
	Msr	= 0x00000002,
	Mst	= 0x00002000
};

struct Rdesc {
	u32	status;
	u16	des1l;
	u16	des1h;
	u32	b1addr;
	u32	b2addr;
	u32	exstat;
	u32	_rsv1;
	u32	timel;
	u32	timeh;
};

struct Tdesc {
	u32	des0;
	u16	des1l;
	u16	des1h;
	u32	b1addr;
	u32	b2addr;
	u32	_rsv1;
	u32	_rsv2;
	u32	timel;
	u32	timeh;
};

