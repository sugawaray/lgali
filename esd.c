#include "sdim.h"
#include "ctl.h"
#include <led.h>
#include <lib.h>
#include <quark.h>
#include <ser.h>

enum {
	Dev	= 20,
	Fn	= 0
};

static
void
pr(unsigned char *b, int sz)
{
	int i;

	for (i = 0; i < sz; ++i) {
		if (i % 16 == 0)
			seroutf("\r\n%X ", i);
		seroutf("%X ", b[i]);
	}
}

int
main()
{
	int r;
	u32 base;
	struct Sdctx ctx;
	char b[512];

	wait();
	ledminit();
	r = serinit();
	if (r != 0) {
		ledmemit(2, 5);
		for (;;)
			;
	}
	init();
	serout("serinit done\r\n");
	pcicsetdev(Dev);
	pcicsetfn(Fn);
	pcicpr();
	pcicr32(POFFBAR0, &base);
	sdsetbar0(base);
	sdinitctx(&ctx);
	sdpwrup();
	sdinireg();
	sdinistat();
	sdenclk();
	sdclrstat();
	sdcmd0();
	r = sdcvalid(SdV27t36);
	seroutf("condition validation result(0x%X)\r\n", r);
	r = sdcvoltwin();
	seroutf("SD_SEND_OP_COND result(0x%X)\r\n", r);
	sdgetcid();
	sdinitrca(&ctx);
	seroutf("RCA: %X\r\n", ctx.rca);
	sdgetcsd(&ctx);
	sdprcsd(&ctx);
	sdselect(&ctx);
	mmemset(b, 0, sizeof b);
	sdread(&ctx, b);
	pr(b, sizeof b);
	serout("processing done\r\n");
	sddbg();
	for (;;)
		;
	return 0;
}
