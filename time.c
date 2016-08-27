#include <time.h>
#include <lapic.h>

enum {
	Hz	= 50,
};

int
nanosleep(const struct timespec *p, struct timespec *ignored)
{
	u32 c, n;
	c = (u32)(Hz * p->tv_sec);
	c += (u32)((double)p->tv_nsec / 1000000000.0 * Hz);
	c += timerticks();
	do {
		n = timerticks();
	} while (n < c);
	return 0;
}
