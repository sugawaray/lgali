#include <eth.h>
#include <quark.h>
#include <led.h>
#include <ser.h>
#include <hstbr.h>

int
main()
{
	int i, r;
	struct Hminfo hmi;
	struct Hbmm hbmm;

	wait();
	ledminit();
	r = serinit();
	if (r != 0) {
		ledmemit(5, 100);
		for (;;)
			;
	}
	serout("serial port initialization has done.\r\n");
	readhminfo(&hmi);
	seroutf("HMINFO(0x%X, 0x%X, 0x%X)\r\n", hmi.hmbound, hmi.hmisc2, hmi.hsmmctl);
	seroutf("hmbound(0x%X)\r\n", MHMBOUND & hmi.hmbound);

	readhbmm(&hbmm);
	seroutf("ESRAMCTRL(0x%X)\r\n", hbmm.esramctrl);
	seroutf("ESRAMPGCTRL_BLOCK(0x%X)\r\n", hbmm.pgctlblk);
#if 0
	seroutf("ESRAMPGCTRL\r\n");
	for (i = 0; i < 128; ++i) {
		seroutf("0x%X(0x%X), ", i, readesrampgctl(i));
	}
	seroutf("\r\n");
#endif
	ethinit();

	for (;;) {
		ethdbg();
	}
	return 0;
}
