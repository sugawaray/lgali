#include "intrim.h"
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

#define RLAPICID	0xFEE00020
#define RSIV	0xFEE000F0
#define MAPICID 0x07000000
#define APICEN	0x00000100

#define MSGADDR	0xFEE00000
#define MSGDATA	(0x00 | VEC)


static u32 base0;
static volatile u32 *rmacconf;
static volatile u32 *rintren;
static volatile u32 *rsiv;
static volatile u32 *ropmod;

static
void
configmsg()
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
intrinit()
{
	u32 v32;
	struct Idtrec *rec;
	char tp[6];

	rec = &idt[VEC];
	sidt(tp);
	mmemcpy(idt, *(u32 *)(tp + 2), *(u16 *)tp + 1);
	cli();
	idtrsetssel(rec, 0x02);
	idtrsettype(rec, IDTRINTR);
	idtrsetad(rec, oneth);
	configmsg();
	pcicw16(POFFCMD, 0x0002);
	pcicr32(POFFBAR0, &v32);
	v32 &= ~0 << 12;
	base0 = v32;

	rmacconf = (u32 *)(base0 + 0x0000);
	seroutf("MAC Configuration register(0x%X)\r\n", *rmacconf);
	rintren = (u32 *)(v32 + 0x101C);

	seroutf("MM interrupt enable register(0x%X, 0x%X)\r\n", rintren, *rintren);
	*rintren |= 0x00000040;
	rsiv = (u32 *)RSIV;
	seroutf("Spurious Interrupt Vector Register(0x%X)\r\n", *rsiv);
	*rsiv = *rsiv | APICEN | VEC;

	ropmod = (u32 *)(base0 + 0x1018);
	seroutf("Operation mode register(0x%X)\r\n", *ropmod);

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
