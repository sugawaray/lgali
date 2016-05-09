#include <quark.h>
#include "serim.h"
#include <ser.h>

static const char hex[] = "0123456789ABCDEF";
int
seroutf(const char *fmt, ...)
{
	int st;
	unsigned c;
	unsigned v;
	int *a;
	const char *p;
	char b[2];

	__asm__("movl %%ebp,%0" : "=r"(a));
	a += 3;
	st = 0;
	p = fmt;
	while (*p) {
		if (st == 0) {
			if (*p == '%')
				st = 1;
			else {
				b[0] = *p;
				b[1] = '\0';
				serout(b);
			}
		} else if (st == 1) {
			switch (*p) {
			case 'X':
				st = 5;
				break;
			}
		}
		++p;
		if (st == 5) {
			int ov;
			v = *(unsigned *)a++;
			for (c = 16; c >= 16 && v / c; c *= 16)
				;
			if (c < 16)
				ov = 1;
			else
				ov = 0;
			do {
				if (ov) {
					c = 0x10000000;
					ov = 0;
				} else
					c /= 16;
				b[0] = hex[v / c];
				b[1] = '\0';
				serout(b);
				v %= c;
			} while (c > 1);
			st = 0;
		}
	}
	return 0;
}
