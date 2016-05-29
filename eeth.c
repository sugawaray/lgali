#include <quark.h>
#include <led.h>
#include <ser.h>

#define IDBUS	0
#define IDDEV	20
#define IDFUN	6
#define RVENDOR	0x0
#define RCMD	0x04
#define RSTATUS	0x06
#define RINTRLINE	0x3C
#define RINTRPIN	0x3D
#define RMSGCTRL	0xA2
#define RMSGADDR	0xA4

int
main()
{
#if 1
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
	for (;;)
		;
	return 0;
#else
	u8 v8;
	u16 v16;
	u32 v32;
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
	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, RVENDOR));
	inw(PCIC_DATA_PORT, &v16);
	seroutf("ethernet vendor id(0x%X)\r\n", v16);
	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, RCMD));
	inw(PCIC_DATA_PORT, &v16);
	seroutf("ethernet command register(0x%X)\r\n", v16);
	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, RINTRLINE));
	inb(PCIC_DATA_PORT, &v8);
	seroutf("ethernet interrupt line register(0x%X)\r\n", v8);

	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, RINTRPIN));
	inb(PCIC_DATA_PORT, &v8);
	seroutf("ethernet interrupt pin register(0x%X)\r\n", v8);

	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, RSTATUS));
	inw(PCIC_DATA_PORT, &v16);
	seroutf("ethernet status register(0x%X)\r\n", v16);

	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, RMSGCTRL));
	inw(PCIC_DATA_PORT, &v16);
	seroutf("ethernet message control register(0x%X)\r\n", v16);

	outl(PCIC_ADDR_PORT, PCICADDR(IDBUS, IDDEV, IDFUN, RMSGADDR));
	inl(PCIC_ADDR_PORT, &v32);
	seroutf("ethernet message address register(0x%X)\r\n", v32);

	for (;;)
		;
	return 0;
#endif
}
