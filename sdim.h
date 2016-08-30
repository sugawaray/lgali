#include <types.h>

enum {
	SdV27t36	= 0x1,
	SdVLowRng	= 0x2
};

struct Sdctx {
	u32	csd[4];
	u16	rca;
};

void sdsetbar0(u32 v);
void sdinitctx(struct Sdctx *o);
void sdpwrup();
void sdswrst();
void sdcmd0();
void sdenclk();
void sdinistat();
int sdcvalid(u8 vhs);
u32 sdgenarg8(u8 v, u8 p);
u16 sdxbfreq(u32 v);
u8 sdcalcdivclk(int base, int tgt);
void sddbg();
void sdclrstat();
void sdinireg();
u16 sdcmd55();
u32 sdgenarg55(u16 rca);
int sdenappcmd();
u16 sdacmd41();
u32 sdgenarga41();
int sdcvoltwin();
u16 sdcmd2();
void sdgetcid();
void sdinitrca(struct Sdctx *o);
void sdgetcsd(struct Sdctx *o);
void sdprcsd(struct Sdctx *o);
u32 sdgenarg9(u16 rca);
void sdselect(struct Sdctx *o);
void sdread(struct Sdctx *o, char *b);
