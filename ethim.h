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
void prrdbuf(const volatile struct Rdesc *o, int i);

struct Rx {
	int bp;
	int pos;
	int fs;
	int ls;
	volatile struct Rdesc *rd;
};
ssize_t read1(struct Rx *o, int fildes, void *buf, size_t nbyte);

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
