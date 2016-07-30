#include <stdio.h>

int
serout(const char *m)
{
	return fprintf(stdout, "%s", m);
}
