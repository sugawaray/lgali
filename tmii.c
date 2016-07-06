#include "miiim.h"
#include <nomagicc.h>

#define S(x)	#x
#define AEQ(l, r)	\
	do {	\
		if ((l) != (r)) {	\
			testerror(testmsg("0x%08X != 0x%08X", (l), (r)));	\
		}	\
	} while (0)	\
/**/

static
void
defanadvt()
{
	u16 v;
	v = defanadv(0xF000);
	AEQ(0xF041, v);
}

int
main()
{
	testrun("def auto-neg adv", defanadvt);
	return 0;
}
