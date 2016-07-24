enum {
	Mphstbr	= 0x03,
	Mpmm	= 0x05
};

struct Hminfo {
	u32	hmbound;
	u32	hmisc2;
	u32	hsmmctl;
};

void readhminfo(struct Hminfo *o);

#define MHMBOUND	0xFFFFF000

struct Hbmm {
	u32	esramctrl;
	u32	pgctlblk;
};

void readhbmm(struct Hbmm *o);
u32 readesrampgctl(u8 n);
