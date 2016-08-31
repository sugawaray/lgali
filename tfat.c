#include <fat.h>
#include <nomagicc.h>
#include "th.h"

static char buf[512];

static
void
rdbin()
{
	TANY fd;
	char *p;

	fd = fopen("./fat0sect", "rb");
	if (fd == 0) {
		testerror("test error rdbin");
		return;
	}
	p = buf;
	do {
		p += fread(p, 1, sizeof buf - (p - buf), fd);
	} while (!feof(fd) && (sizeof buf - (p - buf)) > 0);
	fclose(fd);
}

static
void
rdbsbpb_test()
{
	struct BsBpb o;

	rdbin();
	rdbsbpb(&o, buf);
}

int
main()
{
	testrun("rdbsbpb", rdbsbpb_test);
	return 0;
}
