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
initfi(struct Fi *o)
{
	rdescinit(&o->rdsc, o->buf, sizeof o->buf, 0, 0);
	o->rx.start = 0;
	o->rx.end = 0;
	o->rx.pos = 0;
	o->rx.rd = &o->rdsc;
}

static
void
readwoanybuft()
{
	struct Fi fi;
	ssize_t sz;
	char obuf[0x100];

	initfi(&fi);
	fi.rdsc.status |= RDOWN;
	fi.rdsc.des1l |= Mrdscrer;
	sz = read1(&fi.rx, -1, obuf, sizeof obuf);
	AEQ(0, sz);
}

static
void
readabuft()
{
	int i;
	ssize_t sz;
	char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc.des1l |= Mrdscrer;
	fi.rdsc.status &= ~RDOWN;
	fi.rdsc.status |= Rdfs | Rdls;
	fi.rdsc.status &= ~Mrdfl;
	fi.rdsc.status |= (0x100 << Ordfl) & Mrdfl;
	for (i = 0; i < sizeof fi.buf; ++i)
		fi.buf[i] = i;
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
