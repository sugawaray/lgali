#include "sdim.h"
#include <ser.h>

enum {
	RPreState	= 0x24,
	RArgument	= 0x08,
	RCmd	= 0x0e,
	RSwRst	= 0x2f,
	RResponse0	= 0x10,
	RResponse2	= 0x14,
	RResponse4	= 0x18,
	RResponse6	= 0x1c,
	RHostCtl	= 0x28,
	RPwrCtl	= 0x29,
	RClkCtl	= 0x2c,
	RCapabilities	= 0x40,
	RCapabilities2	= 0x44,
	RNmlIntStatus	= 0x30,
	RErrIntStatus	= 0x32,
	RNrmIntStatusEn	= 0x34,
	RErrIntStatEn	= 0x36,
	RNrmIntSigEn	= 0x38,
	RErrIntSigEn	= 0x3a
};

enum {
	MCmdIndex	= 0x3f,
	MCmdType	= 0x3,
	MRespTypeSel	= 0x3,
	MSdBusVoltSel	= 0x7,
	MBaseClockFrequencySdClk	= 0x3f
};

enum {
	OCmdInhibitCmd	= 0x00,
	OCmdInhibitDat	= 0x01,
	OCmdIndex	= 0x08,
	OCmdType	= 0x06,
	ODataPrSel	= 0x05,
	OCmdIndexChkEn	= 0x04,
	OCmdCrcChkEn	= 0x03,
	ORespTypeSel	= 0x00,
	OSwRstAll	= 0x00,
	OSdBusVoltSel	= 0x1,
	OSdBusPwr	= 0x0,
	OBaseClockFrequencySdClk	= 0x8,
	OSdclkFreqSel	= 0x8,
	OUprSdclkFreqSel	= 0x6,
	OClkGenSel	= 0x5,
	OSdClkEn	= 0x2,
	OIntClkEn	= 0x0,
	OCmdCompStatEn	= 0x0,
	OCmdTimeoutErrStatEn	= 0x0,
	OCmdCrcErrStatEn	= 0x1,
	OCmdEndBitErrStatEn	= 0x2,
	OCmdIndErrStatEn	= 0x3,
	OTxComp	= 0x1,
	OCmdComp	= 0x0,
	OHiSpdEn	= 0x2,
	ODataTxWid	= 0x1,
	OLedCtl	= 0x0
};

enum {
	Cmd0	= 0x00,
	Cmd8	= 0x08,
	NormalCmds	= 0x0,
	RespLen48	= 0x2,
	V33	= 0x7,
	Freq	= 400,
	DividedClockMode	= 0x0,
	HighSpeedMode	= 0x1,
	BitMode1	= 0x0
};

enum {
	OArg8Vhs	= 0x08,
	MArg8Vhs	= 0x0f,
	OArg8CkPt	= 0x00
};

static u32 base;
#define R8(o)	(*(volatile u8 *)(base + (o)))
#define R16(o)	(*(volatile u16 *)(base + (o)))
#define R32(o)	(*(volatile u32 *)(base + (o)))

void
sdsetbar0(u32 v)
{
	base = v;
}

void
sdclrstat()
{
	u16 t;

	t = 0;
	t |= 1 << OTxComp;
	t |= 1 << OCmdComp;
	R16(RNmlIntStatus) = t;

	R16(RErrIntStatus) = 0xffff;
}

static
void
cmd(u16 c, u32 a)
{
	while (R32(RPreState) & OCmdInhibitCmd)
		;
	R32(RArgument) = a;
#if 0
	R16(RCmd) = c;
#else
	R8(RCmd) = c & 0x00ff;
	R8(RCmd + 1) = (u16)(c & 0xff00) >> 0x8;
#endif
}

void
sdswrst()
{
	R8(RSwRst) = 1 << OSwRstAll;
	while (R8(RSwRst) & (1 << OSwRstAll))
		;
	sdcmd0();
}

void
sdcmd0()
{
	u16 c;

	c = 0;
	c |= Cmd0 << OCmdIndex;
	c |= NormalCmds << OCmdType;
	c |= 0 << ODataPrSel;
	c |= 0 << OCmdIndexChkEn;
	c |= 0 << OCmdCrcChkEn;
	c |= 0 << ORespTypeSel;

	cmd(c, 0);
}

