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

int
main()
{
	testrun("ethdefmcnf", defmcnf);
	testrun("ethdefmff", defmff);
	return 0;
}
