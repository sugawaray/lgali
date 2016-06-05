#include "intrim.h"
#include <quark.h>
#include <intr.h>
#include <led.h>
#include <lib.h>
#include <ser.h>

#define IDDEV	20
#define IDFUN	6

#define VEC	0x30

static struct Idtrec idt[0x100];

#define RLAPICID	0xFEE00020

#define MSGADDR	0xFEE00000
#define MSGDATA	(0x00 | VEC)

#define OFFBAR0	0x10
#define OFFMSGADDR	0xA4
#define OFFMSGDATA	0xA8

static u32 *rintren;

static
void
configmsg()
{
	u16 v16;
	u32 *a;
	u32 v;

	a = (u32 *)RLAPICID;
	v = *a;
	pcicw32(OFFMSGADDR, MSGADDR | (v << 0xC));
	v = 0;
	pcicr32(OFFMSGADDR, &v);
	seroutf("MSGADDR(0x%X)\r\n", v);
	pcicw16(OFFMSGDATA, MSGDATA);
	v16 = 0;
	pcicr16(OFFMSGDATA, &v16);
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
	pcicr32(OFFBAR0, &v32);
	v32 &= ~0 << 12;
	rintren = (u32 *)(v32 + 0x101C);

	seroutf("MM interrupt enable register(0x%X, 0x%X)\r\n", rintren, *rintren);
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
		ledmemit(5, 1);
		wait();
		wait();
	}
	return 0;
}
