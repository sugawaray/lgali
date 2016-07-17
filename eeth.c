#include <eth.h>
#include <quark.h>
#include <led.h>
#include <ser.h>

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
	ethinit();

	for (;;) {
		ethdbg();
	}
	return 0;
}
