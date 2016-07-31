#include <nomagicc.h>
#include <quark.h>
#include "ethim.h"
#include <nomagicc.h>
#include <nomagiccu.h>
#include <stdio.h>

#define S(x)	#x
#define A(e)	do { if (!(e)) { testerror(S(e)); } } while (0)
#define AEQ(x, a)	\
	do { if ((x) != (a)) {	\
		testerror(testmsg("0x%08X != 0x%08X", (a), (x)));	\
	} } while (0)

static
void
defmcnf()
{
	u32 v;
	v = ethdefmcnf(0xFFFFFFFF);
	A(v == 0xAFFFBFFF);
	v = ethdefmcnf(0x00000000);
	A(v == 0x2040080C);
}

static
void
defmff()
{
	u32 v;
	v = ethdefmff(0x00000000);
	AEQ(0x80000000, v);
}

static
void
trdescinit()
{
	struct Rdesc o;

	rdescinit(&o, (void*)0x100000, 0x100, (void*)0x100100, 0x200);
	AEQ(0x0200, o.des1h);
	AEQ(0x0100, o.des1l);
	AEQ(0x00100000, o.b1addr);
	AEQ(0x00100100, o.b2addr);
}

static
void
ttdescinit()
{
	struct Tdesc o;

	tdescinit(&o, (void *)0x100000, 0x100, (void *)0x100100, 0x200);
	AEQ(0x70200000, o.des0 & 0xFFFF0000);
	AEQ(0x0100, o.des1l);
	AEQ(0x0200, o.des1h);
	AEQ(0x00100000, o.b1addr);
	AEQ(0x00100100, o.b2addr);
}

static
void
rdisavailt()
{
	struct Rdesc o;

	rdescinit(&o, (void*)0x0, 0x100, (void*)0x100, 0x100);
	o.status |= RDOWN;
	AEQ(0, rdisavail(&o));
}

static
void
rdisavail_2t()
{
	struct Rdesc o;

	rdescinit(&o, (void*)0x0, 0x100, (void*)0x100, 0x100);
	o.status &= ~RDOWN;
	o.status |= Rdfs;
	o.status |= Rdls;
	AEQ(1, rdisavail(&o));
}

static
void
rdflen_nat()
{
	struct Rdesc o;

	rdescinit(&o, (void*)0x0, 0x100, (void*)0x100, 0x100);
	o.status |= RDOWN;
	AEQ(rdflen(&o), -1);
}

static
void
rdflen_fullt()
{
	struct Rdesc o;

	rdescinit(&o, (void*)0x0, 0x100, (void*)0x100, 0x200);
	o.status &= ~RDOWN;
	o.status |= Rdfs;
	o.status &= ~Rdls;
	AEQ(0x300, rdflen(&o));
}

static
void
rdflen_lastt()
{
	struct Rdesc o;

	rdescinit(&o, (void*)0x0, 0xF00, (void*)0xF00, 0xF00);
	o.status &= ~RDOWN;
	o.status |= Rdfs;
	o.status |= Rdls;
	o.status = (o.status & ~Mrdfl) | 0xFFF << Ordfl;
	AEQ(0x0FFF, rdflen(&o));
}

static
void
prrdbuf_mt()
{
	int i;
	struct Rdesc o;
	unsigned char buf[0x100];

	for (i = 0; i < sizeof buf / sizeof buf[0]; ++i)
		buf[i] = i;
	rdescinit(&o, (void*)buf, sizeof buf, (void*)0x0, 0);
	o.status &= ~RDOWN;
	o.status = (o.status & ~Mrdfl) | 0x21 << Ordfl;
	o.status |= Rdfs;
	o.status |= Rdls;
	prrdbuf(&o, 1);
}

int
main()
{
	testrun("ethdefmcnf", defmcnf);
	testrun("ethdefmff", defmff);
	testrun("rdescinit", trdescinit);
	testrun("tdescinit", ttdescinit);
	testrun("rdisavail: if (ST(OWN) == 1) 0", rdisavailt);
	testrun("rdisavail: if (ST(FS) == 1 && ST(LS) == 1) 1", rdisavail_2t);
	testrun("rdflen: when (!rdisavail) -1", rdflen_nat);
	testrun("rdflen: when (ST(LS) == 0) sz", rdflen_fullt);
	testrun("rdflen: when (ST(LS) == 1) fsz", rdflen_lastt);
	testrun("mt prrdbuf", prrdbuf_mt);
	return 0;
}
