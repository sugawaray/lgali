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

#define RVENDOR	0x0
#define RCMD	0x04
#define RSTATUS	0x06
#define RINTRLINE	0x3C
#define RINTRPIN	0x3D
#define RMSGCTRL	0xA2
#define RMSGADDR	0xA4
#define RMSGDATA	0xA8
#define RVECMASK	0xAC

#define ADDR(o)	(PCICADDR(bus, dev, fn, (o)))

void
pcicpr()
{
	u8 v8;
	u16 v16;
	u32 v32;

	outl(PCIC_ADDR_PORT, ADDR(RVENDOR));
	inw(PCIC_DATA_PORT, &v16);
	seroutf("vendor id(0x%X)\r\n", v16);

	outl(PCIC_ADDR_PORT, ADDR(RCMD));
	inw(PCIC_DATA_PORT, &v16);
	seroutf("command register(0x%X)\r\n", v16);

	outl(PCIC_ADDR_PORT, ADDR(RINTRLINE));
	inb(PCIC_DATA_PORT, &v8);
	seroutf("interrupt line register(0x%X)\r\n", v8);

	outl(PCIC_ADDR_PORT, ADDR(RINTRPIN));
	inb(PCIC_DATA_PORT, &v8);
	seroutf("interrupt pin register(0x%X)\r\n", v8);

	outl(PCIC_ADDR_PORT, ADDR(RSTATUS));
	inw(PCIC_DATA_PORT, &v16);
	seroutf("status register(0x%X)\r\n", v16);

	outl(PCIC_ADDR_PORT, ADDR(RMSGCTRL));
	inw(PCIC_DATA_PORT, &v16);
	seroutf("message control register(0x%X)\r\n", v16);

	outl(PCIC_ADDR_PORT, ADDR(RMSGADDR));
	inl(PCIC_DATA_PORT, &v32);
	seroutf("message address register(0x%X)\r\n", v32);

	outl(PCIC_ADDR_PORT, ADDR(RMSGDATA));
	inw(PCIC_DATA_PORT, &v16);
	seroutf("message data register(0x%X)\r\n", v16);

	outl(PCIC_ADDR_PORT, ADDR(RVECMASK));
	inl(PCIC_DATA_PORT, &v32);
	seroutf("mask bits for MSI(0x%X)\r\n", v32);
}
