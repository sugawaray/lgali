#include "ethim.h"
#include <quark.h>
#include <nomagicc.h>
#include "th.h"

#define S(x)	#x
#define A(e)	\
do {	\
	if (!(e)) {	\
		testerror(S(e));	\
	}	\
} while (0)	\
/**/
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
initcompfr(struct Fi *o, int sz)
{
	int i;

	o->rdsc.status &= ~RDOWN;
	o->rdsc.status |= Rdfs | Rdls;
	o->rdsc.status &= ~Mrdfl;
	o->rdsc.status |= (sz << Ordfl) & Mrdfl;
	for (i = 0; i < sizeof o->buf; ++i)
		o->buf[i] = i;
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

static
void
readabbyszt()
{
	ssize_t sz;
	char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc.des1l |= Mrdscrer;
	initcompfr(&fi, 0x100);
	obuf[0xFF] = 0;
	sz = read1(&fi.rx, -1, obuf, sizeof obuf - 1);
	AEQ(0xFF, sz);
	A(memcmp(obuf, fi.buf, 0xFF) == 0);
	A(memcmp(obuf, fi.buf, 0x100) != 0);
}

static
void
readadvptrt()
{
	ssize_t sz;
	char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc.des1l |= Mrdscrer;
	initcompfr(&fi, 0x100);
	read1(&fi.rx, -1, obuf, 3);
	obuf[0xFC] = 0xFF;
	sz = read1(&fi.rx, -1, obuf, 0xFC);
	AEQ(0xFC, sz);
	A(memcmp(obuf, fi.buf + 3, 0xFD) == 0);
}

int
main()
{
	testrun("read without any buf", readwoanybuft);
	testrun("read a buf", readabuft);
	testrun("read a buf by size", readabbyszt);
	testrun("a reading advances the position pointer", readadvptrt);
	return 0;
}
