#include <intr.h>
#include <nomagicc.h>

#define S(x)	#x
#define A(e)	do { if (!(e)) { testerror(S(e)); } } while (0)

static
void
tint()
{
	struct Idtrec re;

	memset(&re, 0xAA, sizeof re);
	intrgsetad(&re, (void (*)())0x12345678);
	A(re.offl == 0x5678);
	A(re.offh == 0x1234);
}

int
main()
{
	testrun("interrupt gate", tint);
}
