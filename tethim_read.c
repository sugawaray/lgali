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
		testerror(testmsg("%s != %s : 0x%08x != 0x%08x", S((e)), S((x)), (e), (x)));	\
	}	\
} while (0)	\
/**/
#define AEQM(e, x, sz)	\
do {	\
	if (memcmp((e), (x), (sz)) != 0) {	\
		testerror(testmsg("memdiff\nexpect\n%sactual\n%s",	\
			testmem((char*)(e), (sz)), testmem((char*)(x), (sz))));	\
	}	\
} while (0)	\
/**/

struct Fi {
	struct Rx rx;
	struct Rdesc rdsc[3];
	unsigned char buf[3][0x800];
};

static
void
initfi(struct Fi *o)
{
	int i;

	for (i = 0; i < sizeof o->rdsc / sizeof o->rdsc[0]; ++i)
		rdescinit(&o->rdsc[i], o->buf[i], sizeof o->buf[i], 0, 0);
	o->rx.bp = 0;
	o->rx.fs = -1;
	o->rx.ls = -1;
	o->rx.pos = 0;
	o->rx.rd = o->rdsc;
}

static
void
initcompfr(struct Fi *o, int b, int sz, int fv)
{
	int i;

	o->rdsc[b].status &= ~RDOWN;
	o->rdsc[b].status |= Rdfs | Rdls;
	o->rdsc[b].status &= ~Mrdfl;
	o->rdsc[b].status |= (sz << Ordfl) & Mrdfl;
	for (i = 0; i < sizeof o->buf[b]; ++i)
		o->buf[b][i] = i + fv;
}

static
void
readwoanybuft()
{
	struct Fi fi;
	ssize_t sz;
	char obuf[0x100];

	initfi(&fi);
	fi.rdsc[0].status |= RDOWN;
	fi.rdsc[0].des1l |= Mrdscrer;
	sz = read1(&fi.rx, -1, obuf, sizeof obuf);
	AEQ(0, sz);
}

static
void
readabuft()
{
	ssize_t sz;
	char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc[0].des1l |= Mrdscrer;
	initcompfr(&fi, 0, 0x100, 0);
	sz = read1(&fi.rx, -1, obuf, sizeof obuf);
	AEQ(0x100, sz);
	AEQ(0, memcmp(obuf, fi.buf[0], sizeof obuf));
}

static
void
readabbyszt()
{
	ssize_t sz;
	char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc[0].des1l |= Mrdscrer;
	initcompfr(&fi, 0, 0x100, 0);
	obuf[0xFF] = 0;
	sz = read1(&fi.rx, -1, obuf, sizeof obuf - 1);
	AEQ(0xFF, sz);
	A(memcmp(obuf, fi.buf[0], 0xFF) == 0);
	A(memcmp(obuf, fi.buf[0], 0x100) != 0);
}

static
void
readadvptrt()
{
	ssize_t sz;
	unsigned char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc[0].des1l |= Mrdscrer;
	initcompfr(&fi, 0, 0x100, 0);
	read1(&fi.rx, -1, obuf, 3);
	obuf[0xFC] = 0xFF;
	sz = read1(&fi.rx, -1, obuf, 0xFC);
	AEQ(0xFC, sz);
	A(memcmp(obuf, fi.buf[0] + 3, 0xFD) == 0);
}

static
void
readfolbuft()
{
	ssize_t sz;
	unsigned char obuf[0x104];
	struct Fi fi;

	initfi(&fi);
	initcompfr(&fi, 0, 0x100, 0);
	initcompfr(&fi, 1, 0x100, 1);
	fi.rdsc[1].status |= Mrdscrer;
	read1(&fi.rx, -1, obuf, 0x100);
	obuf[0x100] = 1;
	sz = read1(&fi.rx, -1, obuf, 0x100);
	AEQ(0x100, sz);
	A(memcmp(&fi.buf[1], obuf, 0x101) == 0);
}

static
void
readcurnextt()
{
	ssize_t sz;
	unsigned char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc[1].status |= Mrdscrer;
	initcompfr(&fi, 0, 0x80, 0);
	initcompfr(&fi, 1, 0x80, 1);
	obuf[0x100] = 0x82;
	sz = read1(&fi.rx, -1, obuf, 0x100);
	AEQ(0x100, sz);
	AEQM(&fi.buf[0], obuf, 0x80);
	AEQM(&fi.buf[1], &obuf[0x80], 0x80);
	A(memcmp(&fi.buf[1], &obuf[0x80], 0x81) != 0);
}

