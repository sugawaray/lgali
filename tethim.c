#include <nomagicc.h>
#include <quark.h>
#include "ethim.h"
#include <stdio.h>

#define S(x)	#x
#define A(e)	do { if (!(e)) { testerror(S(e)); } } while (0)

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

int
main()
{
	testrun("ethdefmcnf", defmcnf);
	return 0;
}
