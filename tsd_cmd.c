#include <nomagicc.h>
#include "sdim.h"

#define AEQ(e, x)	do { if ((e) != (x)) { testerror(testmsg("0x%08x != 0x%08x", (e), (x))); } } while (0)

static
void
cmd55r_test()
{
	u16 v;
	v = sdcmd55();
	AEQ(0x370a, v);
}

static
void
argc55r_test()
{
	u32 v;
	v = sdgenarg55(0xf00f);
	AEQ(0xf00f0000, v);
}

static
void
acmd41r_test()
{
	u16 r;
	r = sdacmd41();
	AEQ(0x290a, r);
}

int
main()
{
	testrun("cmd55 command register", cmd55r_test);
	testrun("cmd55 argument register", argc55r_test);
	testrun("acmd41 command register", acmd41r_test);
	return 0;
}
