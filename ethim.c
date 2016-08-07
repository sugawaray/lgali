#include "intrim.h"
#include "ethim.h"
#include <eth.h>
#include <quark.h>
#include <intr.h>
#include <led.h>
#include <lib.h>
#include <mii.h>
#include <ser.h>
#include <unistd.h>

enum {
	Mmmacconf	= 0x0000,
	Mmmacff	= 0x0004,
	Mmgmiiadd	= 0x0010,
	Mmgmiidat	= 0x0014,
	Mmflowctl	= 0x0018,
	Mmmacdbg	= 0x0024,
	Mmacintr	= 0x0038,
	Mmmacad0h	= 0x0040,
	Mmmacad0l	= 0x0044,
	Mmrdsclad	= 0x100C,
	Mmtdsclad	= 0x1010,
	Mmstatus	= 0x1014,
	Mmopmod	= 0x1018,
	Mmmfroc	= 0x1020,
	Mmintren	= 0x101C,
	Mmcurrdsc	= 0x104C,
	Mmcurrbuf	= 0x1054
};

static u32 base0;
#define MR(a)	(*(volatile u32 *)(base0 + (a)))

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
	MR(Mmstatus) |= 0x1FFFF;
}

#define IDDEV	20
#define IDFUN	6

#define VEC	0x41
#define SVEC	0x40

static struct Idtrec idt[0x100];

enum {
	Rdsclbase	= 0x100000,
	Rdsz	= 0x10,
	Rbuf1ad	= 0x102000,
	Rbuf1sz	= 0x800,
	Rbuf2ad	= 0x102800,
	Rbuf2sz	= 0x800,
	Tdsclbase	= 0x101000,
	Tbuf1ad	= 0x202000,
	Tbuf1sz	= 0x800,
	Tbuf2ad	= 0x202800,
	Tbuf2sz	= 0x800,

	Rxblen	= 0x100,
	Phyaddr	=	0x02
};

static volatile struct Rdesc *rdesc = Rdsclbase;
static volatile struct Tdesc *tdesc = Tdsclbase;

#define RLAPICID	0xFEE00020
#define RSIV	0xFEE000F0
#define RESR	0xFEE00280
#define REOI	0xFEE000B0
#define MAPICID 0x07000000
#define APICEN	0x00000100

#define MSGADDR	0xFEE00000
#define MSGDATA	(0x0000 | VEC)

#define LR(a)	(*(volatile u32 *)(a))


enum {
	Busmasen	=	0x0004,
	Memspcen	=	0x0002,
	Msien	= 0x0001,
	PerVecMskCap	=	0x0100,
#define MBAR0ADDR	0xFFFFF000

#define AE	0x80000000
	Rcvintren	=	0x00018140,
	Mgmiipa	=	0x0000F800,
	Mgmiireg	=	0x000007C0,
	Mcsrclkrng	=	0x0000003C,
	Csrclkrng	=	0x00000008,
	Gmiiwrite	=	0x00000002,
	Gmiibusy	=	0x00000001
};


static
void
disintr()
{
	cli();
	*((volatile u32*)RSIV) &= ~APICEN;
}

static void act();

static
void
enintr()
{
	MR(Mmintren) |= Rcvintren;
	seroutf("Spurious Interrupt Vector Register(0x%X)\r\n", LR(RSIV));
	LR(RSIV) |= APICEN | SVEC;
	act();
	sti();
}

static
u8
rp8(int off)
{
	u8 v;
	pcicr8(off, &v);
	return v;
}

static
u16
rp16(int off)
{
	u16 v;
	pcicr16(off, &v);
	return v;
}

static
void
initpcic()
{
	u32 v32;
	u16 v16;
	u8 v8;

	pcicw16(POFFCMD, Busmasen | Memspcen);

	pcicr32(POFFBAR0, &v32);
	base0 = v32 & MBAR0ADDR;

	pcicr8(POFFNXTCAPP, &v8);
	pcicw16(v8 + 2, PerVecMskCap | Msien);
}

static
void
initmmr()
{
	MR(Mmmacad0h) = AE | MACADDRH;
	MR(Mmmacad0l) = MACADDRL;
	MR(Mmmacconf) = ethdefmcnf(MR(Mmmacconf));
	MR(Mmmacff) = ethdefmff(MR(Mmmacff));
	MR(Mmopmod) &= ~Msr;
	while (MR(Mmopmod) & Msr)
		;
	MR(Mmrdsclad) = Rdsclbase;
	MR(Mmopmod) &= ~Mst;
	while (MR(Mmopmod) & Mst)
		;
	MR(Mmtdsclad) = Tdsclbase;

	MR(Mmopmod) = (~0x00000018 & MR(Mmopmod)) | 0x00000008;
	MR(Mmopmod) = (~0x000000C0 & MR(Mmopmod)) | 0x000000C0;
	MR(Mmopmod) = (~0x00000040 & MR(Mmopmod)) | 0x00000040;

	seroutf("MAC Configuration register(0x%X)\r\n", MR(Mmmacconf));
	seroutf("MM interrupt enable register(0x%X)\r\n", MR(Mmintren));
}