static
void
read2endt()
{
	ssize_t sz;
	unsigned char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc[1].des1l |= Mrdscrer;
	initcompfr(&fi, 0, 0x80, 0);
	initcompfr(&fi, 1, 0x80, 1);
	fi.rdsc[1].status |= RDOWN;
	obuf[0x81] = 2;
	sz = read1(&fi.rx, -1, obuf, sizeof obuf);
	AEQ(0x80, sz);
	AEQM(&fi.buf[0], obuf, 0x80);
	A(memcmp(&fi.buf[0], obuf, 0x81) != 0);
}

static
void
readrer21stt()
{
	ssize_t sz;
	unsigned char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc[1].des1l |= Mrdscrer;
	initcompfr(&fi, 0, 0x80, 0);
	initcompfr(&fi, 1, 0x80, 1);
	read1(&fi.rx, -1, obuf, 0x80);
	fi.rdsc[0].status &= ~RDOWN;
	obuf[0x91] = 0x11;
	sz = read1(&fi.rx, -1, obuf, 0x90);
	AEQ(0x90, sz);
	AEQM(&fi.buf[1], obuf, 0x80);
	AEQM(&fi.buf[0], &obuf[0x80], 0x10);
	A(memcmp(&fi.buf[0], &obuf[0x80], 0x11) != 0);
}

static
void
readmkrdownt()
{
	ssize_t sz;
	unsigned char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc[1].des1l |= Mrdscrer;
	initcompfr(&fi, 0, 0x80, 0);
	initcompfr(&fi, 1, 0x80, 1);
	read1(&fi.rx, -1, obuf, 0xFF);
	obuf[1] = 1;
	sz = read1(&fi.rx, -1, obuf, 0x80);
	AEQ(1, sz);
	AEQ(0x80, obuf[0]);
	AEQ(1, obuf[1]);
}

static
void
dontreadtilcompt()
{
	ssize_t sz;
	unsigned char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc[1].des1l |= Mrdscrer;
	initcompfr(&fi, 0, 0x80, 0);
	fi.rdsc[0].status &= ~Rdls;
	initcompfr(&fi, 1, 0x80, 1);
	fi.rdsc[1].status &= ~(Rdfs | Rdls);
	obuf[0] = 0;
	sz = read1(&fi.rx, -1, obuf, 0x80);
	AEQ(0, sz);
	AEQ(0, obuf[0]);
}


static
void
dontreadnextt()
{
	ssize_t sz;
	unsigned char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc[1].des1l |= Mrdscrer;
	initcompfr(&fi, 0, 0x80, 0);
	initcompfr(&fi, 1, 0x80, 1);
	fi.rdsc[1].status &= ~Rdls;
	obuf[0x80] = 0;
	sz = read1(&fi.rx, -1, obuf, 0x100);
	AEQ(0x80, sz);
	AEQM(&fi.buf[0], obuf, 0x80);
	AEQ(0, obuf[0x80]);
}

static
void
readfromlast1stt()
{
	ssize_t sz;
	unsigned char obuf[0x100];
	struct Fi fi;

	initfi(&fi);
	fi.rdsc[2].des1l |= Mrdscrer;
	initcompfr(&fi, 0, 0x40, 0);
	fi.rdsc[0].status &= ~Rdls;
	initcompfr(&fi, 1, 0x40, 1);
	fi.rdsc[0].status &= ~Rdls;
	initcompfr(&fi, 2, 0x40, 2);
	fi.rdsc[0].status &= ~Rdfs;
	sz = read1(&fi.rx, -1, obuf, 0x100);
	AEQ(0x80, sz);
	AEQM(&fi.buf[1], obuf, 0x40);
	AEQM(&fi.buf[2], &obuf[0x40], 0x40);
}

int
main()
{
	testrun("read without any buf", readwoanybuft);
	testrun("read a buf", readabuft);
	testrun("read a buf by size", readabbyszt);
	testrun("a reading advances the position pointer", readadvptrt);
	testrun("read the following buf after the curr", readfolbuft);
	testrun("read the buf & the part of the next buf", readcurnextt);
	testrun("don't read the next buf if it's owned by the dev", read2endt);
	testrun("read the cur buf(RER) and the next buf(1st)", readrer21stt);
	testrun("reading makes the buf be owned by the dev", readmkrdownt);
	testrun("don't read til frames complete", dontreadtilcompt);
	testrun("don't read the next til it completes", dontreadnextt);
	testrun("read from the last 1st of the frame", readfromlast1stt);
	return 0;
}
