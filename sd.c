#include "sdim.h"
#include <lib.h>
#include <ser.h>

enum {
	RBlkSize	= 0x04,
	RTxMode	= 0x0c,
	RBufDataPort	= 0x20,
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
	OLedCtl	= 0x0,
	OCSAppCmd	= 0x5,
	OCSCurrSt	= 0x9,
	MCSCurrSt	= 0xffff,
	OTrBlkSize	= 0x0,
	OBufRdEn	= 0x0b
};

enum {
	Cmd0	= 0,
	Cmd2	= 2,
	Cmd3	= 3,
	Cmd7	= 7,
	Cmd8	= 8,
	Cmd9	= 9,
	Cmd13	= 13,
	Cmd17	= 17,
	Cmd55	= 55,
	Acmd41	= 41,
	NormalCmds	= 0x0,
	RespLen48	= 0x2,
	RespLen136	= 0x1,
	V33	= 0x7,
	Freq	= 400,
	DividedClockMode	= 0x0,
	HighSpeedMode	= 0x1,
	BitMode1	= 0x0,
	CSTran	= 0x4,
	TrBlkSize512	= 0x0200
};

enum {
	OArg8Vhs	= 0x08,
	MArg8Vhs	= 0x0f,
	OArg8CkPt	= 0x00,
	OArg55Rca	= 0x10,
	OArgA41Hcs	= 0x1e,
	OArg9Rca	= 0x10,
	OArgRca	= 0x10
};

enum {
	OOcrV27t28	= 0x0f,
	OOcrV28t29	= 0x10,
	OOcrV29t30	= 0x11,
	OOcrV30t31	= 0x12,
	OOcrV31t32	= 0x13,
	OOcrV32t33	= 0x14,
	OOcrV33t34	= 0x15,
	OOcrCcs	= 0x1e,
	OOcrBusy	= 0x1f
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
u16
cmdr(u8 ind, u8 resp, int dat)
{
	u16 c;

	c = 0;
	c |= ind << OCmdIndex;
	c |= NormalCmds << OCmdType;
	c |= (dat & 0x1) << ODataPrSel;
	c |= 0 << OCmdIndexChkEn;
	c |= 1 << OCmdCrcChkEn;
	c |= resp << ORespTypeSel;
	return c;
}

static
u32
genrcaarg(u16 rca)
{
	return rca << OArgRca;
}

static
void
cmd(u16 c, u32 a)
{
	while (R32(RPreState) & OCmdInhibitCmd)
		;
	R32(RArgument) = a;
	R16(RCmd) = c;
}

static
void
waitcomp()
{
	while ((R16(RNmlIntStatus) & (1 << OCmdComp)) == 0)
		;
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
	waitcomp();

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
	seroutf("BLK_SIZE(0x%X)\r\n", R16(RBlkSize));
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

	R16(RBlkSize) = (TrBlkSize512 << OTrBlkSize);
}

u16
sdcmd55()
{
	return cmdr(Cmd55, RespLen48, 0);
}

u32
sdgenarg55(u16 rca)
{
	return genrcaarg(rca);
}

int
sdenappcmd()
{
	sdclrstat();
	cmd(sdcmd55(), sdgenarg55(0));
	waitcomp();
	return R32(RResponse0) & (1 << OCSAppCmd);
}

u16
sdacmd41()
{
	return cmdr(Acmd41, RespLen48, 0);
}

u32
sdgenarga41()
{
#if 0
	return (1 << OArgA41Hcs) | (1 << OOcrV33t34);
#else
	return (1 << OArgA41Hcs) | (1 << OOcrV32t33);
#endif
}

int
sdcvoltwin()
{
/* TODO: implement timeout for the case that the busy flag is not set forever. */
	int r;
	u16 c;
	u32 a;
	u32 res;

	c = sdacmd41();
	a = sdgenarga41();
	do {
		r = sdenappcmd();
		if (!r)
			return -1;
		sdclrstat();
		cmd(c, a);
		waitcomp();
		res = R32(RResponse0);
	} while ((res & (1 << OOcrBusy)) == 0);
	if (res & (1 << OOcrCcs))
		seroutf("The card is a SDHC or a SDXC.\r\n");
	else
		seroutf("The card is neighter a SDHC nor a SDXC.\r\n");
	return 0;
}

u16
sdcmd2()
{
	return cmdr(Cmd2, RespLen136, 0);
}

void
sdgetcid()
{
	sdclrstat();
	cmd(sdcmd2(), 0);
	waitcomp();
	seroutf("CID: %X,%X,%X,%X\r\n",
		R32(RResponse6), R32(RResponse4), R32(RResponse2),
		R32(RResponse0));
}

void
sdinitrca(struct Sdctx *o)
{
	u16 stat;

	sdclrstat();
	cmd(cmdr(Cmd3, RespLen48, 0), 0);
	waitcomp();
	o->rca = (R32(RResponse0) >> 0x10) & 0xffff;
	stat = (R32(RResponse0) & 0xffff);
	seroutf("Card status: %X\r\n", stat);
}

void
sdgetcsd(struct Sdctx *o)
{
	sdclrstat();
	cmd(cmdr(Cmd9, RespLen136, 0), sdgenarg9(o->rca));
	waitcomp();
	o->csd[0] = R32(RResponse0);
	o->csd[1] = R32(RResponse2);
	o->csd[2] = R32(RResponse4);
	o->csd[3] = R32(RResponse6);
}

void
sdprcsd(struct Sdctx *o)
{
	seroutf("CSD: 0x%X,%X,%X,%X\r\n", o->csd[3], o->csd[2],
		o->csd[1], o->csd[0]);
}

u32
sdgenarg9(u16 rca)
{
	return genrcaarg(rca);
}

void
sdinitctx(struct Sdctx *o)
{
	mmemset(o, 0, sizeof *o);
}

void
sdselect(struct Sdctx *o)
{
	sdclrstat();
	cmd(cmdr(Cmd7, RespLen48, 0), genrcaarg(o->rca));
	waitcomp();
}

void
sdread(struct Sdctx *o, char *b)
{
	u16 c;
	u32 a, v;
	char *bb;

	R16(RTxMode) = 0x0010;
	c = cmdr(Cmd13, RespLen48, 0);
	a = genrcaarg(o->rca);
	sdclrstat();
	cmd(cmdr(Cmd17, RespLen48, 1), 0);
	waitcomp();
	while ((R32(RPreState) & (1 << OBufRdEn)) == 0)
		;
	bb = b;
	do {
		v = R32(RBufDataPort);
		mmemcpy(b, &v, sizeof v);
		b += sizeof v;
		v = R32(RPreState);
	} while ((v & (1 << OBufRdEn)) && (b - bb) < 512);
}