static
void
initintr()
{
	struct Idtrec *rec;
	char tp[6];
	u8 v8;
	u16 v16;
	u32 *a;
	u32 v;

	a = (u32 *)RLAPICID;
	v = *a;
	seroutf("APIC ID(0x%X)\r\n", v);
	v = ((unsigned)(v & MAPICID) >> 0x18);
	seroutf("MSGADDR VALUE(0x%X)\r\n", MSGADDR | (v << 0xC));

	pcicw32(POFFMSGADDR, MSGADDR | (v << 0xC));

	v = 0;
	pcicr32(POFFMSGADDR, &v);
	seroutf("MSGADDR(0x%X)\r\n", v);
	pcicw16(POFFMSGDATA, MSGDATA);
	v16 = 0;
	pcicr16(POFFMSGDATA, &v16);
	seroutf("MSGDATA(0x%X)\r\n", v16);


	sidt(tp);
	mmemcpy(idt, *(u32 *)(tp + 2), *(u16 *)tp + 1);

	rec = &idt[VEC];
	idtrsetssel(rec, 0x02);
	idtrsettype(rec, IDTRINTR);
	idtrsetad(rec, oneth);

	rec = &idt[SVEC];
	idtrsetssel(rec, 0x02);
	idtrsettype(rec, IDTRINTR);
	idtrsetad(rec, oneth);

	packt((const char *)idt, sizeof idt / sizeof idt[0], tp);
	lidt(tp);
}

static struct Rx rx;

static
void
initrx(struct Rx *o)
{
	o->bp = 0;
	o->fs = -1;
	o->ls = -1;
	o->pos = 0;
	o->rd = rdesc;
}

#define RDSC(list, i)	((volatile struct Rdesc *)((u32)(list) + (Rdsz * (i))))
static
void
initdma()
{
	u32 a1, a2;
	int i;
	for (i = 0; i < Rxblen; ++i) {
		a1 = Rbuf1ad + (Rbuf1sz + Rbuf2sz) * i;
		mmemset((void *)a1, 0, Rbuf1sz);
		a2 = Rbuf2ad + (Rbuf1sz + Rbuf2sz) * i;
		mmemset((void *)a2, 0, Rbuf2sz);
		rdescinit(RDSC(rdesc, i), (void *)a1, Rbuf1sz, (void *)a2, Rbuf2sz);
	}
	RDSC(rdesc, Rxblen - 1)->des1l |= Mrdscrer;
	initrx(&rx);
	for (i = 0; i < Rxblen; ++i)
		RDSC(rdesc, i)->status = RDSC(rdesc, i)->status | RDOWN;
	mmemset((void *)Tbuf1ad, 0, Tbuf1sz);
	mmemset((void *)Tbuf2ad, 0, Tbuf2sz);
	tdescinit(tdesc, (void *)Tbuf1ad, Tbuf1sz, (void *)Tbuf2ad, Tbuf2sz);
}

static
void
act()
{
	MR(Mmopmod) = MR(Mmopmod) | Msr;
	pcicw32(POFFVECPEND, 0);
	MR(Mmmacconf) = ethdefmcnf(MR(Mmmacconf));
}

static
void
initmii()
{
	miisetradd(base0 + Mmgmiiadd);
	miisetrdat(base0 + Mmgmiidat);
	miiresetphy(Phyaddr);
	miiinit(Phyaddr);
}

static
void
init()
{

	disintr();

	initpcic();
	initdma();
	initmmr();
	initmii();

	initintr();
	enintr();
}

void
ethinit()
{
	pcicsetdev(IDDEV);
	pcicsetfn(IDFUN);
	pcicpr();
	init();
}

void
prrdbuf(const volatile struct Rdesc *o, int i)
{
	int len = rdflen(o);
	if (len >= 0 && (o->status & Rdls)) {
		unsigned char *p = (unsigned char *)o->b1addr;
		seroutf("buf[0x%X]\r\n", i);
		for (i = 0; i < len; ++i) {
			if (i % 0x10 == 0)
				seroutf("\r\n0x%X\t", i);
			seroutf("%X ", p[i]);
		}
		seroutf("\r\n");
	}
}

static
void
prrxbufs()
{
	int i;
	for (i = 0; i < Rxblen; ++i)
		prrdbuf(RDSC(rdesc, i), i);
}

static
void
prdat(const unsigned char *b, int sz)
{
	static int pos = 0;
	int i;

	for (i = 0; i < sz; ++i) {
		if (((pos + i) % 16) == 0)
			seroutf("\r\n%X ", pos + i);
		seroutf("%X ", b[i]);
	}
	if (sz >= 0)
		pos += sz;
}