u32
sdgenarg8(u8 v, u8 p)
{
	u32 r;

	r = 0;
	r |= (v << OArg8Vhs) & (MArg8Vhs << OArg8Vhs);
	r |= (p << OArg8CkPt);
	return r;
}

int
sdcvalid(u8 vhs)
{
	u32 a;
	u16 c;

	c = 0;
	c |= Cmd8 << OCmdIndex;
	c |= NormalCmds << OCmdType;
	c |= 0 << ODataPrSel;
	c |= 0 << OCmdIndexChkEn;
	c |= 1 << OCmdCrcChkEn;
	c |= (RespLen48 & MRespTypeSel) << ORespTypeSel;
	a = sdgenarg8(vhs, 0xaa);

	sdclrstat();
	cmd(c, a);
	while ((R16(RNmlIntStatus) & (1 << OCmdComp)) == 0)
		;

	while ((R32(RPreState) & (1 << OCmdInhibitCmd)) || (R32(RPreState) & (1 << OCmdInhibitDat)))
		;
	return R32(RResponse0) == a;
}

void
sdpwrup()
{
	u8 t;
	t = 0;
	t |= (V33 & MSdBusVoltSel) << OSdBusVoltSel;
	t |= 1 << OSdBusPwr;
	R8(RPwrCtl) = t;
}

void
sdenclk()
{
	u16 r;
	u8 d;

	d = sdcalcdivclk(sdxbfreq(R32(RCapabilities)) * 1000, Freq);
	r = 0;
	r |= d << OSdclkFreqSel;
	r |= 0 << OUprSdclkFreqSel;
	r |= DividedClockMode << OClkGenSel;
	r |= 1 << OSdClkEn;
	r |= 1 << OIntClkEn;

	R16(RClkCtl) = r;
}

u16
sdxbfreq(u32 v)
{
	v >>= OBaseClockFrequencySdClk;
	v &= MBaseClockFrequencySdClk;
	return v;
}

u8
sdcalcdivclk(int base, int tgt)
{
	int i;

	if (base < tgt)
		return 0;
	i = 0;
	for (; base > tgt; base >>= 1)
		++i;
	return i;
}

void
sdinistat()
{
	u16 t;

	R16(RNrmIntStatusEn) = 1 << OCmdCompStatEn;

	t = 0;
	t |= 1 << OCmdIndErrStatEn;
	t |= 1 << OCmdEndBitErrStatEn;
	t |= 1 << OCmdCrcErrStatEn;
	t |= 1 << OCmdTimeoutErrStatEn;
	R16(RErrIntStatEn) = t;
}

void
sddbg()
{
	serout("========================================================\r\n");
	seroutf("CAPABILITIES(0x%X)\r\n", R32(RCapabilities));
	seroutf("CAPABILITIES2(0x%X)\r\n", R32(RCapabilities2));
	seroutf("CLK_CTL(0x%X)\r\n", R16(RClkCtl));
	seroutf("HOST_CTL(0x%X)\r\n", R8(RHostCtl));
	seroutf("PRE_STATE(0x%X)\r\n", R32(RPreState));
	seroutf("PWR_CTL(0x%X)\r\n", R8(RPwrCtl));
	seroutf("NML_INT_STATUS(0x%X)\r\n", R16(RNmlIntStatus));
	seroutf("ERR_INT_STATUS(0x%X)\r\n", R16(RErrIntStatus));
	seroutf("NRM_INT_STATUS_EN(0x%X)\r\n", R16(RNrmIntStatusEn));
	seroutf("ERR_INT_STAT_EN(0x%X)\r\n", R16(RErrIntStatEn));
	seroutf("NRM_INT_SIG_EN(0x%X)\r\n", R16(RNrmIntSigEn));
	seroutf("ERR_INT_SIG_EN(0x%X)\r\n", R16(RErrIntSigEn));
	seroutf("Response0246(0x%X,0x%X,0x%X,0x%X), Argument(0x%X)\r\n",
		R32(RResponse0), R32(RResponse2),
		R32(RResponse4), R32(RResponse6),
		R32(RArgument));
}

void
sdinireg()
{
	u8 t;

	t = R8(RHostCtl);
	t &= ~(1 << OHiSpdEn);
	t &= ~(1 << ODataTxWid);
	t |= BitMode1 << ODataTxWid;
	t |= 1 << OLedCtl;
	R8(RHostCtl) = t;

	R16(0xc) = 0;
}
