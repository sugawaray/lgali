#include <quark.h>
#include "mcim.h"
#include <led.h>
#include <mc.h>

int
main()
{
	long le;
	int cnt, r;

	wait();
	r = mchk();
	if (r == 0) {
		cnt = 5;
		le = 2;
	} else {
		cnt = 2;
		le = 10;
	}
	ledminit();
	ledmemit(le, cnt);
}
