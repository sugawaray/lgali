#include <nomagicc.h>
#include <quark.h>
#include "ethim.h"
#include <nomagicc.h>
#include <nomagiccu.h>
#include <stdio.h>

#define S(x)	#x
#define A(e)	do { if (!(e)) { testerror(S(e)); } } while (0)
#define AEQ(x, a)	do { if ((x) != (a)) { testerror(testmeqh((x), (a))); } } while (0)

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
	AEQ(0x8100, o.des1l);
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

int
main()
{
	testrun("ethdefmcnf", defmcnf);
	testrun("ethdefmff", defmff);
	testrun("rdescinit", trdescinit);
	testrun("tdescinit", ttdescinit);
	return 0;
}
