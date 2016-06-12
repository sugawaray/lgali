#include "intrim.h"
#include "ethim.h"
#include <quark.h>
#include <intr.h>
#include <led.h>
#include <lib.h>
#include <ser.h>

#define IDDEV	20
#define IDFUN	6

#define VEC	0x30
#define SVEC	0x31

static struct Idtrec idt[0x100];

enum {
	Rdsclbase	= 0x100000,
	Tdsclbase	= 0x100100
};

static volatile struct Rdesc *rdesc = Rdsclbase;
static volatile struct Tdesc *tdesc = Tdsclbase;


#define RLAPICID	0xFEE00020
#define RSIV	0xFEE000F0
#define MAPICID 0x07000000
#define APICEN	0x00000100

#define MSGADDR	0xFEE00000
#define MSGDATA	(0x00 | VEC)


static u32 base0;
static volatile u32 *rmacconf;
static volatile u32 *rmacff;
static volatile u32 *rmacad0h;
static volatile u32 *rmacad0l;
static volatile u32 *rintren;
static volatile u32 *rsiv;
static volatile u32 *rrdsclad;
static volatile u32 *rtdsclad;
static volatile u32 *ropmod;

enum {
	Busmasen	=	0x0004,
	Memspcen	=	0x0002,

#define AE	0x80000000
	Rcvintren	=	0x00000040
};

enum {
	Mmmacconf	= 0x0000,
	Mmmacff	= 0x0004,
	Mmmacad0h	= 0x0040,
	Mmmacad0l	= 0x0044,
	Mmrdsclad	= 0x100C,
	Mmtdsclad	= 0x1010,
	Mmopmod	= 0x1018,
	Mmintren	= 0x101C
};
static
void
initpcic()
{
	u32 v32;

	pcicw16(POFFCMD, Busmasen | Memspcen);
	pcicr32(POFFBAR0, &v32);
	v32 &= ~0 << 12;
	base0 = v32;
}

static
void
initmmr()
{
	rmacconf = (u32 *)(base0 + Mmmacconf);
	rmacff = (u32 *)(base0 + Mmmacff);
	rmacad0h = (u32 *)(base0 + Mmmacad0h);
	rmacad0l = (u32 *)(base0 + Mmmacad0l);
	rintren = (u32 *)(base0 + Mmintren);
	rrdsclad = (u32 *)(base0 + Mmrdsclad);
	rtdsclad = (u32 *)(base0 + Mmtdsclad);
	ropmod = (u32 *)(base0 + Mmopmod);

	*rmacad0h = AE | MACADDRH;
	*rmacad0l = MACADDRL;
	*rmacconf = ethdefmcnf(*rmacconf);
	*rmacff = ethdefmff(*rmacff);
	*ropmod &= ~Msr;
	while (*ropmod & Msr)
		;
	*rrdsclad = Rdsclbase;
	*ropmod &= ~Mst;
	while (*ropmod & Mst)
		;
	*rtdsclad = Tdsclbase;

	seroutf("MAC Configuration register(0x%X)\r\n", *rmacconf);
	seroutf("MM interrupt enable register(0x%X, 0x%X)\r\n", rintren, *rintren);
}

static
void
init1()
{
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
}

static
void
initintr()
{
	*rintren |= Rcvintren;
	rsiv = (u32 *)RSIV;
	seroutf("Spurious Interrupt Vector Register(0x%X)\r\n", *rsiv);
	*rsiv = *rsiv | APICEN | VEC;
}


static
void
intrinit()
{
	struct Idtrec *rec;
	char tp[6];

	rec = &idt[VEC];
	sidt(tp);
	mmemcpy(idt, *(u32 *)(tp + 2), *(u16 *)tp + 1);
	cli();
	idtrsetssel(rec, 0x02);
	idtrsettype(rec, IDTRINTR);
	idtrsetad(rec, oneth);
	initpcic();
	init1();
	initmmr();
	initintr();
	packt((const char *)idt, sizeof idt / sizeof idt[0], tp);
	lidt(tp);
	sti();
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
	serout("ethernet pci configuration registers.\r\n");
	pcicsetdev(IDDEV);
	pcicsetfn(IDFUN);
	pcicpr();
	intrinit();

	for (;;) {
#if 1
		asm ("int $0x30\n\t");
		ledmemit(10, 1);
#endif
		wait();
		wait();
	}
	return 0;
}
