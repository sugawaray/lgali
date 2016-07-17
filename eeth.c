#include "intrim.h"
#include "ethim.h"
#include <eth.h>
#include <quark.h>
#include <intr.h>
#include <led.h>
#include <lib.h>
#include <mii.h>
#include <ser.h>

#define IDDEV	20
#define IDFUN	6

#define VEC	0x41
#define SVEC	0x40

static struct Idtrec idt[0x100];

enum {
	Rdsclbase	= 0x100000,
	Rbuf1ad	= 0x100200,
	Rbuf1sz	= 0x200,
	Rbuf2ad	= 0x100500,
	Rbuf2sz	= 0x100,
	Tdsclbase	= 0x100600,
	Tbuf1ad	= 0x100700,
	Tbuf1sz	= 0x100,
	Tbuf2ad	= 0x100800,
	Tbuf2sz	= 0x100,

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

#define MR(a)	(*(volatile u32 *)(base0 + (a)))
#define LR(a)	(*(volatile u32 *)(a))

static u32 base0;

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

static
void
initdma()
{
	mmemset((void *)Rbuf1ad, 0, Rbuf1sz);
	mmemset((void *)Rbuf2ad, 0, Rbuf2sz);
	mmemset((void *)Tbuf1ad, 0, Tbuf1sz);
	mmemset((void *)Tbuf2ad, 0, Tbuf2sz);
	rdescinit(rdesc, (void *)Rbuf1ad, Rbuf1sz, (void *)Rbuf2ad, Rbuf2sz);
	tdescinit(tdesc, (void *)Tbuf1ad, Tbuf1sz, (void *)Tbuf2ad, Tbuf2sz);
	rdesc->status = rdesc->status | 0x80000000;
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

int
main()
{
	int r;

	wait();
	ledminit();
	r = serinit();
	if (r != 0) {
		ledmemit(5, 100);
		for (;;)
			;
	}
	serout("serial port initialization has done.\r\n");
#if 0
	pcicsetdev(IDDEV);
	pcicsetfn(IDFUN);
	pcicpr();
	init();
#else
	ethinit();
#endif

	for (;;) {
#if 0
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
		wait();
		wait();
#else
		ethdbg();
#endif
	}
	return 0;
}
