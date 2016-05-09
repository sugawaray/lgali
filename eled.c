#include <quark.h>
#include <led.h>
#include <lib.h>

int
main()
{
	wait();
	ledminit();
	while (1) {
		ledmemit(2, 10);
		wait();
	}
}

