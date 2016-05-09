#include <quark.h>

void
wait()
{
	long i;

	for (i = 0; i < 50000000; ++i)
		nop();
}
