#include "ethim.h"
#include <quark.h>
#include <nomagicc.h>
#include "th.h"

#define S(x)	#x
#define AEQ(e, x)	\
do {	\
	if ((e) != (x)) {	\
		testerror(testmsg("0x%08x != 0x%08x", (e), (x)));	\
	}	\
} while (0)	\
/**/

static
void
readwoanybuft()
{
	ssize_t sz;
	struct Rdesc rdsc;
	struct Rx rx;
	char buf[0x800];
	char obuf[0x100];

	rdescinit(&rdsc, buf, sizeof buf, 0, 0);
	rdsc.status |= RDOWN;
	rdsc.des1l |= Mrdscrer;
	rx.start = 0;
	rx.end = 0;
	rx.pos = 0;
	rx.rd = &rdsc;
	sz = read1(&rx, -1, obuf, sizeof obuf);
	AEQ(0, sz);
}

int
main()
{
	testrun("read without any buf", readwoanybuft);
	return 0;
}
