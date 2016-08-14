#include "tmim.h"
#include <led.h>

static int cnt;

void
ontick()
{
	++cnt;
	if (cnt % 10 == 0)
		ledmemit(2, 1);
}
