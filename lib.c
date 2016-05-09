#include <lib.h>

void
mmemset(void *pp, int v, int sz)
{
	char *p = (char *)pp;
	while (sz-- > 0)
		*p++ = v;
}

void
mmemcpy(void *dd, const void *ss, int sz)
{
	char *d = (char *)dd;
	const char *s = (const char *)ss;
	while (sz-- > 0)
		*d++ = *s++;
}

