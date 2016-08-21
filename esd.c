#include "sdim.h"
#include <led.h>
#include <quark.h>
#include <ser.h>

enum {
	Dev	= 20,
	Fn	= 0
};

int
main()
{
	int r;
	u32 base;

	wait();
	ledminit();
	r = serinit();
	if (r != 0) {
		ledmemit(2, 5);
		for (;;)
			;
	}
	serout("serinit done\r\n");
	pcicsetdev(Dev);
	pcicsetfn(Fn);
	pcicpr();
	pcicr32(POFFBAR0, &base);
	sdsetbar0(base);
	sdpwrup();
	sdinireg();
	sdinistat();
	sdenclk();
	sdclrstat();
	sddbg();
	wait();
	sdcmd0();
#if 1
	r = sdcvalid(SdV27t36);
#else
	r = sdcvalid(SdVLowRng);
#endif
	seroutf("condition validation result(0x%X)\r\n", r);
	serout("processing done\r\n");
	sddbg();
	for (;;)
		;
	return 0;
}
