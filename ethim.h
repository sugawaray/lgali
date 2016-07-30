#include <quark.h>

u32 ethdefmcnf(u32 orig);
u32 ethdefmff(u32 orig);

enum {
	Msr	= 0x00000002,
	Mst	= 0x00002000,

#define RDOWN	0x80000000
	Rdfs	= 0x00000200,
	Rdls	= 0x00000100,
	Mrdfl	= 0x0FFF0000,
	Ordfl	= 0x10
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

void rdescinit(struct Rdesc *o, void *b1, int b1sz, void *b2, int b2sz);
void tdescinit(struct Tdesc *o, void *b1, int b1sz, void *b2, int b2sz);

int rdisavail(const volatile struct Rdesc *o);
int rdflen(const volatile struct Rdesc *o);
void prrdbuf(const volatile struct Rdesc *o);