void
ethdbg()
{
#if 1
	ssize_t sz;
	unsigned char b[0x40];
	sz = read(-1, b, sizeof b);
	if (sz > 0)
		prdat(b, sz);
#else
	seroutf("eflags]0x%X\r\n", eflags());
	seroutf("CMD]0x%X\r\n", rp16(POFFCMD));
	seroutf("STATUS]0x%X\r\n", rp16(POFFSTATUS));
	seroutf("IRR]0x%X\r\n", LR(0xFEE00220));
	miidbg(2);
	seroutf("INTLINE]0x%X\r\n", rp8(POFFINTRLINE));
	seroutf("C:MA]0x%X,0x%X%X\r\n", MR(Mmmacconf), MR(Mmmacad0l), MR(Mmmacad0h));
	seroutf("D:S:O:M]0x%X,0x%X,0x%X,0x%X\r\n",
		MR(Mmmacdbg),
		MR(Mmstatus),
		MR(Mmopmod),
		MR(Mmmfroc));
	seroutf("CRD,CRB]0x%X,0x%X\r\n", MR(Mmcurrdsc), MR(Mmcurrbuf));
	seroutf("Start of Receive List] 0x%X\r\n", MR(Mmrdsclad));
	seroutf("Rdesc]0x%X,0x%X,0x%X,0x%X,0x%X\r\n",
		rdesc->status,
		rdesc->des1l,
		rdesc->des1h,
		rdesc->b1addr,
		rdesc->b2addr
		);
	seroutf("b11st4]0x%X\r\n", *(u32*)rdesc->b1addr);
	seroutf("b11st4-8]0x%X\r\n", *((u32*)rdesc->b1addr + 1));
	seroutf("b21st4]0x%X\r\n", *(u32*)rdesc->b2addr);
	seroutf("b21st4-8]0x%X\r\n", *((u32*)rdesc->b2addr + 1));
	seroutf("ESR]0x%X\r\n", *(u32*)RESR);
	seroutf("Interrupt Register]0x%X\r\n", MR(Mmacintr));
	prrxbufs();
#endif
	wait();
}

int
rdisavail(const volatile struct Rdesc *o)
{
	if (o->status & RDOWN)
		return 0;
	else
		return 1;
}

int
rdflen(const volatile struct Rdesc *o)
{
	if (rdisavail(o)) {
		if (o->status & Rdls)
			return (o->status >> Ordfl) & ((u32)Mrdfl >> Ordfl);
		else
			return (o->des1l & Mrdscb1sz) + (o->des1h & Mrdscb2sz);
	} else
		return -1;
}

static
void
rx2nxt(struct Rx *o)
{
	int i;
	o->fs = -1;
	i = o->bp;
	do {
		if (o->rd[i].status & RDOWN)
			break;
		if (o->rd[i].status & Rdfs)
			o->fs = i;
		if (o->fs != -1 && (o->rd[i].status & Rdls)) {
			o->ls = i;
			break;
		}
		if (o->rd[i].des1l & Mrdscrer)
			i = 0;
		else
			++i;
	} while (o->bp != i);
	if (o->fs != -1)
		o->bp = o->fs;
}

#define XMSKVAL(v, m, o)	(((v) >> (o)) & ((unsigned)(m) >> (o)))
ssize_t
read1(struct Rx *o, int fd, void *buf, size_t nb)
{
	char *bs, *bd;
	int i, d, fl;
	bd = buf;
	while (nb > 0) {
		if (o->ls == -1) {
			rx2nxt(o);
			if (o->ls == -1)
				return bd - (char*)buf;
		}
		if (o->rd[o->bp].status & RDOWN)
			break;
		bs = (char*)o->rd[o->bp].b1addr + o->pos;
		d = fl = XMSKVAL(o->rd[o->bp].status, Mrdfl, Ordfl);
		d -= o->pos;
		if (nb < d)
			d = nb;
		for (i = 0; i < d; ++i)
			bd[i] = bs[i];
		o->pos += d;
		nb -= d;
		bd += d;
		if (o->pos >= fl) {
			o->rd[o->bp].status |= RDOWN;
			o->pos = 0;
			if (o->bp == o->ls)
				o->ls = -1;
			if (o->rd[o->bp].des1l & Mrdscrer)
				o->bp = 0;
			else
				++o->bp;
		}
	}
	return bd - (char*)buf;
}

int
enrx(struct Rx *o)
{
	int rer;
	if (o->bp < 0)
		return -1;
	if (o->ls == -1) {
		rer = o->rd[o->bp].des1l & Mrdscrer;
		o->rd[o->bp].status |= RDOWN;
	} else {
		do {
			rer = o->rd[o->bp].des1l & Mrdscrer;
			o->rd[o->bp].status |= RDOWN;
			if (rer)
				o->bp = 0;
			else
				++o->bp;
			o->rd[o->bp].status |= RDOWN;
		} while (o->bp != o->ls);
	}
	if (rer)
		o->bp = 0;
	else
		++o->bp;
	return 0;
}

ssize_t
read(int fd, void *buf, size_t nb)
{
	return read1(&rx, -1, buf, nb);
}
