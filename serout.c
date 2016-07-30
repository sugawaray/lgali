#include <quark.h>
#include "serim.h"

int
serout(const char *m)
{
	while (*m) {
		while (!(ruart->lsr & (1 << 5)))
			;
		ruart->rbr_thr_dll = (ruart->rbr_thr_dll & 0xFFFFFF00) | *m;
		++m;
	}
	return 0;
}
