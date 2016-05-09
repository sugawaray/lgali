#include <quark.h>
#include "serim.h"
#include <ser.h>
#include <led.h>

int
rd(char *b, int sz)
{
	char *p;
	p = b;
	do {
		p += serrcv(p, sz - (p - b));
	} while (sz - (p - b) > 0 && *(p - 1) != '\n');
	return p - b;
}

int
main()
{
	char b[80];
	int i, r;

	wait();
	ledminit();
	r = serinit();
	if (r != 0) {
		ledmemit(5, 2);
		return 1;
	}
	serout("Now receiving\r\n");
	while (1) {
		r = rd(b, sizeof b - 1);
		b[r] = '\0';
		serout("[");
		serout(b);
		serout("]\r\n");
	}
}
