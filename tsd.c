#include "sdim.h"
#include <nomagicc.h>

#define AEQ(e, x)	do { if ((e) != (x)) { testerror(testmsg("0x%08x != 0x%08x", (e), (x))); } } while (0)

static
void
sdgenarg8t()
{
	u32 r1, r2;
	u8 p1 = 0xaa;
	u8 p2 = 0x55;

	r1 = sdgenarg8(SdV27t36, p1);
	r2 = sdgenarg8(SdV27t36, p2);
	AEQ(0x000001aa, r1);
	AEQ(0x00000155, r2);
}

static
void
sdxbfreqt()
{
	u16 r;

	r = sdxbfreq(0x00003200);
	AEQ(50, r);
	r = sdxbfreq(0x00003000);
	AEQ(48, r);
}

static
void
sdcalcdivclkeqt()
{
	u8 r;

	r = sdcalcdivclk(50, 50);
	AEQ(0x00, r);
}

static
void
sdcalcdivclkerrt()
{
	u8 r;

	r = sdcalcdivclk(25, 50);
	AEQ(0x00, r);
}

static
void
sdcalcdivclk1t()
{
	u8 r;

	r = sdcalcdivclk(50, 20);
	AEQ(1, r);
}

int
main()
{
	testrun("sdgenarg8", sdgenarg8t);
	testrun("sdxbfreq", sdxbfreqt);
	testrun("sdcalcdivclk eq", sdcalcdivclkeqt);
	testrun("sdcalcdivclk err", sdcalcdivclkerrt);
	testrun("sdcalcdivclk 1", sdcalcdivclk1t);
	return 0;
}
