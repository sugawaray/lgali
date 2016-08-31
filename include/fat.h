#include <types.h>

struct BsBpb {
	u8	BS_OEMName[8];
	u32	BPB_HiddSec;
	u32	BPB_TotSec32;
	u16	BPB_BytsPerSec;
	u16	BPB_RsvdSecCnt;
	u16	BPB_RootEntCnt;
	u16	BPB_TotSec16;
	u16	BPB_FATSz16;
	u16	BPB_SecPerTrk;
	u16	BPB_NumHeads;
	u8	BPB_NumFATs;
	u8	BPB_Media;
	u8	BS_jmpBoot[3];
	u8	BPB_SecPerClus;
};

void rdbsbpb(struct BsBpb *b, const char *dat);
