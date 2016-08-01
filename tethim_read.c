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

struct Fi {
	struct Rdesc rdsc;
	struct Rx rx;
	unsigned char buf[0x800];
};

static
void
readwoanybuft()
{
#if 1
	struct Fi fi;
	ssize_t sz;
	char obuf[0x100];

	rdescinit(&fi.rdsc, fi.buf, sizeof fi.buf, 0, 0);
	fi.rdsc.status |= RDOWN;
	fi.rdsc.des1l |= Mrdscrer;
	fi.rx.start = 0;
	fi.rx.end = 0;
	fi.rx.pos = 0;
	fi.rx.rd = &fi.rdsc;
	sz = read1(&fi.rx, -1, obuf, sizeof obuf);
	AEQ(0, sz);
#else
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
#endif
}

static
void
readabuft()
{
	int i;
	ssize_t sz;
	char obuf[0x100];
	struct Fi fi;

	rdescinit(&fi.rdsc, fi.buf, sizeof fi.buf, 0, 0);
	fi.rdsc.des1l |= Mrdscrer;
	fi.rdsc.status &= ~RDOWN;
	fi.rdsc.status |= Rdfs | Rdls;
	fi.rdsc.status &= ~Mrdfl;
	fi.rdsc.status |= (0x100 << Ordfl) & Mrdfl;
	fi.rx.start = 0;
	fi.rx.end = 0;
	fi.rx.pos = 0;
	for (i = 0; i < sizeof fi.buf; ++i)
		fi.buf[i] = i;
	fi.rx.rd = &fi.rdsc;
	sz = read1(&fi.rx, -1, obuf, sizeof obuf);
	AEQ(0x100, sz);
	AEQ(0, memcmp(obuf, fi.buf, sizeof obuf));
}

int
main()
{
	testrun("read without any buf", readwoanybuft);
	testrun("read a buf", readabuft);
	return 0;
}
