#include <quark.h>
#include <led.h>
#include <ser.h>

#define IDBUS	0
#define IDDEV	20
#define IDFUN	7
#define RVENDOR	0x0
#define RCMD	0x04
#define RINTRLINE	0x3C

int
main()
{
	u8 v8;
	u16 v16;
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
	return 0;
}
