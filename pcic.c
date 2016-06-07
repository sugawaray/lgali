#include <quark.h>
#include <ser.h>

static int bus;
static int dev;
static int fn;

void
pcicsetdev(int v)
{
	dev = v;
}

void
pcicsetfn(int v)
{
	fn = v;
}

#define ADDR(o)	(PCICADDR(bus, dev, fn, (o)))

void
pcicpr()
{
	u8 v8;
	u16 v16;
	u32 v32;

	pcicr16(POFFVENDOR, &v16);
	seroutf("vendor id(0x%X)\r\n", v16);

	pcicr16(POFFCMD, &v16);
	seroutf("command register(0x%X)\r\n", v16);

	pcicr8(POFFINTRLINE, &v8);
	seroutf("interrupt line register(0x%X)\r\n", v8);

	pcicr8(POFFINTRPIN, &v8);
	seroutf("interrupt pin register(0x%X)\r\n", v8);

	pcicr16(POFFSTATUS, &v16);
	seroutf("status register(0x%X)\r\n", v16);

	pcicr32(POFFBAR0, &v32);
	seroutf("base address register(0x%X)\r\n", v32);

	pcicr16(POFFMSGCTRL, &v16);
	seroutf("message control register(0x%X)\r\n", v16);

	pcicr32(POFFMSGADDR, &v32);
	seroutf("message address register(0x%X)\r\n", v32);

	pcicr16(POFFMSGDATA, &v16);
	seroutf("message data register(0x%X)\r\n", v16);

	pcicr32(POFFVECMASK, &v32);
	seroutf("mask bits for MSI(0x%X)\r\n", v32);
}

void
pcicw32(u32 off, u32 v)
{
	outl(PCIC_ADDR_PORT, ADDR(off));
	outl(PCIC_DATA_PORT, v);
}

void
pcicr32(u32 off, u32 *v)
{
	outl(PCIC_ADDR_PORT, ADDR(off));
	inl(PCIC_DATA_PORT, v);
}

void
pcicw16(u32 off, u16 v)
{
	outl(PCIC_ADDR_PORT, ADDR(off));
	outw(PCIC_DATA_PORT, v);
}

void
pcicr16(u32 off, u16 *v)
{
	outl(PCIC_ADDR_PORT, ADDR(off));
	inw(PCIC_DATA_PORT, v);
}

void
pcicr8(u32 off, u8 *v)
{
	outl(PCIC_ADDR_PORT, ADDR(off));
	inb(PCIC_DATA_PORT, v);
}
