#include <intr.h>
#include <nomagicc.h>
#include "th.h"

#define S(x)	#x
#define A(e)	do { if (!(e)) { testerror(S(e)); } } while (0)
#define AEQ(e, x)	\
do {	\
	if ((e) != (x)) {	\
		testerror(testmsg("0x%08x != 0x%08x", (e), (x)));	\
	}	\
} while (0)	\
/**/

static
void
tint()
{
	struct Idtrec re;

	memset(&re, 0xAA, sizeof re);
	idtrsetad(&re, (void (*)())0x12345678);
	A(re.offl == 0x5678);
	A(re.offh == 0x1234);
}

static
void
tattrtrap()
{
	struct Idtrec re;

	memset(&re, 0xAA, sizeof re);
	idtrsettype(&re, IDTRTRAP);
	A(re.attr == 0x8F00);
}

static
void
tattrintr()
{
	struct Idtrec re;

	memset(&re, 0xA, sizeof re);
	idtrsettype(&re, IDTRINTR);
	A(re.attr == 0x8E00);
}

static
void
tssel()
{
	struct Idtrec re;

	memset(&re, 0xA, sizeof re);
	idtrsetssel(&re, 2);
	A(re.ssel == 0x10);
}

static
void
invalidate()
{
	struct Idtrec o;

	memset(&o, 0xFF, sizeof o);
	idtrsetvalid(&o, 0);
	AEQ(0x7FFF, o.attr);
}

static
void
validate()
{
	struct Idtrec o;

	memset(&o, 0xAA, sizeof o);
	o.attr &= 0x7FFF;
	idtrsetvalid(&o, 1);
	AEQ(0xAAAA, o.attr);
}

int
main()
{
	testrun("address", tint);
	testrun("trap gate", tattrtrap);
	testrun("interrupt gate", tattrintr);
	testrun("segment selector", tssel);
	testrun("invalidate", invalidate);
	testrun("validate", validate);
	return 0;
}
