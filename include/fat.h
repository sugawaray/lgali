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

struct BsBpb32 {
	u32	BPB_FATSz32;
	u32	BPB_RootClus;
	u32	BS_VolID;
	u16	BPB_ExtFlags;
	u16	BPB_FSVer;
	u16	BPB_FSInfo;
	u16	BPB_BkBootSec;
	u8	BS_VolLab[11];
	u8	BS_DrvNum;
	u8	BS_Reserved1;
	u8	BS_BootSig;
};

void rdbsbpb(struct BsBpb *b, const char *dat);
void rdbsbpb32(struct BsBpb32 *b, const char *dat);
